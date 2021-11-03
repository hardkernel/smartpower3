#include "meas_chan.h"

Meas_chan::Meas_chan(Microchip_PAC193x * mc, int n){
	PAC = mc;
	number = n;
}

void Meas_chan::sample() {
        unsigned long delay;

        // ensure that 1ms has already passed since last refresh
        delay = (1000 - micros() + PAC->refresh_timestamp);
        // do a delay if result is not nagtive (note it's an unsinged variable)
        if (delay < 1000) delayMicroseconds(delay);

	PAC->updateVoltage(PAC1934_VBUS1_AVG_ADDR + number);
	PAC->updateCurrent(PAC1934_VSENSE1_AVG_ADDR + number);

	update(PAC->Voltage, PAC->Current);

}

void Meas_chan::update(float V, float A) {

	Voltage = V;

	if (++Current_buf_cnt >= 1000) {
		Current_buf_cnt = 0;
	}
	Current_buf[Current_buf_cnt] = A;

}

uint16_t Meas_chan::V() {
	return (uint16_t) Voltage;
}

uint16_t Meas_chan::A(int interval) {

	int n;
	float acc = 0;

	// Worst case sampling rate is above 600sps
	// Adjusting value to impossible 500sps
	interval /= 2;

	for (int i = 0; i < interval; i++) {
		n = Current_buf_cnt - i;
		if (n < 0) n+= 1000;
		acc += Current_buf[n];
	}

	return (uint16_t) (acc / interval);
}

uint16_t Meas_chan::W(int interval) {
	return (uint16_t) (Voltage * A(interval) / 1000);
}


Meas_chans::Meas_chans(Microchip_PAC193x * mc, Meas_chan * c0, Meas_chan * c1, Meas_chan * c2, float rs) {
	PAC = mc;
	Ch0 = c0;
	Ch1 = c1;
	Ch2 = c2;
	rsense = rs;
}

void Meas_chans::sample() {

	// This uses the ability of PAC193x to read multiple registers at once.
	
        unsigned long delay;
	uint8_t buffer[12];
	float V; float A;

        // ensure that 1ms has already passed since last refresh
        delay = (1000 - micros() + PAC->refresh_timestamp);
        // do a delay if result is not nagtive (note it's an unsinged variable)
        if (delay < 1000) delayMicroseconds(delay);
	
	// Reading 12 bytes - all voltages and Vsense
	PAC->Read(PAC1934_VBUS1_AVG_ADDR, 12, buffer);
	// Refreshing right after reading out data - can't get faster than that!
	PAC->Refresh();

	for (int i = 0; i < 6; i+=2) {
		V = (float)buffer[i+1];
		V+= (float)buffer[i  ]*256;
		V = V * 32000 / 65536;
		A = (float)buffer[i+7];
		A+= (float)buffer[i+6]*256;
		A = A * 100   / 65536;
		A = A / rsense * 1000000;
		
		switch (i) {
			case 0: Ch0->update(V, A); break;
			case 2: Ch1->update(V, A); break;
			case 4: Ch2->update(V, A); break;
		}
	}


}
