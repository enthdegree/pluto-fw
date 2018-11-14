/*
 ******************************************************************************
 * @file    read_data_simple.c
 * @author  Sensors Software Solution Team
 * @brief   This file show the simplest way to get data from sensor.
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its
 *      contributors may be used to endorse or promote products derived from
 *      this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * This example was developed using the following STMicroelectronics
 * evaluation boards:
 *
 * - STEVAL_MKI109V3
 * - NUCLEO_F411RE + X_NUCLEO_IKS01A2
 *
 * and STM32CubeMX tool with STM32CubeF4 MCU Package
 *
 * Used interfaces:
 *
 * STEVAL_MKI109V3    - Host side:   USB (Virtual COM)
 *                    - Sensor side: SPI(Default) / I2C(supported)
 *
 * NUCLEO_STM32F411RE + X_NUCLEO_IKS01A2 - Host side: UART(COM) to USB bridge
 *                                       - I2C(Default) / SPI(N/A)
 *
 * If you need to run this example on a different hardware platform a
 * modification of the functions: `platform_write`, `platform_read`,
 * `tx_com` and 'platform_init' is required.
 *
 */

/* Includes ------------------------------------------------------------------*/
#include <string.h>

#include "mag3110.h"
#include "io.h"
#include "i2c.h"
#include "lis3mdl/driver/lis3mdl_reg.h"
#include "i2c.h"
#include <msp430.h>

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static axis3bit16_t data_raw_magnetic;
static axis1bit16_t data_raw_temperature;
static float magnetic_mG[3];
static float temperature_degC;
static uint8_t whoamI, rst;
static uint8_t tx_buffer[1000];
static lis3mdl_ctx_t dev_ctx;

/* Extern variables ----------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*
 *   WARNING:
 *   Functions declare in this section are defined at the end of this file
 *   and are strictly related to the hardware platform used.
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len);
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len);
static void tx_com(uint8_t *tx_buffer, uint16_t len);
static void platform_init(void);

// Below this line modified by Christian Chapman, 2018 for Pluto Rev2
#define DEVICE_ADDR (0b00111000)

/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. Was used to select the 
 * 		     correct sensor bus handler, but pluto always uses
 * 		     i2c so not used.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t platform_write(void *handle, uint8_t reg, uint8_t *bufp,
                              uint16_t len)
{
	if(len == 0) return 0;

	uint8_t n = 0;
	n |= hal_i2c_write_byte(1, 0, DEVICE_ADDR); //device addr write, start
	if(n)
		goto out;

	n |= hal_i2c_write_byte(0, 0, reg);
	if(n)
		goto out;

	if(len == 1) n |= hal_i2c_write_byte(0, 1, *bufp); //last, reg value
	if(n)
		goto out;

	if(len > 1){ 
		for(uint8_t i = 0; i<len; i++) {
			uint8_t last = (i == len-1);
			n |= hal_i2c_write_byte(0, last, *(bufp+i)); 
			if(n)
				goto out;
		}
	}
	
	out :
	return n;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. Was used to select the 
 * 		     correct sensor bus handler, but pluto always uses
 * 		     i2c so not used.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp,
                             uint16_t len)
{
	if(len == 0) return 0;

	uint8_t n=0;
	/* uint8_t b=0; */
	n |= hal_i2c_write_byte(1, 0, DEVICE_ADDR); //device addr write, start
	if(n)
		goto out;
	n |= hal_i2c_write_byte(0, 0, reg); //reg addr
	if(n)
		goto out;
	n |= hal_i2c_write_byte(1, 0, DEVICE_ADDR|1); //device addr read, restart
	if(n)
		goto out;
	for(uint8_t i = 0; i<len; i++) {
		uint8_t last = (i == len-1);
		bufp[i] = hal_i2c_read_byte(last, last); // stop, nak
	}
	out :
	return n;
}

/*
 * @brief  platform specific initialization (platform dependent)
 */
static void platform_init(void)
{
	PCONF(J, 7, OUL); /* MAG PWR */
	PCONF(J, 6, IN); /* MAG int1 */

	dev_ctx.write_reg = platform_write;
	dev_ctx.write_reg = platform_read;
	dev_ctx.handle = 0;
}

// External functions

void hal_lis3mdl_init(void) {
	platform_init();
}

void hal_lis3mdl_set_power(uint8_t on) {
	if(!on) {
		// Yank power
		hal_i2c_deinit();
		PJOUT &= ~BIT7;
	}
	else {
		// Configure and put on standby
		
		PJOUT |= BIT7;
		hal_i2c_init();

		/*
		*  Restore default configuration
		*/
		lis3mdl_reset_set(&dev_ctx, PROPERTY_ENABLE);
		do {
		  lis3mdl_reset_get(&dev_ctx, &rst);
		} while (rst);

		/*
		 * Set device on standby
		 */   
		lis3mdl_operating_mode_set(&dev_ctx, LIS3MDL_POWER_DOWN);

		/*
		 * Set on low-power mode
		 */  
		lis3mdl_fast_low_power_set(&dev_ctx, 1);

		/*
		 * Set full scale
		 */  
		lis3mdl_full_scale_set(&dev_ctx, LIS3MDL_16_GAUSS);
	}
}

typedef struct {
	uint16_t x;
	uint16_t y;
	uint16_t z;
} out_t;

// Write X,Y,Z reads 
uint8_t hal_lis3mdl_read(hal_compass_result_t *out) {
	uint8_t buf[6];
	uint8_t nak;

	/*
	 * Take a one-shot magnetometer measurement, wait for it to come in.
	 */   
	lis3mdl_operating_mode_set(&dev_ctx, LIS3MDL_SINGLE_TRIGGER);

	uint8_t status; 
	while(1) {
		lis3mdl_status_get(&dev_ctx, &status);
		if( (status & 8) == 8) break; 
	}
	
	out_t *p = (void*)out;
	p->x = data_raw_magnetic.i16bit[0];
	p->y = data_raw_magnetic.i16bit[1];
	p->z = data_raw_magnetic.i16bit[2];
	return 0;
}

