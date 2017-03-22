/*
  Stepper is a simple non-blocking stepper motor library
  for 2 and 4 wire bipolar and unipolar stepper motor drive circuits
  as well as EasyDriver (http://schmalzhaus.com/EasyDriver/) and
  other step + direction drive circuits.
  Stepper (0.1) by Jeff Hoefs

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
#ifndef Stepper_h
#define Stepper_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define T1_FREQ 1000000L // provides the most accurate step delay values
#define T1_FREQ_148 ((long)((T1_FREQ*0.676)/100)) // divided by 100 and scaled by 0.676

// library interface description
class Stepper {
public:
	Stepper(byte interface = Stepper::DRIVER,
		int steps_per_rev = 200,
		byte pin1 = 2,
		byte pin2 = 3,
		byte pin3 = 3,
		byte pin4 = 4,
		byte lSwitchA = 0,
		byte lSwitchB = 0,
		bool lSwitchAType = true,
		bool lSwitchBType = true);

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

	void setStepsToMove(long steps_to_move, int speed = -1, int accel = -1, int decel = -1);

	// update the stepper position
	bool update();

	long getPosition();
	long getDistanceTo();
	bool getLimitSwitchState(bool side);
	void setSpeed(int speed);
	void setAcceleration(int accel);
	void setDeceleration(int decel);
	void home();
	void setHome();

	byte version(void);

private:
	void stepMotor(byte step_num, byte direction);
	void updateStepPosition();
	bool _running;
	byte _interface;     // Type of interface: DRIVER, TWO_WIRE or FOUR_WIRE
	byte _direction;        // Direction of rotation
	unsigned long _step_delay;    // delay between steps, in microseconds
	int _steps_per_rev;      // number of steps to make one revolution
	long _step_number;        // which step the motor is on
	long _steps_to_move;   // total number of steps to move

	int _accel;
	int _decel;
	int _speed;

	byte _run_state;
	int _accel_count;
	long _min_delay;
	long _decel_start;
	int _decel_val;

	bool _areLimitSwitches;
	byte _limit_switch_a;
	byte _limit_switch_b;
	bool _switch_a_type;
	bool _switch_b_type;
	bool _a_tripped;
	bool _b_tripped;

	long _lastAccelDelay;
	unsigned long _stepCount;
	long _position;
	unsigned int _rest;

	float _alpha;  // PI * 2 / steps_per_rev
	long _at_x100;  // alpha * T1_FREQ * 100
	long _ax20000;  // alph a* 20000
	float _alpha_x2;  // alpha * 2

	// motor pin numbers:
	byte _dir_pin;
	byte _step_pin;
	byte _motor_pin_1;
	byte _motor_pin_2;
	byte _motor_pin_3;
	byte _motor_pin_4;

	unsigned long _last_step_time; // time stamp in microseconds of when the last step was taken
};

#endif