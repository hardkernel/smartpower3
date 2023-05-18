#include "helpers.h"


void clampVariableToCircularRange(int lower_limit, int upper_limit, int8_t direction, int16_t *variable_to_clamp)
{
	if (*variable_to_clamp > upper_limit) {
		if (direction == 1) {
			*variable_to_clamp = lower_limit;
		} else {
			*variable_to_clamp = upper_limit;
		}
	} else if (*variable_to_clamp < lower_limit) {
		if (direction == -1) {
			*variable_to_clamp = upper_limit;
		} else {
			*variable_to_clamp = lower_limit;
		}
	}
}

void clampVoltageDialCountToRange(uint16_t current_volt_set, int16_t *dial_count) {
	int16_t absolute_low_limit = 3000;
	int16_t absolute_high_limit = 20000;
	int16_t step_change_voltage = 11000;
	int8_t lower_limit, upper_limit = 0;
	int8_t all_steps_count = 125;

	if (current_volt_set >= step_change_voltage) {
		upper_limit = (absolute_high_limit-current_volt_set)/200;
		lower_limit = upper_limit-all_steps_count;
	} else {
		lower_limit = -(current_volt_set-absolute_low_limit)/100;
		upper_limit = (all_steps_count+lower_limit);  // lower limit has negative value or is 0
	}

	clampVariableToRange(lower_limit, upper_limit, dial_count);
}
