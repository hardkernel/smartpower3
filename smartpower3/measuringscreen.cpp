#include <measuringscreen.h>
#include "helpers.h"
//#include "settings.h"

bool MeasuringScreen::_int = false;

uint8_t baud_idx;

MeasuringScreen::MeasuringScreen()
{
	tft.init();
	tft.invertDisplay(true);
	tft.setRotation(3);
	tft.fillScreen(BG_COLOR);
	tft.setSwapBytes(true);
}

void MeasuringScreen::begin(Settings *settings, TwoWire *theWire)
{
	Serial.println("in Screen::begin");
	this->settings = settings;
	Serial.println("settings set");
	
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

	setting_screen = new SettingScreen(&tft, settings);
	wifiManager = new WiFiManager(setting_screen, settings);

	fsInit();
	delay(2000);
	tft.setRotation(3);

	tft.fillRect(0, 0, 480, 320, BG_COLOR);
	initScreen();
	initLED();
	header->init(5, 8);
}

void MeasuringScreen::initLED()
{
	ledcSetup(0, FREQ, RESOLUTION);
	ledcSetup(1, FREQ, RESOLUTION);
	ledcAttachPin(LED2, 0);
	ledcAttachPin(LED1, 1);

	ledcWrite(0, 50);
	ledcWrite(1, 50);
}

uint8_t* MeasuringScreen::getOnOff()
{
	return onoff;
}

void MeasuringScreen::run()
{
	checkOnOff();
	if (!shutdown) {
		drawScreen();

		if (!header->getLowInput()) {
			for (int i = 0; i < 2; i++) {
				if (!enabled_stpd01[i])
					continue;
				channel[i]->isr(onoff[i]);
			}
		}

#ifdef SCREEN_CAPTURE
		if (Serial.available()) {
			if (Serial.readString() == "S") {
				uint8_t color[2*8];
				for (uint32_t y = 0; y < 320; y++) {
					for (uint32_t x = 0; x < 480; x+=8) {
						tft.readRect(x, y, 8, 1, (uint16_t *)color);
						Serial.write(color, 2*8);
					}
				}
				Serial.flush();
			}
		}
#endif
	}
}

void MeasuringScreen::setIntFlag(uint8_t ch)
{
	channel[ch]->setIntFlag();
}

void MeasuringScreen::initScreen(void)
{
	tft.fillRect(0, 52, 480, 285, BG_COLOR);
	for (int i = 0; i < 2; i++) {
		tft.drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
		tft.drawLine(0, 274 + i, 480, 274 + i, TFT_DARKGREY);
	}
	for (int i = 0; i < 2; i++)
		tft.drawLine(236 + i, 50, 236 + i, 320, TFT_DARKGREY);

	channel[0]->initScreen(onoff[0]);
	channel[1]->initScreen(onoff[1]);
	selected = dial_cnt = dial_cnt_old = STATE_NONE;
}

void MeasuringScreen::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (onoff[ch] == 1)
		channel[ch]->pushPower(volt, ampere, watt);
}

void MeasuringScreen::pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt)
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

void MeasuringScreen::checkOnOff()
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
			tft.fillRect(0, 0, 480, 320, BG_COLOR);
			setting_screen->turnOffBacklight();
			writeSysLED(0);
			setting_screen->setLogInterval(0);
		} else {
			shutdown = false;
			tft.fillRect(0, 0, 480, 320, BG_COLOR);
			setting_screen->setLogInterval();
			setting_screen->setBacklightLevelPreset();

			header->init(5, 8);
			if (mode >= SETTING) {
				setting_screen->init(10, 80);
				for (int i = 0; i < 2; i++)
					tft.drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
				selected = dial_cnt = dial_cnt_old = STATE_NONE;
				updated_wifi_info = true;
				updated_wifi_icon = true;
				wifiManager->update_wifi_info = true;
				wifiManager->update_udp_info = true;
			} else {
				initScreen();
			}
		}
	}
	old_state_power = state_power;
}

void MeasuringScreen::disablePower()
{
	channel[0]->disabled();
	channel[1]->disabled();
	enabled_stpd01[0] = false;
	enabled_stpd01[1] = false;
}

void MeasuringScreen::deSelect()
{
	channel[0]->deSelect(VOLT);
	channel[0]->deSelect(CURRENT);
	channel[1]->deSelect(VOLT);
	channel[1]->deSelect(CURRENT);
	header->deSelect(LOGGING);
	header->deSelect(WIFI);
}

void MeasuringScreen::deSelectSetting()
{
	setting_screen->deSelectBLLevel();
	setting_screen->deSelectSerialLogging();
	header->deSelect(LOGGING);
	header->deSelect(WIFI);
}

void MeasuringScreen::select()
{
	if (dial_cnt == dial_cnt_old) {
		return;
	}
	dial_cnt_old = dial_cnt;
	clampVariableToCircularRange(0, 5, dial_direct, &dial_cnt);  // 5 is 0 based count of screen_state_base elements

	deSelect();
	selected = dial_cnt;
	switch (dial_cnt) {
		case STATE_VOLT0:
			channel[0]->select(VOLT);
			break;
		case STATE_CURRENT0:
			channel[0]->select(CURRENT);
			break;
		case STATE_CURRENT1:
			channel[1]->select(CURRENT);
			break;
		case STATE_VOLT1:
			channel[1]->select(VOLT);
			break;
		case STATE_WIFI:
			header->select(WIFI);
			break;
		case STATE_LOGGING:
			header->select(LOGGING);
			break;
	}
}

void MeasuringScreen::select_setting()
{
	if (dial_cnt == dial_cnt_old) {
		return;
	}
	dial_cnt_old = dial_cnt;
	// 4 is 1 based count of screen_state_setting elements, lower limit is 1 because 0 is used
	// as a marker for non-selected item that should not be included in the "selection circle"
	clampVariableToCircularRange(1, 4, dial_direct, &dial_cnt);

	deSelectSetting();
	selected = dial_cnt;
	switch (dial_cnt) {
		case STATE_BL:
			setting_screen->selectBLLevel();
			break;
		case STATE_LOG:
			setting_screen->selectSerialLogging();
			break;
		case STATE_SETTING_WIFI:
			header->select(WIFI);
			break;
		case STATE_SETTING_LOGGING:
			header->select(LOGGING);
			break;
	}
}

void MeasuringScreen::drawBase()
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
		setting_screen->init(10, 80);
		updated_wifi_info = true;
		updated_wifi_icon = true;
		wifiManager->update_wifi_info = true;
		wifiManager->update_udp_info = true;
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}

	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
#ifdef DEBUG_STPD01
		channel[0]->clearDebug();
		channel[1]->clearDebug();
#endif
	}
}

void MeasuringScreen::drawBaseMove()
{
	select();
	if ((cur_time - dial_time) > 5000) {
		mode = BASE;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		mode = BASE;
		btn_pressed[2] = false;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == STATE_VOLT0) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(VOLT);
			volt_set = channel[0]->getVolt();
		} else if (selected == STATE_CURRENT0) {
			mode = BASE_EDIT;
			channel[0]->setCompColor(CURRENT);
			current_limit = channel[0]->getCurrentLimit()/100;
		} else if (selected == STATE_VOLT1) {
			mode = BASE_EDIT;
			channel[1]->setCompColor(VOLT);
			volt_set = channel[1]->getVolt();
		} else if (selected == STATE_CURRENT1) {
			mode = BASE_EDIT;
			channel[1]->setCompColor(CURRENT);
			current_limit = channel[1]->getCurrentLimit()/100;
		} else if (selected == STATE_LOGGING) {
			// same as current state, but redraw will check selection timeout
			mode = BASE_MOVE;
			if (setting_screen->isLoggingEnabled()) {
				setting_screen->disableLogging();
			} else {
				setting_screen->enableLogging();
			}
		} else if (selected == STATE_WIFI) {
			// same as current state, but redraw will check selection timeout
			mode = BASE_MOVE;
			if (wifiManager->isWiFiEnabled()) {
				wifiManager->disableWiFi();
			} else {
				wifiManager->enableWiFi();
			}
		}
		dial_state = dial_cnt;
		dial_cnt = dial_cnt_old = 0;
	}
}

void MeasuringScreen::drawBaseEdit()
{
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
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

void MeasuringScreen::drawSetting()
{
	select_setting();
	if ((cur_time - dial_time) > 10000) {
		deSelectSetting();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		if (selected == STATE_NONE) {
			channel[0]->clearHide();
			channel[1]->clearHide();
			initScreen();
			mode = BASE;
		} else {
			deSelectSetting();
		}
		selected = dial_cnt = STATE_NONE;
	}
	if (btn_pressed[1] == true) {
		btn_pressed[1] = false;
		setting_screen->debug();
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == STATE_BL) {
			mode = SETTING_BL;
			dial_cnt = setting_screen->getBacklightLevel();
			setting_screen->selectBLLevel(TFT_GREEN);
		} else if (selected == STATE_LOG) {
			mode = SETTING_LOG;
			dial_cnt = setting_screen->getSerialBaudIndex();
			setting_screen->selectSerialLogging(TFT_GREEN);
			setting_screen->selectSerialBaud(TFT_YELLOW);
		} else if (selected == STATE_SETTING_LOGGING) {
			if (setting_screen->isLoggingEnabled()) {
				setting_screen->disableLogging();
			} else {
				setting_screen->enableLogging();
			}
		} else if (selected == STATE_SETTING_WIFI) {
			if (wifiManager->isWiFiEnabled()) {
				wifiManager->disableWiFi();
			} else {
				wifiManager->enableWiFi();
			}
		}
	}
	dial_cnt_old = dial_cnt;
}

void MeasuringScreen::drawSettingBL()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING;
		deSelectSetting();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		setting_screen->changeBacklight();
		setting_screen->deSelectBLLevel();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_BL;
		deSelectSetting();
		setting_screen->changeBacklight();
		setting_screen->selectBLLevel();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING;
		settings->setBacklightLevelIndex(setting_screen->setBacklightLevelPreset());
		setting_screen->selectBLLevel();
		selected = dial_cnt = STATE_BL;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		clampVariableToRange(0, 6, &dial_cnt);
		dial_cnt_old = dial_cnt;
		setting_screen->changeBacklight(dial_cnt);
	}
}

void MeasuringScreen::drawSettingLOG()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING;
		deSelectSetting();
		selected = dial_cnt = dial_cnt_old = dial_cnt_old = STATE_NONE;
		setting_screen->deSelectSerialBaud(TFT_WHITE);
		setting_screen->restoreSerialBaud();
		setting_screen->deSelectLogInterval(TFT_WHITE);
		setting_screen->restoreLogIntervalValue();
		setting_screen->deSelectSerialLogging(TFT_YELLOW);
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_LOG;
		setting_screen->deSelectSerialBaud(TFT_WHITE);
		setting_screen->restoreSerialBaud();
		setting_screen->deSelectLogInterval(TFT_WHITE);
		setting_screen->restoreLogIntervalValue();
		setting_screen->deSelectSerialLogging(TFT_YELLOW);
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == 5) {
			mode = SETTING;
			setting_screen->setSerialBaud();
			setting_screen->setLogInterval();
			selected = dial_cnt = dial_cnt_old = STATE_LOG;
			setting_screen->deSelectLogInterval(TFT_WHITE);
			setting_screen->deSelectSerialLogging(TFT_YELLOW);
		} else {
			setting_screen->deSelectSerialBaud(TFT_WHITE);
			setting_screen->selectLogInterval();
			dial_cnt = dial_cnt_old = setting_screen->getLogInterval();
			selected = 5;
		}
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		if (selected == 5) {
			clampVariableToRange(0, 6, &dial_cnt);
			setting_screen->changeLogInterval(dial_cnt);
		} else {
			clampVariableToRange(0, 9, &dial_cnt);
			setting_screen->changeSerialBaud(dial_cnt);
		}
		dial_cnt_old = dial_cnt;
	}
}

uint16_t MeasuringScreen::getEnabledLogInterval(void)
{
	uint16_t tmp = setting_screen->getLogIntervalValue();

	if (tmp > 0 and setting_screen->isLoggingEnabled()) {
		header->onLogging();
		return tmp;
	} else if (tmp > 0) {
		header->possibleLogging();
	} else {
		header->offLogging();
	}
	return 0;
}

void MeasuringScreen::setWiFiIconState()
{
	if (WiFi.status() == WL_CONNECTED) {
		header->onWiFi();
	} else if (wifiManager->canConnect()) {
		header->possibleWiFi();
	} else {
		header->offWiFi();
	}
}

void MeasuringScreen::drawScreen()
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
		} else {
			if (updated_wifi_info || wifiManager->update_wifi_info) {
				updated_wifi_info = wifiManager->update_wifi_info = false;
				if (WiFi.status() == WL_CONNECTED) {
					setting_screen->drawSSID(wifiManager->apInfoConnected());
				} else if (wifiManager->hasSavedConnectionInfo()) {
					setting_screen->drawSSID(wifiManager->apInfoSaved());
				} else {
					setting_screen->drawSSID("WiFi not saved");
				}
			}
			if (wifiManager->update_udp_info) {
				setting_screen->drawUDPIpaddr(wifiManager->ipaddr_udp.toString());
				setting_screen->drawUDPport(wifiManager->port_udp);
				wifiManager->update_udp_info = false;
			}
		}
		header->draw();
	}
	channel[0]->drawVoltSet();
	channel[1]->drawVoltSet();
}

void MeasuringScreen::changeVolt(screen_mode_t mode)
{
	if (selected == STATE_VOLT0) {
		clampVoltageDialCountToRange(volt_set, &dial_cnt);
		if (mode == BASE_MOVE) {
			channel[0]->setVolt(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel0Voltage(channel[0]->getVolt());
			}
		} else {
			channel[0]->editVolt(dial_cnt);
		}
	} else if (selected == STATE_CURRENT0) {
		clampVariableToRange(-(current_limit - 5), (30 - current_limit), &dial_cnt);
		if (mode == BASE_MOVE) {
			channel[0]->setCurrentLimit(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel0CurrentLimit(channel[0]->getCurrentLimit());
			}
		} else {
			channel[0]->editCurrentLimit(dial_cnt);
		}

	} else if (selected == STATE_VOLT1) {
		clampVoltageDialCountToRange(volt_set, &dial_cnt);
		if (mode == BASE_MOVE) {
			channel[1]->setVolt(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel1Voltage(channel[1]->getVolt());
			}
		} else {
			channel[1]->editVolt(dial_cnt);
		}
	} else if (selected == STATE_CURRENT1) {
		clampVariableToRange(-(current_limit - 5), (30 - current_limit), &dial_cnt);
		if (mode == BASE_MOVE) {
			channel[1]->setCurrentLimit(dial_cnt);  // this by default sets incremental difference to currently set value
			if (dial_cnt != 0) {
				settings->setChannel1CurrentLimit(channel[1]->getCurrentLimit());
			}
		} else {
			channel[1]->editCurrentLimit(dial_cnt);
		}
	}

}

void MeasuringScreen::isrSTPD01()
{
	for (int i = 0; i < 2; i++) {
		channel[i]->isAvailableSTPD01();
		int_stat[i] = channel[i]->checkInterruptStat(onoff[i]);
	}
}

uint8_t MeasuringScreen::getIntStat(uint8_t channel)
{
	return int_stat[channel];
}

void MeasuringScreen::countDial(int8_t dial_cnt, int8_t direct, uint8_t step, uint32_t milisec)
{
	this->dial_cnt += dial_cnt*step;
	this->dial_time = milisec;
	this->dial_direct = direct;
	this->dial_step = step;
}

void MeasuringScreen::setTime(uint32_t milisec)
{
	if (flag_long_press) {
		if (digitalRead(36) == 0) {
			if (count_long_press) {
				count_long_press = false;
				btn_pressed[4] = true;
				flag_long_press = 0;
			}
		} else {
			if (flag_long_press != 3) {
				btn_pressed[2] = true;
			}
			flag_long_press = 0;
		}
		if (flag_long_press == 1) {
			flag_long_press = 2;
		}
	}
	this->cur_time = milisec;
}

void MeasuringScreen::clearBtnEvent(void)
{
	for (int i = 0; i < 4; i++)
		btn_pressed[i] = false;
}

void MeasuringScreen::getBtnPress(uint8_t idx, uint32_t cur_time, bool long_pressed)
{
	switch (idx) {
	case 0: /* Channel0 ON/OFF */
	case 1: /* Channel1 ON/OFF */
		if (shutdown || mode > BASE_EDIT || long_pressed)
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

void MeasuringScreen::drawBmp(const char *filename, int16_t x, int16_t y)
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
    }
    else Serial.println("BMP format not recognized.");
  }
  bmpFS.close();
}

uint16_t MeasuringScreen::read16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t MeasuringScreen::read32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

void MeasuringScreen::fsInit(void)
{
	uint16_t volt_set0 = 5000;  // 5.0V in millivolts
	uint16_t volt_set1 = 5000;  // 5.0V in millivolts
	uint16_t current_limit0 = 3000;  // 3.0A in milliamps
	uint16_t current_limit1 = 3000;  // 3.0A in milliamps
	uint8_t backlight_level_preset = 3;
	uint8_t log_interval = 0;
	uint32_t serial_baud = 115200;
	uint16_t port_udp = 0;
	IPAddress address;
	address.fromString("0.0.0.0");

	Serial.println("Screen::fsInit");
	//NOTE: When all relevant values have defaults in Settings, this block could be removed
	if (settings->isFirstBoot(true)) {
		Serial.println("First boot!!!");
		settings->setBacklightLevelIndex(backlight_level_preset);
		settings->setSerialBaudRate(serial_baud);
		settings->setLogInterval(log_interval);
		settings->setFirstBoot(false);
		settings->setChannel0Voltage(volt_set0);
		settings->setChannel1Voltage(volt_set1);
		settings->setChannel0CurrentLimit(current_limit0);
		settings->setChannel1CurrentLimit(current_limit1);
		settings->setWifiIpv4UdpLoggingServerIpAddress(address, true);
		settings->setWifiIpv4UdpLoggingServerPort(port_udp);
	}
	volt_set0 = settings->getChannel0Voltage();
	volt_set1 = settings->getChannel1Voltage();
	current_limit0 = settings->getChannel0CurrentLimit();
	current_limit1 = settings->getChannel1CurrentLimit();
	backlight_level_preset = settings->getBacklightLevelIndex(true);
	serial_baud = settings->getSerialBaudRate(true);
	log_interval = settings->getLogInterval(true);

	channel[0]->setVolt(volt_set0, 1);
	channel[0]->setCurrentLimit(current_limit0, 1);
	channel[1]->setVolt(volt_set1, 1);
	channel[1]->setCurrentLimit(current_limit1, 1);
	setting_screen->setBacklightLevelPreset(backlight_level_preset, true);
	setting_screen->setSerialBaud(serial_baud);
	setting_screen->setLogInterval(log_interval);

	wifiManager->ipaddr_udp = settings->getWifiIpv4UdpLoggingServerIpAddress(true);
	wifiManager->port_udp = settings->getWifiIpv4UdpLoggingServerPort(true);
}

void MeasuringScreen::debug()
{
	header->setDebug();
}

void MeasuringScreen::countLowVoltage(uint8_t ch)
{
	channel[ch]->countLowVoltage();
}

void MeasuringScreen::clearLowVoltage(uint8_t ch)
{
	channel[ch]->clearLowVoltage();
}

bool MeasuringScreen::getShutdown(void)
{
	return shutdown;
}

void MeasuringScreen::dimmingLED(uint8_t led)
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

void MeasuringScreen::writeSysLED(uint8_t val)
{
	ledcWrite(0, val);
}

void MeasuringScreen::writePowerLED(uint8_t val)
{
	ledcWrite(1, val);
}

void MeasuringScreen::updateWiFiInfo(void)
{
	this->updated_wifi_info = true;
	this->updated_wifi_icon = true;
}

bool MeasuringScreen::isWiFiEnabled(void) {
	return wifiManager->isWiFiEnabled();
}

void MeasuringScreen::enableWiFi(void) {
	wifiManager->enableWiFi();
}

void MeasuringScreen::disableWiFi(void) {
	wifiManager->disableWiFi();
}
