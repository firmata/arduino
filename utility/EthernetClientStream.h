/*
  EthernetClientStream.h
  An Arduino-Stream that wraps an instance of Client reconnecting to
  the remote-ip in a transparent way. A disconnected client may be
  recognized by the returnvalues -1 from calls to peek or read and
  a 0 from calls to write.

  Copyright (C) 2013 Norbert Truchsess. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated March 10th, 2020
 */

#ifndef ETHERNETCLIENTSTREAM_H
#define ETHERNETCLIENTSTREAM_H

#include <inttypes.h>
#include <Stream.h>

//#define SERIAL_DEBUG
#include "firmataDebug.h"

#define MILLIS_RECONNECT 5000

#define HOST_CONNECTION_DISCONNECTED 0
#define HOST_CONNECTION_CONNECTED    1

// If defined and set to a value higher than 1 all single bytes writes
// will be buffered until one of the following conditions is met:
// 1) write buffer full
// 2) any call to read(), available(), maintain(), peek() or flush()
// By combining the buffered bytes into a single TCP frame this feature will significantly 
// reduce the network and receiver load by the factor 1/(1/20 + 1/bufferedSize).
// Buffer sizes up to 80 have been tested successfully. Note that higher buffer values 
// may cause slight delays between an event and the network transmission.
#define WRITE_BUFFER_SIZE 40


extern "C" {
  // callback function types
  typedef void (*hostConnectionCallbackFunction)(byte);
}

class EthernetClientStream : public Stream
{
  public:
    EthernetClientStream(Client &client, IPAddress localip, IPAddress ip, const char* host, uint16_t port);
    int available();
    int read();
    int peek();
    void flush();
    size_t write(uint8_t);
    void maintain(IPAddress localip);
    void attach(hostConnectionCallbackFunction newFunction);

  private:
    Client &client;
    IPAddress localip;
    IPAddress ip;
    const char* host;
    uint16_t port;
    bool connected;
    uint32_t time_connect;
#ifdef WRITE_BUFFER_SIZE
    uint8_t writeBuffer[WRITE_BUFFER_SIZE];
    uint8_t writeBufferLength;
#endif    
    hostConnectionCallbackFunction currentHostConnectionCallback;
    bool maintain();
    void stop();
};


/*
 * EthernetClientStream.cpp
 * Copied here as a hack to linker issues with 3rd party board packages that don't properly
 * implement the Arduino network APIs.
 */
EthernetClientStream::EthernetClientStream(Client &client, IPAddress localip, IPAddress ip, const char* host, uint16_t port)
  : client(client),
    localip(localip),
    ip(ip),
    host(host),
    port(port),
    connected(false),
#ifdef WRITE_BUFFER_SIZE
    writeBufferLength(0),
#endif
    currentHostConnectionCallback(nullptr)
{
}

int
EthernetClientStream::available()
{
  return maintain() ? client.available() : 0;
}

int
EthernetClientStream::read()
{
  return maintain() ? client.read() : -1;
}

int
EthernetClientStream::peek()
{
  return maintain() ? client.peek() : -1;
}

void EthernetClientStream::flush()
{
  if (maintain())
    client.flush();
}

size_t
EthernetClientStream::write(uint8_t c)
{
#ifdef WRITE_BUFFER_SIZE
  if (connected) {
    // buffer new byte and send buffer when full
    writeBuffer[writeBufferLength++] = c;
    if (writeBufferLength >= WRITE_BUFFER_SIZE) {
      return maintain()? 1 : 0;
    }
    return 1;
  } else {
    return 0;
  }
#else
  return maintain() ? client.write(c) : 0;
#endif
}

void
EthernetClientStream::maintain(IPAddress localip)
{
  // ensure the local IP is updated in the case that it is changed by the DHCP server
  if (this->localip != localip) {
    this->localip = localip;
    if (connected)
      stop();
  }
}

void
EthernetClientStream::stop()
{
  client.stop();
  if (currentHostConnectionCallback)
  {
    (*currentHostConnectionCallback)(HOST_CONNECTION_DISCONNECTED);
  }
  connected = false;
#ifdef WRITE_BUFFER_SIZE
  writeBufferLength = 0;
#endif
  time_connect = millis();
}

void
EthernetClientStream::attach(hostConnectionCallbackFunction newFunction)
{
  currentHostConnectionCallback = newFunction;
}

bool
EthernetClientStream::maintain()
{
  if (client && client.connected()) {
#ifdef WRITE_BUFFER_SIZE
    // send buffered bytes
    if (writeBufferLength) {
      client.write(writeBuffer, writeBufferLength);
      writeBufferLength = 0;
    }
#endif
    return true;
  }

  if (connected) {
    stop();
  }
  // if the client is disconnected, attempt to reconnect every 5 seconds
  else if (millis() - time_connect >= MILLIS_RECONNECT) {
    connected = host ? client.connect(host, port) : client.connect(ip, port);
    if (!connected) {
      time_connect = millis();
      DEBUG_PRINTLN("Connection failed. Attempting to reconnect...");
    } else {
#ifdef WRITE_BUFFER_SIZE
      writeBufferLength = 0;
#endif
      DEBUG_PRINTLN("Connected");
      if (currentHostConnectionCallback)
      {
        (*currentHostConnectionCallback)(HOST_CONNECTION_CONNECTED);
      }
    }
  }
  return connected;
}

#endif /* ETHERNETCLIENTSTREAM_H */
