/*
  WiFiStream.h
  An Arduino Stream that wraps an instance of a WiFi server. For use
  with legacy Arduino WiFi shield and other boards and shields that
  are compatible with the Arduino WiFi library.

  Copyright (C) 2015-2016 Jesse Frush. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.
 */

#ifndef WIFI_STREAM_H
#define WIFI_STREAM_H

#include <inttypes.h>
#include <Stream.h>

class WiFiStream : public Stream
{
private:
  WiFiServer _server = WiFiServer(23);
  WiFiClient _client;

  //configuration members
  IPAddress _local_ip;
  IPAddress _gateway;
  IPAddress _subnet;
  uint16_t _port = 0;
  uint8_t _key_idx = 0;               //WEP
  const char *_key = nullptr;         //WEP
  const char *_passphrase = nullptr;  //WPA
  char *_ssid = nullptr;

  inline int connect_client()
  {
    if( !( _client && _client.connected() ) )
    {
      WiFiClient newClient = _server.available();
      if( !newClient )
      {
        return 0;
      }

      _client = newClient;
    }
    return 1;
  }

  inline bool is_ready()
  {
    uint8_t status = WiFi.status();
    return !( status == WL_NO_SHIELD || status == WL_CONNECTED );
  }

public:
  WiFiStream() {};

#ifndef ESP8266
  // allows another way to configure a static IP before begin is called
  inline void config(IPAddress local_ip)
  {
    _local_ip = local_ip;
    WiFi.config( local_ip );
  }
#endif

  // allows another way to configure a static IP before begin is called
  inline void config(IPAddress local_ip, IPAddress gateway, IPAddress subnet)
  {
    _local_ip = local_ip;
    _gateway = gateway;
    _subnet = subnet;
#ifdef ESP8266
    WiFi.config( local_ip, gateway, subnet );
#else
    WiFi.config( local_ip, IPAddress(0, 0, 0, 0), gateway, subnet );
#endif
  }

  // get DCHP IP
  inline IPAddress localIP()
  {
    return WiFi.localIP();
  }

  /**
   * @return true if connected
   */
  inline bool maintain()
  {
    if( connect_client() ) return true;

    stop();
    int result = 0;
    if( WiFi.status() != WL_CONNECTED )
    {
      if( _local_ip )
      {
#ifdef ESP8266
        WiFi.config( _local_ip, _gateway, _subnet );
#else
        WiFi.config( _local_ip );
#endif
      }

      if( _passphrase )
      {
        result = WiFi.begin( _ssid, _passphrase);
      }
#ifndef ESP8266
      else if( _key_idx && _key )
      {
        result = WiFi.begin( _ssid, _key_idx, _key );
      }
#endif
      else
      {
        result = WiFi.begin( _ssid );
      }
    }
    if( result == 0 ) return false;

    _server = WiFiServer( _port );
    _server.begin();
    return result;
  }

/******************************************************************************
 *           Connection functions with DHCP
 ******************************************************************************/

  //OPEN networks
  inline int begin(char *ssid, uint16_t port)
  {
    if( !is_ready() ) return 0;

    _ssid = ssid;
    _port = port;
    int result = WiFi.begin( ssid );
    if( result == 0 ) return 0;

    _server = WiFiServer( port );
    _server.begin();
    return result;
  }

#ifndef ESP8266
  //WEP-encrypted networks
  inline int begin(char *ssid, uint8_t key_idx, const char *key, uint16_t port)
  {
    if( !is_ready() ) return 0;

    _ssid = ssid;
    _port = port;
    _key_idx = key_idx;
    _key = key;

    int result = WiFi.begin( ssid, key_idx, key );
    if( result == 0 ) return 0;

    _server = WiFiServer( port );
    _server.begin();
    return result;
  }
#endif

  //WPA-encrypted networks
  inline int begin(char *ssid, const char *passphrase, uint16_t port)
  {
    if( !is_ready() ) return 0;

    _ssid = ssid;
    _port = port;
    _passphrase = passphrase;

    int result = WiFi.begin( ssid, passphrase);
    if( result == 0 ) return 0;

    _server = WiFiServer( port );
    _server.begin();
    return result;
  }

/******************************************************************************
 *           Connection functions without DHCP
 ******************************************************************************/

#ifndef ESP8266
  //OPEN networks with static IP
  inline int begin(char *ssid, IPAddress local_ip, uint16_t port)
  {
    if( !is_ready() ) return 0;

    _ssid = ssid;
    _port = port;
    _local_ip = local_ip;

    WiFi.config( local_ip );
    int result = WiFi.begin( ssid );
    if( result == 0 ) return 0;

    _server = WiFiServer( port );
    _server.begin();
    return result;
  }

  //WEP-encrypted networks with static IP
  inline int begin(char *ssid, IPAddress local_ip, uint8_t key_idx, const char *key, uint16_t port)
  {
    if( !is_ready() ) return 0;

    _ssid = ssid;
    _port = port;
    _local_ip = local_ip;
    _key_idx = key_idx;
    _key = key;

    WiFi.config( local_ip );
    int result = WiFi.begin( ssid, key_idx, key );
    if( result == 0 ) return 0;

    _server = WiFiServer( port );
    _server.begin();
    return result;
  }

  //WPA-encrypted networks with static IP
  inline int begin(char *ssid, IPAddress local_ip, const char *passphrase, uint16_t port)
  {
    if( !is_ready() ) return 0;

    _ssid = ssid;
    _port = port;
    _local_ip = local_ip;
    _passphrase = passphrase;

    WiFi.config( local_ip );
    int result = WiFi.begin( ssid, passphrase);
    if( result == 0 ) return 0;

    _server = WiFiServer( port );
    _server.begin();
    return result;
  }
#endif

/******************************************************************************
 *             Stream implementations
 ******************************************************************************/

  inline int available()
  {
    return connect_client() ? _client.available() : 0;
  }

  inline void flush()
  {
    if( _client ) _client.flush();
  }

  inline int peek()
  {
    return connect_client() ? _client.peek(): 0;
  }

  inline int read()
  {
    return connect_client() ? _client.read() : -1;
  }

  inline void stop()
  {
    _client.stop();
  }

  inline size_t write(uint8_t byte)
  {
    if( connect_client() ) _client.write( byte );
  }
};

#endif //WIFI_STREAM_H
