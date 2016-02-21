/*===================================================================================
 * BLE CONFIGURATION
 *
 * If you are using an Arduino 101, you do not need to make any changes to this
 * file. If you are using another supported BLE board or shield, follow the
 * instructions for the specific board or shield below.
 *
 * Supported boards and shields:
 * - Arduino 101 (recommended)
 * - RedBearLab BLE Shield (v2)  ** to be verified **
 *
 *==================================================================================*/

/*
 * RedBearLab BLE Shield
 *
 * If you are using a RedBearLab BLE shield, uncomment the define below.
 * Also, change the define for BLE_RST if you have the jumper set to pin 7 rather than pin 4.
 *
 * You will need to use the shield with an Arduino Mega, Due or other board with sufficient
 * Flash and RAM. Arduino Uno, Leonardo and other ATmega328p and Atmega32u4 boards to not have
 * enough memory to run StandardFirmataBLE.
 *
 * TODO: verify if this works and with which boards it works.
 *
 * Test script: https://gist.github.com/soundanalogous/927360b797574ed50e27
 * (may need to skip capabilities - see https://gist.github.com/soundanalogous/d39bb3eb36333a0906df)
 */
//#define REDBEAR_BLE_SHIELD

#ifdef REDBEAR_BLE_SHIELD
#include <SPI.h>
#include <BLEPeripheral.h>
#include "utility/BLEStream.h"

#define BLE_REQ  9
#define BLE_RDY  8
#define BLE_RST  4 // 4 or 7 via jumper on shield

BLEStream stream(BLE_REQ, BLE_RDY, BLE_RST);
#endif


/*===================================================================================
 * END BLE CONFIGURATION - you should not need to change anything below this line
 *==================================================================================*/

/*
 * Arduino 101
 *
 * Test script: https://gist.github.com/soundanalogous/927360b797574ed50e27
 */
#ifdef _VARIANT_ARDUINO_101_X_
#include <CurieBle.h>
//#include <CurieBLE.h> // switch to this once new Arduino 101 board package is available
#include "utility/BLEStream.h"
BLEStream stream;
#endif


/*
 * RedBearLab BLE Nano
 * Blocked on this issue: https://github.com/RedBearLab/nRF51822-Arduino/issues/46
 * Also, need to skip Capability Query (see example in test script)
 *
 * Test script: https://gist.github.com/soundanalogous/d39bb3eb36333a0906df
 */
// #ifdef BLE_NANO
// #include <BLEPeripheral.h>
// #include "utility/BLEStream.h"
// BLEStream stream;
// #endif


/*
 * RedBearLab Blend and Blend Micro
 * StandardFirmataBLE is requires too much Flash and RAM to run on Blend Micro
 * may work with ConfigurableFirmata selecting only analog and digital I/O
 */
// #if defined(BLEND_MICRO) || defined(BLEND)
// #include <SPI.h>
// #include <BLEPeripheral.h>
// #include "utility/BLEStream.h"

// #define BLE_REQ  6
// #define BLE_RDY  7
// #define BLE_RST  4

// BLEStream stream(BLE_REQ, BLE_RDY, BLE_RST);
// #endif


#if defined(BLE_REQ) && defined(BLE_RDY) && defined(BLE_RST)
#define IS_IGNORE_BLE_PINS(p) ((p) == BLE_REQ || (p) == BLE_RDY || (p) == BLE_RST)
#endif
