/**
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

#include "FirmataStepper.h"

/**
 * Constructor.
 * Sets the default values for a stepper.
 */
FirmataStepper::FirmataStepper() {
  this->step_number = 0;      // which step the motor is on
  this->speed = 0;        // the motor speed, in revolutions per minute
  this->direction = 0;      // motor direction
  this->last_step_time = 0;    // time stamp in ms of the last step taken
  this->number_of_steps = 0;    // total number of steps for this motor
  this->done = false;
  this->running = false;
  this->interface = FirmataStepper::DRIVER; // default to Easy Stepper (or other step + direction driver)
}

/**
 * Configure a stepper for an EasyDriver or other step + direction interface or
 * configure a bipolar or unipolar stepper motor for 2 wire drive mode.
 * @param interface The interface type: FirmataStepper::DRIVER or
 * FirmataStepper::TWO_WIRE
 * @param number_of_steps The number of steps to make 1 revolution.
 * @param first_pin The direction pin (EasyDriver) or the pin attached to the 
 * 1st motor coil (2 wire drive mode)
 * @param second_pin The step pin (EasyDriver) or the pin attached to the 2nd 
 * motor coil (2 wire drive mode)
 */
void FirmataStepper::config(byte interface, int number_of_steps, byte first_pin, byte second_pin) {
  // total number of steps for this motor
  this->number_of_steps = number_of_steps;
  this->interface = interface;

  this->motor_pin_1 = first_pin;
  this->motor_pin_2 = second_pin;
  this->dir_pin = first_pin;
  this->step_pin = second_pin;  

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
}

/**
 * Configure a bipolar or unipolar stepper for 4 wire drive mode.
 * @param interface The interface type: FirmataStepper::DRIVER, 
 * FirmataStepper::TWO_WIRE, or FirmataStepper::FOUR_WIRE
 * @param number_of_steps The number of steps to make 1 revolution.
 * @param motor_pin_1 The pin attached to the 1st motor coil
 * @param motor_pin_2 The pin attached to the 2nd motor coil
 * @param motor_pin_3 The pin attached to the 3rd motor coil
 * @param motor_pin_4 The pin attached to the 4th motor coil
 */
void FirmataStepper::config(byte interface, int number_of_steps, byte motor_pin_1, byte motor_pin_2, byte motor_pin_3, byte motor_pin_4) {
  // total number of steps for this motor
  this->number_of_steps = number_of_steps;
  this->interface = interface;

  this->motor_pin_1 = motor_pin_1;
  this->motor_pin_2 = motor_pin_2;
  this->motor_pin_3 = motor_pin_3;
  this->motor_pin_4 = motor_pin_4;
  
  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);
  pinMode(this->motor_pin_3, OUTPUT);
  pinMode(this->motor_pin_4, OUTPUT);  
}

/**
 * Sets the speed in revolutions per minute.
 * @param speed_rpm The speed in RPM.
 */
void FirmataStepper::setSpeed(int speed_rpm) {
  this->step_delay = 60L * 1000L * 1000L / this->number_of_steps / speed_rpm;
}

/**
 * Set the number of steps to move the motor. If the number is negative, 
 * the motor moves in the reverse direction.
 * @param steps_to_move The number of steps to move the motor.
 */
void FirmataStepper::setNumSteps(long steps_to_move) {    
  // number of steps
  this->seq_steps_left = abs(steps_to_move);

  // determine direction
  if (steps_to_move > 0) {this->direction = 1;}
  if (steps_to_move < 0) {this->direction = 0;}
}

/**
 * Call this as frequently as possible to update stepper position.
 * @return True if step sequence has completed.
 */
bool FirmataStepper::update() {
  done = false;
	if(this->seq_steps_left > 0) {
    updateStepPosition();
    running = true;
	} else{
    if(this->seq_steps_left == 0 && running){
        done = true;
    }
    running = false;
  }
  return done;
}

/**
 * Update the step position.
 * @private
 */
void FirmataStepper::updateStepPosition() {
  if (micros() - this->last_step_time >= this->step_delay) {
    // get the timeStamp of when you stepped:
    this->last_step_time = micros();

    // increment or decrement the step number,
    // depending on direction:
    if (this->direction == 1) {
      this->step_number++;
      if (this->step_number == this->number_of_steps) {
        this->step_number = 0;
      }
    } else {
      if (this->step_number == 0) {
        this->step_number = this->number_of_steps;
      }
      this->step_number--;
    }

    // decrement the steps left:
    this->seq_steps_left--;
    // step the motor to step number 0, 1, 2, or 3:
    stepMotor(this->step_number % 4, this->direction);
  }	
}

/**
 * Moves the motor forward or backwards.
 * @param step_num For 2 or 4 wire configurations, this is the current step in
 * the 2 or 4 step sequence.
 * @param direction The direction of rotation
 */
void FirmataStepper::stepMotor(byte step_num, byte direction) {
  if (this->interface == FirmataStepper::DRIVER) {
    digitalWrite(dir_pin, direction);
	  delayMicroseconds(1);
	  digitalWrite(step_pin, LOW);
    delayMicroseconds(1);
	  digitalWrite(step_pin, HIGH);
  } else if (this->interface == FirmataStepper::TWO_WIRE) {
    switch (step_num) {
      case 0: /* 01 */
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      break;
      case 1: /* 11 */
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, HIGH);
      break;
      case 2: /* 10 */
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      break;
      case 3: /* 00 */
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, LOW);
      break;
    } 
  } else if (this->interface == FirmataStepper::FOUR_WIRE) {
    switch (step_num) {
      case 0:    // 1010
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
      break;
      case 1:    // 0110
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, HIGH);
      digitalWrite(motor_pin_4, LOW);
      break;
      case 2:    //0101
      digitalWrite(motor_pin_1, LOW);
      digitalWrite(motor_pin_2, HIGH);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
      break;
      case 3:    //1001
      digitalWrite(motor_pin_1, HIGH);
      digitalWrite(motor_pin_2, LOW);
      digitalWrite(motor_pin_3, LOW);
      digitalWrite(motor_pin_4, HIGH);
      break;
    }     
  }
}

/**
 * @return The version number of this library.
 */
byte FirmataStepper::version(void) {
  return 1;
}
