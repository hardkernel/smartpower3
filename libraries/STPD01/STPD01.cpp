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
	//write8(STPD01_REGISTER_5, 0x00);
	write8(STPD01_REGISTER_4, 0x00);
}

uint8_t STPD01::read8(uint8_t reg)
{
	byte error;
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    error = _wire->endTransmission();
	if (error != 0)
		return -1;

    _wire->requestFrom(_i2caddr, (byte)1);
    return _wire->read();
}

uint8_t STPD01::readInterrupt()
{
	byte tmp;
	tmp = read8(STPD01_REGISTER_2);

	return tmp;
}

void STPD01::write8(uint8_t reg, uint8_t val)
{
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
}

void STPD01::setInterrupt(uint8_t reg)
{
	/*
	uint8_t tmp;
	tmp = read8(STPD01_REGISTER_4);
	tmp &= ~(1 << reg);
	Serial.println(tmp);
	write8(STPD01_REGISTER_4, tmp);
	*/
	write8(STPD01_REGISTER_4, 0x00);
}

void STPD01::clearInterrupt(uint8_t reg)
{
	uint8_t tmp;
	tmp = read8(STPD01_REGISTER_4);
	tmp |= reg;
	write8(STPD01_REGISTER_4, tmp);
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

uint16_t STPD01::readCurrentLimit()
{
	uint8_t val = 0;
	uint16_t current = 0;
	val = read8(STPD01_REGISTER_1);
	current = (val + 1)*100;
	return current;
}

bool STPD01::readOnOff(void)
{
	byte tmp;
	tmp = read8(STPD01_REGISTER_6);
	tmp = tmp & 0b01;
	return tmp;
}

bool STPD01::on(void)
{
	byte tmp;
	tmp = read8(STPD01_REGISTER_6);
	tmp = tmp & 0b10;
	tmp |= 1;
	write8(STPD01_REGISTER_6, tmp);
	if (readOnOff() != 1)
		return 1;
	return 0;
}

bool STPD01::off(void)
{
	byte tmp;
	tmp = read8(STPD01_REGISTER_6);
	tmp = tmp & 0b10;
	write8(STPD01_REGISTER_6, tmp);
	if (readOnOff() != 0)
		return 1;
	return 0;
}

uint16_t STPD01::readVoltage()
{
	uint8_t val = 0;
	uint16_t volt = 0;
	val = read8(STPD01_REGISTER_0);
	if (val < 0x91)
		volt = val*20 + 3000;
	else if (val < 0xc4)
		volt = (val - 0x91)*100 + 5900;
	else if (val < 0xf1)
		volt = (val - 0xc4)*200 + 11000;
	return volt;

}

void STPD01::setVoltage(uint16_t volt)
{
	uint8_t val = 0;
	if (volt < 3000) {
		val = 0x00;
	} else if (volt < 5900) {
		val = (volt - 3000)/20;
	} else if (volt < 11000) {
		val = 0x91 + (volt - 5900)/100;
	} else if (volt < 20000) {
		val = 0xc4 + (uint16_t)(volt - 11000)/200;
	} else if (volt >= 20000) {
		val = 0xf1;
	}
	write8(STPD01_REGISTER_0, val);
}
