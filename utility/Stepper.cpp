/**
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

/**
 * Constructor.
 *
 * Configure a stepper for an EasyDriver or other step + direction interface or
 * configure a bipolar or unipolar stepper motor for 2 wire drive mode.
 * Configure a bipolar or unipolar stepper for 4 wire drive mode.
 * @param interface The interface type: Stepper::DRIVER or
 * Stepper::TWO_WIRE
 * @param steps_per_rev The number of steps to make 1 revolution.
 * @param first_pin The direction pin (EasyDriver) or the pin attached to the
 * 1st motor coil (2 wire drive mode)
 * @param second_pin The step pin (EasyDriver) or the pin attached to the 2nd
 * motor coil (2 wire drive mode)
 * @param motor_pin_3 The pin attached to the 3rd motor coil
 * @param motor_pin_4 The pin attached to the 4th motor coil
 */
#include "Stepper.h"

Stepper::Stepper(byte interface,
	int step_per_rev,
	byte pin1,
	byte pin2,
	byte pin3,
	byte pin4,
	byte lSwitchA,
	byte lSwitchB,
	bool lSwitchAType,
	bool lSwitchBType) {

	_step_number = 0;      // which step the motor is on
	_direction = 0;      // motor direction
	_last_step_time = 0;    // time stamp in ms of the last step taken
	_steps_per_rev = step_per_rev;    // total number of steps for this motor
	_running = false;
	_interface = interface; // default to Easy Stepper (or other step + direction driver)

	_motor_pin_1 = pin1;
	_motor_pin_2 = pin2;
	_dir_pin = pin1;
	_step_pin = pin2;

	_accel = 0;
	_decel = 0;
	_speed = 100;
	_position = 0;

	_areLimitSwitches = false;
	_limit_switch_a = lSwitchA;
	_limit_switch_b = lSwitchB;

	if (lSwitchAType){
		pinMode(_limit_switch_a, INPUT_PULLUP);
		_switch_a_type = false; //set to opposite state so we know when the switch is tripped LOW == 0/false
	}
	else {
		pinMode(_limit_switch_a, INPUT);
		_switch_a_type = true; //set to opposite state so we know when the switch is tripped HIGH == a/true
	}

	if (lSwitchBType){
		pinMode(_limit_switch_b, INPUT_PULLUP);
		_switch_b_type = false; //set to opposite state so we know when the switch is tripped LOW == 0/false
	}
	else {
		pinMode(_limit_switch_b, INPUT);
		_switch_b_type = true; //set to opposite state so we know when the switch is tripped HIGH == a/true
	}

	if (_limit_switch_a > 0 || _limit_switch_b > 0) {
		_areLimitSwitches = true;
	}
	// setup the pins on the microcontroller:
	pinMode(_motor_pin_1, OUTPUT);
	pinMode(_motor_pin_2, OUTPUT);

	if (_interface == Stepper::FOUR_WIRE) {
		_motor_pin_3 = pin3;
		_motor_pin_4 = pin4;
		pinMode(_motor_pin_3, OUTPUT);
		pinMode(_motor_pin_4, OUTPUT);
	}


	_alpha = TWO_PI / _steps_per_rev;
	_at_x100 = (long)(_alpha * T1_FREQ * 100);
	_ax20000 = (long)(_alpha * 20000);
	_alpha_x2 = _alpha * 2;

}

//position of the stepper since init or distance since homing
long Stepper::getPosition(){
	return _position;
}

//distance from current target
long Stepper::getDistanceTo(){
	return (_steps_to_move - _stepCount) * (_direction ? 1 : -1);
}

bool Stepper::getLimitSwitchState(bool side){
	return side? _a_tripped : _b_tripped;
}

void Stepper::setSpeed(int speed){
	_speed = speed;
}

void Stepper::setAcceleration(int accel){
	_accel = accel;
}

void Stepper::setDeceleration(int decel){
	_decel = decel;
}

//returns motor to position 0
void Stepper::home(){
	setStepsToMove(-_position);
}

//set current position as the home position
void Stepper::setHome(){
	_position=0;
}
/**
 * Move the stepper a given number of steps at the specified
 * speed (rad/sec), acceleration (rad/sec^2) and deceleration (rad/sec^2).
 *
 * @param steps_to_move The number ofsteps to move the motor
 * @param speed [optional] Max speed in 0.01*rad/sec
 * @param accel [optional] Acceleration in 0.01*rad/sec^2
 * @param decel [optional] Deceleration in 0.01*rad/sec^2
 * for the optional parameters, we will never send a signed value so using -1
 * as a flag is safe
 */
void Stepper::setStepsToMove(long steps_to_move, int speed, int accel, int decel) {
	unsigned long maxStepLimit;
	unsigned long accelerationLimit;

	_step_number = 0;
	_lastAccelDelay = 0;
	_stepCount = 0;
	_rest = 0;
	_position += steps_to_move;

	if (speed != -1)
		_speed = speed;
	if (accel != -1)
		_accel = accel;
	if (decel != -1)
		_decel = decel;

	if (steps_to_move < 0) {
		_direction = Stepper::CCW;
		steps_to_move = -steps_to_move;
	}
	else {
		_direction = Stepper::CW;
	}

	_steps_to_move = steps_to_move;

	// set max speed limit, by calc min_delay
	// min_delay = (alpha / tt)/w
	_min_delay = _at_x100 / _speed;

	// if acceleration or deceleration are not defined
	// start in RUN state and do no decelerate
	if (_accel == 0 || _decel == 0) {
		_step_delay = _min_delay;

		_decel_start = _steps_to_move;
		_run_state = Stepper::RUN;
		_accel_count = 0;
		_running = true;

		return;
	}

	// if only moving 1 step
	if (_steps_to_move == 1) {

		// move one step
		_accel_count = -1;
		_run_state = Stepper::DECEL;

		_step_delay = _min_delay;
		_running = true;
	}
	else if (_steps_to_move != 0) {
		// set initial step delay
		// step_delay = 1/tt * sqrt(2*alpha/accel)
		// step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
		_step_delay = (long)((T1_FREQ_148 * sqrt(_alpha_x2 / _accel)) * 1000);

		// find out after how many steps does the speed hit the max speed limit.
		// maxSpeedLimit = speed^2 / (2*alpha*accel)
		maxStepLimit = (long)_speed*_speed / (long)(((long)_ax20000*_accel) / 100);

		// if we hit max spped limit before 0.5 step it will round to 0.
		// but in practice we need to move at least 1 step to get any speed at all.
		if (maxStepLimit == 0) {
			maxStepLimit = 1;
		}

		// find out after how many steps we must start deceleration.
		// n1 = (n1+n2)decel / (accel + decel)
		accelerationLimit = (long)((_steps_to_move*_decel) / (_accel + _decel));

		// we must accelerate at least 1 step before we can start deceleration
		if (accelerationLimit == 0) {
			accelerationLimit = 1;
		}

		// use the limit we hit first to calc decel
		if (accelerationLimit <= maxStepLimit) {
			_decel_val = accelerationLimit - _steps_to_move;
		}
		else {
			_decel_val = -(long)(maxStepLimit*_accel) / _decel;
		}

		// we must decelerate at least 1 step to stop
		if (_decel_val == 0) {
			_decel_val = -1;
		}

		// find step to start deceleration
		_decel_start = _steps_to_move + _decel_val;

		// if the max spped is so low that we don't need to go via acceleration state.
		if (_step_delay <= _min_delay) {
			_step_delay = _min_delay;
			_run_state = Stepper::RUN;
		}
		else {
			_run_state = Stepper::ACCEL;
		}

		// reset counter
		_accel_count = 0;
		_running = true;
	}
}


bool Stepper::update() {
	bool done = false;
	long newStepDelay;

	if (_limit_switch_a > 0) {
		if (digitalRead(_limit_switch_a) == _switch_a_type){
			_a_tripped = true;
		}
		else {
			_a_tripped = false;
		}
	}
	if (_limit_switch_b > 0) {
		if (digitalRead(_limit_switch_b) == _switch_b_type){
			_b_tripped = true;
		}
		else {
			_b_tripped = false;
		}
	}


	unsigned long curTimeVal = micros();
	long timeDiff = curTimeVal - _last_step_time;

	if (_running == true && timeDiff >= _step_delay) {

		_last_step_time = curTimeVal;

		switch (_run_state) {
		case Stepper::STOP:
			_stepCount = 0;
			_rest = 0;
			if (_running) {
				done = true;
			}
			_running = false;
			break;

		case Stepper::ACCEL:
			updateStepPosition();
			_stepCount++;
			_accel_count++;
			newStepDelay = _step_delay - (((2 * (long)_step_delay) + _rest) / (4 * _accel_count + 1));
			_rest = ((2 * (long)_step_delay) + _rest) % (4 * _accel_count + 1);

			// check if we should start deceleration
			if (_stepCount >= _decel_start) {
				_accel_count = _decel_val;
				_run_state = Stepper::DECEL;
				_rest = 0;
			}
			// check if we hit max speed
			else if (newStepDelay <= _min_delay) {
				_lastAccelDelay = newStepDelay;
				newStepDelay = _min_delay;
				_rest = 0;
				_run_state = Stepper::RUN;
			}
			break;

		case Stepper::RUN:
			updateStepPosition();
			_stepCount++;
			newStepDelay = _min_delay;

			// if no accel or decel was specified, go directly to STOP state
			if (_stepCount >= _steps_to_move) {
				_run_state = Stepper::STOP;
			}
			// check if we should start deceleration
			else if (_stepCount >= _decel_start) {
				_accel_count = _decel_val;
				// start deceleration with same delay that accel ended with
				newStepDelay = _lastAccelDelay;
				_run_state = Stepper::DECEL;
			}
			break;

		case Stepper::DECEL:
			updateStepPosition();
			_stepCount++;
			_accel_count++;

			newStepDelay = _step_delay - (((2 * (long)_step_delay) + _rest) / (4 * _accel_count + 1));
			_rest = ((2 * (long)_step_delay) + _rest) % (4 * _accel_count + 1);

			if (newStepDelay < 0) newStepDelay = -newStepDelay;
			// check if we are at the last step
			if (_accel_count >= 0) {
				_run_state = Stepper::STOP;
			}

			break;
		}

		_step_delay = newStepDelay;

	}

	return done;

}

/**
 * Update the step position.
 * @private
 */
void Stepper::updateStepPosition() {
	// increment or decrement the step number,
	// depending on direction:
	if (_direction == Stepper::CW) {
		_step_number++;
		if (_step_number >= _steps_per_rev) {
			_step_number = 0;
		}
	}
	else {
		if (_step_number <= 0) {
			_step_number = _steps_per_rev;
		}
		_step_number--;
	}

	// step the motor to step number 0, 1, 2, or 3:
	stepMotor(_step_number % 4, _direction);
}

/**
 * Moves the motor forward or backwards.
 * @param step_num For 2 or 4 wire configurations, this is the current step in
 * the 2 or 4 step sequence.
 * @param direction The direction of rotation
 */
void Stepper::stepMotor(byte step_num, byte direction) {
	//since this will run regardless asyncronously we need to have it check for limit switches
	//switch a is assumed to be in the LOW direction, b in the high
	//are there limit switches? are they tripped? is the direction of movement towards them
	if (!_areLimitSwitches || (!_a_tripped && !_b_tripped) || ((_a_tripped && direction == 1) || (_b_tripped && direction == 0))){
		if (_interface == Stepper::DRIVER) {
			digitalWrite(_dir_pin, direction);
			delayMicroseconds(1);
			digitalWrite(_step_pin, LOW);
			delayMicroseconds(1);
			digitalWrite(_step_pin, HIGH);
		}
		else if (_interface == Stepper::TWO_WIRE) {
			switch (step_num) {
			case 0: /* 01 */
				digitalWrite(_motor_pin_1, LOW);
				digitalWrite(_motor_pin_2, HIGH);
				break;
			case 1: /* 11 */
				digitalWrite(_motor_pin_1, HIGH);
				digitalWrite(_motor_pin_2, HIGH);
				break;
			case 2: /* 10 */
				digitalWrite(_motor_pin_1, HIGH);
				digitalWrite(_motor_pin_2, LOW);
				break;
			case 3: /* 00 */
				digitalWrite(_motor_pin_1, LOW);
				digitalWrite(_motor_pin_2, LOW);
				break;
			}
		}
		else if (_interface == Stepper::FOUR_WIRE) {
			switch (step_num) {
			case 0:    // 1010
				digitalWrite(_motor_pin_1, HIGH);
				digitalWrite(_motor_pin_2, LOW);
				digitalWrite(_motor_pin_3, HIGH);
				digitalWrite(_motor_pin_4, LOW);
				break;
			case 1:    // 0110
				digitalWrite(_motor_pin_1, LOW);
				digitalWrite(_motor_pin_2, HIGH);
				digitalWrite(_motor_pin_3, HIGH);
				digitalWrite(_motor_pin_4, LOW);
				break;
			case 2:    //0101
				digitalWrite(_motor_pin_1, LOW);
				digitalWrite(_motor_pin_2, HIGH);
				digitalWrite(_motor_pin_3, LOW);
				digitalWrite(_motor_pin_4, HIGH);
				break;
			case 3:    //1001
				digitalWrite(_motor_pin_1, HIGH);
				digitalWrite(_motor_pin_2, LOW);
				digitalWrite(_motor_pin_3, LOW);
				digitalWrite(_motor_pin_4, HIGH);
				break;
			}
		}
	}
}

/**
 * @return The version number of this library.
 */
byte Stepper::version(void) {
	return 1;
}