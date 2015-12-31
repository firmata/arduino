/*
  BLEStream.h

  Based on BLESerial.cpp by Voita Molda
  https://github.com/vojtamolda/BLEPeripheral/blob/master/examples/uart/BLESerial.h
 */

#ifndef _BLE_STREAM_H_
#define _BLE_STREAM_H_

#include <Arduino.h>
#include <CurieBle.h>

class BLEStream : public BLEPeripheral, public Stream
{
  public:
    BLEStream(unsigned char);

    void begin(...);
    bool poll();
    void end();

    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t byte);
    using Print::write;
    virtual operator bool();

  private:
    bool _connected;
    unsigned long _flushed;
    static BLEStream* _instance;

    size_t _rxHead;
    size_t _rxTail;
    size_t _rxCount() const;
    unsigned char _rxBuffer[256];
    size_t _txCount;
    unsigned char _txBuffer[BLE_MAX_ATTR_DATA_LEN];

    BLEService _uartService = BLEService("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
    BLEDescriptor _uartNameDescriptor = BLEDescriptor("2901", "UART");
    BLECharacteristic _rxCharacteristic = BLECharacteristic("6E400002-B5A3-F393-E0A9-E50E24DCCA9E", BLEWriteWithoutResponse, BLE_MAX_ATTR_DATA_LEN);
    BLEDescriptor _rxNameDescriptor = BLEDescriptor("2901", "RX - Receive Data (Write)");
    BLECharacteristic _txCharacteristic = BLECharacteristic("6E400003-B5A3-F393-E0A9-E50E24DCCA9E", BLENotify, BLE_MAX_ATTR_DATA_LEN);
    BLEDescriptor _txNameDescriptor = BLEDescriptor("2901", "TX - Transfer Data (Notify)");

    void _received(const unsigned char* data, size_t size);
    static void _received(BLECentral& /*central*/, BLECharacteristic& rxCharacteristic);
};

#endif
