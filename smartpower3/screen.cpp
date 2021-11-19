#include "screen.h"

bool Screen::_int = false;

Screen::Screen()
{
	tft.init();
	tft.setRotation(3);
	tft.fillScreen(TFT_BLACK);
	tft.fillScreen(TFT_BLACK);
	tft.setSwapBytes(true);
}

void Screen::begin(TwoWire *theWire)
{
	_wire = theWire;
	header = new Header(&tft);
	channel[0] = new Channel(&tft, _wire, 0, 50, 0);
	channel[1] = new Channel(&tft, _wire, 246, 50, 1);

	if (!SPIFFS.begin(false)) {
		Serial.println("SPIFFS mount error");
		return;
	}
	fs = &SPIFFS;
	tft.setRotation(0);
	drawBmp("/logo_hardkernel.bmp", 0, 0);
	tft.setRotation(3);
	tft.loadFont(NotoSansBold20);
	tft.drawString("Build date : ", 80, 295, 2);
	tft.drawString(String(__DATE__), 200, 295, 2);
	tft.drawString(String(__TIME__), 320, 295, 2);
	tft.unloadFont();

	setting = new Setting(&tft);

	fsInit();
	delay(2000);
	tft.setRotation(3);

	tft.fillRect(0, 0, 480, 320, TFT_BLACK);
	initScreen();
	initLED();
	header->init(5, 8);
}

void Screen::initLED()
{
	ledcSetup(0, FREQ, RESOLUTION);
	ledcSetup(1, FREQ, RESOLUTION);
	ledcAttachPin(LED2, 0);
	ledcAttachPin(LED1, 1);

	ledcWrite(0, 50);
	ledcWrite(1, 50);
}

int8_t* Screen::getOnOff()
{
	return onoff;
}

void Screen::run()
{
	checkOnOff();
	if (!shutdown) {
		drawScreen();

		if (!header->getLowInput()) {
			for (int i = 0; i < 2; i++) {
				if (!enabled_stpd01[i])
					continue;
				channel[i]->isr();
			}
		}
	};
}

void Screen::setIntFlag(uint8_t ch)
{
	channel[ch]->setIntFlag();
}

void Screen::initScreen(void)
{
	tft.fillRect(0, 52, 480, 285, TFT_BLACK);
	for (int i = 0; i < 2; i++) {
		tft.drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
		tft.drawLine(0, 274 + i, 480, 274 + i, TFT_DARKGREY);
	}
	for (int i = 0; i < 2; i++)
		tft.drawLine(236 + i, 50, 236 + i, 320, TFT_DARKGREY);

	channel[0]->initScreen(onoff[0]);
	channel[1]->initScreen(onoff[1]);
	activated = dial_cnt = dial_cnt_old = STATE_NONE;
}

void Screen::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (onoff[ch] == 1)
		channel[ch]->pushPower(volt, ampere, watt);
}

void Screen::pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt)
{
	if ((volt < 6000 || volt >= 26000) && !low_input) {
		low_input = true;
		header->setLowInput(true);
		state_power = 1;
	} else if ((volt >= 6000 && volt <= 26000) && header->getLowInput()) {
		low_input = false;
		header->setLowInput(false);
		state_power = 3;
	}

	header->pushPower(volt, ampere, watt);
}

void Screen::checkOnOff()
{
	if (state_power == 1) {
		state_power = 2;
		channel[0]->off();
		channel[1]->off();
		disablePower();
		onoff[0] = 2;
		onoff[1] = 2;
	} else if (state_power == 3) {
#ifdef USE_SINGLE_IRQ_STPD01
		for (int i = 0; i < 2; i++) {
			channel[i]->enable();
			channel[i]->off();
		}
#endif
		state_power = 4;
	} else if (state_power == 4) {
		state_power = 5;
	} else if (state_power == 5) {
		state_power = 6;
	} else if (state_power == 6) {
		for (int i = 0; i < 2; i++) {
			if (onoff[i] == 3) {
				if (!enabled_stpd01[i]) {
					enabled_stpd01[i] = true;
					channel[i]->enable();
				}
				channel[i]->on();
				onoff[i] = 1;
			} else if (onoff[i] == 2) {
				onoff[i] = 0;
				channel[i]->off();
				channel[i]->drawChannel(true);
			}
		}
	}

	if (btn_pressed[4]) {
		btn_pressed[2] = false;
		btn_pressed[4] = false;
		if (!shutdown) {
			shutdown = true;
			onoff[0] = 0;
			onoff[1] = 0;
			channel[0]->off();
			channel[1]->off();
			tft.fillRect(0, 0, 480, 320, TFT_BLACK);
			setting->setBacklightLevel(0);
			writeSysLED(0);
			setting->setLogIntervalValue(0);
		} else {
			shutdown = false;
			tft.fillRect(0, 0, 480, 320, TFT_BLACK);
			setting->setLogInterval();
			setting->setBacklightLevel();

			header->init(5, 8);
			if (mode >= SETTING) {
				setting->init(10, 100);
				for (int i = 0; i < 2; i++)
					tft.drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
				activated = dial_cnt = dial_cnt_old = STATE_NONE;
			} else {
				initScreen();
			}
		}
	}

	if (state_power != old_state_power)
		old_state_power = state_power;
}

void Screen::disablePower()
{
	channel[0]->disabled();
	channel[1]->disabled();
	enabled_stpd01[0] = false;
	enabled_stpd01[1] = false;
}

void Screen::deActivate()
{
	channel[0]->deActivate(VOLT);
	channel[0]->deActivate(CURRENT);
	channel[1]->deActivate(VOLT);
	channel[1]->deActivate(CURRENT);
}

void Screen::deActivateSetting()
{
	setting->deActivateBLLevel();
	setting->deActivateSerialLogging();
}

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	if (dial_cnt > 3) {
		dial_cnt = 3;
		if (dial_direct == 1)
			dial_cnt = 0;
	} else if (dial_cnt < 0) {
		dial_cnt = 0;
		if (dial_direct == -1)
			dial_cnt = 3;
	}

	deActivate();
	activated = dial_cnt;
	switch (dial_cnt) {
		case STATE_VOLT0:
			channel[0]->activate(VOLT);
			break;
		case STATE_CURRENT0:
			channel[0]->activate(CURRENT);
			break;
		case STATE_CURRENT1:
			channel[1]->activate(CURRENT);
			break;
		case STATE_VOLT1:
			channel[1]->activate(VOLT);
			break;
	}
}

void Screen::activate_setting()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	if (dial_cnt > 2) {
		dial_cnt = 2;
		if (dial_direct == 1)
			dial_cnt = 0;
	} else if (dial_cnt < 0) {
		dial_cnt = 0;
		if (dial_direct == -1)
			dial_cnt = 2;
	}

	deActivateSetting();
	activated = dial_cnt;
	switch (dial_cnt) {
		case STATE_BL:
			setting->activateBLLevel();
			break;
		case STATE_LOG:
			setting->activateSerialLogging();
			break;
	}
}

void Screen::drawBase()
{
	if (dial_cnt != dial_cnt_old) {
		clearBtnEvent();
		mode = BASE_MOVE;
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		channel[0]->setHide();
		channel[1]->setHide();
		mode = SETTING;
		setting->init(10, 100);
		activated = dial_cnt = dial_cnt_old = STATE_NONE;
	}

	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
#ifdef DEBUG_STPD01
		channel[0]->clearDebug();
		channel[1]->clearDebug();
#endif
	}
}

void Screen::drawBaseMove()
{
	activate();
	if ((cur_time - dial_time) > 5000) {
		mode = BASE;
		deActivate();
		activated = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		mode = BASE;
		btn_pressed[2] = false;
		deActivate();
		activated = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (activated == STATE_VOLT0) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(VOLT);
			volt_set = channel[0]->getVolt();
		} else if (activated == STATE_CURRENT0) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(CURRENT);
			current_limit = channel[0]->getCurrentLimit();
		} else if (activated == STATE_VOLT1) {
			mode = BASE_EDIT;
			channel[1]->setCompColor(VOLT);
			volt_set = channel[1]->getVolt();
		} else if (activated == STATE_CURRENT1) {
			mode = BASE_EDIT;
			channel[1]->setCompColor(CURRENT);
			current_limit = channel[1]->getCurrentLimit();
		}
		dial_state = dial_cnt;
		dial_cnt = 0;
		dial_cnt_old = 0;
	}
}

void Screen::drawBaseEdit()
{
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		deActivate();
		activated = dial_cnt = dial_cnt_old = STATE_NONE;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		mode = BASE_MOVE;
		dial_cnt = dial_cnt_old = dial_state;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		btn_pressed[2] = false;
		return;
	}
	if (btn_pressed[3] == true) {
		mode = BASE_MOVE;
		btn_pressed[3] = false;
		changeVolt(mode);
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		dial_cnt = dial_state;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		dial_cnt_old = dial_cnt;
		changeVolt(mode);
		channel[0]->pushPowerEdit();
		channel[1]->pushPowerEdit();
	}
}

void Screen::drawSetting()
{
	activate_setting();
	if ((cur_time - dial_time) > 10000) {
		dial_cnt = 0;
		dial_cnt_old = 0;
		deActivateSetting();
		activated = STATE_NONE;
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		if (activated == STATE_NONE) {
			channel[0]->clearHide();
			channel[1]->clearHide();
			initScreen();
			mode = BASE;
		} else {
			deActivateSetting();
		}
		activated = dial_cnt =  STATE_NONE;
	}
	if (btn_pressed[1] == true) {
		btn_pressed[1] = false;
		setting->debug();
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (activated == STATE_BL) {
			mode = SETTING_BL;
			dial_cnt = setting->getBacklightLevel();
			setting->activateBLLevel(TFT_GREEN);
		} else if (activated == STATE_LOG) {
			mode = SETTING_LOG;
			dial_cnt = setting->getSerialBaudLevel();
			setting->activateSerialLogging(TFT_GREEN);
			setting->activateSerialBaud(TFT_YELLOW);
		}
	}
	if (dial_cnt != dial_cnt_old) {
		dial_cnt_old = dial_cnt;
	}
}

void Screen::drawSettingBL()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING;
		deActivateSetting();
		activated = dial_cnt = STATE_NONE;
		dial_cnt_old = STATE_NONE;
		setting->changeBacklight();
		setting->deActivateBLLevel();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING;
		activated = dial_cnt = dial_cnt_old = STATE_BL;
		deActivateSetting();
		setting->changeBacklight();
		setting->activateBLLevel();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING;
		NVS.setInt("bl_level", setting->setBacklightLevel());
		setting->activateBLLevel();
		activated = dial_cnt = STATE_BL;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		if (dial_cnt < 0)
			dial_cnt = 0;
		else if (dial_cnt > 6)
			dial_cnt = 6;
		dial_cnt_old = dial_cnt;
		setting->changeBacklight(dial_cnt);
	}
}

void Screen::drawSettingLOG()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING;
		deActivateSetting();
		activated = dial_cnt = dial_cnt_old = STATE_NONE;
		dial_cnt_old = STATE_NONE;
		setting->deActivateSerialBaud(TFT_WHITE);
		setting->restoreSerialBaud();
		setting->deActivateLogInterval(TFT_WHITE);
		setting->restoreLogInterval();
		setting->deActivateSerialLogging(TFT_YELLOW);
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING;
		activated = dial_cnt = dial_cnt_old = STATE_LOG;
		setting->deActivateSerialBaud(TFT_WHITE);
		setting->restoreSerialBaud();
		setting->deActivateLogInterval(TFT_WHITE);
		setting->restoreLogInterval();
		setting->deActivateSerialLogging(TFT_YELLOW);
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (activated == 5) {
			mode = SETTING;
			NVS.setInt("serial_baud", setting->setSerialBaud());
			NVS.setInt("log_interval", setting->setLogInterval());
			activated = dial_cnt = dial_cnt_old = STATE_LOG;
			setting->deActivateLogInterval(TFT_WHITE);
			setting->deActivateSerialLogging(TFT_YELLOW);
		} else {
			setting->deActivateSerialBaud(TFT_WHITE);
			setting->activateLogInterval();
			dial_cnt = 0;
			dial_cnt_old = 1;
			activated = 5;
		}
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		if (activated == 5) {
			if (dial_cnt < 0)
				dial_cnt = 0;
			else if (dial_cnt > 6)
				dial_cnt = 6;
			setting->changeLogInterval(dial_cnt);
		} else {
			if (dial_cnt < 0)
				dial_cnt = 0;
			else if (dial_cnt > 9)
				dial_cnt = 9;
			setting->changeSerialBaud(dial_cnt);
		}
		dial_cnt_old = dial_cnt;
	}
}

uint16_t Screen::getLogInterval(void)
{
	uint16_t tmp;

	tmp = setting->getLogIntervalValue();
	if (tmp > 0) {
		header->onLogging();
	} else {
		header->offLogging();
	}

	return tmp;
}

void Screen::drawScreen()
{
	switch (mode) {
	case BASE:
		drawBase();
		break;
	case BASE_MOVE:
		drawBaseMove();
		break;
	case BASE_EDIT:
		drawBaseEdit();
		break;
	case SETTING:
		drawSetting();
		break;
	case SETTING_BL:
		drawSettingBL();
		break;
	case SETTING_LOG:
		drawSettingLOG();
		break;
	}
	if ((cur_time - fnd_time) > 300) {
		fnd_time = cur_time;
		if (mode < SETTING) {
			if (onoff[0])
				channel[0]->drawChannel();
			if (onoff[1])
				channel[1]->drawChannel();
			isrSTPD01();
		}
		header->draw();
	}
			channel[0]->drawVoltSet();
			channel[1]->drawVoltSet();
}

void Screen::changeVolt(screen_mode_t mode)
{
	if (activated == STATE_VOLT0) {
		if (dial_cnt < -(volt_set/100 - 30))
			dial_cnt = -(volt_set/100 - 30);
		else if (dial_cnt + (volt_set/100) > 200)
			dial_cnt = 200 - (volt_set/100);
		if (mode == BASE_MOVE) {
			channel[0]->setVolt(dial_cnt);
			if (dial_cnt != 0) {
				NVS.setString("voltage0", String(channel[0]->getVolt()/1000.0));

			}
		} else {
			channel[0]->editVolt(dial_cnt);
		}
	} else if (activated == STATE_CURRENT0) {
		if (dial_cnt > (30 - current_limit))
			dial_cnt = (30 - current_limit);
		else if (dial_cnt < -(current_limit - 5))
			dial_cnt = -(current_limit - 5);
		if (mode == BASE_MOVE) {
			channel[0]->setCurrentLimit(dial_cnt);
			if (dial_cnt != 0) {
				NVS.setString("current_limit0", String(channel[0]->getCurrentLimit()/10.0));
			}
		} else {
			channel[0]->editCurrentLimit(dial_cnt);
		}

	} else if (activated == STATE_VOLT1) {
		if (dial_cnt < -(volt_set/100 - 30))
			dial_cnt = -(volt_set/100 - 30);
		else if (dial_cnt + (volt_set/100) > 200)
			dial_cnt = 200 - (volt_set/100);
		if (mode == BASE_MOVE) {
			channel[1]->setVolt(dial_cnt);
			if (dial_cnt != 0) {
				NVS.setString("voltage1", String(channel[1]->getVolt()/1000.0));
			}
		} else {
			channel[1]->editVolt(dial_cnt);
		}
	} else if (activated == STATE_CURRENT1) {
		if (dial_cnt > (30 - current_limit))
			dial_cnt = (30 - current_limit);
		else if (dial_cnt < -(current_limit - 5))
			dial_cnt = -(current_limit - 5);
		if (mode == BASE_MOVE) {
			channel[1]->setCurrentLimit(dial_cnt);
			if (dial_cnt != 0) {
				NVS.setString("current_limit1", String(channel[1]->getCurrentLimit()/10.0));
			}
		} else {
			channel[1]->editCurrentLimit(dial_cnt);
		}
	}

}


void Screen::isrSTPD01()
{
	for (int i = 0; i < 2; i++) {
		channel[i]->isAvailableSTPD01();
		int_stat[i] = channel[i]->checkInterruptStat(onoff[i]);
	}
}

uint8_t Screen::getIntStat(uint8_t channel)
{
	return int_stat[channel];
}

void Screen::countDial(int8_t dial_cnt, int8_t direct, uint8_t step, uint32_t milisec)
{
	this->dial_cnt += dial_cnt*step;
	this->dial_time = milisec;
	this->dial_direct = direct;
	this->dial_step = step;
}

void Screen::setTime(uint32_t milisec)
{
	if (flag_long_press) {
		if (digitalRead(36) == 0) {
			if (count_long_press) {
				count_long_press = false;
				btn_pressed[4] = true;
				flag_long_press = 0;
			}
		} else {
			if (flag_long_press != 3)
				btn_pressed[2] = true;
			flag_long_press = 0;
		}
		if (flag_long_press == 1) {
			flag_long_press = 2;
		}
	}
	this->cur_time = milisec;
}

void Screen::clearBtnEvent(void)
{
	for (int i = 0; i < 4; i++)
		btn_pressed[i] = false;
}

void Screen::getBtnPress(uint8_t idx, uint32_t cur_time, bool long_pressed)
{
	switch (idx) {
	case 0: /* Channel0 ON/OFF */
	case 1: /* Channel1 ON/OFF */
		if (shutdown)
			break;
		if (mode > BASE_EDIT)
			break;
		if (long_pressed)
			break;
		if (onoff[idx] == 1)
			onoff[idx] = 2;
		else if (onoff[idx] == 0)
			onoff[idx] = 3;
		btn_pressed[idx] = true;
		break;
	case 2:  /* MENU/CANCEL */
		dial_time = cur_time;
		flag_long_press = 1;
		count_long_press = long_pressed;
		break;
	case 3: /* Set value */
		if (shutdown)
			break;
		dial_time = cur_time;
		btn_pressed[idx] = true;
		break;
	}
}

void Screen::readFile(const char * path){
    File file = fs->open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
	char tmp;
    while(file.available()){
		tmp = file.read();
		if (tmp == '\n') {
			Serial.printf(" : %d \n\r", file.position());
			continue;
		}
		Serial.write(tmp);
    }
    file.close();
}

void Screen::listDir(const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\r\n", dirname);

    File root = fs->open(dirname);
    if(!root){
        Serial.println("- failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println(" - not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(file.name(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("\tSIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void Screen::drawBmp(const char *filename, int16_t x, int16_t y)
{
	if ((x >= tft.width()) || (y >= tft.height()))
		return;

	File bmpFS= fs->open(filename, "r");
	if (!bmpFS)
	{
		Serial.println("File not found");
		return;
	}

  uint32_t seekOffset;
  uint16_t w, h, row, col;
  uint8_t  r, g, b;

  uint32_t startTime = millis();

  if (read16(bmpFS) == 0x4D42)
  {
    read32(bmpFS);
    read32(bmpFS);
    seekOffset = read32(bmpFS);
    read32(bmpFS);
    w = read32(bmpFS);
    h = read32(bmpFS);

    if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0))
    {
      y += h - 1;

      bool oldSwapBytes = tft.getSwapBytes();
      tft.setSwapBytes(true);
      bmpFS.seek(seekOffset);

      uint16_t padding = (4 - ((w * 3) & 3)) & 3;
      uint8_t lineBuffer[w * 3 + padding];

      for (row = 0; row < h; row++) {
        bmpFS.read(lineBuffer, sizeof(lineBuffer));
        uint8_t*  bptr = lineBuffer;
        uint16_t* tptr = (uint16_t*)lineBuffer;
        // Convert 24 to 16 bit colours
        for (col = 0; col < w; col++)
        {
          b = *bptr++;
          g = *bptr++;
          r = *bptr++;
          *tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
        }

        // Push the pixel row to screen, pushImage will crop the line if needed
        // y is decremented as the BMP image is drawn bottom up
        tft.pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
      }
      tft.setSwapBytes(oldSwapBytes);
      Serial.print("Loaded in "); Serial.print(millis() - startTime);
      Serial.println(" ms");
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

uint16_t Screen::read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t Screen::read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}


void Screen::fsInit(void)
{
	float volt_set0 = 5.0;
	float volt_set1 = 5.0;
	float current_limit0 = 3.0;
	float current_limit1 = 3.0;
	uint8_t backlight_level = 3;
	uint8_t log_interval = 0;
	uint32_t serial_baud = 115200;

	if (isFirstBoot()) {
		Serial.println("First boot!!!");
		NVS.setInt("autorun", 0);
		NVS.setInt("bl_level", 3);
		NVS.setInt("serial_baud", 115200);
		NVS.setInt("log_interval", 0);
		NVS.setInt("firstboot", 1);
		NVS.setString("voltage0", "5.0");
		NVS.setString("voltage1", "5.0");
		NVS.setString("current_limit0", "3.0");
		NVS.setString("current_limit1", "3.0");
	}
	volt_set0 = NVS.getString("voltage0").toFloat()*1000;
	volt_set1 = NVS.getString("voltage1").toFloat()*1000;
	current_limit0 = NVS.getString("current_limit0").toFloat()*1000;
	current_limit1 = NVS.getString("current_limit1").toFloat()*1000;
	backlight_level = NVS.getInt("bl_level");
	serial_baud = NVS.getInt("serial_baud");
	log_interval = NVS.getInt("log_interval");

	channel[0]->setVolt(volt_set0, 1);
	channel[0]->setCurrentLimit(current_limit0, 1);
	channel[1]->setVolt(volt_set1, 1);
	channel[1]->setCurrentLimit(current_limit1, 1);
	setting->setBacklightLevel(backlight_level, true);
	setting->setSerialBaud(serial_baud);
	setting->setLogIntervalValue(log_interval);
}

bool Screen::isFirstBoot()
{
	if (NVS.getInt("firstboot"))
		return 0;
	else
		return 1;
}

void Screen::debug()
{
	header->setDebug();
}

void Screen::countLowVoltage(uint8_t ch)
{
	channel[ch]->countLowVoltage();
}

void Screen::clearLowVoltage(uint8_t ch)
{
	channel[ch]->clearLowVoltage();
}

bool Screen::getShutdown(void)
{
	return shutdown;
}

void Screen::dimmingLED(uint8_t led)
{
	for (int i = 0; i < 150; i++) {
		ledcWrite(led, i);
		delay(10);
	}
	for (int i = 150; i > 0; i--) {
		ledcWrite(led, i);
		delay(10);
	}
}

void Screen::writeSysLED(uint8_t val)
{
	ledcWrite(0, val);
}

void Screen::writePowerLED(uint8_t val)
{
	ledcWrite(1, val);
}
