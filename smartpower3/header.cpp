#include "header.h"

Header::Header(TFT_eSPI *tft) : Component(tft)
{
	this->tft = tft;
	input = new Component(tft, 120, 30, 4);
	mode = new Component(tft, 76, 22, 4);
	display_mode = new Component(tft, 110, 22, 4);
	icon_input = new FndWidget(tft);

}

void Header::init(uint16_t x, uint16_t y)
{
	this->x = x;
	this->y = y;
	input->init(TFT_BLACK, TFT_RED, 1, TL_DATUM);
	input->setCoordinate(x + 35, y);
	input->draw("0.0V");
	icon_input->icon_init(9, x, y, FG_COLOR, BG_COLOR);

	tft->drawString(String(debug), 130, 10, 2);

	tft->drawString("v1.2", 420, y, 4);

	display_mode->init(TFT_BLACK, TFT_YELLOW, 1, TL_DATUM);
	display_mode->setCoordinate(x + 300, y);
	display_mode->draw("POWER");

	mode->init(TFT_YELLOW, TFT_BLACK, 1, TL_DATUM);
	mode->setCoordinate(x + 200, y);
	mode->draw("MODE");
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
			input->setTextColor(TFT_BLACK, TFT_RED);
			icon_input->setIconColor(TFT_RED, BG_ENABLED_INT);
			icon_input->icon_input_write();
		} else {
			input->setTextColor(TFT_BLACK, TFT_GREEN);
			icon_input->setIconColor(TFT_GREEN, BG_ENABLED_INT);
			icon_input->icon_input_write();
		}
		input->clear();
		String tmp_v = String(in_volt/1000.0, 1);
		String tmp_a = String(in_ampere/1000.0, 1);
		input->draw(tmp_v + "V " + tmp_a + "A");
	}
	tft->drawString(String(debug), 180, 10, 2);
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
