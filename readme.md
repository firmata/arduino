#Firmata

Firmata is a protocol for communicating with microcontrollers from software on a host computer. The protocol can be implemented in firmware on any microcontroller architecture as well as software on any host computer software package. The arduino repository described here is a Firmata library for Arduino and Arduino-compatible devices.

##Usage

There are two main models of usage of Firmata. In one model, the author of the Arduino sketch uses the various methods provided by the Firmata library to selectively send and receive data between the Arduino device and the software running on the host computer. For example, a user can send analog data to the host using ``` Firmata.sendAnalog(analogPin, analogRead(analogPin)) ``` or send data packed in a string using ``` Firmata.sendString(stringToSend) ```. See File -> Examples -> Firmata -> AnalogFirmata & EchoString respectively for examples.

The second and more common model is to load a general purpose sketch called StandardFirmata on the Arduino board and then use the host computer exclusively to interact with the Arduino board. StandardFirmata is located in the Arduino IDE in File -> Examples -> Firmata.

##Firmata Client Libraries
Most of the time you will be interacting with arduino with a client library on the host computers. Several Firmata client libraries have been implemented in a variety of popular programming languages:

* procesing
  * [https://github.com/firmata/processing]
  * [http://funnel.cc]
* python
  * [https://github.com/firmata/pyduino]
  * [https://github.com/lupeke/python-firmata]
* perl
  * [https://github.com/amimoto/perl-firmata]
  * [https://github.com/rcaputo/rx-firmata]
* ruby 
  * [https://github.com/hardbap/firmata]
  * [https://github.com/PlasticLizard/rufinol]
  * [http://funnel.cc]
* clojure
  * [https://github.com/nakkaya/clodiuno]
* javascript 
  * [https://github.com/jgautier/firmata]
  * [http://breakoutjs.com]
  * [https://github.com/rwldrn/johnny-five]
* java 
  * [https://github.com/4ntoine/Firmata]
  * [https://github.com/shigeodayo/Javarduino]
* .NET
  * [http://www.imagitronics.org/projects/firmatanet/]
* Flash/AS3
  * [http://funnel.cc]
  * [http://code.google.com/p/as3glue/]

Note: The above libraries may support various versions of the Firmata protocol and therefore may not support all features of the latest Firmata spec nor all arduino and arduino-compatible boards. Refer to the respective projects for details.

##Updating Firmata in the Arduino IDE
The version of firmata in the arduino IDE contains the latest stable version of firmata (Arduino 1.0 includes Firmata 2.3).  If you need to use any updates made to this repository just clone the repo into the location of firmata in the arduino IDE.

Mac OSX:

```
rm -r /Applications/Arduino.app/Contents/Resources/Java/libraries/Firmata
git clone git@github.com:firmata/arduino.git /Applications/Arduino.app/Contents/Resources/Java/libraries/Firmata
```

Windows:

```
TODO
```

Linux:

```
TODO
```
