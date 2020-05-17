/*
  Firmata is a generic protocol for communicating with microcontrollers
  from software on a host computer. It is intended to work with
  any host computer software package.

  To download a host software package, please click on the following link
  to open the list of Firmata client libraries in your default browser.

  https://github.com/firmata/arduino#firmata-client-libraries

  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2015-2016 Jesse Frush. All rights reserved.
  Copyright (C) 2016 Jens B. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated August 17th, 2017
*/

#include <Servo.h>
#include <Wire.h>
#include "utility/ExampleStandardFirmataCommon.h"

/*
  README

  StandardFirmataWiFi enables the use of Firmata over a TCP connection. It can be configured as
  either a TCP server or TCP client.

  To use StandardFirmataWiFi you will need to have one of the following
  boards or shields:

  - Arduino WiFi Shield (or clone)
  - Arduino WiFi Shield 101
  - Arduino MKR1000 board
  - Arduino MKRWIFI1010 board
  - ESP8266 WiFi board compatible with ESP8266 Arduino core

  Follow the instructions in the wifiConfig.h file (wifiConfig.h tab in Arduino IDE) to
  configure your particular hardware.

  Dependencies:
  - WiFi Shield 101 requires version 0.7.0 or higher of the WiFi101 library (available in Arduino
    1.6.8 or higher, or update the library via the Arduino Library Manager or clone from source:
    https://github.com/arduino-libraries/WiFi101)
  - ESP8266 requires the Arduino ESP8266 core v2.1.0 or higher which can be obtained here:
    https://github.com/esp8266/Arduino

  In order to use the WiFi Shield 101 with Firmata you will need a board with at least 35k of Flash
  memory. This means you cannot use the WiFi Shield 101 with an Arduino Uno or any other
  ATmega328p-based microcontroller or with an Arduino Leonardo or other ATmega32u4-based
  microcontroller. Some boards that will work are:

  - Arduino Zero
  - Arduino Due
  - Arduino 101
  - Arduino Mega

  NOTE: If you are using an Arduino WiFi (legacy) shield you cannot use the following pins on
  the following boards. Firmata will ignore any requests to use these pins:

  - Arduino Uno or other ATMega328 boards: (D4, D7, D10, D11, D12, D13)
  - Arduino Mega: (D4, D7, D10, D50, D51, D52, D53)
  - Arduino Due, Zero or Leonardo: (D4, D7, D10)

  If you are using an Arduino WiFi 101 shield you cannot use the following pins on the following
  boards:

  - Arduino Due or Zero: (D5, D7, D10)
  - Arduino Mega: (D5, D7, D10, D50, D52, D53)
*/

/*
 * Uncomment the #define SERIAL_DEBUG line below to receive serial output messages relating to your
 * connection that may help in the event of connection issues. If defined, some boards may not begin
 * executing this sketch until the Serial console is opened.
 */
//#define SERIAL_DEBUG
#include "utility/firmataDebug.h"

/*
 * Uncomment the following include to enable interfacing with Serial devices via hardware or
 * software serial.
 */
// In order to use software serial, you will need to compile this sketch with
// Arduino IDE v1.6.6 or higher. Hardware serial should work back to Arduino 1.0.
//#include "utility/SerialFirmata.h"

// follow the instructions in wifiConfig.h to configure your particular hardware
#include "wifiConfig.h"

#define MAX_CONN_ATTEMPTS           20  // [500 ms] -> 10 s

/*==============================================================================
 * GLOBAL VARIABLES
 *============================================================================*/

#ifdef STATIC_IP_ADDRESS
IPAddress local_ip(STATIC_IP_ADDRESS);
#endif
#ifdef SUBNET_MASK
IPAddress subnet(SUBNET_MASK);
#endif
#ifdef GATEWAY_IP_ADDRESS
IPAddress gateway(GATEWAY_IP_ADDRESS);
#endif

int connectionAttempts = 0;
bool streamConnected = false;
// -----------------------------------------------------------------------------
// function forward declarations for xtensa compiler (ESP8266)
void enableI2CPins();
void disableI2CPins();
void reportAnalogCallback(byte analogPin, int value);

/*
 * Called when a TCP connection is either connected or disconnected.
 * TODO:
 * - report connected or reconnected state to host (to be added to protocol)
 * - report current state to host (to be added to protocol)
 */
void hostConnectionCallback(byte state)
{
  switch (state) {
    case HOST_CONNECTION_CONNECTED:
      DEBUG_PRINTLN( "TCP connection established" );
      break;
    case HOST_CONNECTION_DISCONNECTED:
      DEBUG_PRINTLN( "TCP connection disconnected" );
      break;
  }
}

/*
 * Print the status of the WiFi connection. This is the connection to the access point rather
 * than the TCP connection.
 */
void printWifiStatus() {
  if ( WiFi.status() != WL_CONNECTED )
  {
    DEBUG_PRINT( "WiFi connection failed. Status value: " );
    DEBUG_PRINTLN( WiFi.status() );
  }
  else
  {
    // print the SSID of the network you're attached to:
    DEBUG_PRINT( "SSID: " );
    DEBUG_PRINTLN( WiFi.SSID() );

    // print your WiFi shield's IP address:
    DEBUG_PRINT( "IP Address: " );
    IPAddress ip = WiFi.localIP();
    DEBUG_PRINTLN( ip );

    // print the received signal strength:
    DEBUG_PRINT( "signal strength (RSSI): " );
    long rssi = WiFi.RSSI();
    DEBUG_PRINT( rssi );
    DEBUG_PRINTLN( " dBm" );
  }
}

/*
 * StandardFirmataWiFi communicates with WiFi shields over SPI. Therefore all
 * SPI pins must be set to IGNORE. Otherwise Firmata would break SPI communication.
 * Additional pins may also need to be ignored depending on the particular board or
 * shield in use.
 */
void ignorePins()
{
#ifdef IS_IGNORE_PIN
  for (byte i = 0; i < TOTAL_PINS; i++) {
    if (IS_IGNORE_PIN(i)) {
      Firmata.setPinMode(i, PIN_MODE_IGNORE);
    }
  }
#endif

  //Set up controls for the Arduino WiFi Shield SS for the SD Card
#ifdef ARDUINO_WIFI_SHIELD
  // Arduino WiFi Shield has SD SS wired to D4
  pinMode(PIN_TO_DIGITAL(4), OUTPUT);    // switch off SD card bypassing Firmata
  digitalWrite(PIN_TO_DIGITAL(4), HIGH); // SS is active low;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  pinMode(PIN_TO_DIGITAL(53), OUTPUT); // configure hardware SS as output on MEGA
#endif //defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)

#endif //ARDUINO_WIFI_SHIELD
}

void initTransport()
{
  // This statement will clarify how a connection is being made
  DEBUG_PRINT( "StandardFirmataWiFi will attempt a WiFi connection " );
#if defined(WIFI_101)
  DEBUG_PRINTLN( "using the WiFi 101 library." );
#elif defined(ARDUINO_WIFI_SHIELD)
  DEBUG_PRINTLN( "using the legacy WiFi library." );
#elif defined(ESP8266_WIFI)
  DEBUG_PRINTLN( "using the ESP8266 WiFi library." );
#elif defined(HUZZAH_WIFI)
  DEBUG_PRINTLN( "using the HUZZAH WiFi library." );
#elif defined(WIFI_NINA)
  DEBUG_PRINTLN( "using the WiFi NINA library." );
  //else should never happen here as error-checking in wifiConfig.h will catch this
#endif  //defined(WIFI_101)

  // Configure WiFi IP Address
#ifdef STATIC_IP_ADDRESS
  DEBUG_PRINT( "Using static IP: " );
  DEBUG_PRINTLN( local_ip );
#if defined(ESP8266_WIFI) || (defined(SUBNET_MASK) && defined(GATEWAY_IP_ADDRESS))
  stream.config( local_ip , gateway, subnet );
#else
  // you can also provide a static IP in the begin() functions, but this simplifies
  // ifdef logic in this sketch due to support for all different encryption types.
  stream.config( local_ip );
#endif
#else
  DEBUG_PRINTLN( "IP will be requested from DHCP ..." );
#endif

  stream.attach(hostConnectionCallback);

  // Configure WiFi security and initiate WiFi connection
#if defined(WIFI_WEP_SECURITY)
  DEBUG_PRINT( "Attempting to connect to WEP SSID: " );
  DEBUG_PRINTLN(ssid);
  stream.begin(ssid, wep_index, wep_key);
#elif defined(WIFI_WPA_SECURITY)
  DEBUG_PRINT( "Attempting to connect to WPA SSID: " );
  DEBUG_PRINTLN(ssid);
  stream.begin(ssid, wpa_passphrase);
#else                          //OPEN network
  DEBUG_PRINTLN( "Attempting to connect to open SSID: " );
  DEBUG_PRINTLN(ssid);
  stream.begin(ssid);
#endif //defined(WIFI_WEP_SECURITY)
  DEBUG_PRINTLN( "WiFi setup done" );

  // Wait for connection to access point to be established.
  while (WiFi.status() != WL_CONNECTED && ++connectionAttempts <= MAX_CONN_ATTEMPTS) {
    delay(500);
    DEBUG_PRINT(".");
  }
  printWifiStatus();
}

void initFirmata()
{
  initFirmataCommonBegin();

  ignorePins();

  // Initialize Firmata to use the WiFi stream object as the transport.
  Firmata.begin(stream);

  initFirmataCommonEnd();
}

void setup()
{
  DEBUG_BEGIN(9600);

  initTransport();

  initFirmata();
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  loopFirmataCommon();
  stream.maintain();
}
