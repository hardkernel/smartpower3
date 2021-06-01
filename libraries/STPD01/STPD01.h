#ifndef __STPD01_H__
#define __STPD01_H__

#include <Arduino.h>
#include <Wire.h>

#define STPD01_ADDRESS 0x04

enum {
	STPD01_REGISTER_0 = 0x0,
	STPD01_REGISTER_1,
	STPD01_REGISTER_2,
	STPD01_REGISTER_3,
	STPD01_REGISTER_4,
	STPD01_REGISTER_5,
	STPD01_REGISTER_6
};

/*
 *	0 ~ 20V
 *	step :
 *	0 ~ 5.9 : 0.02V
 *	5.9 ~ 11 : 0.1V
 *	11 ~ 20 : 0.2V
 */

#define STPD01_MAX_VOLTAGE			0xF1
#define STPD01_MAX_CURRENT_LIMIT	0x1D // 100m ~ 3A, step : 100mA
#define STPD01_DIGITAL_ENABLE 0x1

enum {
	INT_OVERVOLTAGE_PROTECTION = 0x1,
	INT_CONSTANT_CURRENT_FUNCTION = 0x02,
	INT_SHORT_CIRCUIT_PROTECTION = 0x04,
	INT_POWER_ON = 0x08,
	INT_WATCHDOG = 0x10,
	INT_OVERTEMPERATURE_PROTECTION = 0x20, // Junction temperature 165 celcious degrees.
	INT_OVERTEMPERATURE_WARNING = 0x40, // Junction temperature 145 celcious degrees.
	INT_INDUCTOR_PEAK_CURRENT_PROTECTION = 0x80
};

class STPD01
{
private:
	uint8_t _i2caddr;
	TwoWire *_wire;
	bool onoff;
public:
	STPD01();
	~STPD01();
	void begin(uint8_t addr = STPD01_ADDRESS, TwoWire *theWire = &Wire);
	uint8_t read8(uint8_t reg);
	void write8(uint8_t reg, uint8_t val);
	void onOff(void);
	void setVoltage(uint16_t val);
	void setCurrentLimit(uint16_t milliampere);
	void monitorInterrupt(uint8_t ch);
};

#endif
