/*
  Firmata.h - Firmata library v2.5.4 - 2016-10-23
  Copyright (c) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2009-2016 Jeff Hoefs.  All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
*/

#ifndef FirmataMarshaller_h
#define FirmataMarshaller_h

#if defined(__cplusplus) && !defined(ARDUINO)
  #include <cstddef>
  #include <cstdint>
#else
  #include <stddef.h>
  #include <stdint.h>
#endif

#include <Stream.h>

class FirmataMarshaller
{
  friend class FirmataClass;
  public:
    /* constructors */
    FirmataMarshaller();

    /* public methods */
    void begin(Stream &s);
    void end();

    /* serial send handling */
    void sendAnalog(uint8_t pin, uint16_t value) const;
    void sendDigital(uint8_t pin, uint16_t value) const;
    void sendDigitalPort(uint8_t portNumber, uint16_t portData) const;
    void sendString(const char *string) const;
    void sendSysex(uint8_t command, size_t bytec, uint8_t *bytev) const;

  private:
    /* utility methods */
    void sendValueAsTwo7bitBytes(uint16_t value) const;

    Stream *FirmataStream;
};

#endif /* FirmataMarshaller_h */

