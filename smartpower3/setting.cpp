#include "setting.h"
#include "ChewyRegular24.h"
#include "ChewyRegular32.h"

Setting::Setting(TFT_eSPI *tft)
{
	this->tft = tft;

	ledcSetup(2, FREQ, RESOLUTION);
	ledcAttachPin(BL_LCD, 2);

	/*
	popup = new TFT_eSprite(tft);
	popup->createSprite(100, 100);
	*/

	com_serial_baud = new Component(tft, 115, 20, 2);
	com_log_interval = new Component(tft, 85, 20, 2);
	com_ssid = new Component(tft, 220, 20, 2);
	//com_ipaddr = new Component(tft, 220, 20, 2);
	com_udp_ipaddr = new Component(tft, 170, 20, 2);
	com_udp_port = new Component(tft, 60, 20, 2);
}


void Setting::init(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;

	tft->fillRect(0, 52, 480, 285, TFT_BLACK);
	tft->loadFont(NotoSansBold20);
	tft->drawString("Build date : ", x + 140, y + 215, 2);
	tft->drawString(String(__DATE__), x + 260, y + 215, 2);
	tft->drawString(String(__TIME__), x + 380, y + 215, 2);
	tft->unloadFont();

	tft->loadFont(ChewyRegular32);
	tft->drawString("Backlight Level", x, y, 4);

	tft->drawString("Logging", x, y + Y_LOGGING, 4);

	tft->drawString("Serial", x + 120, y + Y_LOGGING, 4);

	tft->drawString("WiFi Info", x, y + Y_WIFI_INFO + 10, 4);
	tft->unloadFont();

	tft->fillRect(x + X_BL_LEVEL, y, 135, 26, TFT_BLACK);
	tft->drawRect(x + X_BL_LEVEL, y-1, 135, 28, TFT_YELLOW);
	changeBacklight(backlight_level);

	com_serial_baud->init(TFT_WHITE, TFT_BLACK, 1, MC_DATUM);
	com_serial_baud->setCoordinate(x + X_BAUD_RATE-10, y + 30 + Y_SERIAL);
	com_log_interval->init(TFT_WHITE, TFT_BLACK, 1, MC_DATUM);
	com_log_interval->setCoordinate(x + X_BAUD_RATE + 130, y + 30 + Y_SERIAL);

	com_ssid->init(TFT_WHITE, TFT_BLACK, 1, MC_DATUM);
	com_ssid->setCoordinate(x + X_SSID, y + Y_WIFI_INFO);
	/*
	com_ipaddr->init(TFT_WHITE, TFT_BLACK, 1, MC_DATUM);
	com_ipaddr->setCoordinate(x + X_IPADDR, y + Y_WIFI_INFO + 30);
	*/
	com_udp_ipaddr->init(TFT_WHITE, TFT_BLACK, 1, MC_DATUM);
	com_udp_ipaddr->setCoordinate(x + X_IPADDR + 10, y + Y_WIFI_INFO + 30);
	com_udp_port->init(TFT_WHITE, TFT_BLACK, 1, MC_DATUM);
	com_udp_port->setCoordinate(x + X_IPADDR + 175, y + Y_WIFI_INFO + 30);

	tft->loadFont(ChewyRegular24);
	tft->drawString("Baud Rate  /", x + X_BAUD_RATE, y + Y_SERIAL);
	tft->drawString("/", x + X_BAUD_RATE + 110, y + Y_SERIAL + 30);
	tft->drawString(" Interval", x + X_LOG_LEVEL+100, y + Y_SERIAL);
	tft->drawString("UDP : ", x + 180, y + Y_WIFI_INFO + 28, 4);
	tft->unloadFont();

	drawSerialBaud(this->serial_baud);
	drawLogIntervalValue(log_value[log_interval]);
}

void Setting::popUp(void)
{
	popup->fillSprite(TFT_DARKGREY);
	popup->setTextDatum(MC_DATUM);
	popup->drawString("Sprite", 200, 200, 4);
	popup->pushSprite(200, 200);
}

uint8_t Setting::_setBacklightLevel(uint8_t level)
{
	if (level > 6)
		level = 6;
	else if (level < 0)
		level = 0;
	ledcWrite(2, bl_value[level]);
	return level;
}

uint8_t Setting::setBacklightLevel(void)
{
	backlight_level = backlight_level_edit;
	return _setBacklightLevel(backlight_level);
}

void Setting::setBacklightLevel(uint8_t level, bool edit)
{
	backlight_level = _setBacklightLevel(level);
	if (edit)
		backlight_level_edit = backlight_level;
}

void Setting::setBacklightLevel(uint8_t level)
{
	backlight_level = _setBacklightLevel(level);
}

void Setting::setLogInterval(uint8_t val)
{
	log_interval = val;
}

uint8_t Setting::setLogInterval(void)
{
	return log_interval = log_interval_edit;
}

uint32_t Setting::setSerialBaud()
{
	Serial.flush();
	Serial.begin(this->serial_baud_edit);
	return this->serial_baud = this->serial_baud_edit;
}

uint32_t Setting::setSerialBaud(uint32_t baud)
{
	Serial.flush();
	Serial.end();
	Serial.begin(baud);
	return serial_baud = baud;
}

uint8_t Setting::getBacklightLevel(void)
{
	return backlight_level;
}

uint8_t Setting::getLogInterval(void)
{
	return log_interval;
}

uint16_t Setting::getLogIntervalValue(void)
{
	return log_value[log_interval];
}

uint32_t Setting::getSerialBaud(void)
{
	return serial_baud;
}

uint8_t Setting::getSerialBaudLevel(void)
{
	for (int i = 0; i < 10; i++) {
		if (serial_value[i] >= serial_baud) {
			serial_baud_edit = serial_baud;
			return i;
		}
	}
	return 0;
}

void Setting::restoreBacklight()
{
	backlight_level_edit = backlight_level;
}

void Setting::changeBacklight(uint8_t level)
{
	if (level == 255) {
		level = backlight_level;
	}
	drawBacklightLevel(level);
	backlight_level_edit = level;
	ledcWrite(2, bl_value[level]);
}

void Setting::restoreLogIntervalValue()
{
	drawLogIntervalValue(log_value[this->log_interval]);
	log_interval_edit = this->log_interval;
}

void Setting::changeLogInterval(uint8_t log_interval)
{
	double tmp, ms;
	tmp = this->serial_baud_edit/780;
	ms = (1/tmp)*1000;
	if (log_interval != 0) {
		if (log_interval > 6)
			log_interval = 6;
		else if (log_interval < 0)
			log_interval = 0;
		while (log_value[log_interval] < ms)
			log_interval++;
	}

	drawLogIntervalValue(log_value[log_interval]);
	log_interval_edit = log_interval;
}

void Setting::restoreSerialBaud()
{
	drawSerialBaud(this->serial_baud);
	serial_baud_edit = this->serial_baud;

}

void Setting::changeSerialBaud(uint8_t baud_level)
{
	drawSerialBaud(serial_value[baud_level]);
	serial_baud_edit = serial_value[baud_level];
}

void Setting::activateBLLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_BL_LEVEL-i, y-1-i, 135+i*2, 28+i*2, color);
}

void Setting::activateSerialLogging(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_LOG_LEVEL -i-50, y-1-i + Y_SERIAL -10, 135+i*2+120, 28+i*2+40, color);
}

void Setting::activateLogInterval(uint16_t color)
{
	com_log_interval->setTextColor(color, TFT_BLACK);
	com_log_interval->activate();
}

void Setting::deActivateBLLevel(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_BL_LEVEL -i, y-1-i, 135+i*2, 28+i*2, color);
}

void Setting::deActivateLogInterval(uint16_t color)
{
	com_log_interval->setTextColor(color, TFT_BLACK);
	drawLogIntervalValue(log_value[log_interval]);
	com_log_interval->deActivate();
}

void Setting::deActivateSerialLogging(uint16_t color)
{
	for (int i = 1; i < 4; i++)
		tft->drawRect(x + X_LOG_LEVEL -i-50, y-1-i + Y_SERIAL -10, 135+i*2+120, 28+i*2+40, color);
}


void Setting::drawBacklightLevel(uint8_t level)
{
	tft->fillRect(x + X_BL_LEVEL + 2, y+1, 130, 24, TFT_BLACK);
	for (int i = 0; i < level; i++) {
		tft->fillRect(x + X_BL_LEVEL + 2 + (i*22), y + 1, 20, 24, TFT_YELLOW);
	}
}

void Setting::activateSerialBaud(uint16_t color)
{
	com_serial_baud->setTextColor(color, TFT_BLACK);
	drawSerialBaud(this->serial_baud);
	com_log_interval->clear();
	com_log_interval->draw("    ");
	com_serial_baud->activate();
}

void Setting::deActivateSerialBaud(uint16_t color)
{
	com_serial_baud->setTextColor(color, TFT_BLACK);
	drawSerialBaud(this->serial_baud_edit);
	com_serial_baud->deActivate();
}

void Setting::drawLogIntervalValue(uint16_t log_value)
{
	com_log_interval->clear();
	//com_log_interval->loadFont("Chewy-Regular24");
	com_log_interval->loadFont(NotoSansBold20);
	if (log_value == 0)
		com_log_interval->draw("OFF");
	else
		com_log_interval->draw(String(log_value) + " ms");
	com_log_interval->unloadFont();
}

void Setting::drawSerialBaud(uint32_t baud)
{
	com_serial_baud->clear();
	com_serial_baud->loadFont(NotoSansBold20);
	//com_serial_baud->loadFont("Chewy-Regular24");
	com_serial_baud->draw(String(baud) + " bps");
	com_serial_baud->unloadFont();
}

void Setting::drawSSID(String ssid)
{
	com_ssid->clear();
	com_ssid->loadFont(NotoSansBold20);
	com_ssid->draw(ssid);
	com_ssid->unloadFont();
}

void Setting::drawIpaddr(String ipaddr)
{
	com_ipaddr->clear();
	com_ipaddr->loadFont(NotoSansBold20);
	com_ipaddr->draw(ipaddr);
	com_ipaddr->unloadFont();
}

void Setting::drawUDPIpaddr(String ipaddr)
{
	com_udp_ipaddr->clear();
	com_udp_ipaddr->loadFont(NotoSansBold20);
	com_udp_ipaddr->draw(ipaddr);
	com_udp_ipaddr->unloadFont();
}

void Setting::drawUDPport(uint16_t port)
{
	com_udp_port->clear();
	com_udp_port->loadFont(NotoSansBold20);
	com_udp_port->draw(" : " + String(port));
	com_udp_port->unloadFont();
}

void Setting::debug()
{
	com_serial_baud->clear();
}
