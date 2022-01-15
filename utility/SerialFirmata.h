/*
  SerialFirmata.h
  Copyright (C) 2016 Jeff Hoefs. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  This version of SerialFirmata.h differs from the ConfigurableFirmata
  version in the following ways:

  - Defines FIRMATA_SERIAL_FEATURE (could add to Configurable version as well)
  - Imports Firmata.h rather than ConfigurableFirmata.h

  Last updated March 11th, 2020
*/

#ifndef SerialFirmata_h
#define SerialFirmata_h

#include <Firmata.h>
#include "FirmataFeature.h"
// SoftwareSerial is currently only supported for AVR-based boards and the Arduino 101.
// Limited to Arduino 1.6.6 or higher because Arduino builder cannot find SoftwareSerial
// prior to this release.
#if (ARDUINO > 10605) && (defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_ARC32) || defined(ESP8266))
#include <SoftwareSerial.h>
#endif

// If defined and set to a value between 0 and 255 milliseconds the received bytes
// will be not be read until until one of the following conditions are met:
// 1) the expected number of bytes have been received
// 2) the serial receive buffer is filled to 50 % (default size is 64 bytes)
// 3) the delay since the last received byte exceeds the configured FIRMATA_SERIAL_RX_DELAY
//    hints: 5 bytes at 9600 baud take 5 ms, human perception of a delay starts at 50 ms
// This feature can significantly reduce the load on the transport layer when
// the byte receive rate is equal or lower than the average Firmata main loop execution
// duration by preventing single byte transmits if the underlying Firmata stream supports
// transmit buffering (currently only available with EthernetClientStream). The effect
// can be increased with higher values of FIRMATA_SERIAL_RX_DELAY.
// Notes
// 1) Enabling this feature will delay the received data and may concatenate
//    bytes into one transmit that would otherwise be transmitted separately.
// 2) The usefulness and configuration of this feature depends on the baud rate and the serial message type:
//    a) continuous streaming at higher baud rates: enable but set to 0 (receive buffer store & forward)
//    b) messages: set to a value below min. inter message delay (message store & forward)
//    c) continuous streaming at lower baud rates or random characters: undefine or set to -1 (disable)
// 3) Smaller delays may not have the desired effect, especially with less powerful CPUs,
//    if set to a value near or below the average Firmata main loop duration.
// 4) The Firmata stream write buffer size must be equal or greater than the max.
//    serial buffer/message size and the Firmata frame size (4 bytes) to prevent fragmentation
//    on the transport layer.
//#define FIRMATA_SERIAL_RX_DELAY 50 // [ms]

#define FIRMATA_SERIAL_FEATURE

// Serial port Ids
#define HW_SERIAL0                  0x00
#define HW_SERIAL1                  0x01
#define HW_SERIAL2                  0x02
#define HW_SERIAL3                  0x03
#define HW_SERIAL4                  0x04
#define HW_SERIAL5                  0x05
#define HW_SERIAL6                  0x06
#define HW_SERIAL7                  0x07

#define SW_SERIAL0                  0x08
#define SW_SERIAL1                  0x09
#define SW_SERIAL2                  0x0A
#define SW_SERIAL3                  0x0B
// extensible up to 0x0F

#define SERIAL_PORT_ID_MASK         0x0F
#define MAX_SERIAL_PORTS            8
#define SERIAL_READ_ARR_LEN         12

// map configuration query response resolution value to serial pin type
#define RES_RX0                     0x00
#define RES_TX0                     0x01
#define RES_RX1                     0x02
#define RES_TX1                     0x03
#define RES_RX2                     0x04
#define RES_TX2                     0x05
#define RES_RX3                     0x06
#define RES_TX3                     0x07
#define RES_RX4                     0x08
#define RES_TX4                     0x09
#define RES_RX5                     0x0a
#define RES_TX5                     0x0b
#define RES_RX6                     0x0c
#define RES_TX6                     0x0d
#define RES_RX7                     0x0e
#define RES_TX7                     0x0f

// Serial command bytes
#define SERIAL_CONFIG               0x10
#define SERIAL_WRITE                0x20
#define SERIAL_READ                 0x30
#define SERIAL_REPLY                0x40
#define SERIAL_CLOSE                0x50
#define SERIAL_FLUSH                0x60
#define SERIAL_LISTEN               0x70

// Serial read modes
#define SERIAL_READ_CONTINUOUSLY    0x00
#define SERIAL_STOP_READING         0x01
#define SERIAL_MODE_MASK            0xF0

namespace {

  struct serial_pins {
    uint8_t rx;
    uint8_t tx;
  };

  /*
   * Get the serial serial pin type (RX1, TX1, RX2, TX2, etc) for the specified pin.
   */
  inline uint8_t getSerialPinType(uint8_t pin) {
  #if defined(PIN_SERIAL_RX)
    // TODO when use of HW_SERIAL0 is enabled
  #endif
  #if defined(PIN_SERIAL0_RX)
    if (pin == PIN_SERIAL0_RX) return RES_RX0;
    if (pin == PIN_SERIAL0_TX) return RES_TX0;
  #endif
  #if defined(PIN_SERIAL1_RX)
    if (pin == PIN_SERIAL1_RX) return RES_RX1;
    if (pin == PIN_SERIAL1_TX) return RES_TX1;
  #endif
  #if defined(PIN_SERIAL2_RX)
    if (pin == PIN_SERIAL2_RX) return RES_RX2;
    if (pin == PIN_SERIAL2_TX) return RES_TX2;
  #endif
  #if defined(PIN_SERIAL3_RX)
    if (pin == PIN_SERIAL3_RX) return RES_RX3;
    if (pin == PIN_SERIAL3_TX) return RES_TX3;
  #endif
  #if defined(PIN_SERIAL4_RX)
    if (pin == PIN_SERIAL4_RX) return RES_RX4;
    if (pin == PIN_SERIAL4_TX) return RES_TX4;
  #endif
  #if defined(PIN_SERIAL5_RX)
    if (pin == PIN_SERIAL5_RX) return RES_RX5;
    if (pin == PIN_SERIAL5_TX) return RES_TX5;
  #endif
  #if defined(PIN_SERIAL6_RX)
    if (pin == PIN_SERIAL6_RX) return RES_RX6;
    if (pin == PIN_SERIAL6_TX) return RES_TX6;
  #endif
  #if defined(PIN_SERIAL7_RX)
    if (pin == PIN_SERIAL7_RX) return RES_RX7;
    if (pin == PIN_SERIAL7_TX) return RES_TX7;
  #endif
    return 0;
  }

  /*
   * Get the RX and TX pins numbers for the specified HW serial port.
   */
  inline serial_pins getSerialPinNumbers(uint8_t portId) {
    serial_pins pins;
    switch (portId) {
  #if defined(PIN_SERIAL_RX)
        // case HW_SERIAL0:
        //   // TODO when use of HW_SERIAL0 is enabled
        //   break;
  #endif
  #if defined(PIN_SERIAL0_RX)
      case HW_SERIAL0:
        pins.rx = PIN_SERIAL0_RX;
        pins.tx = PIN_SERIAL0_TX;
        break;
  #endif
  #if defined(PIN_SERIAL1_RX)
      case HW_SERIAL1:
        pins.rx = PIN_SERIAL1_RX;
        pins.tx = PIN_SERIAL1_TX;
        break;
  #endif
  #if defined(PIN_SERIAL2_RX)
      case HW_SERIAL2:
        pins.rx = PIN_SERIAL2_RX;
        pins.tx = PIN_SERIAL2_TX;
        break;
  #endif
  #if defined(PIN_SERIAL3_RX)
      case HW_SERIAL3:
        pins.rx = PIN_SERIAL3_RX;
        pins.tx = PIN_SERIAL3_TX;
        break;
  #endif
  #if defined(PIN_SERIAL4_RX)
      case HW_SERIAL4:
        pins.rx = PIN_SERIAL4_RX;
        pins.tx = PIN_SERIAL4_TX;
        break;
  #endif
  #if defined(PIN_SERIAL5_RX)
      case HW_SERIAL5:
        pins.rx = PIN_SERIAL5_RX;
        pins.tx = PIN_SERIAL5_TX;
        break;
  #endif
  #if defined(PIN_SERIAL6_RX)
      case HW_SERIAL6:
        pins.rx = PIN_SERIAL6_RX;
        pins.tx = PIN_SERIAL6_TX;
        break;
  #endif
  #if defined(PIN_SERIAL7_RX)
      case HW_SERIAL7:
        pins.rx = PIN_SERIAL7_RX;
        pins.tx = PIN_SERIAL7_TX;
        break;
  #endif
      default:
        pins.rx = 0;
        pins.tx = 0;
    }
    return pins;
  }

} // end namespace


class SerialFirmata: public FirmataFeature
{
  public:
    SerialFirmata();
    boolean handlePinMode(byte pin, int mode);
    void handleCapability(byte pin);
    boolean handleSysex(byte command, byte argc, byte* argv);
    void update();
    void reset();
    void checkSerial();

  private:
    byte reportSerial[MAX_SERIAL_PORTS];
    int serialBytesToRead[SERIAL_READ_ARR_LEN];
    signed char serialIndex;

#if defined(FIRMATA_SERIAL_RX_DELAY)
    byte maxRxDelay[SERIAL_READ_ARR_LEN];
    int lastBytesAvailable[SERIAL_READ_ARR_LEN];
    unsigned long lastBytesReceived[SERIAL_READ_ARR_LEN];
#endif

#if defined(SoftwareSerial_h)
    Stream *swSerial0;
    Stream *swSerial1;
    Stream *swSerial2;
    Stream *swSerial3;
#endif

    Stream* getPortFromId(byte portId);

};

#endif /* SerialFirmata_h */
