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
