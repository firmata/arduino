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
