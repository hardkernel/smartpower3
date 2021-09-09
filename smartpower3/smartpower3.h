#include <Microchip_PAC193x.h>
#include "screen.h"
#include "inputmanager.h"
#include "wifi_manager.h"

#define BUTTON_MENU 36
#define BUTTON_CH0 39
#define BUTTON_CH1 34
#define BUTTON_DIAL 35

#define STPD01_CH0 27
#define STPD01_CH1 14

Button button[4] = {
	Button(BUTTON_CH0), Button(BUTTON_CH1), Button(BUTTON_MENU), Button(BUTTON_DIAL)
};

Screen screen;
WifiManager wifiManager;
//Microchip_PAC193x PAC = Microchip_PAC193x(4000);
Microchip_PAC193x PAC = Microchip_PAC193x(15000);

TwoWire I2CA = TwoWire(0);
TwoWire I2CB = TwoWire(1);

uint8_t _addr = 0x5;
struct dial_t dial;
unsigned long cur_time;

uint16_t volt[3];
uint16_t amp[3];
uint16_t watt[3];
int8_t *onoff;
bool low_input;

uint8_t read8(TwoWire *theWire, uint8_t reg) {
	theWire->beginTransmission(_addr);
	theWire->write(reg);
	theWire->endTransmission();

	theWire->requestFrom(_addr, (byte)1);
	return theWire->read();
}

void write8(TwoWire *theWire, uint8_t reg, uint8_t val) {
	theWire->beginTransmission(_addr);
	theWire->write(reg);
	theWire->write(val);
	theWire->endTransmission();
}

uint8_t mode;
