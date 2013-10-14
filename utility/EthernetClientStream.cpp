#include "EthernetClientStream.h"
#include <Arduino.h>
#include <Ethernet.h>

#define MILLIS_RECONNECT 5000

EthernetClientStream::EthernetClientStream(Client &client, IPAddress localip, IPAddress ip, const char* host, uint16_t port)
: ip(ip),
  host(host),
  port(port),
  connected(false),
  client(client),
  localip(localip)
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
  return maintain() ? client.write(c) : 0;
}

void
EthernetClientStream::maintain(IPAddress localip)
{
  if (this->localip!=localip)
    {
      this->localip = localip;
      if (connected)
        stop();
    }
}

void
EthernetClientStream::stop()
{
  client.stop();
  connected = false;
  time_connect = millis();
}

bool
EthernetClientStream::maintain()
{
  if (client && client.connected())
    return true;

  if (connected)
    {
      stop();
    }
  else if (millis()-time_connect >= MILLIS_RECONNECT)
    {
      connected = host ? client.connect(host,port) : client.connect(ip,port);
      if (!connected)
        time_connect = millis();
    }
  return connected;
}
