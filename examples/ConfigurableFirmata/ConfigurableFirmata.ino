/*
 * Firmata is a generic protocol for communicating with microcontrollers
 * from software on a host computer. It is intended to work with
 * any host computer software package.
 *
 * To download a host software package, please click on the following link
 * to open the download page in your default browser.
 *
 * http://firmata.org/wiki/Download
 */

/*
  Copyright (C) 2006-2008 Hans-Christoph Steiner.  All rights reserved.
  Copyright (C) 2010-2011 Paul Stoffregen.  All rights reserved.
  Copyright (C) 2009 Shigeru Kobayashi.  All rights reserved.
  Copyright (C) 2009-2013 Jeff Hoefs.  All rights reserved.
  Copyright (C) 2013 Norbert Truchsess. All rights reserved.
  Copyright (C) 2014 Nicolas Panel. All rights reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  See file LICENSE.txt for further informations on licensing terms.

  formatted using the GNU C formatting and indenting
*/


#include <Firmata.h>

/*
 * by default Firmata uses the Serial-port (over USB) of Arduino.
 * ConfigurableFirmata may also comunicate over ethernet using tcp/ip.
 * To configure this 'Network Firmata' to use the original WIZ5100-based
 * ethernet-shield or Arduino Ethernet uncomment the includes of 'SPI.h' and 'Ethernet.h':
 */

//#include <SPI.h>
//#include <Ethernet.h>

/*
 * To configure 'Network Firmata' to use an ENC28J60 based board include
 * 'UIPEthernet.h' (no SPI.h required). The UIPEthernet-library can be downloaded
 * from: https://github.com/ntruchsess/arduino_uip
 */

//#include <UIPEthernet.h>

/*
 * To execute Network Firmata on Yun uncomment Bridge.h and YunClient.h.
 * Do not include Ethernet.h or SPI.h in this case.
 * On Yun there's no need to configure local_ip and mac in the sketch
 * as this is configured on the linux-side of Yun.
 */

//#include <Bridge.h>
//#include <YunClient.h>

#if defined ethernet_h || defined UIPETHERNET_H || defined _YUN_CLIENT_H_
/*==============================================================================
 * Network configuration for Network Firmata
 *============================================================================*/
#define NETWORK_FIRMATA
//replace with ip of server you want to connect to, comment out if using 'remote_host'
#define remote_ip IPAddress(192,168,0,1)
//replace with hostname of server you want to connect to, comment out if using 'remote_ip'
#define remote_host "server.local"
//replace with the port that your server is listening on
#define remote_port 3030
//replace with arduinos ip-address. Comment out if Ethernet-startup should use dhcp. Is ignored on Yun
#define local_ip IPAddress(192,168,0,6)
//replace with ethernet shield mac. It's mandatory every device is assigned a unique mac. Is ignored on Yun
const byte mac[] = {0x90, 0xA2, 0xDA, 0x0D, 0x07, 0x02};
#endif

// To configure, save this file to your working directory so you can edit it
// then comment out the include and declaration for any features that you do
// not need below.

// Also note that the current compile size for an Arduino Uno with all of the
// following features enabled is about 22.4k. If you are using an older Arduino
// or other microcontroller with less memory you will not be able to include
// all of the following feature classes.

#include <utility/DigitalInputFirmata.h>
DigitalInputFirmata digitalInput;

#include <utility/DigitalOutputFirmata.h>
DigitalOutputFirmata digitalOutput;

#include <utility/AnalogInputFirmata.h>
AnalogInputFirmata analogInput;

#include <utility/AnalogOutputFirmata.h>
AnalogOutputFirmata analogOutput;

#include <Servo.h> //wouldn't load from ServoFirmata.h in Arduino1.0.3
#include <utility/ServoFirmata.h>
ServoFirmata servo;

#include <Wire.h> //wouldn't load from I2CFirmata.h in Arduino1.0.3
#include <utility/I2CFirmata.h>
I2CFirmata i2c;

#include <utility/OneWireFirmata.h>
OneWireFirmata oneWire;

#include <utility/StepperFirmata.h>
StepperFirmata stepper;

#include <utility/FirmataExt.h>
FirmataExt firmataExt;

#include <utility/FirmataScheduler.h>
FirmataScheduler scheduler;

#include <utility/EncoderFirmata.h>
EncoderFirmata encoder;


// dependencies. Do not comment out the following lines
#if defined AnalogOutputFirmata_h || defined ServoFirmata_h
#include <utility/AnalogWrite.h>
#endif

#if defined AnalogInputFirmata_h || defined I2CFirmata_h || defined EncoderFirmata_h
#include <utility/FirmataReporting.h>
FirmataReporting reporting;
#endif

// dependencies for Network Firmata. Do not comment out.
#ifdef NETWORK_FIRMATA
#if defined remote_ip && defined remote_host
#error "cannot define both remote_ip and remote_host at the same time!"
#endif
#include <utility/EthernetClientStream.h>
#ifdef _YUN_CLIENT_H_
YunClient client;
#else
EthernetClient client;
#endif
#if defined remote_ip && !defined remote_host
#ifdef local_ip
EthernetClientStream stream(client, local_ip, remote_ip, NULL, remote_port);
#else
EthernetClientStream stream(client, IPAddress(0, 0, 0, 0), remote_ip, NULL, remote_port);
#endif
#endif
#if !defined remote_ip && defined remote_host
#ifdef local_ip
EthernetClientStream stream(client, local_ip, IPAddress(0, 0, 0, 0), remote_host, remote_port);
#else
EthernetClientStream stream(client, IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), remote_host, remote_port);
#endif
#endif
#endif

/*==============================================================================
 * FUNCTIONS
 *============================================================================*/

void systemResetCallback()
{
  // initialize a defalt state

  // pins with analog capability default to analog input
  // otherwise, pins default to digital output
  for (byte i = 0; i < TOTAL_PINS; i++) {
    if (IS_PIN_ANALOG(i)) {
#ifdef AnalogInputFirmata_h
      // turns off pullup, configures everything
      Firmata.setPinMode(i, ANALOG);
#endif
    } else if (IS_PIN_DIGITAL(i)) {
#ifdef DigitalOutputFirmata_h
      // sets the output to 0, configures portConfigInputs
      Firmata.setPinMode(i, OUTPUT);
#endif
    }
  }

#ifdef FirmataExt_h
  firmataExt.reset();
#endif
}

/*==============================================================================
 * SETUP()
 *============================================================================*/

void setup()
{
#ifdef NETWORK_FIRMATA
#ifdef _YUN_CLIENT_H_
  Bridge.begin();
#else
#ifdef local_ip
  Ethernet.begin((uint8_t *)mac, local_ip); //start ethernet
#else
  Ethernet.begin((uint8_t *)mac); //start ethernet using dhcp
#endif
#endif
  delay(1000);
#endif
  Firmata.setFirmwareVersion(FIRMATA_MAJOR_VERSION, FIRMATA_MINOR_VERSION);

#if defined AnalogOutputFirmata_h || defined ServoFirmata_h
  /* analogWriteCallback is declared in AnalogWrite.h */
  Firmata.attach(ANALOG_MESSAGE, analogWriteCallback);
#endif

#ifdef FirmataExt_h
#ifdef DigitalInputFirmata_h
  firmataExt.addFeature(digitalInput);
#endif
#ifdef DigitalOutputFirmata_h
  firmataExt.addFeature(digitalOutput);
#endif
#ifdef AnalogInputFirmata_h
  firmataExt.addFeature(analogInput);
#endif
#ifdef AnalogOutputFirmata_h
  firmataExt.addFeature(analogOutput);
#endif
#ifdef ServoFirmata_h
  firmataExt.addFeature(servo);
#endif
#ifdef I2CFirmata_h
  firmataExt.addFeature(i2c);
#endif
#ifdef OneWireFirmata_h
  firmataExt.addFeature(oneWire);
#endif
#ifdef StepperFirmata_h
  firmataExt.addFeature(stepper);
#endif
#ifdef FirmataReporting_h
  firmataExt.addFeature(reporting);
#endif
#ifdef FirmataScheduler_h
  firmataExt.addFeature(scheduler);
#endif
#ifdef EncoderFirmata_h
  firmataExt.addFeature(encoder);
#endif
#endif
  /* systemResetCallback is declared here (in ConfigurableFirmata.ino) */
  Firmata.attach(SYSTEM_RESET, systemResetCallback);

  // Network Firmata communicates with Ethernet-shields over SPI. Therefor all
  // SPI-pins must be set to IGNORE. Otherwise Firmata would break SPI-communication.
  // add Pin 10 and configure pin 53 as output if using a MEGA with Ethernetshield.
  // No need to ignore pin 10 on MEGA with ENC28J60, as here pin 53 should be connected to SS:
#ifdef NETWORK_FIRMATA
  // ignore SPI and pin 4 that is SS for SD-Card on Ethernet-shield
  for (byte i = 0; i < TOTAL_PINS; i++) {
    if (IS_PIN_SPI(i)
        || 4 == i
        // || 10==i //explicitly ignore pin 10 on MEGA as 53 is hardware-SS but Ethernet-shield uses pin 10 for SS
       ) {
      Firmata.setPinMode(i, IGNORE);
    }
  }
  //  pinMode(PIN_TO_DIGITAL(53), OUTPUT); configure hardware-SS as output on MEGA
  pinMode(PIN_TO_DIGITAL(4), OUTPUT); // switch off SD-card bypassing Firmata
  digitalWrite(PIN_TO_DIGITAL(4), HIGH); // SS is active low;

  // start up Network Firmata:
  Firmata.begin(stream);
#else
  // start up the default Firmata using Serial interface:
  Firmata.begin(57600);
#endif
  systemResetCallback();  // reset to default config
}

/*==============================================================================
 * LOOP()
 *============================================================================*/
void loop()
{
#ifdef DigitalInputFirmata_h
  /* DIGITALREAD - as fast as possible, check for changes and output them to the
   * stream buffer using Firmata.write()  */
  digitalInput.report();
#endif

  /* STREAMREAD - processing incoming messagse as soon as possible, while still
   * checking digital inputs.  */
  while (Firmata.available()) {
    Firmata.processInput();
#ifdef FirmataScheduler_h
    if (!Firmata.isParsingMessage()) {
      goto runtasks;
    }
  }
  if (!Firmata.isParsingMessage()) {
runtasks: scheduler.runTasks();
#endif
  }

  /* SEND STREAM WRITE BUFFER - TO DO: make sure that the stream buffer doesn't go over
   * 60 bytes. use a timer to sending an event character every 4 ms to
   * trigger the buffer to dump. */

#ifdef FirmataReporting_h
  if (reporting.elapsed()) {
#ifdef AnalogInputFirmata_h
    /* ANALOGREAD - do all analogReads() at the configured sampling interval */
    analogInput.report();
#endif
#ifdef I2CFirmata_h
    // report i2c data for all device with read continuous mode enabled
    i2c.report();
#endif
#ifdef EncoderFirmata_h
    // report encoders positions if reporting enabled.
    encoder.report();
#endif
  }
#endif
#ifdef StepperFirmata_h
  stepper.update();
#endif
#if defined NETWORK_FIRMATA && !defined local_ip &&!defined _YUN_CLIENT_H_
  if (Ethernet.maintain())
  {
    stream.maintain(Ethernet.localIP());
  }
#endif
}
