#include <Arduino.h>

class PAC1933
{
	private:
	public:
		uint16_t readVoltage(void);
		uint16_t readAmpere(void);
		uint16_t readWatt(void);
};
