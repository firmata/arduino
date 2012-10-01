/*
  FirmataStepper is a modification of Stepper.cpp that gives adds
  non-blocking capability and EasyDriver (http://schmalzhaus.com/EasyDriver/) 
  support. As long as you call update() often, you don't have to wait for 
  the stepper to complete its movement before you can make other 
  function calls.
  
  Modifications:
  EasyDriver + Arduino Stepper (0.5) by Jeff Hoefs
  Based on modifications by Chris Coleman and Rob Seward
  
  The original stepper.cpp v0.4 notes:
  
  Stepper.cpp - - Stepper library for Wiring/Arduino - Version 0.4
  
  Original library     (0.1) by Tom Igoe.
  Two-wire modifications   (0.2) by Sebastian Gassner
  Combination version   (0.3) by Tom Igoe and David Mellis
  Bug fix for four-wire   (0.4) by Tom Igoe, bug fix from Noah Shibley  

  Drives a unipolar or bipolar stepper motor using  2 wires or 4 wires

  When wiring multiple stepper motors to a microcontroller,
  you quickly run out of output pins, with each motor requiring 4 connections. 

  By making use of the fact that at any time two of the four motor
  coils are the inverse  of the other two, the number of
  control connections can be reduced from 4 to 2. 

  A slightly modified circuit around a Darlington transistor array or an L293 H-bridge
  connects to only 2 microcontroler pins, inverts the signals received,
  and delivers the 4 (2 plus 2 inverted ones) output signals required
  for driving a stepper motor.

  The sequence of control signals for 4 control wires is as follows:

  Step C0 C1 C2 C3
     1  1  0  1  0
     2  0  1  1  0
     3  0  1  0  1
     4  1  0  0  1

  The sequence of controls signals for 2 control wires is as follows
  (columns C1 and C2 from above):

  Step C0 C1
     1  0  1
     2  1  1
     3  1  0
     4  0  0

  The circuits can be found at 
 
  http://www.arduino.cc/en/Tutorial/Stepper

*/

// ensure this library description is only included once
#ifndef FirmataStepper_h
#define FirmataStepper_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

// library interface description
class FirmataStepper {
  public:
    FirmataStepper();

    enum Interface {
      DRIVER = 1,
      TWO_WIRE = 2,
      FOUR_WIRE = 4
    };

    // configure the stepper (set interface type, num steps/rev, and pin numbers)
    void config(byte interface, int number_of_steps, byte pin1, byte pin2);
    void config(byte interface, int number_of_steps, byte pin1, byte pin2, byte pin3, byte pin4);

    // speed setter method:
    void setSpeed(int speed_rpm);

    // set the number of steps
    void setNumSteps(long number_of_steps);

    // update the stepper position
		bool update();

    byte version(void);

  private:
    void stepMotor(byte step_num, byte direction);
    void updateStepPosition();
    bool done;
    bool running;
    byte interface;     // Type of interface: Driver (step + dir), 2 wire, or 4 wire
    byte direction;        // Direction of rotation
    int speed;          // Speed in RPMs
    unsigned long step_delay;    // delay between steps, in ms, based on speed
    int number_of_steps;      // total number of steps this motor can take
    long step_number;        // which step the motor is on
    long seq_steps_left;   //number of steps left if running a sequence
    
    // motor pin numbers:
    byte dir_pin;
    byte step_pin;
    byte motor_pin_1;
    byte motor_pin_2;
    byte motor_pin_3;
    byte motor_pin_4;
    
    long last_step_time;      // time stamp in ms of when the last step was taken
};

#endif

