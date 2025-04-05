/*!
 * @file TMP42x.c
 *
 * @mainpage TMP42x Remote and Local Temperature Sensor with N-Factor and Series-R Correction
 *
 * @section intro_sec Introduction
 *
 * The TMP421, TMP422, and TMP423 are remote temperature sensor monitors with a built-in local temperature sensor.
 * The remote temperature sensor diode-connected transistors are typically low-cost, NPN- or PNP-type transistors
 * or diodes that are an integral part of microcontrollers, microprocessors, or FPGAs.
 *
 * Remote accuracy is ±1°C for multiple IC manufacturers, with no calibration needed. The two-wire serial interface
 * accepts SMBus write byte, read byte, send byte, and receive byte commands to configure the device.
 * 
 * The TMP421, TMP422, and TMP423 include series resistance cancellation, programmable non-ideality factor,
 * wide remote temperature measurement range (up to +150°C), and diode fault detection.
 *
 * The TMP421, TMP422, and TMP423 are all available in a SOT23-8 package. The TMP421C is also available in a DSBGA (WCSP) package.
 *
 * @section author Author
 *
 * Written by Salman Motlaq (<a href="https://github.com/SMotlaq">@SMotlaq</a> on Github)
 *
 * @section license License
 *
 * MIT License
 *
 * @section start Getting Started
 * Go to [my Github page](https://github.com/SMotlaq/tmp42x) to get started. You can also see the [functions list](./TMP42x_8c.html) to learn more.
 *
 */

#ifndef __TMP42x_H_
#define __TMP42x_H_

#include "main.h"
#include "i2c.h"

#define LOCAL_TEMP_MSB						0x00
#define REMOTE1_TEMP_MSB					0x01
#define REMOTE2_TEMP_MSB					0x02
#define REMOTE3_TEMP_MSB					0x03
#define STATUS_REGISTER						0x08
#define CONFIG1_REGISTER					0x09
#define CONFIG2_REGISTER					0x0A
#define CONVERSION_RATE_REGISTER	0x0B
#define ONE_SHOT_START_REGISTER		0x0F
#define LOCAL_TEMP_LSB						0x10
#define REMOTE1_TEMP_LSB					0x11
#define REMOTE2_TEMP_LSB					0x12
#define REMOTE3_TEMP_LSB					0x13
#define nCORRECTION1_REGISTER			0x21
#define nCORRECTION2_REGISTER			0x22
#define nCORRECTION3_REGISTER			0x23
#define SOFTWARE_RESET_REGISTER		0xFC
#define MAN_ID_REGISTER						0xFE
#define DEVICE_ID_REGISTER				0xFF

typedef enum TMP42x_Type						{TMP421, TMP422, TMP423} TMP42x_Type;
typedef enum TMP42x_Status					{TMP42x_OK, TMP42x_TimeOut, TMP42x_Arg_Error} TMP42x_Status;
typedef enum TMP42x_Range						{TMP42x_Normal, TMP42x_Extended} TMP42x_Range;
typedef enum TMP42x_Shutdown				{TMP42x_Run, TMP42x_ShutDown} TMP42x_Shutdown;
typedef enum TMP42x_ConversionRate	{TMP42x_0_0625, TMP42x_0_125, TMP42x_0_25, TMP42x_0_5, TMP42x_1, TMP42x_2, TMP42x_4, TMP42x_8} TMP42x_ConversionRate;

/*! 
    @brief  Class (struct) that stores variables for interacting with TMP42x
*/
typedef struct tmp42x{
	
	I2C_HandleTypeDef*	hi2c;										/*!< Specifies the I2C handler. */
	uint8_t 						I2C_ADDR;								/*!< Specifies the I2C device address. */
	
	TMP42x_Type						type;									/*!< Specifies the chip type: TMP421, TMP422, or TMP423. */
	TMP42x_Shutdown				shutdown;							/*!< Specifies the operation mode: Running or Shutdown */
	TMP42x_Range					range;								/*!< Specifies the temperature range: Normal (-40 to +127) or Extended (-55 to +150). */
	TMP42x_ConversionRate	conversion_rate;			/*!< Specifies the convertion rate. */
	float									n_eff1;								/*!< Specifies the adjusted effective n for channel 1. */
	float									n_eff2;								/*!< Specifies the adjusted effective n for channel 2. (TMP422 and TMP423). */
	float									n_eff3;								/*!< Specifies the adjusted effective n for channel 3. (TMP423) */
	
	union _reg16 {
		uint8_t raw_data[2];
		
		struct _temp_register{
			int16_t OPEN:1, PVLD:1, RESERVED:2, TEMP:12;
		} temp_register;

	} reg16;
	
	union _reg8 {
		uint8_t raw_data;
		
		struct _status_register{
			uint8_t RESERVED:7, BUSY:1;
		} status_register;
		
		struct _temp_int_register{
			int8_t TEMP;
		} temp_int_register;
		
		struct _temp_frac_register{
			uint8_t OPEN:1, PVLD:1, RESERVED:2, TEMP:4;
		} temp_frac_register;
		
		struct _config1_register{
			uint8_t RESERVED1:2, TEMP_RANGE:1, RESERVED2:3, SD:1, RESERVED3:1;
		} config1_register;
		
		struct _config2_register{
			uint8_t RESERVED1:2, RC:1, LEN:1, REN:1, REN2:1, REN3:1, RESERVED2:1;
		} _config2_register;
		
		struct _conversion_rate_register{
			uint8_t RESERVED:5, RATE:3;
		} conversion_rate_register;
		
		struct _n_correction_register{
			int8_t N;
		} n_correction_register;
			
		struct _man_id_register{
			uint8_t MANUFACTURE_ID;
		} man_id_register;
		
		struct _dev_id_register{
			uint8_t DEVID;
		} dev_id_register;
		
	} reg8;

} TMP42x;

// Privates ----------------------------------
TMP42x_Status __TMP42x_readByte(TMP42x* self, uint8_t MemAddress);
TMP42x_Status __TMP42x_readTwoBytes(TMP42x* self, uint8_t MemAddress);
TMP42x_Status __TMP42x_writeByte(TMP42x* self, uint8_t MemAddress);

// Public ------------------------------------
TMP42x_Status TMP42x_init(TMP42x* self, TMP42x_Type type, I2C_HandleTypeDef* hi2c, uint8_t I2C_ADDR, TMP42x_Shutdown shutdown, TMP42x_Range range, TMP42x_ConversionRate conversion_rate, float n_eff1, float n_eff2, float n_eff3);
double TMP42x_getLocalTemp(TMP42x* self);
int8_t TMP42x_getLocalTemp_Int(TMP42x* self);
double TMP42x_getRemoteTemp(TMP42x* self, uint8_t channel_number);
int8_t TMP42x_getRemoteTemp_Int(TMP42x* self, uint8_t channel_number);
TMP42x_Status TMP42x_oneShotStart(TMP42x* self);
TMP42x_Status TMP42x_softwareReset(TMP42x* self);
uint8_t TMP42x_getManID(TMP42x* self);
uint8_t TMP42x_getDevID(TMP42x* self);

#endif
