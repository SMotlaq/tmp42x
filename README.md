[![Doxygen Action](https://github.com/SMotlaq/tmp42x/actions/workflows/main2.yml/badge.svg)](https://github.com/SMotlaq/tmp42x/actions/workflows/main2.yml)
[![pages-build-deployment](https://github.com/SMotlaq/tmp42x/actions/workflows/pages/pages-build-deployment/badge.svg)](https://github.com/SMotlaq/tmp42x/actions/workflows/pages/pages-build-deployment)


# TMP42x HAL Based Library

The TMP421, TMP422, and TMP423 are remote temperature sensor monitors with a built-in local temperature sensor. The remote temperature sensor diode-connected transistors are typically low-cost, NPN- or PNP-type transistors or diodes that are an integral part of microcontrollers, microprocessors, or FPGAs. ([Click for more info](https://www.ti.com/product/TMP421))

This library is a software library that works with the TMP42x remote and local temperature sensor. This library provides a convenient and efficient way to access the I2C interfaces of the chip, allowing developers to easily integrate this sensor into their systems.

The library is designed to be easy to use and provides a simple, intuitive API for accessing the I2C interfaces of the TMP42x. It includes a range of functions for performing common I2C operations, such as sending and receiving data, querying the status of the chip, reading the measured parameters, and configuring the TMP42x settings.

With this library, developers can quickly and easily integrate the TMP42x into their systems, enabling them to take full advantage of the chip's capabilities.

<p align="center">
  <img src="https://github.com/SMotlaq/tmp42x/blob/images/images/all.png"/>
</p>

## Key Features

* Easy-to-use API for accessing the I2C interfaces of the INA236
* Support for common I2C operations, such as sending and receiving data, querying the status of the chip, reading the measured parameters, and configuring the TMP42x settings
* Full feature library

## Documentations

The full documents are available [here](https://smotlaq.github.io/tmp42x/)

## Schematic Symbol and Footprint

Footprint and schematic symbols are available in [my Altium library](https://github.com/SMotlaq/altium-library).

<p align="center">
  <img src="https://github.com/SMotlaq/tmp42x/blob/images/images/symbols.png"/>
</p>

## Donate
Is it helpfull?

<p align="left">
  <a href="http://smotlaq.ir/LQgQF">
  <img src="https://raw.githubusercontent.com/SMotlaq/LoRa/master/bmc.png" width="200" alt="Buy me a Coffee"/>
  </a>
</p>

# Getting Started

## Quick Start

1. Download the library source from the [latest release](http://github.com/smotlaq/tmp42x/releases/latest)

2. Copy `TMP42x.c` and `TMP42x.h` file to your project directory and add them to your IDE if necessary.

3. Inclued the library into your project:
   ```C
   #include "TMP42x.h"
   ```

4. Create an object (instanse) from TMP42x struct with desired name. I named it tmp422 as an example:
   ```C
   TMP42x tmp422;
   ```

5. Initialize the chip:
   ```C
   TMP42x_init(&tmp422, TMP422, &hi2c1, 0x4C, TMP42x_Run, TMP42x_Normal, TMP42x_0_5, 1.008, 1.008, 0);
   ```
   Each argument is described on the [doc page](https://smotlaq.github.io/tmp42x/_t_m_p42x_8c.html#ad0a1fb414abc2855469d08634cbe4c21).

6. Now you can call `TMP42x_getRemoteTemp` function to read the meassured data:
   ```C
   double remote_temp1 = TMP42x_getRemoteTemp(&tmp422, 1);
   double remote_temp2 = TMP42x_getRemoteTemp(&tmp422, 2);
   double local_temp   = TMP42x_getLocalTemp(&tmp422);
   ```

Here is the whole code:
```C
#include "TMP42x.h"

TMP42x tmp422;
double  remote_temp1, remote_temp2, local_temp;

if(TMP42x_OK == TMP42x_init(&tmp422, TMP422, &hi2c1, 0x4C, TMP42x_Run, TMP42x_Normal, TMP42x_0_5, 1.008, 1.008, 0)){

  remote_temp1 = TMP42x_getRemoteTemp(&tmp422, 1);
  remote_temp2 = TMP42x_getRemoteTemp(&tmp422, 2);
  local_temp   = TMP42x_getLocalTemp(&tmp421);

}
```


If you want to use UART or virtual USB COM port on youe microcontroller, it is recommended to use this print function:
```C
// Print setting -------------------
#define DEBUG_ENABLE  1
#define USB_DEBUG     0
#define DEBUG_UART    (&huart1)
// ---------------------------------

#if DEBUG_ENABLE
  #include "stdarg.h"
  #include "string.h"
  #include "stdlib.h"

  #if USB_DEBUG
    #include "usbd_cdc_if.h"
  #endif
#endif

void DEBUG(const char* _str, ...){
  #if DEBUG_ENABLE
    va_list args;
    va_start(args, _str);
    char buffer[150];
    memset(buffer, 0, 150);
    int buffer_size = vsprintf(buffer, _str, args);
    #if USB_DEBUG
      CDC_Transmit_FS((uint8_t*) buffer, buffer_size);
    #else
      HAL_UART_Transmit(DEBUG_UART, (uint8_t*)buffer, buffer_size, 5000);
    #endif
  #endif
}
```


By applying the above trick, you can simply use this one to see the variables on the serial terminal:
```C
#include "TMP42x.h"

TMP42x tmp422;
double  remote_temp1, remote_temp2, local_temp;

if(TMP42x_OK == TMP42x_init(&tmp422, TMP422, &hi2c1, 0x4C, TMP42x_Run, TMP42x_Normal, TMP42x_0_5, 1.008, 1.008, 0)){

  DEBUG("Local Temp: %.2f - Remote Temp 1: %.2f - Remote Temp 2: %.2f", TMP42x_getLocalTemp(&tmp422), TMP42x_getRemoteTemp(&tmp422, 1), TMP42x_getRemoteTemp(&tmp422, 2));

}
else{

  DEBUG("----- TMP422 init failed -----\r\n");

}
```
## Advanced Options

### Getting the Integer Part of the Temperature

You can read the integer part of the temperature value by calling `TMP42x_getRemoteTemp_Int` and `TMP42x_getLocalTemp_Int`
```C
int8_t remote_temp1 = TMP42x_getRemoteTemp_Int(&tmp422, 1);
int8_t remote_temp2 = TMP42x_getRemoteTemp_Int(&tmp422, 2);
int8_t local_temp   = TMP42x_getLocalTemp_Int(&tmp422);
```
Each function is described on the [doc page](https://smotlaq.github.io/temp42x/_t_m_p42x_8c.html#a8822aa9e24f446908583fe7cecd18f67).

### One-Shot Trigger

Instead of continous conversion (run mode), you can read start a single conversion when the device is in shutdown mode by calling the `TMP42x_oneShotStart` function ([see more](https://smotlaq.github.io/tmp42x/_t_m_p42x_8c.html#a75f165d360795ff352b6716df3d1e886))

```C
TMP42x_oneShotStart(&tmp422);
double remote_temp1 = TMP42x_getRemoteTemp(&tmp422, 1);
```

### Soft Reset

You can send a reset command to TMP42x chip by calling `TMP42x_softwareReset` function. ([see more](https://smotlaq.github.io/tmp42x/_t_m_p42x_8c.html#ac5cb2c0895a3f9a35a263aee39f92d5c))

### Getting Manufacturer and Device ID

If you want to get the manufacturer or device ID, you can use these functions:
* `TMP42x_getManID` ([see more](https://smotlaq.github.io/tmp42x/_t_m_p42x_8c.html#a11b6c233ddc9a8ba392fd838c50413a9))
* `TMP42x_getDevID` ([see more](https://smotlaq.github.io/tmp42x/_t_m_p42x_8c.html#a3125ba6aecab65bec02bdabbe8446886))

For example:
```C
printf("Manufacturer ID is 0x%2X \r\n", INA236_getManID(&tmp422));
printf("      Device ID is 0x%2X \r\n", TMP42x_getDevID(&tmp422));
```
