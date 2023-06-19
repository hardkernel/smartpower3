#include "FS.h"
#include "SPIFFS.h"
#include "settings.h"
#include "logoscreen.h"


LogoScreen::LogoScreen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager *wifi_manager, uint8_t *onoff) : Screen(tft, header, settings, wifi_manager, onoff)
{
}

LogoScreen::~LogoScreen(void)
{
	delete fs;
}

void LogoScreen::init()
{
	if (!SPIFFS.begin(false)) {
		Serial.println(F("SPIFFS mount error"));
	}
	fs = &SPIFFS;

	tft->setRotation(0);
	drawBmp(F("/logo_hardkernel.bmp"), 0, 0);
	tft->setRotation(3);
	tft->loadFont(getFont(NOTOSANSBOLD20));
	tft->drawString(F("Build date : "), 80, 295, 2);
	tft->drawString(F(__DATE__), 200, 295, 2);
	tft->drawString(F(__TIME__), 320, 295, 2);
	tft->unloadFont();
}

bool LogoScreen::draw()
{
	while (cur_time < 3000) {
		show_next_screen = false;
	}
	show_next_screen = true;
	return show_next_screen;
}

uint16_t LogoScreen::read16(fs::File &f)
{
	uint16_t result = 0;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read(); // MSB
	return result;
}

uint32_t LogoScreen::read32(fs::File &f)
{
	uint32_t result = 0;
	((uint8_t *)&result)[0] = f.read(); // LSB
	((uint8_t *)&result)[1] = f.read();
	((uint8_t *)&result)[2] = f.read();
	((uint8_t *)&result)[3] = f.read(); // MSB
	return result;
}

void LogoScreen::drawBmp(const char *filename, int16_t x, int16_t y)
{
	if ((x >= tft->width()) || (y >= tft->height()))
		return;

	fs::File bmpFS = fs->open(filename, "r");
	if (!bmpFS)
	{
		Serial.println(F("File not found"));
		return;
	}

	if (read16(bmpFS) == 0x4D42) {

		uint32_t seekOffset;
		uint16_t w, h;

		read32(bmpFS);
		read32(bmpFS);
		seekOffset = read32(bmpFS);
		read32(bmpFS);
		w = read32(bmpFS);
		h = read32(bmpFS);

		if ((read16(bmpFS) == 1) && (read16(bmpFS) == 24) && (read32(bmpFS) == 0)) {
			uint16_t row, col;
			uint8_t  r, g, b;

			y += h - 1;

			bool oldSwapBytes = tft->getSwapBytes();
			tft->setSwapBytes(true);
			bmpFS.seek(seekOffset);

			uint16_t padding = (4 - ((w * 3) & 3)) & 3;
			uint8_t lineBuffer[w * 3 + padding];

			for (row = 0; row < h; row++) {
				bmpFS.read(lineBuffer, sizeof(lineBuffer));
				uint8_t*  bptr = lineBuffer;
				uint16_t* tptr = (uint16_t*)lineBuffer;
				// Convert 24 to 16 bit colours
				for (col = 0; col < w; col++) {
					b = *bptr++;
					g = *bptr++;
					r = *bptr++;
					*tptr++ = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
				}

				// Push the pixel row to screen, pushImage will crop the line if needed
				// y is decremented as the BMP image is drawn bottom up
				tft->pushImage(x, y--, w, 1, (uint16_t*)lineBuffer);
			}
			tft->setSwapBytes(oldSwapBytes);
		}
		else Serial.println(F("BMP format not recognized."));
	}
	bmpFS.close();
}

void LogoScreen::onShutdown(void)
{
}

void LogoScreen::onWakeup(void)
{
}

screen_t LogoScreen::getType(void)
{
	return LOGO_SCREEN;
}
