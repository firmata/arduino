
#include <Servo.h>
#include <Wire.h>
#include <Firmata.h>

#include <ESP8266mDNS.h>        // esp8266 Arduino standard library
#include <WebSocketsServer.h>   // https://github.com/Links2004/arduinoWebSockets
#include <PipedStream.h>        // https://github.com/paulo-raca/ArduinoBufferedStreams

#include <utility/ExampleStandardFirmataCommon.h>

#define LOG_ENABLED     1 // 0 or 1
#define DEBUG_ENABLED   1 // 0 or 1

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

PipedStreamPair pipe; // streamify data coming from websocket
PipedStream& firmataInternalStream = pipe.first;
PipedStream& firmataStream = pipe.second;
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
      // !! there _is_StreamFirmataInternal a Print::write(ptr,size_t) !?
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
  LOG(Serial.begin(115200));

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

  size_t firmataStreamLen = firmataStream.available();

#if 0 && DEBUG_ENABLED
  static unsigned long last = 0;
  if (millis() - last > 5000)
  {
    last += 1000;
    Serial.printf("Firmata to WS: available=%zd --- WS to Firmata: available=%zd\n",
        firmataStreamLen, 
        firmataInternalStream.available());
  }
#endif

  if (clients && firmataStreamLen)
  {
    DEBUG(Serial.printf("Firmata -> WS: %zd bytes\n", firmataStreamLen));

    static byte tempBuffer [MAX_DATA_BYTES];
    if (firmataStreamLen > sizeof(tempBuffer))
      firmataStreamLen = sizeof(tempBuffer);
    if (   firmataStream.readBytes(tempBuffer, firmataStreamLen) == firmataStreamLen
        && webSocket.broadcastBIN(tempBuffer, firmataStreamLen))
    {
      DEBUG(Serial.printf("Successfully broadcasted-to-websocket a binary message of %zd bytes\n", firmataStreamLen));
    }
    else
    {
      LOG(Serial.printf("Error broadcasting-to-websocket a binary message of %zd bytes\n", firmataStreamLen));
    }
  }
}
