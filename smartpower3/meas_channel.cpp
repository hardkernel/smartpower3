#include <meas_channel.h>

MeasChannel::MeasChannel(Microchip_PAC193x * pac, int channel_number)
{
	this->pac = pac;
	this->channel_number = channel_number;
}

void MeasChannel::update(uint16_t V, uint16_t A)
{
	voltage = V;

	if (++current_buffer_count >= 1000) {
		current_buffer_count = 0;
	}
	current_buffer[current_buffer_count] = A;
}

uint16_t MeasChannel::V()
{
	return (uint16_t) voltage;
}

uint16_t MeasChannel::A(int interval)
{
	int n;
	float acc = 0;

	// Worst case sampling rate is above 600sps
	// Adjusting value to impossible 500sps
	interval /= 2;

	for (int i = 0; i < interval; i++) {
		n = current_buffer_count - i;
		if (n < 0) {
			n += 1000;
		}
		acc += current_buffer[n];
	}

	return (uint16_t) (acc / interval);
}

uint16_t MeasChannel::W(int interval)
{
	return (uint16_t) (voltage * A(interval) / 1000);
}

MeasChannels::MeasChannels(Microchip_PAC193x * mc, MeasChannel * c0, MeasChannel * c1, MeasChannel * c2, uint16_t rs)
{
	pac = mc;
	channel0 = c0;
	channel1 = c1;
	channel2 = c2;
	rsense = rs;
}

void MeasChannels::sample()
{
	// This uses the ability of PAC193x to read multiple registers at once.
	unsigned long delay;
	uint8_t buffer[12];
	float V, A;

	// ensure that 1ms has already passed since last refresh
	delay = (1000 - micros() + pac->refresh_timestamp);
	// do a delay if result is not nagtive (note it's an unsinged variable)
	if (delay < 1000) delayMicroseconds(delay);

	// Reading 12 bytes - all voltages and Vsense
	pac->Read(PAC1934_VBUS1_AVG_ADDR, 12, buffer);
	// Refreshing right after reading out data - can't get faster than that!
	pac->Refresh();

	for (int i = 0; i < 6; i+=2) {
		V = (float)buffer[i+1];
		V+= (float)buffer[i]*256;
		V = V * 32000 / 65536;
		A = (float)buffer[i+7];
		A+= (float)buffer[i+6]*256;
		A = A * 100 / 65536;
		A = A / rsense * 1000000;

/*		Serial.printf("%6.10f\n\r", V);
		Serial.printf("%6.10f\n\r", A);
		Serial.printf("%d\n\r", static_cast<uint16_t>(V));
		Serial.printf("%d\n\r", static_cast<uint16_t>(A));*/

		switch (i) {
			case 0:
				channel0->update(V, A);
				break;
			case 2:
				channel1->update(V, A);
				break;
			case 4:
				channel2->update(V, A);
				break;
		}
	}
}

MeasChannel* MeasChannels::getChannel(uint8_t channel_number)
{
	switch(channel_number) {
		case 0:
			return this->channel0;
		case 1:
			return this->channel1;
		case 2:
			return this->channel2;
		default:
			return NULL;
	}
}
