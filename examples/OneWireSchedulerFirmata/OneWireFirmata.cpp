#include "OneWireFirmata.h"

void OneWireFirmataClass::oneWireConfig(byte pin, boolean power) {
  ow_device_info *info = &pinOneWire[pin];
  if (info->device==NULL) {
    info->device = new OneWire(pin);
  }
  for (int i=0;i<8;i++) {
    info->addr[i]=0x0;
  }
  info->power = power;
}

void OneWireFirmataClass::handleOneWireRequest(byte subcommand, byte argc, byte *argv) {
  byte pin = argv[0];
  ow_device_info *info = &pinOneWire[pin];
  OneWire *device = info->device;
  if (device) {
    switch(subcommand) {
    case ONEWIRE_SEARCH_REQUEST:
    case ONEWIRE_SEARCH_ALARMS_REQUEST:
      {
        device->reset_search();
        FirmataWrite(START_SYSEX);
        FirmataWrite(ONEWIRE_DATA);
        boolean isAlarmSearch = (subcommand == ONEWIRE_SEARCH_ALARMS_REQUEST);
        FirmataWrite(isAlarmSearch ? (byte)ONEWIRE_SEARCH_ALARMS_REPLY : (byte)ONEWIRE_SEARCH_REPLY);
        FirmataWrite(pin);
        Encoder7Bit.startBinaryWrite();
        byte addrArray[8];
        while (isAlarmSearch ? device->search_alarms(addrArray) : device->search(addrArray)) {
          for (int i=0;i<8;i++) {
            Encoder7Bit.writeBinary(addrArray[i]);
          }
        }
        Encoder7Bit.endBinaryWrite();
        FirmataWrite(END_SYSEX);
        break;
      }
    case ONEWIRE_CONFIG_REQUEST:
      {
        if (argc==2) {
          oneWireConfig(pin, argv[1]);
        }
        break;
      }
    default:
      {
        if (subcommand & ONEWIRE_RESET_REQUEST_BIT) {
          device->reset();
          for (int i=0;i<8;i++) {
            info->addr[i]=0x0;
          }
        }
        if (subcommand & ONEWIRE_SKIP_REQUEST_BIT) {
          device->skip();
          for (byte i=0;i<8;i++) {
            info->addr[i]=0x0;
          }
        }
        if (subcommand & ONEWIRE_WITHDATA_REQUEST_BITS) {
          int numBytes=num7BitOutbytes(argc-1);
          int numReadBytes=0;
          argv++;
          Encoder7Bit.readBinary(numBytes,argv,argv); //decode inplace

          if (subcommand & ONEWIRE_SELECT_REQUEST_BIT) {
            if (numBytes<8) return;
            device->select(argv);
            for (int i=0;i<8;i++) {
              info->addr[i]=argv[i];
            }
            argv+=8;
            numBytes-=8;
          }

          if (subcommand & ONEWIRE_READ_REQUEST_BIT) {
            if (numBytes<2) return;
            numReadBytes = *((int*)argv);
            argv+=2;
            numBytes-=2;
          }

          if (subcommand & ONEWIRE_DELAY_REQUEST_BIT) {
            if (numBytes<4) return;
            FirmataScheduler.delayTask(*((long*)argv));
            argv+=4;
            numBytes-=4;
          }

          if (subcommand & ONEWIRE_WRITE_REQUEST_BIT) {
            for (int i=0;i<numBytes;i++) {
              info->device->write(argv[i],info->power);
            }
          }

          if (numReadBytes>0) {
            FirmataWrite(START_SYSEX);
            FirmataWrite(ONEWIRE_DATA);
            FirmataWrite(ONEWIRE_READ_REPLY);
            FirmataWrite(pin);
            Encoder7Bit.startBinaryWrite();
            for (int i=0;i<8;i++) {
              Encoder7Bit.writeBinary(info->addr[i]);
            }
            for (int i=0;i<numReadBytes;i++) {
              Encoder7Bit.writeBinary(device->read());
            }
            Encoder7Bit.endBinaryWrite();
            FirmataWrite(END_SYSEX);
          }
        }
      }
    }
  }
}

void OneWireFirmataClass::reset() {
  for (int i=0;i<TOTAL_PINS;i++) {
    if (pinOneWire[i].device) {
      free(pinOneWire[i].device);
      pinOneWire[i].device=NULL;
    }
    for (int j=0;j<8;j++) {
      pinOneWire[i].addr[j]=0;
    }
    pinOneWire[i].power=false;
  }
}

OneWireFirmataClass OneWireFirmata;

