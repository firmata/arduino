
This is the Firmata library for the Arduino and Wiring environments.  For
complete documentation, see the Firmata web page:

http://firmata.org

To install Firmata into Arduino, you'll need to delete the included one and
replace it with this one.  First find the existing 'Firmata' library and
either delete or rename it.  Here are some typical locations based on OS:

GNU/Linux: ~/Desktop/arduino-0011/hardware/libraries
Windows:   C:\Program Files\arduino-0011\hardware\libraries
Mac OS X:  /Applications/Arduino.app/Contents/Resources/Java/hardware/libraries
   (to see this, right-click on Arduino.app and select "Show Package Contents")

 0. delete the existing 'Firmata' library in the above location
 1. unzip the Firmata zip file
 2. move the included "Firmata" folder into your Arduino installation.  (Do not
      move the whole "Firmata-2.1beta7" folder, just the included "Firmata" folder.)

Now you can launch Arduino and install a Firmata example firmware.
'StandardFirmata' is probably the best place to start.  You can tell if the
above update succeeded because you'll see the new 'I2CFirmata' example firmata
in with the Firmata examples.
