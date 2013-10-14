#ifndef ETHERNETCLIENTSTREAM_H
#define ETHERNETCLIENTSTREAM_H

#include <inttypes.h>
#include <stdio.h>
#include <Stream.h>
#include <Client.h>
#include <IPAddress.h>

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

private:
  IPAddress localip;
  IPAddress ip;
  const char* host;
  uint16_t port;
  Client &client;
  bool connected;
  uint32_t time_connect;
  bool maintain();
  void stop();
};

#endif
