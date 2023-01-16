/*
    ModbusSerial.cpp - Source for Modbus Serial Library
    Copyright (C) 2014 André Sarmento Barbosa
*/
#include "ModbusSerial.h"

ModbusSerial::ModbusSerial() {

}

bool ModbusSerial::setSlaveId(byte slaveId){
    _slaveId = slaveId;
    return true;
}

byte ModbusSerial::getSlaveId() {
    return _slaveId;
}

bool ModbusSerial::config(Serial_* port, long baud, u_int format, int txPin) {
    this->_port = port;
    this->_txPin = txPin;
    (*port).begin(baud, format);
    while (!(*port));

    if (txPin >= 0) {
        pinMode(txPin, OUTPUT);
        digitalWrite(txPin, LOW);
    }

    if (baud > 19200) {
        _t15 = 750;
        _t35 = 1750;
    } else {
        _t15 = 15000000/baud; // 1T * 1.5 = T1.5
        _t35 = 35000000/baud; // 1T * 3.5 = T3.5
    }

    return true;
}

bool ModbusSerial::receive(byte* frame) {
    //first byte of frame = address
    byte address = frame[0];
    //Last two bytes = crc
    u_int crc = ((frame[_len - 2] << 8) | frame[_len - 1]);

    //Slave Check
    if (address != 0xFF && address != this->getSlaveId()) {
		return false;
	}

    //CRC Check
    if (crc != this->calcCrc(_frame[0], _frame+1, _len-3)) {
		return false;
    }

    //PDU starts after first byte
    //framesize PDU = framesize - address(1) - crc(2)
    this->receivePDU(frame+1);
    //No reply to Broadcasts
    if (address == 0xFF) _reply = MB_REPLY_OFF;
    return true;
}

bool ModbusSerial::send(byte* frame) {
    byte i;

    if (this->_txPin >= 0) {
        digitalWrite(this->_txPin, HIGH);
        delay(1);
    }

    for (i = 0 ; i < _len ; i++) {
        (*_port).write(frame[i]);
    }

    (*_port).flush();
    delayMicroseconds(_t35);

    if (this->_txPin >= 0) {
        digitalWrite(this->_txPin, LOW);
    }
}

bool ModbusSerial::sendPDU(byte* pduframe) {
    if (this->_txPin >= 0) {
        digitalWrite(this->_txPin, HIGH);
        delay(1);
    }

    //Send slaveId
    (*_port).write(_slaveId);

    //Send PDU
    byte i;
    for (i = 0 ; i < _len ; i++) {
        (*_port).write(pduframe[i]);
    }

    //Send CRC
    word crc = calcCrc(_slaveId, _frame, _len);
    (*_port).write(crc >> 8);
    (*_port).write(crc & 0xFF);

    (*_port).flush();
    delayMicroseconds(_t35);

    if (this->_txPin >= 0) {
        digitalWrite(this->_txPin, LOW);
    }
}

word ModbusSerial::task() {
    _len = 0;

    while ((*_port).available() > _len)	{
        _len = (*_port).available();
        delayMicroseconds(_t15);
    }

    if (_len == 0) return 0x0000;

    byte i;
    _frame = (byte*) malloc(_len);
    for (i=0 ; i < _len ; i++) _frame[i] = (*_port).read();
    
    if (this->receive(_frame)) {
        if (_reply == MB_REPLY_NORMAL)
            this->sendPDU(_frame);
        else
        if (_reply == MB_REPLY_ECHO)
            this->send(_frame);
    }
    
    // Calculate the address of the register being read/written
    word addr = (word)_frame[2] << 8 | (word)_frame[3];
    free(_frame);
    _len = 0;
    return addr;
}

word ModbusSerial::calcCrc(byte address, byte* pduFrame, byte pduLen) {
	byte CRCHi = 0xFF, CRCLo = 0x0FF, Index;

    Index = CRCHi ^ address;
    CRCHi = CRCLo ^ _auchCRCHi[Index];
    CRCLo = _auchCRCLo[Index];

    while (pduLen--) {
        Index = CRCHi ^ *pduFrame++;
        CRCHi = CRCLo ^ _auchCRCHi[Index];
        CRCLo = _auchCRCLo[Index];
    }

    return (CRCHi << 8) | CRCLo;
}

