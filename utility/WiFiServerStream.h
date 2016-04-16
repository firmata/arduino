/*
  WiFiServerStream.h

  An Arduino Stream extension for a WiFiClient or WiFiServer to be used
  with legacy Arduino WiFi shield and other boards and shields that
  are compatible with the Arduino WiFi library.

  Copyright (C) 2016 Jens B. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Parts of this class are based on

  - WiFiStream - Copyright (C) 2015-2016 Jesse Frush. All rights reserved.

  published under the same license.

  Last updated April 16th, 2016
 */

#ifndef WIFI_SERVER_STREAM_H
#define WIFI_SERVER_STREAM_H

#include "WiFiStream.h"

class WiFiServerStream : public WiFiStream
{
protected:
  WiFiServer _server = WiFiServer(3030);
  bool _listening = false;

  /**
   * check if TCP client is connected
   * @return true if connected
   */
  virtual inline bool connect_client()
  {
    if( _client && _client.connected() ) return true;

    if( _connected )
    {
      stop();
    }

    // passive TCP connect (accept)
    WiFiClient newClient = _server.available();
    if( !_client ) return false;  // could this work on all platforms? if( !(_client && _client.connected()) ) return false;
    _client = newClient;

    return true;
  }

public:
  /**
   * create a WiFi stream with a TCP server
   */
  WiFiServerStream(uint16_t server_port) : WiFiStream(server_port) {}

  /**
   * maintain WiFi and TCP connection
   * @return true if WiFi and TCP connection are established
   */
  virtual inline bool maintain()
  {
    if( connect_client() ) return true;

    stop();

    if( !_listening && WiFi.status() == WL_CONNECTED )
    {
      // start TCP server after first WiFi connect
      _server = WiFiServer(_port);
      _server.begin();
      _listening = true;
    }

    return false;
  }

  /**
   * stop client connection
   */
  virtual inline void stop()
  {
    if( _client)
    {
      _client.stop();
    }
    _connected = false;
  }

};

#endif //WIFI_SERVER_STREAM_H
