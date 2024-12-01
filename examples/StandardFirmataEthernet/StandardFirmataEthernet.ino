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
  Copyright (C) 2009-2017 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated March 10th, 2020
*/

/*
  README

  StandardFirmataEthernet is a TCP client/server implementation. You will need a Firmata client library
  with a network transport that can act as a TCP server or client in order to establish a connection between
  StandardFirmataEthernet and the Firmata client application.

  To use StandardFirmataEthernet you will need to have one of the following
  boards or shields:

  - Arduino Ethernet shield (or clone)
  - Arduino Ethernet board (or clone)
  - Arduino Yun

  Follow the instructions in the ethernetConfig.h file (ethernetConfig.h tab in Arduino IDE) to
  configure your particular hardware.

  NOTE: If you are using an Arduino Ethernet shield you cannot use the following pins on
  the following boards. Firmata will ignore any requests to use these pins:

  - Arduino Uno or other ATMega328 boards: (D4, D10, D11, D12, D13)
  - Arduino Mega: (D4, D10, D50, D51, D52, D53)
  - Arduino Leonardo: (D4, D10)
  - Arduino Due: (D4, D10)
  - Arduino Zero: (D4, D10)

  If you are using an ArduinoEthernet board, the following pins cannot be used (same as Uno):
  - D4, D10, D11, D12, D13
*/

#include <Servo.h>
#include <Wire.h>
#include <Firmata.h>
#include <utility/ExampleStandardFirmataCommon.h>


/*
 * Uncomment the #define SERIAL_DEBUG line below to receive serial output messages relating to your
 * connection that may help in the event of connection issues. If defined, some boards may not begin
 * executing this sketch until the Serial console is opened.
 */
//#define SERIAL_DEBUG
#include "utility/firmataDebug.h"

// follow the instructions in ethernetConfig.h to configure your particular hardware
#include "ethernetConfig.h"
#include "utility/EthernetClientStream.h"
#include "utility/EthernetServerStream.h"

/*
 * Uncomment the following include to enable interfacing with Serial devices via hardware or
 * software serial.
 *
 * DO NOT uncomment if you are running StandardFirmataEthernet on an Arduino Leonardo,
 * Arduino Micro or other ATMega32u4-based board or you will not have enough Flash and RAM
 * remaining to reliably run Firmata. Arduino Yun is okay because it doesn't import the Ethernet
 * libraries.
 */
// In order to use software serial, you will need to compile this sketch with
// Arduino IDE v1.6.6 or higher. Hardware serial should work back to Arduino 1.0.
//#include "utility/SerialFirmata.h"

#ifdef ETHERNETCLIENTSTREAM_H
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
#endif

void printEthernetStatus()
{
  DEBUG_PRINT("Local IP Address: ");
  IPAddress ip = Ethernet.localIP();
  DEBUG_PRINTLN(ip);
#ifdef remote_ip
  DEBUG_PRINT("Connecting to server at: ");
  DEBUG_PRINTLN(remote_ip);
#endif
}

/*
 * StandardFirmataEthernet communicates with Ethernet shields over SPI. Therefore all
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

#ifdef WIZ5100_ETHERNET
  // Arduino Ethernet and Arduino EthernetShield have SD SS wired to D4
  pinMode(PIN_TO_DIGITAL(4), OUTPUT);    // switch off SD card bypassing Firmata
  digitalWrite(PIN_TO_DIGITAL(4), HIGH); // SS is active low;

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  pinMode(PIN_TO_DIGITAL(53), OUTPUT); // configure hardware SS as output on MEGA
#endif

#endif // WIZ5100_ETHERNET
}

void initTransport()
{
#ifdef ETHERNETCLIENTSTREAM_H
  stream.attach(hostConnectionCallback);
#endif

#ifdef YUN_ETHERNET
  Bridge.begin();
#else
#ifdef local_ip
  Ethernet.begin((uint8_t *)mac, local_ip); //start ethernet
#else
  DEBUG_PRINTLN("Local IP will be requested from DHCP...");
  //start ethernet using dhcp
  if (Ethernet.begin((uint8_t *)mac) == 0) {
    DEBUG_PRINTLN("Failed to configure Ethernet using DHCP");
  }
#endif
#endif
  printEthernetStatus();
}

void initFirmata()
{
  initFirmataCommonBegin();

  ignorePins();

  // start up Network Firmata:
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

#if !defined local_ip && !defined YUN_ETHERNET
  // only necessary when using DHCP, ensures local IP is updated appropriately if it changes
  if (Ethernet.maintain()) {
    stream.maintain(Ethernet.localIP());
  }
#endif

}
