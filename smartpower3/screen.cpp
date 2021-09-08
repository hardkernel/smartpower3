#include "screen.h"
#include <ArduinoTrace.h>

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
	channel[0] = new Channel(&tft, _wire, 10, 40, 0);
	channel[1] = new Channel(&tft, _wire, 260, 40, 1);

	setting = new Setting(&tft);
	fsInit();

	initScreen();
	header->init(3, 3);
}

int8_t* Screen::getOnOff()
{
	return onoff;
}

void Screen::run()
{
	checkOnOff();
	drawScreen();

	if (btn_pressed[1]) {
		btn_pressed[1] = false;
	}
	if (btn_pressed[2]) {
		btn_pressed[2] = false;
	}
	if (btn_pressed[3]) {
		btn_pressed[3] = false;
	}

	if (!header->getLowInput()) {
		for (int i = 0; i < 2; i++) {
			if (!enabled_stpd01[i])
				continue;
			channel[i]->isr();
		}
	}
	isrSTPD01();
}

void Screen::setIntFlag(uint8_t ch)
{
	channel[ch]->setIntFlag();
}

void Screen::initScreen(void)
{
	tft.fillRect(0, 35, 480, 285, TFT_BLACK);

	for (int i = 0; i < 3; i++) {
		tft.drawLine(0, 33 + i, 480, 33 + i, TFT_WHITE);
		tft.drawLine(0, 250 + i, 480, 250 + i, TFT_WHITE);
	}
	for (int i = 0; i < 4; i++)
		tft.drawLine(238 + i, 35, 238 + i, 320, TFT_WHITE);

	channel[0]->initScreen();
	channel[1]->initScreen();
}

void Screen::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (onoff[ch] == 1) {
		channel[ch]->pushPower(volt, ampere, watt);
	} else {
		channel[ch]->pushPower(0, 0, 0);
	}
}

void Screen::pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt)
{
	if ((volt < 6000 || volt >= 26000) && !low_input) {
		Serial.println("Hello low input");
		low_input = true;
		header->setLowInput(true);
		state_power = 1;
	} else if ((volt >= 6000 && volt <= 26000) && header->getLowInput()) {
		low_input = false;
		header->setLowInput(false);
		state_power = 3;
	}

	if (header->getInputVoltage()/1000 != volt/1000) {
		header->pushPower(volt, ampere, watt);
	}
}

void Screen::checkOnOff()
{
	if (state_power == 1) {
		state_power = 2;
#ifdef USE_SINGLE_IRQ_STPD01
		channel[0]->off();
		channel[1]->off();
#else
		disablePower();
#endif
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
		Serial.println("check interrupt");
		state_power = 5;
	} else if (state_power == 5) {
		Serial.println("check autostart");
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
				channel[i]->off();
				channel[i]->drawChannel(true);
				onoff[i] = 0;
			}
		}
	}

	if (state_power != old_state_power) {
		old_state_power = state_power;
		Serial.printf("[ power state ] : %d\n\r", state_power);
	}
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
	activated = 0;
	header->deActivate();
	channel[0]->deActivate(VOLT);
	channel[0]->deActivate(CURRENT);
	channel[1]->deActivate(VOLT);
	channel[1]->deActivate(CURRENT);
}

void Screen::deActivateSetting()
{
	header->deActivate();
	setting->deActivateBLLevel();
	setting->deActivateFanLevel();
}

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	if (dial_cnt > 4)
		dial_cnt = 0;
	else if (dial_cnt < 0)
		dial_cnt = 4;

	deActivate();
	activated = dial_cnt;
	switch (dial_cnt) {
		case STATE_HEADER:
			header->activate();
			break;
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
	if (dial_cnt > 2)
		dial_cnt = 2;
	else if (dial_cnt < 0)
		dial_cnt = 0;

	deActivateSetting();
	activated = dial_cnt;
	switch (dial_cnt) {
		case STATE_SETTING:
			header->activate();
			break;
		case STATE_BL:
			setting->activateBLLevel();
			break;
		case STATE_FAN:
			setting->activateFanLevel();
			break;
	}
}

void Screen::drawBase()
{
	if (dial_cnt != dial_cnt_old) {
		clearBtnEvent();
		mode = BASE_MOVE;
	}
}

void Screen::drawBaseMove()
{
	activate();
	if ((cur_time - dial_time) > 5000) {
		mode = BASE;
		deActivate();
		dial_cnt = 0;
		dial_cnt_old = 0;
	}
	if (btn_pressed[2] == true) {
		mode = BASE;
		btn_pressed[2] = false;
		deActivate();
		dial_cnt = 0;
		dial_cnt_old = 0;
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
		} else if (activated == STATE_HEADER) {
			channel[0]->setHide();
			channel[1]->setHide();
			mode = SETTING;
			tft.fillRect(0, 35, 480, 285, TFT_BLACK);
			header->activate();
			setting->init(10, 100);
			activated = dial_cnt = dial_cnt_old = STATE_SETTING;
			return;
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
		dial_cnt = 0;
		dial_cnt_old = 0;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
	}
	if (btn_pressed[2] == true) {
		mode = BASE_MOVE;
		dial_cnt = 0;
		dial_cnt_old = 0;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		btn_pressed[2] = false;
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
	Serial.printf("draw setting : dial_cnt : %d, dial_cnt_old : %d\n\r", dial_cnt, dial_cnt_old);
	Serial.println(activated);
	activate_setting();
	if ((cur_time - dial_time) > 10000) {
		dial_cnt = 0;
		dial_cnt_old = 0;
		deActivateSetting();
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		if (activated == STATE_NONE) {
			channel[0]->clearHide();
			channel[1]->clearHide();
			initScreen();
			mode = BASE_MOVE;
			header->activate();
			activated = dial_cnt = 0;
		} else {
			deActivateSetting();
			activated = dial_cnt =  STATE_NONE;
		}
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (activated == STATE_SETTING) {
			channel[0]->clearHide();
			channel[1]->clearHide();
			initScreen();
			mode = BASE_MOVE;
			header->activate();
			activated = dial_cnt = 0;
		} else if (activated == STATE_BL) {
			mode = SETTING_BL;
			dial_cnt = setting->getBacklightLevel();
			setting->activateBLLevel(TFT_GREEN);
		} else if (activated == STATE_FAN) {
			mode = SETTING_FAN;
			dial_cnt = setting->getFanLevel();
			setting->activateFanLevel(TFT_GREEN);
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
	if (btn_pressed[2] == true) {
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
		setSysParam("backlight_level", String(setting->setBacklightLevel()));
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

void Screen::drawSettingFAN()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING;
		deActivateSetting();
		activated = dial_cnt = dial_cnt_old = STATE_NONE;
		dial_cnt_old = STATE_NONE;
		setting->changeFan();
		setting->deActivateFanLevel();
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		mode = SETTING;
		activated = dial_cnt = dial_cnt_old = STATE_FAN;
		deActivateSetting();
		setting->changeFan();
		setting->activateFanLevel();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING;
		setSysParam("fan_level", String(setting->setFanLevel()));
		setting->activateFanLevel();
		activated = dial_cnt = dial_cnt_old = STATE_FAN;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		if (dial_cnt < 0)
			dial_cnt = 0;
		else if (dial_cnt > 6)
			dial_cnt = 6;
		dial_cnt_old = dial_cnt;
		setting->changeFan(dial_cnt);
	}
}

void Screen::drawScreen()
{
	switch (mode) {
	case BASE:
		header->drawMode("POWER");
		drawBase();
		break;
	case BASE_MOVE:
		header->drawMode("DIAL");
		drawBaseMove();
		break;
	case BASE_EDIT:
		header->drawMode("EDIT");
		drawBaseEdit();
		break;
	case SETTING:
		header->drawMode("SETTING");
		drawSetting();
		break;
	case SETTING_BL:
		header->drawMode("EDIT_BL");
		drawSettingBL();
		break;
	case SETTING_FAN:
		header->drawMode("EDIT_FAN");
		drawSettingFAN();
		break;
	}
	if (mode != SETTING) {
		if ((cur_time - fnd_time) > 300) {
			fnd_time = cur_time;
			channel[0]->drawChannel();
			channel[1]->drawChannel();
		}
		channel[0]->drawVoltSet();
		channel[1]->drawVoltSet();
	}
	header->draw();
}

void Screen::changeVolt(screen_mode_t mode)
{
	if (activated == STATE_VOLT0) {
		if (dial_cnt < -(volt_set/100 - 30))
			dial_cnt = -(volt_set/100 - 30);
		if (mode == BASE_MOVE) {
			channel[0]->setVolt(dial_cnt);
			Serial.println("set Volt!");
			if (dial_cnt != 0) {
				setSysParam("voltage0", channel[0]->getVolt()/1000.0);
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
				setSysParam("current_limit0", channel[0]->getCurrentLimit()/10.0);
			}
		} else {
			channel[0]->editCurrentLimit(dial_cnt);
		}

	} else if (activated == STATE_VOLT1) {
		if (dial_cnt < -(volt_set/100 - 30))
			dial_cnt = -(volt_set/100 - 30);
		if (mode == BASE_MOVE) {
			channel[1]->setVolt(dial_cnt);
			if (dial_cnt != 0) {
				setSysParam("voltage1", channel[1]->getVolt()/1000.0);
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
				setSysParam("current_limit1", channel[1]->getCurrentLimit()/10.0);
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
		//channel[i]->checkInterrupt();
	}
}

void Screen::countDial(int8_t dial_cnt, bool direct, uint32_t milisec)
{
	this->dial_cnt += dial_cnt;
	this->dial_time = milisec;
	this->dial_direct = direct;
	//Serial.printf("%d, %d\n\r", this->dial_cnt, this->dial_time);
}

void Screen::setTime(uint32_t milisec)
{
	this->cur_time = milisec;
}

void Screen::clearBtnEvent(void)
{
	for (int i = 0; i < 4; i++)
		btn_pressed[i] = false;
}

void Screen::getBtnPress(uint8_t idx, uint32_t cur_time)
{
	//Serial.printf("button pressed %d\n\r", idx);
	btn_pressed[idx] = true;
	switch (idx) {
	case 0: /* Channel0 ON/OFF */
	case 1: /* Channel1 ON/OFF */
		if (mode > BASE_EDIT)
			break;
		if (onoff[idx] == 1)
			onoff[idx] = 2;
		else if (onoff[idx] == 0)
			onoff[idx] = 3;
		break;
	case 2:  /* MENU/CANCEL */
		dial_time = cur_time;
		break;
	case 3: /* Set value */
		dial_time = cur_time;
		break;
	}
}

void Screen::readFile(const char * path){
    Serial.printf("Reading file: %s\r\n", path);

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
		//Serial.printf("%c : %d\n\r", file.read(), file.position());
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

void Screen::fsInit(void)
{
	float volt_set0, volt_set1;
	float current_limit0, current_limit1;
	uint8_t backlight_level = 0;
	uint8_t fan_level = 0;

	if (!SPIFFS.begin(false)) {
		Serial.println("SPIFFS mount error");
		return;
	}

	fs = &SPIFFS;


	if (isFirstBoot()) {
		Serial.println("First boot!!!");
		File f = fs->open("/setting.txt", "w");
		f.print("autorun=0\n\r");
		f.print("voltage0=05.0\n\r");
		f.print("voltage1=05.0\n\r");
		f.print("current_limit0=03.0\n\r");
		f.print("current_limit1=03.0\n\r");
		f.print("blacklight_level=3\n\r");
		f.print("fan_level=0\n\r");
		f.print("firstboot=0");
		f.flush();
		current_limit0 = 3.0;
		current_limit1 = 3.0;
		volt_set0 = 5.0;
		volt_set1 = 5.0;
		backlight_level = 3;
		fan_level = 0;

	} else {
		File f = fs->open("/setting.txt", "r");
		f.seek(0, SeekSet);
		f.findUntil("voltage0", "\n\r");
		f.seek(1, SeekCur);
		volt_set0 = f.readStringUntil('\n').toFloat()*1000;
		DUMP(volt_set0);
		f.findUntil("voltage1", "\n\r");
		f.seek(1, SeekCur);
		volt_set1 = f.readStringUntil('\n').toFloat()*1000;
		DUMP(volt_set1);
		f.findUntil("current_limit0", "\n\r");
		f.seek(1, SeekCur);
		current_limit0 = f.readStringUntil('\n').toFloat()*1000;
		f.findUntil("current_limit1", "\n\r");
		f.seek(1, SeekCur);
		current_limit1 = f.readStringUntil('\n').toFloat()*1000;
		f.findUntil("backlight_level", "\n\r");
		f.seek(1, SeekCur);
		backlight_level = f.readStringUntil('\n').toInt();
		f.findUntil("fan_level", "\n\r");
		f.seek(1, SeekCur);
		fan_level = f.readStringUntil('\n').toInt();
		f.close();
	}
	channel[0]->setVolt(volt_set0, 1);
	channel[0]->setCurrentLimit(current_limit0, 1);
	channel[1]->setVolt(volt_set1, 1);
	channel[1]->setCurrentLimit(current_limit1, 1);
	setting->setBacklightLevel(backlight_level);
	setting->setFanLevel(fan_level);
	readFile("/setting.txt");
}

bool Screen::isFirstBoot()
{
	File f = fs->open("/setting.txt", "r");
	f.seek(0, SeekSet);
	f.findUntil("firstboot", "\n\r");
	f.seek(1, SeekCur);
	int value = f.readStringUntil('\n').toInt();
	Serial.println(value);
	f.close();

	if (value)
		return true;
	else
		return false;
}

void Screen::setSysParam(char *key, float value)
{
	char str[5];
	sprintf(str, "%04.1f", value);
	File f = fs->open("/setting.txt", "r+");
	f.seek(0, SeekSet);
	f.findUntil(key, "\n\r");
	f.seek(1, SeekCur);
	f.print(str);
	f.flush();
	f.close();
}

void Screen::setSysParam(char *key, String value)
{
	File f = fs->open("/setting.txt", "r+");
	Serial.printf("size of file %d, value %s\n\r", f.size(), value);
	f.seek(0, SeekSet);
	f.findUntil(key, "\n\r");
	f.seek(1, SeekCur);
	f.print(value);
	f.flush();
	f.close();
}

void Screen::debug()
{
	header->setDebug();
}
