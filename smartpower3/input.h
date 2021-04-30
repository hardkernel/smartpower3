#include <ESP32Encoder.h>
#include <Arduino.h>

class Input
{
private:
	float val_volt = 3;
	float val_ampere = 0;
	float val_watt = 0;
public:
	Input(void);
	ESP32Encoder encoder;
	void initEncoder(void);
	void countEncoder(void *);
};
