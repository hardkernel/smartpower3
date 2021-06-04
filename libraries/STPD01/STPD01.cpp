#include "STPD01.h"

STPD01::STPD01()
{
	onoff = 1;
}

STPD01::~STPD01()
{
}

void STPD01::begin(uint8_t addr, TwoWire *theWire)
{
	_i2caddr = addr;
	_wire = theWire;
}

uint8_t STPD01::read8(uint8_t reg)
{
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->endTransmission();

    _wire->requestFrom(_i2caddr, (byte)1);
    return _wire->read();
}

void STPD01::write8(uint8_t reg, uint8_t val)
{
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

void STPD01::monitorInterrupt(uint8_t ch)
{
	uint8_t reg2, reg3, reg4;
	reg2 = read8(STPD01_REGISTER_2);
	reg3 = read8(STPD01_REGISTER_3);
	reg4 = read8(STPD01_REGISTER_4);
	Serial.printf("interrupt ch%d : \n\r", ch);
	Serial.print("status: ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg2, i));
	Serial.println();
	Serial.print("latch:  ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg3, i));
	Serial.println();
	Serial.print("mask:   ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg4, i));
	Serial.println();
}

void STPD01::setCurrentLimit(uint16_t milliampere)
{
	uint8_t val;
	if ((milliampere < 100) and (milliampere > 3000)) {
		Serial.printf("Invalid current limit value : %d", milliampere);
		return;
	}
	val = milliampere/100 -1;
	write8(STPD01_REGISTER_1, val);
}

void STPD01::onOff(void)
{
	byte tmp;
	onoff = !onoff;
	tmp = read8(STPD01_REGISTER_6);
	tmp = tmp & 0b10;
	tmp |= onoff;
	write8(STPD01_REGISTER_6, tmp);
}

void STPD01::setVoltage(uint16_t volt)
{
	uint8_t val;
	if (volt < 5900) {
		val = (volt - 3000)/20;
	} else if (volt < 11000) {
		val = 0x91 + (volt - 5900)/100;
	} else if (volt < 20000) {
		val = 0xc4 + (uint16_t)(volt - 11000)/200;
	}
	write8(STPD01_REGISTER_0, val);
}
