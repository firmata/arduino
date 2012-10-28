/*
  FirmataStepper is a simple non-blocking stepper motor library
  for 2 and 4 wire bipolar and unipolar stepper motor drive circuits
  as well as EasyDriver (http://schmalzhaus.com/EasyDriver/) and 
  other step + direction drive circuits.

  FirmataStepper (0.1) by Jeff Hoefs
  
  EasyDriver support based on modifications by Chris Coleman

  Acceleration / Deceleration algorithms and code based on:
  app note: http://www.atmel.com/dyn/resources/prod_documents/doc8017.pdf
  source code: http://www.atmel.com/dyn/resources/prod_documents/AVR446.zip
  
  stepMotor function based on Stepper.cpp Stepper library for
  Wiring/Arduino created by Tom Igoe, Sebastian Gassner
  David Mellis and Noah Shibley.

  Relevant notes from Stepper.cpp:

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

#define PI_2 2*3.14159
#define T1_FREQ 1000000L // provides the most accurate step delay values
#define T1_FREQ_148 ((long)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676

// library interface description
class FirmataStepper {
  public:
    FirmataStepper(byte interface = FirmataStepper::DRIVER, 
                    int steps_per_rev = 200, 
                    byte pin1 = 2, 
                    byte pin2 = 3, 
                    byte pin3 = 3, 
                    byte pin4 = 4);

    enum Interface {
      DRIVER = 1,
      TWO_WIRE = 2,
      FOUR_WIRE = 4
    };

    enum RunState {
      STOP = 0,
      ACCEL = 1,
      DECEL = 2,
      RUN = 3
    };

    enum Direction {
      CCW = 0,
      CW = 1
    };

    void setStepsToMove(long steps_to_move, int speed, int accel=0, int decel=0);

    // update the stepper position
		bool update();

    byte version(void);

  private:
    void stepMotor(byte step_num, byte direction);
    void updateStepPosition();
    bool running;
    byte interface;     // Type of interface: DRIVER, TWO_WIRE or FOUR_WIRE
    byte direction;        // Direction of rotation
    unsigned long step_delay;    // delay between steps, in microseconds
    int steps_per_rev;      // number of steps to make one revolution
    long step_number;        // which step the motor is on
    long steps_to_move;   // total number of teps to move

    byte run_state;
    int accel_count;
    long min_delay;
    long decel_start;
    int decel_val;

    long lastAccelDelay;
    unsigned long stepCount;
    unsigned int rest;    

    float alpha;  // PI * 2 / steps_per_rev
    long at_x100;  // alpha * T1_FREQ * 100
    long ax20000;  // alph a* 20000
    float alpha_x2;  // alpha * 2
    
    // motor pin numbers:
    byte dir_pin;
    byte step_pin;
    byte motor_pin_1;
    byte motor_pin_2;
    byte motor_pin_3;
    byte motor_pin_4;
    
    unsigned long last_step_time; // time stamp in microseconds of when the last step was taken
};

#endif

