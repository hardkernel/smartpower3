#include "helpers.h"
#include "screens/settingscreen.h"

SettingScreen::SettingScreen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t *onoff) : Screen(tft, header, settings, wifi_manager, onoff)
{
	/*
	popup = new TFT_eSprite(tft);
	popup->createSprite(100, 100);
	*/

	com_serial_baud = new Component(tft, 115, 20, 2);
	com_log_interval = new Component(tft, 85, 20, 2);
	com_ssid = new Component(tft, 284, 22, 2);
	//com_ipaddr = new Component(tft, 220, 20, 2);
	com_udp_ipaddr = new Component(tft, 170, 20, 2);
	com_udp_port = new Component(tft, 60, 20, 2);

	this->x = SETTING_SCREEN_INIT_POSITION_X;
	this->y = SETTING_SCREEN_INIT_POSITION_Y;
}

void SettingScreen::init()
{
	for (int i = 0; i < 2; i++) {
		tft->drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
	}

	tft->fillRect(0, 52, 480, 285, BG_COLOR);
	tft->loadFont(getFont("NotoSansBold20"));
	tft->drawString("Build date : ", x + 140, y + 215, 2);
	tft->drawString(String(__DATE__), x + 260, y + 215, 2);
	tft->drawString(String(__TIME__), x + 380, y + 215, 2);
	tft->unloadFont();

	tft->loadFont(getFont("ChewyRegular32"));
	tft->drawString("Backlight Level", x, y, 4);
	tft->drawString("Logging", x, y + Y_LOGGING, 4);
	tft->drawString("Serial", x + 120, y + Y_LOGGING, 4);
	tft->drawString("WiFi Info", x, y + Y_WIFI_INFO + 10, 4);
	tft->unloadFont();

	tft->fillRect(x + X_BL_LEVEL, y, 135, 26, BG_COLOR);
	tft->drawRect(x + X_BL_LEVEL, y-1, 135, 28, TFT_YELLOW);
	changeBacklight(backlight_level_preset);

	com_serial_baud->init(TFT_WHITE, BG_COLOR, 1, MC_DATUM);
	com_serial_baud->setCoordinate(x + X_BAUD_RATE-10, y + 30 + Y_SERIAL);
	com_log_interval->init(TFT_WHITE, BG_COLOR, 1, MC_DATUM);
	com_log_interval->setCoordinate(x + X_BAUD_RATE + 130, y + 30 + Y_SERIAL);

	com_ssid->init(TFT_WHITE, BG_COLOR, 1, MC_DATUM);
	com_ssid->setCoordinate(x + X_SSID, y + Y_WIFI_INFO);

	/*
	com_ipaddr->init(TFT_WHITE, BG_COLOR, 1, MC_DATUM);
	com_ipaddr->setCoordinate(x + X_IPADDR, y + Y_WIFI_INFO + 30);
	*/
	com_udp_ipaddr->init(TFT_WHITE, BG_COLOR, 1, MC_DATUM);
	com_udp_ipaddr->setCoordinate(x + X_IPADDR + 10, y + Y_WIFI_INFO + 30);
	com_udp_port->init(TFT_WHITE, BG_COLOR, 1, MC_DATUM);
	com_udp_port->setCoordinate(x + X_IPADDR + 175, y + Y_WIFI_INFO + 30);

	tft->loadFont(getFont("ChewyRegular24"));
	tft->drawString("Baud Rate  /", x + X_BAUD_RATE, y + Y_SERIAL);
	tft->drawString("/", x + X_BAUD_RATE + 110, y + Y_SERIAL + 30);
	tft->drawString(" Interval", x + X_LOG_LEVEL+100, y + Y_SERIAL);
	tft->drawString("UDP : ", x + 180, y + Y_WIFI_INFO + 28, 4);
	tft->unloadFont();

	drawSerialBaud(this->serial_baud);
	drawLogIntervalValue(log_value[log_interval]);

	is_initialized = true;
}

bool SettingScreen::draw(void)
{
	show_next_screen = false;

	if (!this->isInitialized()) {
		this->init();
	}

	switch (mode) {
		case SETTING_SETTING:
			drawSetting();
			break;
		case SETTING_SETTING_BL:
			drawSettingBL();
			break;
		case SETTING_SETTING_LOG:
			drawSettingLOG();
			break;
	}

	if (updated_wifi_info || wifi_manager->update_wifi_info) {
		updated_wifi_info = wifi_manager->update_wifi_info = false;
		this->selectAndDrawSSIDText();
	}
	if (wifi_manager->update_udp_info) {
		wifi_manager->update_udp_info = false;
		this->drawUDPIpaddrAndPort();
	}

	header->draw();
	return show_next_screen;
}

void SettingScreen::drawSetting()
{
	select();
	if ((cur_time - dial_time) > 10000) {
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
	}
	if (btn_pressed[2] == true) {
		btn_pressed[2] = false;
		if (selected == STATE_NONE) {
			show_next_screen = true;
		} else {
			deSelect();
			show_next_screen = false;
		}
		selected = dial_cnt = STATE_NONE;
	}
	if (btn_pressed[1] == true) {
		btn_pressed[1] = false;
		this->debug();
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == STATE_BL) {
			mode = SETTING_SETTING_BL;
			dial_cnt = this->getBacklightLevel();
			this->selectBLLevel(TFT_GREEN);
		} else if (selected == STATE_LOG) {
			mode = SETTING_SETTING_LOG;
			dial_cnt = this->getSerialBaudIndex();
			this->selectSerialLogging(TFT_GREEN);
			this->selectSerialBaud(TFT_YELLOW);
		} else if (selected == STATE_SETTING_LOGGING) {
			settings->switchLogging();
		} else if (selected == STATE_SETTING_WIFI) {
			settings->switchWifi();
		}
	}
	dial_cnt_old = dial_cnt;
}

void SettingScreen::drawSettingLOG()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING_SETTING;
		deSelect();
		selected = dial_cnt = dial_cnt_old = dial_cnt_old = STATE_NONE;
		this->deSelectSerialBaud(TFT_WHITE);
		this->restoreSerialBaud();
		this->deSelectLogInterval(TFT_WHITE);
		this->restoreLogIntervalValue();
		this->deSelectSerialLogging(TFT_YELLOW);
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING_SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_LOG;
		this->deSelectSerialBaud(TFT_WHITE);
		this->restoreSerialBaud();
		this->deSelectLogInterval(TFT_WHITE);
		this->restoreLogIntervalValue();
		this->deSelectSerialLogging(TFT_YELLOW);
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		if (selected == 5) {
			mode = SETTING_SETTING;
			this->setSerialBaud();
			this->setLogInterval();
			selected = dial_cnt = dial_cnt_old = STATE_LOG;
			this->deSelectLogInterval(TFT_WHITE);
			this->deSelectSerialLogging(TFT_YELLOW);
		} else {
			this->deSelectSerialBaud(TFT_WHITE);
			this->selectLogInterval();
			dial_cnt = dial_cnt_old = this->getLogInterval();
			selected = 5;
		}
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		if (selected == 5) {
			clampVariableToRange(0, 6, &dial_cnt);
			this->changeLogInterval(dial_cnt);
		} else {
			clampVariableToRange(0, 9, &dial_cnt);
			this->changeSerialBaud(dial_cnt);
		}
		dial_cnt_old = dial_cnt;
	}
}

void SettingScreen::drawSettingBL()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING_SETTING;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		this->changeBacklight();
		this->deSelectBLLevel();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING_SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_BL;
		deSelect();
		this->changeBacklight();
		this->selectBLLevel();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING_SETTING;
		settings->setBacklightLevelIndex(this->setBacklightLevelPreset());
		this->selectBLLevel();
		selected = dial_cnt = STATE_BL;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		clampVariableToRange(0, 6, &dial_cnt);
		dial_cnt_old = dial_cnt;
		this->changeBacklight(dial_cnt);
	}
}

void SettingScreen::select()
{
	if (dial_cnt == dial_cnt_old) {
		return;
	}
	dial_cnt_old = dial_cnt;
	// 4 is 1 based count of screen_state_setting elements, lower limit is 1 because 0 is used
	// as a marker for non-selected item that should not be included in the "selection circle"
	clampVariableToCircularRange(1, 4, dial_direct, &dial_cnt);

	deSelect();
	selected = dial_cnt;
	switch (dial_cnt) {
		case STATE_BL:
			this->selectBLLevel();
			break;
		case STATE_LOG:
			this->selectSerialLogging();
			break;
		case STATE_SETTING_WIFI:
			header->select(WIFI);
			break;
		case STATE_SETTING_LOGGING:
			header->select(LOGGING);
			break;
		default:
			break;
	}
}

void SettingScreen::deSelect()
{
	this->deSelectBLLevel();
	this->deSelectSerialLogging();
	header->deSelect(LOGGING);
	header->deSelect(WIFI);
}

/*void SettingScreen::popUp(void)
{
	popup->fillSprite(TFT_DARKGREY);
	popup->setTextDatum(MC_DATUM);
	popup->drawString("Sprite", 200, 200, 4);
	popup->pushSprite(200, 200);
}*/

uint8_t SettingScreen::_setBacklightLevelPreset(uint8_t level_preset)
{
	clampVariableToRange(0, 6, &level_preset);
	this->setBacklightLevel(bl_value_preset[level_preset]);
	return level_preset;
}

uint8_t SettingScreen::setBacklightLevelPreset(void)
{
	backlight_level_preset = backlight_level_edit;
	return _setBacklightLevelPreset(backlight_level_preset);
}

void SettingScreen::setBacklightLevelPreset(uint8_t level_preset, bool edit)
{
	backlight_level_preset = _setBacklightLevelPreset(level_preset);
	if (edit) {
		backlight_level_edit = backlight_level_preset;
	}
}

void SettingScreen::setBacklightLevel(uint8_t level)
{
	clampVariableToRange(0, 255, &level);
	ledcWrite(2, level);
}

void SettingScreen::turnOffBacklight(void)
{
	this->setBacklightLevel(0);
}

void SettingScreen::setLogInterval(uint8_t val)
{
	settings->setLogInterval(val);
	log_interval = val;
}

uint8_t SettingScreen::setLogInterval(void)
{
	settings->setLogInterval(log_interval_edit);
	return log_interval = log_interval_edit;
}

uint32_t SettingScreen::setSerialBaud()
{
	settings->setSerialBaudRate(this->serial_baud_edit);
	Serial.flush();
	Serial.begin(this->serial_baud_edit);
	return this->serial_baud = this->serial_baud_edit;
}

uint32_t SettingScreen::setSerialBaud(uint32_t baud)
{
	settings->setSerialBaudRate(baud);
	Serial.flush();
	Serial.end();
	Serial.begin(baud);
	return serial_baud = baud;
}

uint8_t SettingScreen::getBacklightLevel(void)
{
	return backlight_level_preset;
}

uint8_t SettingScreen::getLogInterval(void)
{
	return log_interval;
}

uint16_t SettingScreen::getLogIntervalValue(void)
{
	return log_value[log_interval];
}

uint32_t SettingScreen::getSerialBaud(void)
{
	return settings->getSerialBaudRate();
}

uint8_t SettingScreen::getSerialBaudIndex(void)
{
	serial_baud_edit = serial_baud;
	return settings->getSerialBaudRateIndex();
}

void SettingScreen::restoreBacklight()
{
	backlight_level_edit = backlight_level_preset;
}

void SettingScreen::changeBacklight(uint8_t level)
{
	if (level == 255) {
		level = backlight_level_preset;
	}
	drawBacklightLevel(level);
	backlight_level_edit = level;
	ledcWrite(2, bl_value_preset[level]);
}

void SettingScreen::restoreLogIntervalValue()
{
	drawLogIntervalValue(log_value[this->log_interval]);
	log_interval_edit = this->log_interval;
}

void SettingScreen::changeLogInterval(uint8_t log_interval)
{
	double ms = (1/static_cast<double>(this->serial_baud_edit/780))*1000;

	if (log_interval != 0) {
		clampVariableToRange(0, 6, &log_interval);
		while (log_value[log_interval] < ms) {
			log_interval++;
		}
	}
	drawLogIntervalValue(log_value[log_interval]);
	log_interval_edit = log_interval;
}

void SettingScreen::restoreSerialBaud()
{
	drawSerialBaud(this->serial_baud);
	serial_baud_edit = this->serial_baud;
}

void SettingScreen::changeSerialBaud(uint8_t baud_level)
{
	drawSerialBaud(serial_value[baud_level]);
	serial_baud_edit = serial_value[baud_level];
	changeLogInterval(log_interval);
}

void SettingScreen::selectBLLevel(uint16_t color)
{
	for (int i = 1; i < SELECTION_BORDER_WIDTH+1; i++) {
		tft->drawRect(x + X_BL_LEVEL-i, y-1-i, 135+i*2, 28+i*2, color);
	}
}

void SettingScreen::selectSerialLogging(uint16_t color)
{
	for (int i = 1; i < SELECTION_BORDER_WIDTH+1; i++) {
		tft->drawRect(x + X_LOG_LEVEL -i-50, y-1-i + Y_SERIAL -10, 135+i*2+120, 28+i*2+40, color);
	}
}

void SettingScreen::selectLogInterval(uint16_t color)
{
	com_log_interval->setTextColor(color, BG_COLOR);
	com_log_interval->select();
}

void SettingScreen::deSelectBLLevel(uint16_t color)
{
	for (int i = 1; i < SELECTION_BORDER_WIDTH+1; i++) {
		tft->drawRect(x + X_BL_LEVEL -i, y-1-i, 135+i*2, 28+i*2, color);
	}
}

void SettingScreen::deSelectLogInterval(uint16_t color)
{
	com_log_interval->setTextColor(color, BG_COLOR);
	drawLogIntervalValue(log_value[log_interval]);
	com_log_interval->deSelect();
}

void SettingScreen::deSelectSerialLogging(uint16_t color)
{
	for (int i = 1; i < SELECTION_BORDER_WIDTH+1; i++) {
		tft->drawRect(x + X_LOG_LEVEL -i-50, y-1-i + Y_SERIAL -10, 135+i*2+120, 28+i*2+40, color);
	}
}

void SettingScreen::drawBacklightLevel(uint8_t level)
{
	tft->fillRect(x + X_BL_LEVEL + 2, y+1, 130, 24, BG_COLOR);
	for (int i = 0; i < level; i++) {
		tft->fillRect(x + X_BL_LEVEL + 2 + (i*22), y + 1, 20, 24, TFT_YELLOW);
	}
}

void SettingScreen::selectSerialBaud(uint16_t color)
{
	com_serial_baud->setTextColor(color, BG_COLOR);
	drawSerialBaud(this->serial_baud);
	com_serial_baud->select();
}

void SettingScreen::deSelectSerialBaud(uint16_t color)
{
	com_serial_baud->setTextColor(color, BG_COLOR);
	drawSerialBaud(this->serial_baud_edit);
	com_serial_baud->deSelect();
}

void SettingScreen::drawLogIntervalValue(uint16_t log_value)
{
	com_log_interval->clearAndDrawWithFont(getFont("NotoSansBold20"), (log_value == 0) ? "OFF" : String(log_value) + " ms");
}

void SettingScreen::drawSerialBaud(uint32_t baud)
{
	com_serial_baud->clearAndDrawWithFont(getFont("NotoSansBold20"), String(baud) + " bps");
}

void SettingScreen::drawSSID(String ssid)
{
	com_ssid->clearAndDrawWithFont(getFont("NotoSansBold20"), ssid);
}

/*void SettingScreen::drawIpaddr(String ipaddr)
{
	com_ipaddr->clearAndDrawWithFont(getFont("NotoSansBold20"), ipaddr);
}*/

void SettingScreen::drawUDPIpaddr(String ipaddr)
{
	com_udp_ipaddr->clearAndDrawWithFont(getFont("NotoSansBold20"), ipaddr);
}

void SettingScreen::drawUDPport(uint16_t port)
{
	com_udp_port->clearAndDrawWithFont(getFont("NotoSansBold20"), (" : " + String(port)));
}

void SettingScreen::debug()
{
	com_serial_baud->clear();
}

uint16_t SettingScreen::getEnabledLogInterval(void)
{
	uint16_t tmp = this->getLogIntervalValue();

	if (tmp > 0 and settings->isLoggingEnabled()) {
		header->onLogging();
		return tmp;
	} else if (tmp > 0) {
		header->possibleLogging();
	} else {
		header->offLogging();
	}
	return 0;
}

void SettingScreen::onShutdown(void)
{
	this->shutdown = true;
	this->turnOffBacklight();
	this->log_interval = 0;
	this->is_initialized = false;
}

void SettingScreen::onWakeup(void)
{
	this->shutdown = false;
	this->log_interval = settings->getLogInterval();
	selected = dial_cnt = dial_cnt_old = STATE_NONE;
}

void SettingScreen::onEnter(void)
{
	wifi_manager->update_wifi_info = wifi_manager->update_udp_info = true;
}

void SettingScreen::selectAndDrawSSIDText(void)
{
	if (WiFi.status() == WL_CONNECTED) {
		this->drawSSID(wifi_manager->apInfoConnected());
	} else if (wifi_manager->hasSavedConnectionInfo()) {
		this->drawSSID(wifi_manager->apInfoSaved());
	} else {
		this->drawSSID("WiFi not saved");
	}
}

void SettingScreen::drawUDPIpaddrAndPort(void)
{
	this->drawUDPIpaddr(wifi_manager->ipaddr_udp.toString());
	this->drawUDPport(wifi_manager->port_udp);
}

screen_t SettingScreen::getType(void)
{
	return SETTING_SCREEN;
}
