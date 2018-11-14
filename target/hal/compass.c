#include "compass.h"
#include "mag3110.h"

void hal_compass_init(void) {
	hal_lis3mdl_init();
}

void hal_compass_set_power(uint8_t on) {
	hal_lis3mdl_set_power(on);
}

uint8_t hal_compass_read(hal_compass_result_t *out) {
	return hal_lis3mdl_read(out);
}
