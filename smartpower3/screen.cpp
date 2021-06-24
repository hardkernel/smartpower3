#include "screen.h"

bool Screen::_int = false;

Screen::Screen()
{
	tft.init();
	tft.setRotation(3);
	tft.fillScreen(TFT_BLACK);
	tft.fillScreen(TFT_BLACK);
	tft.setSwapBytes(true);
	pinMode(TFT_BL, OUTPUT);
	digitalWrite(TFT_BL, HIGH);
	for (int i = 0; i < 3; i++) {
		tft.drawLine(0, 33 + i, 480, 33 + i, TFT_WHITE);
		tft.drawLine(0, 270 + i, 480, 270 + i, TFT_WHITE);
	}
	for (int i = 0; i < 4; i++)
		tft.drawLine(238 + i, 35, 238 + i, 320, TFT_WHITE);
}

void Screen::readFile(const char * path){
    Serial.printf("Reading file: %s\r\n", path);

    File file = fs->open(path);
    if(!file || file.isDirectory()){
        Serial.println("- failed to open file for reading");
        return;
    }

    Serial.println("- read from file:");
    while(file.available()){
        Serial.write(file.read());
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

void Screen::begin(TwoWire *theWire)
{
	_wire = theWire;
	header = new Header(&tft);
	header->init(0, 0);

	pinMode(25, INPUT_PULLUP);
	digitalWrite(25, HIGH);

	channel[0] = new Channel(&tft, _wire, 0);
	channel[0]->initScreen(10, 45);
	channel[1] = new Channel(&tft, _wire, 1);
	channel[1]->initScreen(260, 45);

	SPIFFS.begin(false);
	fs = &SPIFFS;
	File f = fs->open("/setting.txt");
	if (!f || f.isDirectory()) {
		Serial.println("- failed to open file for reading");
		return;
	}

	f.seek(0, SeekSet);
	f.findUntil("voltage0", "\n\r");
	f.seek(1, SeekCur);
	channel[0]->setVolt(f.readStringUntil('\n').toFloat()*1000, 1);
	f.findUntil("voltage1", "\n\r");
	f.seek(1, SeekCur);
	channel[1]->setVolt(f.readStringUntil('\n').toFloat()*1000, 1);
	//channel[0]->setCurrentLimit(dial_cnt);
	f.close();
	//channel[0]->drawChannel();
}

void Screen::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt, uint8_t ch)
{
	if (mode == BASE_EDIT) {
		if (onoff[ch] == 1) {
			channel[ch]->pushPowerEdit(volt, ampere, watt);
		} else {
			channel[ch]->pushPowerEdit(0, 0, 0);
		}
	} else {
		if (onoff[ch] == 1) {
			channel[ch]->pushPower(volt, ampere, watt);
		} else {
			channel[ch]->pushPower(0, 0, 0);
		}
	}
}

void Screen::pushInputPower(uint16_t volt, uint16_t ampere, uint16_t watt)
{
	if (volt < 6000 && !low_input) {
		low_input = true;
		header->setLowInput(true);
		state_power = 1;
	} else if (volt >= 6000 && low_input) {
		low_input = false;
		header->setLowInput(false);
		state_power = 3;
	}

	if (header->getInputVoltage() != volt)
		header->pushPower(volt, ampere, watt);
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
		btn_pressed[3] = false;
	}
}

void Screen::drawBaseEdit()
{
	if ((cur_time - dial_time) > 10000) {
		mode = BASE;
		deActivate();
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
	}
	if (btn_pressed[2] == true) {
		mode = BASE_MOVE;
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		btn_pressed[2] = false;
	}
	if (btn_pressed[3] == true) {
		mode = BASE_MOVE;
		btn_pressed[3] = false;

		///
		///
		if (activated == STATE_VOLT0) {
			if (dial_cnt < -(volt_set/100 - 30))
				dial_cnt = -(volt_set/100 - 30);
			channel[0]->setVolt(dial_cnt);
		} else if (activated == STATE_CURRENT0) {
			if (dial_cnt > (30 - current_limit))
				dial_cnt = (30 - current_limit);
			else if (dial_cnt < -(current_limit - 5))
				dial_cnt = -(current_limit - 5);
			channel[0]->setCurrentLimit(dial_cnt);
		} else if (activated == STATE_VOLT1) {
			if (dial_cnt < -(volt_set/100 - 30))
				dial_cnt = -(volt_set/100 - 30);
			channel[1]->setVolt(dial_cnt);
		} else if (activated == STATE_CURRENT1) {
			if (dial_cnt > (30 - current_limit))
				dial_cnt = (30 - current_limit);
			else if (dial_cnt < -(current_limit - 5))
				dial_cnt = -(current_limit - 5);
			channel[1]->setCurrentLimit(dial_cnt);
		}
		channel[0]->clearCompColor();
		channel[1]->clearCompColor();
		dial_cnt = dial_state;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		dial_cnt_old = dial_cnt;
		///
		///
		if (activated == STATE_VOLT0) {
			if (dial_cnt < -(volt_set/100 - 30))
				dial_cnt = -(volt_set/100 - 30);
			channel[0]->editVolt(dial_cnt);
		} else if (activated == STATE_CURRENT0) {
			if (dial_cnt > (30 - current_limit))
				dial_cnt = (30 - current_limit);
			else if (dial_cnt < -(current_limit - 5))
				dial_cnt = -(current_limit - 5);
			channel[0]->editCurrentLimit(dial_cnt);
		} else if (activated == STATE_VOLT1) {
			if (dial_cnt < -(volt_set/100 - 30))
				dial_cnt = -(volt_set/100 - 30);
			channel[1]->editVolt(dial_cnt);
		} else if (activated == STATE_CURRENT1) {
			if (dial_cnt > (30 - current_limit))
				dial_cnt = (30 - current_limit);
			else if (dial_cnt < -(current_limit - 5))
				dial_cnt = -(current_limit - 5);
			channel[1]->editCurrentLimit(dial_cnt);
		}
	}
}

void Screen::drawScreen()
{
	switch (mode) {
	case BASE:
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
		break;
	}
	channel[0]->drawChannel();
	channel[1]->drawChannel();
	header->draw();
	/*
	if (btn_pressed[2]) {
		btn_pressed[2] = false;
		tft.fillRoundRect(150, 50, 200, 50, 10, TFT_WHITE);
		for (int i = 0; i < 3; i++) {
			tft.drawRoundRect(150-i, 50-i, 200 +(i*2), 50 + (i*2), 10, TFT_RED);
		}
		tft.drawString("Change the voltage set", 160, 80, 4);
		sleep(3);
		tft.fillRect(150, 50, 200, 50, 10, TFT_BLACK);
		channel[0]->init(10, 45);
		channel[1]->init(260, 45);
	}
	*/
}

void Screen::checkOnOff()
{
	if (state_power == 1) {
		state_power = 2;
		digitalWrite(27, LOW);
		digitalWrite(14, LOW);
		channel[0]->off();
		channel[1]->off();
		onoff[0] = 2;
		onoff[1] = 2;
	} else if (state_power == 3) {
		state_power = 4;
		digitalWrite(27, HIGH);
		digitalWrite(14, HIGH);
		channel[0]->initPower();
		channel[1]->initPower();
	} else if (state_power == 4) {
		Serial.println("check interrupt");
		state_power = 5;
	} else if (state_power == 5) {
		Serial.println("check autostart");
		state_power = 6;
	} else if (state_power == 6) {
		for (int i = 0; i < 2; i++) {
			if (onoff[i] == 3) {
				channel[i]->on();
				onoff[i] = 1;
				Serial.printf("Channel %d on\n\r", i);
			} else if (onoff[i] == 2) {
				channel[i]->off();
				onoff[i] = 0;
				Serial.printf("Channel %d off\n\r", i);
			}
		}

	}
	

	/*
	for (int i = 0; i < 2; i++) {
		if (header->getLowInput() and onoff[i] == 1) {
			channel[i]->off();
			onoff[i] = 0;
		} else if (onoff[i] == 3) {
			if (header->getLowInput()) {
				onoff[i] = 0;
				continue;
			}
			channel[i]->on();
			channel[i]->checkInterrupt();
			onoff[i] = 1;
		} else if (onoff[i] == 2) {
			channel[i]->off();
			onoff[i] = 0;
		}
	}
	*/
	//Serial.printf("onoff0 : %d, onoff1 : %d\n\r", onoff[0], onoff[1]);
	if (state_power != old_state_power) {
		old_state_power = state_power;
		Serial.printf("[ power state ] : %d\n\r", state_power);
	}
}

void Screen::run()
{
	checkOnOff();
	drawScreen();
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

void Screen::activate()
{
	if (dial_cnt == dial_cnt_old)
		return;
	dial_cnt_old = dial_cnt;
	if (dial_cnt > 4)
		dial_cnt = 0;
	else if (dial_cnt < 0)
		dial_cnt = 4;
	Serial.printf("dial_cnt : %d\n", dial_cnt);

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

void Screen::countDial(int8_t dial_cnt, bool direct, uint32_t milisec)
{
	this->dial_cnt += dial_cnt;
	this->dial_time = milisec;
	this->dial_direct = direct;
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
	Serial.printf("button pressed %d\n\r", idx);
	btn_pressed[idx] = true;
	switch (idx) {
	case 0: /* Channel0 ON/OFF */
	case 1: /* Channel1 ON/OFF */
		if (onoff[idx] == 1)
			onoff[idx] = 2;
		else if (onoff[idx] == 0)
			onoff[idx] = 3;
		Serial.printf("onoff0 : %d, onoff1 : %d\n\r", onoff[0], onoff[1]);
		break;
	case 2:  /* MENU/CANCEL */
		dial_time = cur_time;
		break;
	case 3: /* Set value */
		dial_time = cur_time;
		break;
	}
}
