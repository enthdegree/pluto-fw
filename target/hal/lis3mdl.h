#pragma once
#include <stdint.h>
#include "common/hal/hal.h"

void hal_lis3mdl_init(void);
void hal_lis3mdl_set_power(uint8_t on);
uint8_t hal_lis3mdl_read(hal_compass_result_t *out);
