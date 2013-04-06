#ConfigurableFirmata

ConfigurableFirmata allows you to assemble the features (analog, digital,
servo, i2c, one-wire, etc) you need for your project. This enables us to
provide a greater number of features than StandardFirmata can support.

##Usage

By default, all features are enabled. Note however that microcontrollers with
limited memory (< 16k) are not able to support all features simultaneously.

1. Copy ConfigurableFirmata to you Arduino sketch folder (or other working 
directory).
2. In the ConfigurableFirmata.ino sketch, on lines 41 - 71, comment out the 
feature class declaration and associated include for any features you do not 
need for your project. For example, if you don't need servo then comment out 
lines 53 - 55.
3. Save the sketch under a new name for your project, compile and upload.


##TO DO

- Add description about include dependencies.
- Write up a guide/tutorial for creating new feature classes
- Describe use of IGNORE
- Create a GUI application that generates an .ino file for user selecte features.
