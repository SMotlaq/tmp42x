/*!
 * @file TMP42x.c
 *
 * @mainpage TMP42x
 * Remote and Local Temperature Sensor with N-Factor and Series-R Correction
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

#include "TMP42x.h"

// Privates

/*!
		@brief Read a byte of data from TMP42x and stores in the tmp42x::_reg8::raw_data
		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.
		@param	MemAddress
						Address of the target register
		@return	The status of initialization
		@retval ::TMP42x_OK in case of success
		@retval ::TMP42x_TimeOut in case of failure
*/
TMP42x_Status __TMP42x_readByte(TMP42x* self, uint8_t MemAddress){
	uint8_t data;
	if(HAL_I2C_Mem_Read(self->hi2c, self->I2C_ADDR << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000)) return TMP42x_TimeOut;
	self->reg8.raw_data = data;
	return TMP42x_OK;
}

/*!
    @brief  Write a byte of data to TMP42x from the tmp42x::_reg8::raw_data
		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.
		@param	MemAddress
						Address of the target register
		@return	The status of initialization
		@retval ::TMP42x_OK in case of success
		@retval ::TMP42x_TimeOut in case of failure
*/
TMP42x_Status __TMP42x_writeByte(TMP42x* self, uint8_t MemAddress){
	uint8_t data[1];
	data[0] = self->reg8.raw_data;
	if(HAL_I2C_Mem_Write(self->hi2c, self->I2C_ADDR << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, data, 1, 1000)) return TMP42x_TimeOut;
	return TMP42x_OK;
}

/*!
    @brief Read two bytes of data from TMP42x and stores in the tmp42x::_reg16::raw_data
		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.
		@param MemAddress
						Address of the target register
		@return	The status of initialization
		@retval ::TMP42x_OK in case of success
		@retval ::TMP42x_TimeOut in case of failure
*/
TMP42x_Status __TMP42x_readTwoBytes(TMP42x* self, uint8_t MemAddress){
	if(HAL_OK == HAL_I2C_Mem_Read(self->hi2c, self->I2C_ADDR << 1, MemAddress, I2C_MEMADD_SIZE_8BIT, self->reg16.raw_data, 2, 100)){
		
		self->reg16.raw_data[0] ^= self->reg16.raw_data[1];
		self->reg16.raw_data[1] ^= self->reg16.raw_data[0];
		self->reg16.raw_data[0] ^= self->reg16.raw_data[1];
		
		return TMP42x_OK;
	}
	else{
		return TMP42x_TimeOut;
	}
}

//Public
/*!
    @brief  Initialize the TMP42x with the given config
		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.
		@param	type
						Chip part number.
						- ::TMP421
						- ::TMP422
						- ::TMP423
		@param	hi2c
						A pointer to the I2C handler that is connected to TMP42x
		@param	I2C_ADDR
						The I2C address of the TMP42x. It depends on the state of A0 and A1 of the chip (TMP421), DXx pins status (TMP422), or the part number (TMP423)
		@param	shutdown
						Operating mode:
						- ::TMP42x_Run
						- ::TMP42x_ShutDown
		@param	range
						Temperature range:
						- ::TMP42x_Normal -40 to +127 °C
						- ::TMP42x_Extended -55 to +150 °C
		@param	conversion_rate
						Rate of the temperature conversion:
						- ::TMP42x_0_0625 0.0625 conversions per second (each 16 seconds)
						- ::TMP42x_0_125 0.125 conversions per second (each 8 seconds)
						- ::TMP42x_0_25 0.25 conversions per second (each 4 seconds)
						- ::TMP42x_0_5 0.5 conversions per second (each 1 seconds)
						- ::TMP42x_1 1 conversion per second (each second)
						- ::TMP42x_2 2 conversions per second (each 0.5 seconds)
						- ::TMP42x_4 4 conversions per second (each 0.25 seconds) NOTE: Conversion rate shown is for only one or two enabled measurement channels. When three channels are enabled, the conversion rate is 2 and 2/3 conversions-per-second. When four channels are enabled, the conversion rate is 2 per second.
						- ::TMP42x_8 8 conversions per second (each 0.125 seconds) NOTE: Conversion rate shown is for only one enabled measurement channel. When two channels are enabled, the conversion rate is 4 conversions-per-second. When three channels are enabled, the conversion rate is 2 and 2/3 conversions-per-second.  When four channels are enabled, the conversion rate is 2 conversions-per-second.
		@param	n_eff1
						Effective n value for channel1. From 0.706542 to 1.747977.
		@param	n_eff2
						Effective n value for channel2 (TMP422 or TMP423). From 0.706542 to 1.747977.
		@param	n_eff3
						Effective n value for channel3 (TMP423). From 0.706542 to 1.747977.
		@return	The status of initialization
		@retval ::TMP42x_OK in case of success
		@retval ::TMP42x_TimeOut in case of failure
*/
TMP42x_Status TMP42x_init(TMP42x* self, TMP42x_Type type, I2C_HandleTypeDef* hi2c, uint8_t I2C_ADDR, TMP42x_Shutdown shutdown, TMP42x_Range range, TMP42x_ConversionRate conversion_rate, float n_eff1, float n_eff2, float n_eff3){
	
	self->hi2c = hi2c;
	self->I2C_ADDR = I2C_ADDR;
	self->type = type;
	self->shutdown = shutdown;
	self->range = range;
	self->conversion_rate = conversion_rate;
	self->n_eff1 = n_eff1;
	self->n_eff2 = n_eff2;
	self->n_eff3 = n_eff3;
	
	// Config 1
	if(__TMP42x_readByte(self, CONFIG1_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	self->reg8.config1_register.SD = self->shutdown;
	self->reg8.config1_register.TEMP_RANGE = self->range;
	if(__TMP42x_writeByte(self, CONFIG1_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	
	// Config 2
	if(__TMP42x_readByte(self, CONFIG2_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	self->reg8._config2_register.LEN  = 1;
	self->reg8._config2_register.RC   = 1;
	self->reg8._config2_register.REN  = 1;
	if(self->type != TMP421) self->reg8._config2_register.REN2 = 1;
	if(self->type == TMP423) self->reg8._config2_register.REN3 = 1;
	if(__TMP42x_writeByte(self, CONFIG2_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	
	// Conversion Rate
	if(__TMP42x_readByte(self, CONVERSION_RATE_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	self->reg8.conversion_rate_register.RATE = conversion_rate;
	if(__TMP42x_writeByte(self, CONVERSION_RATE_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	
	// eta 1
	if(__TMP42x_readByte(self, nCORRECTION1_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	int8_t n_adjust = (int8_t)(300 - (302.4/self->n_eff1));
	self->reg8.n_correction_register.N = n_adjust;
	if(__TMP42x_writeByte(self, nCORRECTION1_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	
	
	// eta 2
	if(self->type != TMP421){
		if(__TMP42x_readByte(self, nCORRECTION2_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
		n_adjust = (int8_t)(300 - (302.4/self->n_eff2));
		self->reg8.n_correction_register.N = n_adjust;
		if(__TMP42x_writeByte(self, nCORRECTION2_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	}

	// eta 3
	if(self->type == TMP423){
		if(__TMP42x_readByte(self, nCORRECTION3_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
		n_adjust = (int8_t)(300 - (302.4/self->n_eff3));
		self->reg8.n_correction_register.N = n_adjust;
		if(__TMP42x_writeByte(self, nCORRECTION3_REGISTER)!=TMP42x_OK) return TMP42x_TimeOut;
	}
		
	return TMP42x_OK;
}

/*!
		@brief Reads the **full** (floating-point) remote temperature from a specified channel.

		This function retrieves the temperature (in degrees Celsius) from a specified remote channel
		of the TMP42x sensor series. The returned value includes both integer and fractional parts.

		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.

		@param channel_number
					 Index of the remote channel to read from. Valid values are:
					 - `1` : Channel 1
					 - `2` : Channel 2 (only available on TMP422 or TMP423)
					 - `3` : Channel 3 (only available on TMP423)

		@return
						Temperature value in degrees Celsius as a floating-point number if successful.

		@retval -999
						Returned if the requested channel number does not exist for the device type.
*/
double TMP42x_getRemoteTemp(TMP42x* self, uint8_t channel_number){
	if(channel_number > (self->type + 1)) return -999;
	__TMP42x_readTwoBytes(self, REMOTE1_TEMP_MSB + channel_number - 1);
	return self->reg16.temp_register.TEMP * 0.0625 - self->range * 0x40;
}

/*!
		@brief Reads the **integer** part of the remote temperature from a specified channel.

		This function retrieves the temperature (in degrees Celsius) from a specified remote channel
		of the TMP42x sensor series. It only returns the integer part of the temperature.

		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.

		@param channel_number
					 Index of the remote channel to read from. Valid values are:
					 - `1` : Channel 1
					 - `2` : Channel 2 (only available on TMP422 or TMP423)
					 - `3` : Channel 3 (only available on TMP423)

		@return
						Temperature value (integer part only) in degrees Celsius if successful.

		@retval -128
						Returned if the requested channel number does not exist for the device type.
*/
int8_t TMP42x_getRemoteTemp_Int(TMP42x* self, uint8_t channel_number){
	if(channel_number > (self->type + 1)) return -128;
	__TMP42x_readByte(self, REMOTE1_TEMP_MSB + channel_number - 1);
	return self->reg8.raw_data - self->range * 0x40;
}

/*!
		@brief Reads the **full** (floating-point) local temperature from a specified channel.

		This function retrieves the temperature (in degrees Celsius) from
		the TMP42x sensor series. The returned value includes both integer and fractional parts.

		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.

		@return
						Temperature value in degrees Celsius as a floating-point number if successful.
*/
double TMP42x_getLocalTemp(TMP42x* self){
	__TMP42x_readTwoBytes(self, LOCAL_TEMP_MSB);
	return self->reg16.temp_register.TEMP * 0.0625 - self->range * 0x40;
}

/*!
		@brief Reads the **integer** part of the local temperature.

		This function retrieves the temperature (in degrees Celsius) from 
		the TMP42x sensor series. It only returns the integer part of the temperature.

		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.

		@return
						Temperature value (integer part only) in degrees Celsius if successful.
*/
int8_t TMP42x_getLocalTemp_Int(TMP42x* self){
	__TMP42x_readByte(self, LOCAL_TEMP_MSB);
	return self->reg8.raw_data - self->range * 0x40;
}

/*!
		@brief Starts a single conversion if the device is in shutdown mode.

		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.

		@return	The status of start
		@retval ::TMP42x_OK in case of success
		@retval ::TMP42x_TimeOut in case of failure
*/
TMP42x_Status TMP42x_oneShotStart(TMP42x* self){
	self->reg8.raw_data = 0x69;
	return __TMP42x_writeByte(self, ONE_SHOT_START_REGISTER);
}

/*!
		@brief Resets the device

		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.

		@return	The status of start
		@retval ::TMP42x_OK in case of success
		@retval ::TMP42x_TimeOut in case of failure
*/
TMP42x_Status TMP42x_softwareReset(TMP42x* self){
	self->reg8.raw_data = 0x85;
	return __TMP42x_writeByte(self, SOFTWARE_RESET_REGISTER);
}

/*!
    @brief  Get the manufacturer ID
		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.
		@return	a 8bit manufacturer ID
*/
uint8_t TMP42x_getManID(TMP42x* self){
	__TMP42x_readByte(self, MAN_ID_REGISTER);
	return self->reg8.man_id_register.MANUFACTURE_ID;	
}

/*!
    @brief  Get the device ID
		@param self
					 Pointer to a `TMP42x` structure representing the sensor instance.
		@return	a 8bit device ID
*/
uint8_t TMP42x_getDevID(TMP42x* self){
	__TMP42x_readByte(self, DEVICE_ID_REGISTER);
	return self->reg8.dev_id_register.DEVID;	
}
