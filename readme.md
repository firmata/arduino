#Firmata

Firmata is a arduino library that establishes a protocol for talking to the Arduino from the host software. The aim is to allow people to completely control the Arduino from software on the host computer.

##Usage

To use on your arduino you just need to upload a sketch included in the arduino IDE: 

File > Examples > Firmata > StandardFirmata

Most of the time you will be interacting with arduino with a client library on the host computers.  Firmata is implemented in a number of popular programming languages.

* procesing
  * [https://github.com/firmata/processing]
* python
  * [https://github.com/firmata/pyduino]
  * [https://github.com/lupeke/python-firmata]
* perl
  * [https://github.com/amimoto/perl-firmata]
  * [https://github.com/rcaputo/rx-firmata]
* ruby 
  * [https://github.com/patcoll/ruby-firmata]
  * [https://github.com/PlasticLizard/rufinol]
* clojure
  * [https://github.com/nakkaya/clodiuno]
* javascript 
  * [https://github.com/jgautier/firmata]
* java 
  * [https://github.com/4ntoine/Firmata]
  * [https://github.com/shigeodayo/Javarduino]
* .NET
  * [http://www.imagitronics.org/projects/firmatanet/]

The version of firmata in the arduino IDE contains the latest stable version of firmata.  If you need to use any updates made to this repository just clone the repo into the location of firmata in the arduino IDE.

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
