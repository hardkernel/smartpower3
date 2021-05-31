#include "pac1933.h"

uint16_t PAC1933::readVoltage(void)
{
	return (uint16_t)random(1400, 1600);
}

uint16_t PAC1933::readAmpere(void)
{
	return (uint16_t)random(10, 150);
}

uint16_t PAC1933::readWatt(void)
{
	return (uint16_t)random(1400, 160*15);
}
