#RCSwitchFirmata

RCSwitchFirmata is an extension of ConfigurableFirmata for support of the
RCSwitch library. See ConfigurableFirmata documentation for details.

##Usage

By default, all features are enabled. Note however that microcontrollers with
limited memory (< 16k) are not able to support all features simultaneously.

1. Copy RCSwitchFirmata to you Arduino sketch folder (or other working 
directory).
2. In the RCSwitchFirmata.ino sketch, on lines 40 - 118, comment out the 
feature class declaration and associated include for any features you do not 
need for your project. For example, if you don't need servo then comment out 
lines 90 - 91.
3. Save the sketch under a new name for your project, compile and upload.

## Protocol details

[RCSwitch](http://code.google.com/p/rc-switch/) is a library to send and receive messages to/from radio controlled devices. Sender and receiver are referred to as *devices* within the context of this proposal. Multiple devices may be used at the same time; the only requirement is a pin per device. All devices may be used and configured independently. Thus, this proposal separates the main functions *send* and *receive*.

###Send

```
PINMODE_RC_SEND     0x0A


// configuration of sender pin: set protocol
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CONFIG_PROTOCOL  (0x11)
3  senderPin        
4,5 protocol (int)
6 END_SYSEX

// configuration of sender pin: set pulse length
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CONFIG_PULSE_LENGTH (0x12)
3  senderPin        
4,5 pulseLength (int)
6 END_SYSEX

// configuration of sender pin: set repeat transmit
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CONFIG_REPEAT_TRANSMIT (0x14)
3  senderPin        
4,5 repeatTransmit (int)
6 END_SYSEX

// send tristate code (char array)
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CODE_TRISTATE    (0x21)
3  senderPin        
4..n RC data (packed as 7-bit): char array with tristate bits ('0', '1', 'F')
n+1 END_SYSEX

// send long code
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CODE_LONG        (0x22)
3  senderPin
4..n RC data (packed as 7-bit): 2 bytes (int) with the number of bits to send, 4 bytes (long) data bits
10 END_SYSEX

// send char[] code
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CODE_CHAR        (0x24)
3  senderPin        
4..n RC data (packed as 7-bit): char array with characters to send
n+1 END_SYSEX

// send tristate code (packed)
0  START_SYSEX
1  RESERVED_COMMAND (0x00)
2  CODE_TRISTATE_PACKED (0x24)
3  senderPin        
4..n RC data (packed as 7-bit): byte array with 4 tristate bits per byte
n+1 END_SYSEX
```

###Receive

```
PINMODE_RC_RECEIVE     0x0B

// configuration of receiver pin: set receive tolerance (in percent)
0 START_SYSEX
1 RESERVED_COMMAND (0x00)
2 CONFIG_TOLERANCE  (0x31)
3 receiverPin        
4 tolerance
5 END_SYSEX

// configuration of receiver pin: enable raw data
0 START_SYSEX
1 RESERVED_COMMAND (0x00)
2 CONFIG_ENABLE_RAW_DATA (0x32)
3 receiverPin        
4 rawdataEnabled
5 END_SYSEX

// receive message
0 START_SYSEX
1 RESERVED_COMMAND (0x00)
2 MESSAGE (0x41)
3 receiverPin        
4,5,6,7 value (long)
8,9 bitlength (int)
10,11 delay (int) 
12,13 protocol (int)
14-n raw data (int[]) optional, only if rawdataEnabled was set to 1
n+1 END_SYSEX
```

###Tristate bits
RCSwitch supports - besides the types long and char[] - so-called *tristate* bits. A tristate bit has one of the values 0, 1, or F. Each tristate bit is coded as 2 bits as follows:
```
TRISTATE_0              0x00
TRISTATE_F              0x01
TRISTATE_RESERVED       0x02
TRISTATE_1              0x03
```
Thus, 1 byte consisting of 8 bits ABCDEFGH may hold up to 4 tristate bits AB, CD, EF and GH. The leftmost 2 bits represent the first tristate bit, the rightmost 2 bits represent the fourth tristate bit. If less than 4 tristate bits are used, the byte is filled with the unused value 0x02.

