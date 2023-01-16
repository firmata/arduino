/*
    Modbus.h - Header for Modbus Base Library
    Copyright (C) 2014 André Sarmento Barbosa
*/
#include "Arduino.h"

#ifndef MODBUS_H
#define MODBUS_H

#define MAX_REGS     32
#define MAX_FRAME   128
//#define USE_HOLDING_REGISTERS_ONLY

typedef unsigned int u_int;

//Function Codes
enum {
    MB_FC_READ_COILS       = 0x01, // Read Coils (Output) Status 0xxxx
    MB_FC_READ_INPUT_STAT  = 0x02, // Read Input Status (Discrete Inputs) 1xxxx
    MB_FC_READ_REGS        = 0x03, // Read Holding Registers 4xxxx
    MB_FC_READ_INPUT_REGS  = 0x04, // Read Input Registers 3xxxx
    MB_FC_WRITE_COIL       = 0x05, // Write Single Coil (Output) 0xxxx
    MB_FC_WRITE_REG        = 0x06, // Preset Single Register 4xxxx
    MB_FC_WRITE_COILS      = 0x0F, // Write Multiple Coils (Outputs) 0xxxx
    MB_FC_WRITE_REGS       = 0x10, // Write block of contiguous registers 4xxxx
};

//Exception Codes
enum {
    MB_EX_ILLEGAL_FUNCTION = 0x01, // Function Code not Supported
    MB_EX_ILLEGAL_ADDRESS  = 0x02, // Output Address not exists
    MB_EX_ILLEGAL_VALUE    = 0x03, // Output Value not in Range
    MB_EX_SLAVE_FAILURE    = 0x04, // Slave Deive Fails to process request
};

//Reply Types
enum {
    MB_REPLY_OFF    = 0x01,
    MB_REPLY_ECHO   = 0x02,
    MB_REPLY_NORMAL = 0x03,
};

typedef struct TRegister {
    word address;
    word value;
    struct TRegister* next;
} TRegister;

class Modbus {
    private:
        TRegister *_regs_head;
        TRegister *_regs_last;

        void readRegisters(word startreg, word numregs);
        void writeSingleRegister(word reg, word value);
        void writeMultipleRegisters(byte* frame,word startreg, word numoutputs, byte bytecount);
        void exceptionResponse(byte fcode, byte excode);
        #ifndef USE_HOLDING_REGISTERS_ONLY
            void readCoils(word startreg, word numregs);
            void readInputStatus(word startreg, word numregs);
            void readInputRegisters(word startreg, word numregs);
            void writeSingleCoil(word reg, word status);
            void writeMultipleCoils(byte* frame,word startreg, word numoutputs, byte bytecount);
        #endif

        TRegister* searchRegister(word addr);

        void addReg(word address, word value = 0);
        bool Reg(word address, word value);
        word Reg(word address);

    protected:
        byte *_frame;
        byte  _len;
        byte  _reply;
        void receivePDU(byte* frame);

    public:
        Modbus();

        void addHreg(word offset, word value = 0);
        bool Hreg(word offset, word value);
        word Hreg(word offset);

        #ifndef USE_HOLDING_REGISTERS_ONLY
            void addCoil(word offset, bool value = false);
            void addIsts(word offset, bool value = false);
            void addIreg(word offset, word value = 0);

            bool Coil(word offset, bool value);
            bool Ists(word offset, bool value);
            bool Ireg(word offset, word value);

            bool Coil(word offset);
            bool Ists(word offset);
            word Ireg(word offset);
        #endif
};

#endif //MODBUS_H
