#Firmata

[![Gitter](https://badges.gitter.im/Join Chat.svg)](https://gitter.im/firmata/arduino?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Firmata is a protocol for communicating with microcontrollers from software on a host computer. The [protocol](https://github.com/firmata/protocol) can be implemented in firmware on any microcontroller architecture as well as software on any host computer software package. The arduino repository described here is a Firmata library for Arduino and Arduino-compatible devices. If you would like to contribute to Firmata, please see the [Contributing](#contributing) section below.

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
  * [https://github.com/tino/pyFirmata]
  * [https://github.com/MrYsLab/PyMata]
* perl
  * [https://github.com/ntruchsess/perl-firmata]
  * [https://github.com/rcaputo/rx-firmata]
* ruby
  * [https://github.com/hardbap/firmata]
  * [https://github.com/PlasticLizard/rufinol]
  * [http://funnel.cc]
* clojure
  * [https://github.com/nakkaya/clodiuno]
  * [https://github.com/peterschwarz/clj-firmata]
* javascript
  * [https://github.com/jgautier/firmata]
  * [http://breakoutjs.com]
  * [https://github.com/rwldrn/johnny-five]
* java
  * [https://github.com/4ntoine/Firmata]
  * [https://github.com/shigeodayo/Javarduino]
  * [https://github.com/kurbatov/firmata4j]
* .NET
  * [https://github.com/SolidSoils/Arduino]
  * [http://www.imagitronics.org/projects/firmatanet/]
* Flash/AS3
  * [http://funnel.cc]
  * [http://code.google.com/p/as3glue/]
* PHP
  * [https://bitbucket.org/ThomasWeinert/carica-firmata]
  * [https://github.com/oasynnoum/phpmake_firmata]
* Haskell
  * [http://hackage.haskell.org/package/hArduino]
* iOS
  * [https://github.com/jacobrosenthal/iosfirmata]
* Dart
  * [https://github.com/nfrancois/firmata]
* Max/MSP
  * [http://www.maxuino.org/]

Note: The above libraries may support various versions of the Firmata protocol and therefore may not support all features of the latest Firmata spec nor all arduino and arduino-compatible boards. Refer to the respective projects for details.

##Updating Firmata in the Arduino IDE
The version of firmata in the Arduino IDE contains an outdated version of Firmata. To update Firmata, download the latest [release](https://github.com/firmata/arduino/releases/tag/v2.4.3) (for Arduino 1.0.x or Arduino 1.5.6 or higher) and replace the existing Firmata folder in your Arduino application. See the instructions below for your platform.

*Note that Arduino 1.5.0 - 1.5.5 are not supported. Please use Arduino 1.5.6 or higher (or Arduino 1.0.5 or 1.0.6).*

###Mac OSX:

The Firmata library is contained within the Arduino package.

1. Navigate to the Arduino application
2. Right click on the application icon and select `Show Package Contents`
3. Navigate to: `/Contents/Resources/Java/libraries/` and replace the existing
`Firmata` folder with latest [Firmata release](https://github.com/firmata/arduino/releases/tag/v2.4.3) (note there is a different download
for Arduino 1.0.x vs 1.6.x)
4. Restart the Arduino application and the latest version of Firmata will be available.

If you are using the Java 7 version of Arduino 1.5.7 or higher, the file path
will differ slightly: `Contents/Java/libraries/Firmata` (no Resources directory).

###Windows:

1. Navigate to `c:/Program\ Files/arduino-1.x/libraries/` and replace the existing
`Firmata` folder with the latest [Firmata release](https://github.com/firmata/arduino/releases/tag/v2.4.3) (note there is a different download
for Arduino 1.0.x vs 1.6.x).
2. Restart the Arduino application and the latest version of Firmata will be available.

*Update the path and arduino version as necessary*

###Linux:

1. Navigate to `~/arduino-1.x/libraries/` and replace the existing
`Firmata` folder with the latest [Firmata release](https://github.com/firmata/arduino/releases/tag/v2.4.3) (note there is a different download
for Arduino 1.0.x vs 1.6.x).
2. Restart the Arduino application and the latest version of Firmata will be available.

*Update the path and arduino version as necessary*

###Using the Source code rather than release archive

Clone this repo directly into the core Arduino libraries directory. If you are using
Arduino 1.5.x or 1.6.x, the repo directory structure will not match the Arduino
library format, however it should still compile as long as you are using Arduino 1.5.7
or higher.

You will first need to remove the existing Firmata library, then clone firmata/arduino
into an empty Firmata directory:

```bash
$ rm -r /Applications/Arduino.app/Contents/Java/libraries/Firmata/
$ git clone git@github.com:firmata/arduino.git /Applications/Arduino.app/Contents/Java/libraries/Firmata/
```

*Update paths if you're using Windows or Linux*

To generate properly formatted versions of Firmata (for Arduino 1.0.x and Arduino 1.6.x), run the
`release.sh` script.


<a name="contributing" />
##Contributing

If you discover a bug or would like to propose a new feature, please open a new [issue](https://github.com/firmata/arduino/issues?sort=created&state=open). Due to the limited memory of standard Arduino boards we cannot add every requested feature to StandardFirmata. Requests to add new features to StandardFirmata will be evaluated by the Firmata developers. However it is still possible to add new features to other Firmata implementations (Firmata is a protocol whereas StandardFirmata is just one of many possible implementations).

To contribute, fork this repository and create a new topic branch for the bug, feature or other existing issue you are addressing. Submit the pull request against the *master* branch.

If you would like to contribute but don't have a specific bugfix or new feature to contribute, you can take on an existing issue, see issues labeled "pull-request-encouraged". Add a comment to the issue to express your intent to begin work and/or to get any additional information about the issue.

You must thoroughly test your contributed code. In your pull request, describe tests performed to ensure that no existing code is broken and that any changes maintain backwards compatibility with the existing api. Test on multiple Arduino board variants if possible. We hope to enable some form of automated (or at least semi-automated) testing in the future, but for now any tests will need to be executed manually by the contributor and reviewers.

Use [Artistic Style](http://astyle.sourceforge.net/) (astyle) to format your code. Set the following rules for the astyle formatter:

```
style = ""
indent-spaces = 2
indent-classes = true
indent-switches = true
indent-cases = true
indent-col1-comments = true
pad-oper = true
pad-header = true
keep-one-line-statements = true
```

If you happen to use Sublime Text, [this astyle plugin](https://github.com/timonwong/SublimeAStyleFormatter) is helpful. Set the above rules in the user settings file.
