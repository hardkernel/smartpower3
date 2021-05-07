#include "pac1933.h"

uint16_t PAC1933::readVoltage(void)
{
	return (uint16_t)random(140, 160);
}

uint16_t PAC1933::readAmpere(void)
{
	return (uint16_t)random(1, 15);
}

uint16_t PAC1933::readWatt(void)
{
	return (uint16_t)random(140, 2400);
}
