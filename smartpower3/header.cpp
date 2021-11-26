#include "header.h"

Header::Header(TFT_eSPI *tft) : Component(tft)
{
	this->tft = tft;
	input_v = new FndWidget(tft);
	input_a = new FndWidget(tft);
	mode = new Component(tft, 76, 22, 4);
	display_mode = new Component(tft, 110, 22, 4);
	icon_input = new IconWidget(tft);
	icon_wifi = new IconWidget(tft);
	icon_log = new IconWidget(tft);
	icon_v = new IconWidget(tft);
	icon_a = new IconWidget(tft);
}

void Header::init(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;

	input_v->fnd_init(3, 1, true, x + 35, y, FG_COLOR, BG_COLOR, FND_FONT_16x32, 100);
	input_v->pushValue(0);
	input_v->fnd_update(true);
	input_a->fnd_init(2, 1, true, x + 110, y, FG_COLOR, BG_COLOR, FND_FONT_16x32, 100);
	input_a->pushValue(0);
	input_a->fnd_update(true);

	icon_v->init(8, x+86, y, FG_COLOR, BG_COLOR);
	icon_v->setIconColor(TFT_GREEN, BG_ENABLED_INT);
	icon_v->unitsWrite(3);
	icon_a->init(8, x+146, y, FG_COLOR, BG_COLOR);
	icon_a->setIconColor(TFT_GREEN, BG_ENABLED_INT);
	icon_a->unitsWrite(4);


	icon_input->init(9, x, y, FG_COLOR, BG_COLOR);
	icon_wifi->init(9, x + 400, y, FG_COLOR, BG_COLOR);
	icon_log->init(9, x + 440, y, FG_COLOR, BG_COLOR);

	display_mode->init(TFT_BLACK, TFT_YELLOW, 1, TL_DATUM);
	display_mode->setCoordinate(x + 300, y);

	mode->init(TFT_YELLOW, TFT_BLACK, 1, TL_DATUM);
	mode->setCoordinate(x + 200, y);

	icon_wifi->setIconColor(TFT_GREEN, BG_ENABLED_INT);
	icon_wifi->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	icon_wifi->wifiWrite();
	icon_log->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
	icon_log->logWrite();
}

void Header::onLogging(void)
{
	flag_logging = true;
	update_logging_icon = true;
}

void Header::offLogging(void)
{
	flag_logging = false;
	update_logging_icon = true;
}

void Header::onWiFi(void)
{
	flag_wifi = true;
	update_wifi_icon = true;
}

void Header::offWiFi(void)
{
	flag_wifi = false;
	update_wifi_icon = true;
}

void Header::activate(void)
{
	mode->activate();
}

void Header::deActivate(void)
{
	mode->deActivate();
}

void Header::drawMode(String str)
{
	display_mode->clear();
	display_mode->draw(str);
}

void Header::setLowInput(bool low_input)
{
	this->low_input = low_input;
}

bool Header::getLowInput(void)
{
	return low_input;
}

uint16_t Header::getInputVoltage(void)
{
	return in_volt;
}

void Header::draw(void)
{
	if (updated) {
		updated = false;
		if (low_input) {
			input_v->setTextColor(TFT_RED, TFT_BLACK);
			input_a->setTextColor(TFT_RED, TFT_BLACK);

			icon_input->setIconColor(TFT_RED, BG_ENABLED_INT);
			icon_input->inputWrite();

			icon_v->setIconColor(TFT_RED, BG_ENABLED_INT);
			icon_v->unitsWrite(3);
			icon_a->setIconColor(TFT_RED, BG_ENABLED_INT);
			icon_a->unitsWrite(4);
		} else {
			input_v->setTextColor(TFT_GREEN, TFT_BLACK);
			input_a->setTextColor(TFT_GREEN, TFT_BLACK);

			icon_input->setIconColor(TFT_GREEN, BG_ENABLED_INT);
			icon_input->inputWrite();

			icon_v->setIconColor(TFT_GREEN, BG_ENABLED_INT);
			icon_v->unitsWrite(3);
			icon_a->setIconColor(TFT_GREEN, BG_ENABLED_INT);
			icon_a->unitsWrite(4);
		}
		input_v->pushValue(in_volt);
		input_v->fnd_update();
		input_a->pushValue((in_ampere/100)*100);
		input_a->fnd_update();
	}
#ifdef DEBUG_STPD01
	tft->drawString(String(debug), 220, 10, 2);
#endif

	if (update_logging_icon) {
		update_logging_icon = false;
		if (flag_logging) {
			icon_log->setIconColor(TFT_GREEN, BG_ENABLED_INT);
			icon_log->logWrite();
		} else {
			icon_log->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
			icon_log->logWrite();
		}
	}

	if (update_wifi_icon) {
		update_wifi_icon = false;
		if (flag_wifi) {
			icon_wifi->setIconColor(TFT_GREEN, BG_ENABLED_INT);
			icon_wifi->wifiWrite();
		} else {
			icon_wifi->setIconColor(TFT_DARKGREY, BG_ENABLED_INT);
			icon_wifi->wifiWrite();
		}
	}
}

void Header::pushPower(uint16_t volt, uint16_t ampere, uint16_t watt)
{
	in_volt = volt;
	in_ampere = ampere;
	updated = true;
}

void Header::setDebug()
{
	debug++;
}
