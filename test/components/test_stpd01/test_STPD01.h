#include <Arduino.h>
#include <Wire.h>
#include "STPD01.h"


#define STPD01_ADDRESS 0x04


class Test_STPD01 : public STPD01
{
private:
	uint8_t _i2caddr;
	TwoWire *_wire;
	bool onoff;
public:
	Test_STPD01(uint8_t addr, TwoWire *theWire);
	~Test_STPD01();
	uint8_t read8(uint8_t reg);
	void write8(uint8_t reg, uint8_t val);
};
