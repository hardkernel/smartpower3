#ifndef SMARTPOWER3_SCREENS_LOGOSCREEN_H_
#define SMARTPOWER3_SCREENS_LOGOSCREEN_H_


#include "FS.h"
#include "SPIFFS.h"
#include "screens/screen.h"


class LogoScreen : public Screen
{
public:
	LogoScreen();
	LogoScreen(TFT_eSPI *tft, Header *header, Settings *settings, WiFiManager * wifi_manager, uint8_t *onoff);
	virtual ~LogoScreen(void);
	virtual bool draw(void);
	virtual void init(void);

	virtual void onShutdown(void);
	virtual void onWakeup(void);
	virtual screen_t getType(void);
private:
	fs::FS *fs;
	void drawBmp(const char *filename, int16_t x, int16_t y);
	uint16_t read16(fs::File &f);
	uint32_t read32(fs::File &f);
};


#endif /* SMARTPOWER3_SCREENS_LOGOSCREEN_H_ */
