#ifndef SMARTPOWER3_HELPERS_H_
#define SMARTPOWER3_HELPERS_H_

#include <Arduino.h>

#define BG_COLOR TFT_BLACK


template<typename I, typename S> void clampVariableToRange(I, I, S*);
void clampVariableToCircularRange(int lower_limit, int upper_limit, int8_t direction, int16_t *variable_to_clamp);


/*
 * This also checks and changes some pretty serious values, like voltage and current limit in Channel,
 * so be super careful when making modifications.
 */
template<typename I, typename S>
void clampVariableToRange(I lower_limit, I upper_limit, S* variable_to_clamp) {
	if (*variable_to_clamp > upper_limit) {
		*variable_to_clamp = upper_limit;
	} else if (*variable_to_clamp < lower_limit) {
		*variable_to_clamp = lower_limit;
	}
}

#endif /* SMARTPOWER3_HELPERS_H_ */
