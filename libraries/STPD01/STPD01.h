#ifndef __STPD01_H__
#define __STPD01_H__

#include <Arduino.h>
#include <Wire.h>

#define STPD01_ADDRESS 0x04
#define STPD01_EN_CH0 27
#define STPD01_EN_CH1 14

enum {
	STPD01_REGISTER_0 = 0x0,	// output voltage configuration
	STPD01_REGISTER_1,			// output current limitation configuration
	STPD01_REGISTER_2,			// interrupt status
	STPD01_REGISTER_3,			// interrupt latch
	STPD01_REGISTER_4,			// interrupt mask
	STPD01_REGISTER_5,			// discharge, switching frequency, cable drop etc
	STPD01_REGISTER_6			// digital enable, watchdog enable
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
#define STPD01_WATCHDOG_ENABLE	0x2

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

enum {
	DISCHARGE				= 0x1,
	DITHERING				= 0x02,
	SWITCHING_FREQUENCY		= 0x04,
	CABLE_DROP_COMPENSATION	= 0x18,
	WATCHDOG_TIMER			= 0x80
};

class STPD01
{
private:
	uint8_t _i2caddr;
	TwoWire *_wire;
	bool onoff;
	uint8_t reg_status;
	uint8_t reg_mask;
	uint8_t reg_latch;
public:
	STPD01();
	~STPD01();
	void begin(uint8_t addr = STPD01_ADDRESS, TwoWire *theWire = &Wire);
	uint8_t read8(uint8_t reg);
	void write8(uint8_t reg, uint8_t val);
	bool on(void);
	bool off(void);
	uint8_t setVoltage(uint16_t val);
	uint8_t setCurrentLimit(uint16_t milliampere);
	void monitorInterrupt(uint8_t ch);
	void setInterrupt(uint8_t reg);
	void clearInterrupt(uint8_t reg);
	uint8_t readInterrupt(void);
	bool readOnOff(void);
	uint16_t readVoltage();
	uint16_t readVoltSet();
	uint16_t readCurrentLimit();
	uint8_t readIntMask();
	uint8_t readIntStatus();
	uint8_t readIntLatch();
	void initInterrupt();
};

#endif
