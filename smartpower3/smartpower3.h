#include <meas_channel.h>
#include <Microchip_PAC193x.h>
#include "screen.h"
#include "inputmanager.h"
#include "settings.h"

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
Settings settings = Settings();
Microchip_PAC193x PAC = Microchip_PAC193x(15000);

MeasChannel mCh0(&PAC, 0);
MeasChannel mCh1(&PAC, 1);
MeasChannel mCh2(&PAC, 2);
MeasChannels mChs (&PAC, &mCh0, &mCh1, &mCh2, 15000);

TwoWire I2CA = TwoWire(0);
TwoWire I2CB = TwoWire(1);

struct dial_t dial;

uint16_t volt[3];
uint16_t amp[3];
uint16_t watt[3];
uint8_t *onoff;
bool low_input;

