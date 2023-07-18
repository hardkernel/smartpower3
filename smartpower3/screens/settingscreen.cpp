#include "helpers.h"
#include "screens/settingscreen.h"

SettingScreen::SettingScreen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t *onoff) : Screen(tft, header, settings, wifi_manager, onoff)
{
	/*
	popup = new TFT_eSprite(tft);
	popup->createSprite(100, 100);
	*/

	com_serial_baud = new Component(tft, BL_LEVEL_WIDTH, 24, 2);
	com_log_interval = new Component(tft, BL_LEVEL_WIDTH, 24, 2);
	com_ssid = new Component(tft, 284, 22, 2);
	com_udp_ipaddr = new Component(tft, 170, 20, 2);
	com_udp_port = new Component(tft, 60, 20, 2);
	com_mode = new Component(tft, BL_LEVEL_WIDTH, 20, 2);

	this->x = SETTING_SCREEN_INIT_POSITION_X;
	this->y = SETTING_SCREEN_INIT_POSITION_Y;
}

void SettingScreen::init()
{
	for (int i = 0; i < 2; i++) {
		tft->drawLine(0, 50 + i, 480, 50 + i, TFT_DARKGREY);
	}

	tft->fillRect(0, 52, 480, 285, BG_COLOR);
	tft->loadFont(getFont(NOTOSANSBOLD20));

	tft->drawString("Backlight Level", x, y, 4);
	tft->drawString("Serial Baud Rate", x, y + LINE_SPACING, 4);
	tft->drawString("Logging Interval", x, y + 2*LINE_SPACING, 4);
	tft->drawString("WiFi AP", x, y + 3*LINE_SPACING, 4);
	tft->drawString("Logging target (UDP)", x, y + 4*LINE_SPACING, 4);
	tft->drawString("Mode", x, y + 5*LINE_SPACING, 4);
	tft->unloadFont();

	tft->fillRect(x + X_BL_LEVEL, y + Y_BL_LEVEL, BL_LEVEL_WIDTH, BL_LEVEL_HEIGHT, BG_COLOR);
	tft->drawRect(x + X_BL_LEVEL-1, y + Y_BL_LEVEL - 1, BL_LEVEL_WIDTH + 2, BL_LEVEL_HEIGHT + 2, TFT_GREEN);
	changeBacklight(backlight_level_preset);

	com_serial_baud->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_serial_baud->setCoordinate(x + X_BL_LEVEL, y + LINE_SPACING - 1);
	com_log_interval->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_log_interval->setCoordinate(x + X_BL_LEVEL, y + 2*LINE_SPACING - 1);

	com_ssid->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_ssid->setCoordinate(x + X_SSID, y + 3*LINE_SPACING);  //y + Y_WIFI_INFO);

	com_udp_ipaddr->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_udp_ipaddr->setCoordinate(x + X_IPADDR, y + 4*LINE_SPACING);  //y + Y_WIFI_INFO + 30);
	com_udp_port->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_udp_port->setCoordinate(x + X_IPADDR + 170, y + 4*LINE_SPACING);  //y + Y_WIFI_INFO + 30);

	com_mode->init(TFT_WHITE, BG_COLOR, 1, MR_DATUM);
	com_mode->setCoordinate(x + X_BL_LEVEL, y + 5*LINE_SPACING);

	this->serial_baud_edit = this->serial_baud;

	this->drawSerialBaud(this->serial_baud);
	this->drawLogIntervalValue(log_value[log_interval]);
	this->drawMode(this->operation_mode);

	selected = dial_cnt = dial_cnt_old = STATE_NONE;

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
		case SETTING_SETTING_BAUD_RATE:
			drawSettingBaudRate();
			break;
		case SETTING_SETTING_LOG_INTERVAL:
			drawSettingLogInterval();
			break;
		case SETTING_SETTING_OPERATION_MODE:
			drawOperationMode();
			break;
		default:
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
	if (wifi_manager->update_mode_info) {
		wifi_manager->update_mode_info = false;
		this->drawMode(settings->getOperationMode());
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
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
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
			this->selectBLLevel(COLOR_RECTANGLE_ACTIVATED);
		} else if (selected == STATE_BAUD_RATE) {
			mode = SETTING_SETTING_BAUD_RATE;
			dial_cnt = this->getSerialBaudIndex();
			this->selectSerialBaud(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_ACTIVATED);
		} else if (selected == STATE_LOG_INTERVAL) {
			mode = SETTING_SETTING_LOG_INTERVAL;
			dial_cnt = this->getLogInterval();
			this->selectLogInterval(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_ACTIVATED);
		} else if (selected == STATE_OPERATION_MODE) {
			mode = SETTING_SETTING_OPERATION_MODE;
			dial_cnt = this->getOperationMode();
			this->selectOperationMode(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_ACTIVATED);
		} else if (selected == STATE_SETTING_LOGGING_ICON) {
			settings->switchLogging();
		} else if (selected == STATE_SETTING_WIFI_ICON) {
			settings->switchWifi();
		}
	}
	dial_cnt_old = dial_cnt;
}

void SettingScreen::drawSettingLogInterval()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING_SETTING;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		this->deSelectLogInterval(TFT_WHITE);
		this->restoreLogIntervalValue();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING_SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_LOG_INTERVAL;
		this->deSelectLogInterval(TFT_WHITE);
		this->restoreLogIntervalValue();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING_SETTING;
		this->setLogInterval();
		this->selectLogInterval(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_SELECTED);
		selected = dial_cnt = dial_cnt_old = STATE_LOG_INTERVAL;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		clampVariableToRange(0, 6, &dial_cnt);
		this->changeLogInterval(dial_cnt);
		dial_cnt_old = dial_cnt;
	}
}

void SettingScreen::drawOperationMode()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING_SETTING;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		this->deSelectOperationMode(TFT_WHITE);
		this->restoreOperationMode();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING_SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_OPERATION_MODE;
		this->deSelectOperationMode(TFT_WHITE);
		this->restoreOperationMode();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING_SETTING;
		this->setOperationMode();
		this->selectOperationMode(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_SELECTED);
		selected = dial_cnt = dial_cnt_old = STATE_OPERATION_MODE;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		clampVariableToRange(0, 1, &dial_cnt);
		this->changeOperationMode(static_cast<device_operation_mode>(dial_cnt));
		dial_cnt_old = dial_cnt;
	}
}

void SettingScreen::drawSettingBaudRate()
{
	if ((cur_time - dial_time) > 10000) {
		mode = SETTING_SETTING;
		deSelect();
		selected = dial_cnt = dial_cnt_old = STATE_NONE;
		this->deSelectSerialBaud(COLOR_TEXT_DESELECTED);
		this->restoreSerialBaud();
		this->restoreLogIntervalValue();
	}
	if ((btn_pressed[2] == true) || (flag_long_press == 2)){
		flag_long_press = 3;
		btn_pressed[2] = false;
		mode = SETTING_SETTING;
		selected = dial_cnt = dial_cnt_old = STATE_BAUD_RATE;
		this->deSelectSerialBaud(COLOR_TEXT_DESELECTED);
		this->restoreSerialBaud();
		this->restoreLogIntervalValue();
		return;
	}
	if (btn_pressed[3] == true) {
		btn_pressed[3] = false;
		mode = SETTING_SETTING;
		this->setSerialBaud();
		this->setLogInterval();
		this->selectSerialBaud(COLOR_TEXT_SELECTED, COLOR_RECTANGLE_SELECTED);
		selected = dial_cnt = dial_cnt_old = STATE_BAUD_RATE;
		return;
	}
	if (dial_cnt != dial_cnt_old) {
		clampVariableToRange(0, 9, &dial_cnt);
		this->changeSerialBaud(dial_cnt);
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
		selected = dial_cnt = dial_cnt_old = STATE_BL;
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
	// 6 is 1 based count of screen_state_setting elements, lower limit is 1 because 0 is used
	// as a marker for non-selected item that should not be included in the "selection circle"
	clampVariableToCircularRange(1, 6, dial_direct, &dial_cnt);
	dial_cnt_old = dial_cnt;

	deSelect();
	selected = dial_cnt;
	switch (dial_cnt) {
		case STATE_SETTING_WIFI_ICON:
			header->select(WIFI);
			break;
		case STATE_SETTING_LOGGING_ICON:
			header->select(LOGGING);
			break;
		case STATE_OPERATION_MODE:
			this->selectOperationMode();
			break;
		case STATE_LOG_INTERVAL:
			this->selectLogInterval();
			break;
		case STATE_BAUD_RATE:
			this->selectSerialBaud();
			break;
		case STATE_BL:
			this->selectBLLevel();
			break;
		default:
			break;
	}
}

void SettingScreen::deSelect()
{
	this->deSelectBLLevel();
	this->deSelectSerialBaud();
	this->deSelectLogInterval();
	this->deSelectOperationMode();
	header->deSelect(LOGGING);
	header->deSelect(WIFI);
}

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

device_operation_mode SettingScreen::setOperationMode()
{
	settings->setOperationMode(operation_mode_edit);
	return this->operation_mode = this->operation_mode_edit;
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

uint16_t SettingScreen::getOperationMode(void)
{
	operation_mode_edit = operation_mode;
	return static_cast<uint16_t>(settings->getOperationMode());
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

void SettingScreen::changeLogInterval(uint8_t log_interval, bool color_changed_value)
{
	double ms = (1/static_cast<double>(this->serial_baud_edit/780))*1000;

	if (log_interval != 0) {
		clampVariableToRange(0, 6, &log_interval);
		while (log_value[log_interval] < ms) {
			log_interval++;
		}
	}
	if (log_interval != log_interval_edit) {
		if (log_interval != this->log_interval && color_changed_value) {
			com_log_interval->setTextColor(COLOR_TEXT_ERROR, BG_COLOR);
		}
		drawLogIntervalValue(log_value[log_interval]);
		com_log_interval->setTextColor(COLOR_TEXT_DESELECTED, BG_COLOR);
		log_interval_edit = log_interval;
	}
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
	changeLogInterval(log_interval, true);
}

void SettingScreen::restoreOperationMode()
{
	drawMode(settings->getOperationMode());
}

void SettingScreen::changeOperationMode(device_operation_mode operation_mode)
{
	drawMode(operation_mode);
	operation_mode_edit = operation_mode;
}

void SettingScreen::selectBLLevel(uint16_t rectangle_color)
{
	for (int i = 0; i < SELECTION_BORDER_WIDTH; i++) {
		tft->drawRect(x + X_BL_LEVEL-1-i, y+Y_BL_LEVEL-1-i, BL_LEVEL_WIDTH+2+i*2, BL_LEVEL_HEIGHT+2+i*2, rectangle_color);
	}
}

void SettingScreen::selectLogInterval(uint16_t text_color, uint16_t rectangle_color)
{
	com_log_interval->select(rectangle_color);
}

void SettingScreen::deSelectBLLevel(uint16_t rectangle_color)
{
	for (int i = 1; i < SELECTION_BORDER_WIDTH; i++) {
		tft->drawRect(x + X_BL_LEVEL-1-i, y+Y_BL_LEVEL-1-i, BL_LEVEL_WIDTH+2+i*2, BL_LEVEL_HEIGHT+2+i*2, rectangle_color);
	}
}

void SettingScreen::deSelectLogInterval(uint16_t text_color, uint16_t rectangle_color)
{
	com_log_interval->deSelect(rectangle_color);
}

void SettingScreen::drawBacklightLevel(uint8_t level)
{
	// clear inside space of level component
	tft->fillRect(x + X_BL_LEVEL, y + Y_BL_LEVEL, BL_LEVEL_WIDTH, BL_LEVEL_HEIGHT, BG_COLOR);
	for (int i = 0; i < level; i++) {
		// draw individual level blocks
		tft->fillRect(
			// x start position + left inside space + (level_number * ((total_width - left_inside_space)/all_level_count))
			x + X_BL_LEVEL + BL_LEVEL_SPACING + (i*((BL_LEVEL_WIDTH-BL_LEVEL_SPACING)/6)),
			// y start_position + top_inside_space
			y + Y_BL_LEVEL + BL_LEVEL_SPACING,
			// block_width = ((total_width-left_inside_space)/all_level_count) - right_spacing_of_level_block
			((BL_LEVEL_WIDTH-BL_LEVEL_SPACING)/6)-BL_LEVEL_SPACING,
			// block_height = total_height - (top_spacing + bottom_spacing)
			BL_LEVEL_HEIGHT-2*BL_LEVEL_SPACING,
			TFT_GREEN);
	}
}

void SettingScreen::selectSerialBaud(uint16_t text_color, uint16_t rectangle_color)
{
	com_serial_baud->select(rectangle_color);
}

void SettingScreen::deSelectSerialBaud(uint16_t text_color, uint16_t rectangle_color)
{
	com_serial_baud->deSelect(rectangle_color);
}

void SettingScreen::selectOperationMode(uint16_t text_color, uint16_t rectangle_color)
{
	com_mode->select(rectangle_color);
}

void SettingScreen::deSelectOperationMode(uint16_t text_color, uint16_t rectangle_color)
{
	com_mode->deSelect(rectangle_color);
}

void SettingScreen::drawLogIntervalValue(uint16_t log_value)
{
	if (log_value == 0) {
		com_log_interval->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), F("OFF"));
	} else {
		char output_log_value[9];
		snprintf(output_log_value, 9, F("%u ms"), log_value);
		com_log_interval->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), output_log_value);
	}
}

void SettingScreen::drawSerialBaud(uint32_t baud)
{
	char baud_buffer[11];
	snprintf(baud_buffer, 11, F("%u bps"), baud);
	com_serial_baud->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), baud_buffer);
}

void SettingScreen::drawSSID(const char* ssid)
{
	com_ssid->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), ssid);
}

void SettingScreen::drawUDPIpaddr(const char* ipaddr)
{
	com_udp_ipaddr->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), ipaddr);
}

void SettingScreen::drawUDPport(uint16_t port)
{
	char port_buffer[9];
	snprintf(port_buffer, 9, " : %u", port);
	com_udp_port->clearAndDrawWithFont(getFont(NOTOSANSBOLD20), port_buffer);
}

void SettingScreen::drawSerialLoggingEnabled(bool is_enabled)
{
	tft->drawRect(x + 155, y+2*LINE_SPACING, 20, 20, TFT_GREEN);
}

void SettingScreen::drawWifiLoggingEnabled(bool is_enabled)
{
	tft->drawRect(x + 245, y+2*LINE_SPACING, 20, 20, TFT_GREEN);
}

void SettingScreen::drawMode(device_operation_mode default_mode)
{
	com_mode->clearAndDrawWithFont(
			getFont(NOTOSANSBOLD20),
			(default_mode == OPERATION_MODE_DEFAULT) ? F("Default") : F("SCPI"));
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
		this->drawSSID(wifi_manager->apInfoConnected().c_str());
	} else if (wifi_manager->hasSavedConnectionInfo()) {
		this->drawSSID(wifi_manager->apInfoSaved().c_str());
	} else {
		this->drawSSID("WiFi not saved");
	}
}

void SettingScreen::drawUDPIpaddrAndPort(void)
{
	this->drawUDPIpaddr(wifi_manager->ipaddr_udp.toString().c_str());
	this->drawUDPport(wifi_manager->port_udp);
}

screen_t SettingScreen::getType(void)
{
	return SETTING_SCREEN;
}
