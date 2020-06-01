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
  Copyright (C) 2020 david gauchard. All rights reserved (esp8266/websocket)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  Last updated June 1st, 2020
*/

#ifndef STASSID
// WiFi credentials
#define STASSID "your_network_name"
#define STAPSK  "your_wpa_passphrase"
#endif

#define LOG_ENABLED     1       // 0 or 1
#define DEBUG_ENABLED   0       // 0 or 1
#define WS_PORT         3031    // websocket port
#define SERIAL_BAUD     9600

#include <Servo.h>
#include <Wire.h>
#include <Firmata.h>

#include <ESP8266mDNS.h>        // mDNS from esp8266 Arduino standard library
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets 
#include <PipedStream.h>        // https://github.com/paulo-raca/ArduinoBufferedStreams

#include <utility/ExampleStandardFirmataCommon.h>

#if LOG_ENABLED
#define LOG(x...) do { x; } while (0)
#define IS_IGNORE_PIN(i) ((i) == 1) // 1 == Serial TX
#else
#define LOG(x...) do { (void)0; } while (0)
#endif

#if LOG_ENABLED && DEBUG_ENABLED
#define DEBUG(x...) do { x; } while (0)
#else
#define DEBUG(x...) do { (void)0; } while (0)
#endif

// everything written to firmataStream will be readable from firmataInternalStream
// everything written to firmataInternalStream will be readable from firmataStream
// firmataInternalStream is the stream given to Firmata
// websocket functions will read and write on firmataStream
PipedStreamPair pipe; // streamify data coming from websocket
PipedStream& firmataInternalStream = pipe.first;
PipedStream& firmataStream = pipe.second;

WebSocketsServer webSocket(WS_PORT);

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
}

void initTransport()
{
  WiFi.persistent(false);
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);
  LOG(Serial.printf("Connecting to SSID '%s'...\n", STASSID));
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    LOG(Serial.print('.'));
  }
  LOG(Serial.printf("Connected, IP address: %s\n", WiFi.localIP().toString().c_str()));
}

int clients = 0;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  DEBUG(Serial.printf("[ws:%u,t:%d,l:%zd]: ", num, (int)type, length));

  switch(type) {

    case WStype_DISCONNECTED:
      DEBUG(Serial.println("Disconnected!"));
      clients--;
      break;

    case WStype_CONNECTED:
      {
        DEBUG(Serial.printf("Connected from %s url:%s\n", webSocket.remoteIP(num).toString().c_str(), (const char*)payload));
        webSocket.sendTXT(num, "Firmata WebSocket Server ready for your network/wireless service");
        clients++;
      }
      break;

    case WStype_TEXT:
      DEBUG(Serial.printf("Informative Text from WebSocket: '%s'\n", (const char*)payload));
      break;

    case WStype_BIN:
      DEBUG(Serial.printf("binary from WebSocket\n"));
#if 1
      // !! there _is_  a Print::write(ptr,size_t) !?
      for (size_t i = 0; i < length; i++)
      {
        if (firmataStream.write(payload[i]) != 1) {
          LOG(Serial.println("stream error writing to firmataStream"));
        }
      }
#else
      if (firmataStream.write(payload, length) != length) {
        LOG(Serial.println("stream error writing to firmataStream"));
      }
#endif
      break;

    default:
      DEBUG(Serial.println("TODO: unhandled WS message type"));
  }
}

void initWebSocket()
{
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  LOG(Serial.println("WS: initialized"));
}

void initFirmata()
{
  initFirmataCommonBegin();
  ignorePins();
  Firmata.begin(firmataInternalStream);
  initFirmataCommonEnd();
}

void setup()
{
  LOG(Serial.begin(SERIAL_BAUD));

  initTransport();
  initWebSocket();
  initFirmata();
  MDNS.begin("firmata"); // "firmata.local" will be the mDNS name in LAN
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  loopFirmataCommon();
  webSocket.loop();
  MDNS.update();

  size_t firmata2WSLen = firmataStream.available();

#if DEBUG_ENABLED
  static unsigned long last = 0;
  if (millis() - last > 5000)
  {
    last += 1000;
    Serial.printf("Firmata to WS: available=%zd --- WS to Firmata: available=%zd\n",
        firmata2WSLen,
        firmataInternalStream.available());
  }
#endif

  if (clients && firmata2WSLen)
  {
    DEBUG(Serial.printf("Firmata -> WS: %zd bytes\n", firmata2WSLen));

    static byte tempBuffer [MAX_DATA_BYTES];
    if (firmata2WSLen > sizeof(tempBuffer))
      firmata2WSLen = sizeof(tempBuffer);
    if (   firmataStream.readBytes(tempBuffer, firmata2WSLen) == firmata2WSLen
        && webSocket.broadcastBIN(tempBuffer, firmata2WSLen))
    {
      DEBUG(Serial.printf("Successfully broadcasted-to-websocket a binary message of %zd bytes\n", firmata2WSLen));
    }
    else
    {
      LOG(Serial.printf("Error broadcasting-to-websocket a binary message of %zd bytes\n", firmata2WSLen));
    }
  }
}
