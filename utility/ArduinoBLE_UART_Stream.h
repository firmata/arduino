/*
  ArduinoBLE_UART_Stream.h

  Based on BLEStream.h and the HardwareBLESerial library:
  https://github.com/Uberi/Arduino-HardwareBLESerial
 */

#ifndef _ARDUINO_BLE_UART_STREAM_H_
#define _ARDUINO_BLE_UART_STREAM_H_

#include <ArduinoBLE.h>

#define BLE_ATTRIBUTE_MAX_VALUE_LENGTH 20


class ArduinoBLE_UART_Stream : public Stream
{
  public:
    ArduinoBLE_UART_Stream();

    void setLocalName(const char *localName);
    void setAdvertisingInterval(unsigned short advertisingInterval);
    void setConnectionInterval(unsigned short minConnInterval, unsigned short maxConnInterval);
    void setFlushInterval(int flushInterval);

    void begin();
    bool poll();
    void end();

    // Print overrides
    size_t write(uint8_t byte);
    using Print::write;  // Expose other write variants

    // Stream overrides
    int available();
    int read();
    int peek();
    void flush();

  private:
    void dataReceived(const unsigned char *data, size_t size);

    static void connectedHandler(BLEDevice central);
    static void disconnectedHandler(BLEDevice central);

    static void rxWrittenHandler(BLEDevice central, BLECharacteristic characteristic);

    static void txSubscribedHandler(BLEDevice central, BLECharacteristic characteristic);
    static void txUnsubscribedHandler(BLEDevice central, BLECharacteristic characteristic);

    BLEService uartService;
    BLECharacteristic rxCharacteristic;
    BLECharacteristic txCharacteristic;

    String localName;
    unsigned short advertisingInterval;
    unsigned short minConnInterval;
    unsigned short maxConnInterval;
    int flushInterval;

    bool connected;

    unsigned char rxBuffer[256];
    size_t rxHead;
    size_t rxTail;

    bool txSubscribed;
    unsigned char txBuffer[BLE_ATTRIBUTE_MAX_VALUE_LENGTH];
    size_t txCount;
    unsigned long lastFlushTime;

    static ArduinoBLE_UART_Stream *instance;
};


ArduinoBLE_UART_Stream::ArduinoBLE_UART_Stream() :
  uartService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E"),
  rxCharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse | BLEWrite, BLE_ATTRIBUTE_MAX_VALUE_LENGTH),
  txCharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, BLE_ATTRIBUTE_MAX_VALUE_LENGTH),
  advertisingInterval(0),
  minConnInterval(0),
  maxConnInterval(0),
  flushInterval(100),  // Default flush interval is 100ms
  connected(false),
  rxHead(0),
  rxTail(0),
  txSubscribed(false),
  txCount(0),
  lastFlushTime(0)
{
  instance = this;
}

void ArduinoBLE_UART_Stream::setLocalName(const char *localName)
{
  this->localName = localName;
}

void ArduinoBLE_UART_Stream::setAdvertisingInterval(unsigned short advertisingInterval)
{
  this->advertisingInterval = advertisingInterval;
}

void ArduinoBLE_UART_Stream::setConnectionInterval(unsigned short minConnInterval, unsigned short maxConnInterval)
{
  this->minConnInterval = minConnInterval;
  this->maxConnInterval = maxConnInterval;
}

void ArduinoBLE_UART_Stream::setFlushInterval(int flushInterval)
{
  // The minimum allowed connection interval is 7.5ms, so don't try to flush
  // more frequently than that
  this->flushInterval = max(flushInterval, 8);
}

void ArduinoBLE_UART_Stream::begin()
{
  BLE.begin();

  if (localName.length() > 0) {
    BLE.setLocalName(localName.c_str());
  }
  if (advertisingInterval > 0) {
    BLE.setAdvertisingInterval(advertisingInterval);
  }
  if (minConnInterval > 0 && maxConnInterval > 0) {
    BLE.setConnectionInterval(minConnInterval, maxConnInterval);
  }

  BLE.setEventHandler(BLEConnected, connectedHandler);
  BLE.setEventHandler(BLEDisconnected, disconnectedHandler);

  rxCharacteristic.setEventHandler(BLEWritten, rxWrittenHandler);
  uartService.addCharacteristic(rxCharacteristic);

  txCharacteristic.setEventHandler(BLESubscribed, txSubscribedHandler);
  txCharacteristic.setEventHandler(BLEUnsubscribed, txUnsubscribedHandler);
  uartService.addCharacteristic(txCharacteristic);

  BLE.addService(uartService);
  BLE.setAdvertisedService(uartService);
  BLE.advertise();
}

bool ArduinoBLE_UART_Stream::poll()
{
  if (millis() - lastFlushTime > flushInterval) {
    flush();  // Always calls BLE.poll()
  } else {
    BLE.poll();
  }
  return connected;
}

void ArduinoBLE_UART_Stream::end()
{
  flush();
  txCharacteristic.setEventHandler(BLEUnsubscribed, NULL);
  txCharacteristic.setEventHandler(BLESubscribed, NULL);
  txSubscribed = false;

  rxCharacteristic.setEventHandler(BLEWritten, NULL);
  rxHead = 0;
  rxTail = 0;

  BLE.setEventHandler(BLEDisconnected, NULL);
  BLE.setEventHandler(BLEConnected, NULL);
  connected = false;

  BLE.end();
}

size_t ArduinoBLE_UART_Stream::write(uint8_t byte)
{
  if (!txSubscribed) {
    return 0;
  }
  txBuffer[txCount] = byte;
  txCount++;
  if (txCount == sizeof(txBuffer)) {
    flush();
  }
  return 1;
}

int ArduinoBLE_UART_Stream::available()
{
  return (rxHead - rxTail + sizeof(rxBuffer)) % sizeof(rxBuffer);
}

int ArduinoBLE_UART_Stream::read()
{
  if (rxTail == rxHead) {
    return -1;
  }
  uint8_t byte = rxBuffer[rxTail];
  rxTail = (rxTail + 1) % sizeof(rxBuffer);
  return byte;
}

int ArduinoBLE_UART_Stream::peek()
{
  if (rxTail == rxHead) {
    return -1;
  }
  return rxBuffer[rxTail];
}

void ArduinoBLE_UART_Stream::flush()
{
  if (txCount > 0) {
    txCharacteristic.setValue(txBuffer, txCount);
    txCount = 0;
  }
  lastFlushTime = millis();
  BLE.poll();
}

void ArduinoBLE_UART_Stream::dataReceived(const unsigned char *data, size_t size)
{
  for (size_t i = 0; i < size; i++) {
    rxBuffer[rxHead] = data[i];
    rxHead = (rxHead + 1) % sizeof(rxBuffer);
  }
}

void ArduinoBLE_UART_Stream::connectedHandler(BLEDevice central)
{
  instance->connected = true;
}

void ArduinoBLE_UART_Stream::disconnectedHandler(BLEDevice central)
{
  instance->connected = false;
}

void ArduinoBLE_UART_Stream::rxWrittenHandler(BLEDevice central, BLECharacteristic characteristic)
{
  instance->dataReceived(characteristic.value(), characteristic.valueLength());
}

void ArduinoBLE_UART_Stream::txSubscribedHandler(BLEDevice central, BLECharacteristic characteristic)
{
  instance->txSubscribed = true;
}

void ArduinoBLE_UART_Stream::txUnsubscribedHandler(BLEDevice central, BLECharacteristic characteristic)
{
  instance->txSubscribed = false;
}

ArduinoBLE_UART_Stream * ArduinoBLE_UART_Stream::instance = NULL;


#endif // _ARDUINO_BLE_UART_STREAM_H_
