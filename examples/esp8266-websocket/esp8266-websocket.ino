
#include <Servo.h>
#include <Wire.h>
#include <Firmata.h>

#include <ESP8266mDNS.h>        // esp8266 Arduino standard library
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets
#include <PipedStream.h>        // https://github.com/paulo-raca/ArduinoBufferedStreams

#include <utility/ExampleStandardFirmataCommon.h>

#define DEBUG_ENABLED   0 // 0 or 1

#if DEBUG_ENABLED
#define DEBUG(x...) do { x; } while (0)
#define IS_IGNORE_PIN(i) ((i) == 1) // 1 == Serial TX
#else
#define DEBUG(x...) do { (void)0; } while (0)
#endif

PipedStreamPair pipe; // streamify data coming from websocket
PipedStream& StreamFirmataInternal = pipe.first;
PipedStream& StreamAsFirmata = pipe.second;
WebSocketsServer webSocket(3031);

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
    WiFi.mode(WIFI_STA);
    WiFi.begin(STASSID, STAPSK);
    DEBUG(Serial.printf("Connecting to SSID '%s'...\n", STASSID));
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        DEBUG(Serial.print('.'));
    }    
    DEBUG(Serial.printf("Connected, IP address: %s\n", WiFi.localIP().toString().c_str()));
}

int clients = 0;

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  DEBUG(Serial.printf("[ws:%u,t:%d,l:%z]: ", num, (int)type, length));

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
      // !! there is a Print::write(ptr,size_t) !?
      //DEBUG(Serial.print("before: ");
      //DEBUG(Serial.println(StreamFirmataInternal.available());
      for (size_t i = 0; i < length; i++)
      {
        //Serial.printf("0x%02x ", (int)payload[i]);
        if (StreamAsFirmata.write(payload[i]) != 1) {
          DEBUG(Serial.println("stream error writing to StreamAsFirmata"));
        }
      }
      //DEBUG(Serial.println();
      //DEBUG(Serial.print("after: ");
      //DEBUG(Serial.println(StreamFirmataInternal.available());
#else
      if (StreamAsFirmata.write(payload, length) != length) {
        DEBUG(Serial.println("stream error writing to StreamAsFirmata"));
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
  DEBUG(Serial.println("WS: initialized"));
}

void initFirmata()
{
  initFirmataCommonBegin();
  ignorePins();

  // Using the pipes stream
  // it is written to by the websocket event function
  // it is read from by the main loop
  Firmata.begin(StreamFirmataInternal);

  initFirmataCommonEnd();
}

void setup()
{
  DEBUG(Serial.begin(115200));

  initTransport();
  initWebSocket();
  initFirmata();
  MDNS.begin("firmata"); // "firmata.local" will be the dns name in LAN
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
  loopFirmataCommon();
  webSocket.loop();
  MDNS.update();

  size_t StreamAsFirmataLen = StreamAsFirmata.available();

#if DEBUG_ENABLED
  static unsigned long last = 0;
  if (millis() - last > 1000)
  {
    last += 1000;
    DEBUG(Serial.printf("Firmata to WS: available=%zd --- WS to Firmata: available=%zd\n",
        StreamAsFirmataLen, 
        StreamFirmataInternal.available()));
  }
#endif

  if (clients && StreamAsFirmataLen)
  {
    DEBUG(Serial.printf("Firmata -> WS: %zd bytes", StreamAsFirmataLen));

    static byte tempBuffer [MAX_DATA_BYTES];
    if (StreamAsFirmataLen > sizeof(tempBuffer))
      StreamAsFirmataLen = sizeof(tempBuffer);
    if (   StreamAsFirmata.readBytes(tempBuffer, StreamAsFirmataLen) == StreamAsFirmataLen
        && webSocket.broadcastBIN(tempBuffer, StreamAsFirmataLen))
    {
      DEBUG(Serial.printf("Successfully broadcasted-to-websocket a binary message of %zd bytes\n", StreamAsFirmataLen));
    }
    else
    {
      DEBUG(Serial.printf("Error broadcasting-to-websocket a binary message of %zd bytes", StreamAsFirmataLen));
    }
  }
}
