#ifndef meas_chan_h
#define meas_chan_h

#include <Microchip_PAC193x.h>

class MeasChannel
{
public:
	MeasChannel (Microchip_PAC193x *pac, int channel_number);
	void update (uint16_t V, uint16_t A);  // Update externally
	uint16_t V (void);
	uint16_t A (int interval);
	uint16_t W (int interval);
private:
	Microchip_PAC193x *pac;  // Pointer to PAC
	int channel_number;  // Channel number (0,1,2)
	uint16_t voltage;
	uint16_t current_buffer[1000];  // Circular current samplex buffer
	int current_buffer_count = 0;
};

// An entity that samples data for all three channels using faster method
class MeasChannels
{
public:
	MeasChannels (Microchip_PAC193x *mc, MeasChannel *Ch0, MeasChannel *Ch1, MeasChannel *Ch2, uint16_t rsense);
	void sample ();
	MeasChannel* getChannel(uint8_t channel_number);
private:
	Microchip_PAC193x *pac;  // Pointer to PAC
	MeasChannel *channel0;
	MeasChannel *channel1;
	MeasChannel *channel2;
	uint16_t rsense;
};

#endif
