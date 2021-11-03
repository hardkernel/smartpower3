#ifndef meas_chan_h
#define meas_chan_h

#include <Microchip_PAC193x.h>

class Meas_chan {
	public:
		Meas_chan(Microchip_PAC193x * mc, int n);

		void update(float V, float A);	// Update externally

		uint16_t V();
		uint16_t A(int interval);
		uint16_t W(int interval);

	private:
		Microchip_PAC193x * PAC; 	// Pointer to PAC
		int number;			// Channel number (0,1,2)

		float Voltage;

		float Current_buf[1000];	// Circular current samplex buffer			
		int Current_buf_cnt = 0;
};

// An entity that samples data for all three channels using faster method
class Meas_chans {
	public:

		Meas_chans(Microchip_PAC193x * mc, Meas_chan * Ch0, Meas_chan * Ch1, Meas_chan * Ch2, float rsense);
		void sample();

	private:
		Microchip_PAC193x * PAC; 	// Pointer to PAC
		Meas_chan * Ch0;
		Meas_chan * Ch1;
		Meas_chan * Ch2;
		float rsense;
};


#endif
