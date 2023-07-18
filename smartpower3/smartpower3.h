#include <meas_channel.h>
#include <Microchip_PAC193x.h>
#include "inputmanager.h"
#include "settings.h"
#include "wifimanager.h"
#include "scpimanager.h"

#define BUTTON_MENU 36
#define BUTTON_CH0 39
#define BUTTON_CH1 34
#define BUTTON_DIAL 35

#define STPD01_CH0 27
#define STPD01_CH1 14


Button button[4] = {
	Button(BUTTON_CH0), Button(BUTTON_CH1), Button(BUTTON_MENU), Button(BUTTON_DIAL)
};

ScreenManager screen_manager = ScreenManager();
Settings settings = Settings();
WiFiManager *wifi_manager;
SCPIManager *scpi_manager;

Microchip_PAC193x PAC = Microchip_PAC193x(15000);

MeasChannel mCh0(&PAC, 0);
MeasChannel mCh1(&PAC, 1);
MeasChannel mCh2(&PAC, 2);
MeasChannels mChs (&PAC, &mCh0, &mCh1, &mCh2, 15000);

TwoWire I2CA = TwoWire(0);
TwoWire I2CB = TwoWire(1);

struct dial_t dial;

uint8_t *onoff;
bool low_input;

TaskHandle_t wifi_handle;
TaskHandle_t screen_handle;
TaskHandle_t input_handle;
TaskHandle_t button_handle;
TaskHandle_t log_handle;
