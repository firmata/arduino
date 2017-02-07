/*
  Firmata.cpp - Firmata library v2.5.4 - 2016-10-23
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

//******************************************************************************
//* Includes
//******************************************************************************

#include "FirmataMarshaller.h"

#if defined(__cplusplus) && !defined(ARDUINO)
  #include <cstring>
#else
  #include <string.h>
#endif

#include "FirmataConstants.h"

//******************************************************************************
//* Support Functions
//******************************************************************************

/**
 * Request or halt a stream of analog readings from the Firmata host application. The range of pins is
 * limited to [0..15] when using the REPORT_ANALOG. The maximum result of the REPORT_ANALOG is limited to 14 bits
 * (16384). To increase the pin range or value, see the documentation for the EXTENDED_ANALOG
 * message.
 * @param pin The analog pin for which to request the value (limited to pins 0 - 15).
 * @param stream_enable A zero value will disable the stream, a non-zero will enable the stream
 * @note The maximum resulting value is 14-bits (16384).
 */
void FirmataMarshaller::reportAnalog(uint8_t pin, bool stream_enable)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  // pin can only be 0-15, so chop higher bits
  FirmataStream->write(REPORT_ANALOG | (pin & 0xF));
  FirmataStream->write(stream_enable);
}

/**
 * Request or halt an 8-bit port stream from the Firmata host application (protocol v2 and later).
 * Send 14-bits in a single digital message (protocol v1).
 * @param portNumber The port number for which to request the value. Note that this is not the same as a "port" on the
 * physical microcontroller. Ports are defined in order per every 8 pins in ascending order
 * of the Arduino digital pin numbering scheme. Port 0 = pins D0 - D7, port 1 = pins D8 - D15, etc.
 * @param stream_enable A zero value will disable the stream, a non-zero will enable the stream
 */
void FirmataMarshaller::reportDigitalPort(uint8_t portNumber, bool stream_enable)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  FirmataStream->write(REPORT_DIGITAL | (portNumber & 0xF));
  FirmataStream->write(stream_enable);
}

/**
 * Split a 16-bit integer into two 7-bit values and write each value.
 * @param value The 16-bit value to be split and written separately.
 */
void FirmataMarshaller::sendValueAsTwo7bitBytes(uint16_t value)
const
{
  FirmataStream->write(value & 0x7F); // LSB
  FirmataStream->write(value >> 7 & 0x7F); // MSB
}

//******************************************************************************
//* Constructors
//******************************************************************************

/**
 * The FirmataMarshaller class.
 */
FirmataMarshaller::FirmataMarshaller()
:
  FirmataStream((Stream *)NULL)
{
}

//******************************************************************************
//* Public Methods
//******************************************************************************

/**
 * Reassign the Firmata stream transport.
 * @param s A reference to the Stream transport object. This can be any type of
 * transport that implements the Stream interface. Some examples include Ethernet, WiFi
 * and other UARTs on the board (Serial1, Serial2, etc).
 */
void FirmataMarshaller::begin(Stream &s)
{
  FirmataStream = &s;
}

/**
 * Closes the FirmataMarshaller stream by setting its stream reference to `(Stream *)NULL`
 */
void FirmataMarshaller::end(void)
{
  FirmataStream = (Stream *)NULL;
}

//******************************************************************************
//* Output Stream Handling
//******************************************************************************

/**
 * Halt the stream of analog readings from the Firmata host application. The range of pins is
 * limited to [0..15] when using the REPORT_ANALOG. The maximum result of the REPORT_ANALOG is limited to 14 bits
 * (16384). To increase the pin range or value, see the documentation for the EXTENDED_ANALOG
 * message.
 * @param pin The analog pin for which to request the value (limited to pins 0 - 15).
 */
void FirmataMarshaller::reportAnalogDisable(uint8_t pin)
const
{
  reportAnalog(pin, false);
}

/**
 * Request a stream of analog readings from the Firmata host application. The range of pins is
 * limited to [0..15] when using the REPORT_ANALOG. The maximum result of the REPORT_ANALOG is limited to 14 bits
 * (16384). To increase the pin range or value, see the documentation for the EXTENDED_ANALOG
 * message.
 * @param pin The analog pin for which to request the value (limited to pins 0 - 15).
 */
void FirmataMarshaller::reportAnalogEnable(uint8_t pin)
const
{
  reportAnalog(pin, true);
}

/**
 * Halt an 8-bit port stream from the Firmata host application (protocol v2 and later).
 * Send 14-bits in a single digital message (protocol v1).
 * @param portNumber The port number for which to request the value. Note that this is not the same as a "port" on the
 * physical microcontroller. Ports are defined in order per every 8 pins in ascending order
 * of the Arduino digital pin numbering scheme. Port 0 = pins D0 - D7, port 1 = pins D8 - D15, etc.
 */
void FirmataMarshaller::reportDigitalPortDisable(uint8_t portNumber)
const
{
  reportDigitalPort(portNumber, false);
}

/**
 * Request an 8-bit port stream from the Firmata host application (protocol v2 and later).
 * Send 14-bits in a single digital message (protocol v1).
 * @param portNumber The port number for which to request the value. Note that this is not the same as a "port" on the
 * physical microcontroller. Ports are defined in order per every 8 pins in ascending order
 * of the Arduino digital pin numbering scheme. Port 0 = pins D0 - D7, port 1 = pins D8 - D15, etc.
 */
void FirmataMarshaller::reportDigitalPortEnable(uint8_t portNumber)
const
{
  reportDigitalPort(portNumber, true);
}

/**
 * Send an analog message to the Firmata host application. The range of pins is limited to [0..15]
 * when using the ANALOG_MESSAGE. The maximum value of the ANALOG_MESSAGE is limited to 14 bits
 * (16384). To increase the pin range or value, see the documentation for the EXTENDED_ANALOG
 * message.
 * @param pin The analog pin to send the value of (limited to pins 0 - 15).
 * @param value The value of the analog pin (0 - 1024 for 10-bit analog, 0 - 4096 for 12-bit, etc).
 * The maximum value is 14-bits (16384).
 */
void FirmataMarshaller::sendAnalog(uint8_t pin, uint16_t value)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  // pin can only be 0-15, so chop higher bits
  FirmataStream->write(ANALOG_MESSAGE | (pin & 0xF));
  sendValueAsTwo7bitBytes(value);
}

/**
 * Send an analog mapping query to the Firmata host application. The resulting sysex message will
 * have an ANALOG_MAPPING_RESPONSE command byte, followed by a list of pins [0-n]; where each
 * pin will specify its corresponding analog pin number or 0x7F (127) if not applicable.
 */
void FirmataMarshaller::sendAnalogMappingQuery(void)
const
{
  sendSysex(ANALOG_MAPPING_QUERY, 0, NULL);
}

/**
 * Send a capability query to the Firmata host application. The resulting sysex message will have
 * a CAPABILITY_RESPONSE command byte, followed by a list of byte tuples (mode and mode resolution)
 * for each pin; where each pin list is terminated by 0x7F (127).
 */
void FirmataMarshaller::sendCapabilityQuery(void)
const
{
  sendSysex(CAPABILITY_QUERY, 0, NULL);
}

/**
 * Send a single digital pin value to the Firmata host application.
 * @param pin The digital pin to send the value of.
 * @param value The value of the pin.
 */
void FirmataMarshaller::sendDigital(uint8_t pin, uint8_t value)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  FirmataStream->write(SET_DIGITAL_PIN_VALUE);
  FirmataStream->write(pin & 0x7F);
  FirmataStream->write(value != 0);
}


/**
 * Send an 8-bit port in a single digital message (protocol v2 and later).
 * Send 14-bits in a single digital message (protocol v1).
 * @param portNumber The port number to send. Note that this is not the same as a "port" on the
 * physical microcontroller. Ports are defined in order per every 8 pins in ascending order
 * of the Arduino digital pin numbering scheme. Port 0 = pins D0 - D7, port 1 = pins D8 - D15, etc.
 * @param portData The value of the port. The value of each pin in the port is represented by a bit.
 */
void FirmataMarshaller::sendDigitalPort(uint8_t portNumber, uint16_t portData)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  FirmataStream->write(DIGITAL_MESSAGE | (portNumber & 0xF));
  FirmataStream->write((uint8_t)portData % 128); // Tx bits 0-6 (protocol v1 and higher)
  FirmataStream->write(portData >> 7);  // Tx bits 7-13 (bit 7 only for protocol v2 and higher)
}

/**
 * Send the pin mode/configuration. The pin configuration (or mode) in Firmata represents the
 * current function of the pin. Examples are digital input or output, analog input, pwm, i2c,
 * serial (uart), etc.
 * @param pin The pin to configure.
 * @param config The configuration value for the specified pin.
 */
void FirmataMarshaller::sendPinMode(uint8_t pin, uint8_t config)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  FirmataStream->write(SET_PIN_MODE);
  FirmataStream->write(pin);
  FirmataStream->write(config);
}

/**
 * Send a sysex message where all values after the command byte are packet as 2 7-bit bytes
 * (this is not always the case so this function is not always used to send sysex messages).
 * @param command The sysex command byte.
 * @param bytec The number of data bytes in the message (excludes start, command and end bytes).
 * @param bytev A pointer to the array of data bytes to send in the message.
 */
void FirmataMarshaller::sendSysex(uint8_t command, size_t bytec, uint8_t *bytev)
const
{
  if ( (Stream *)NULL == FirmataStream ) { return; }
  size_t i;
  FirmataStream->write(START_SYSEX);
  FirmataStream->write(command);
  for (i = 0; i < bytec; ++i) {
    sendValueAsTwo7bitBytes(bytev[i]);
  }
  FirmataStream->write(END_SYSEX);
}

/**
 * Send a string to the Firmata host application.
 * @param string A pointer to the char string
 */
void FirmataMarshaller::sendString(const char *string)
const
{
  sendSysex(STRING_DATA, strlen(string), (uint8_t *)string);
}
