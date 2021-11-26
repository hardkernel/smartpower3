#include "STPD01.h"

STPD01::STPD01(uint8_t addr, TwoWire *theWire)
{
	onoff = 1;
	_i2caddr = addr;
	_wire = theWire;
}

STPD01::~STPD01()
{
}

void STPD01::begin(uint8_t addr, TwoWire *theWire)
{
	// Disable auto discharge
	write8(STPD01_REGISTER_5, 0x00);
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
	delay(5);
    return _wire->read();
}

void STPD01::write8(uint8_t reg, uint8_t val)
{
    _wire->beginTransmission(_i2caddr);
    _wire->write(reg);
    _wire->write(val);
    _wire->endTransmission();
	delay(5);
}

bool STPD01::available()
{
    _wire->beginTransmission(_i2caddr);
	return !_wire->endTransmission();
}

uint8_t STPD01::readInterrupt()
{
	byte tmp;
	tmp = read8(STPD01_REGISTER_3);

	return tmp;
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

uint8_t STPD01::readIntStatus()
{
	return read8(STPD01_REGISTER_2);
}

uint8_t STPD01::readIntLatch()
{
	return read8(STPD01_REGISTER_3);
}

uint8_t STPD01::readIntMask()
{
	return read8(STPD01_REGISTER_4);
}

void STPD01::initInterrupt()
{
	write8(STPD01_REGISTER_4, 0x00);
	write8(STPD01_REGISTER_5, 0x00);
}

void STPD01::monitorInterrupt(uint8_t ch)
{
	reg_status = read8(STPD01_REGISTER_2);
	reg_latch = read8(STPD01_REGISTER_3);
	reg_mask = read8(STPD01_REGISTER_4);
	Serial.print("status: ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg_status, i));
	Serial.print(", latch:  ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg_latch, i));
	Serial.print(", mask:   ");
	for (int i = 7; i >= 0; i--)
		Serial.print(bitRead(reg_mask, i));
	Serial.println();
}

uint8_t STPD01::setCurrentLimit(uint16_t milliampere)
{
	uint8_t val, tmp;
	if ((milliampere < 100) and (milliampere > 3000)) {
		Serial.printf("Invalid current limit value : %d\n\r", milliampere);
		return 1;
	}
	val = milliampere/100 -1;
	write8(STPD01_REGISTER_1, val);
	tmp = read8(STPD01_REGISTER_1);
	if (tmp != val) {
		Serial.printf("write error : %x, %x\n\r", tmp, val);
		return 2;
	}
	return 0;
}

uint16_t STPD01::readVoltSet()
{
	uint8_t val = 0;
	uint16_t current = 0;
	val = read8(STPD01_REGISTER_1);
	current = (val + 1)*100;
	return current;
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
	write8(STPD01_REGISTER_6, 0x1);
	if (readOnOff() != 1)
		return 1;
	return 0;
}

bool STPD01::off(void)
{
	write8(STPD01_REGISTER_6, 0x0);
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

uint8_t STPD01::setVoltage(uint16_t volt)
{
	uint8_t val, tmp;
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
	} else {
		val = 0x00;
	}

	write8(STPD01_REGISTER_0, val);
	tmp = read8(STPD01_REGISTER_0);
	if (tmp != val) {
		Serial.printf("write error : %x, %x\n\r", tmp, val);
		return 2;
	}
	return 0;
}
