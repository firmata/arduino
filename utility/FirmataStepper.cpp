/**
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

#include "FirmataStepper.h"

/**
 * Constructor.
 *
 * Configure a stepper for an EasyDriver or other step + direction interface or
 * configure a bipolar or unipolar stepper motor for 2 wire drive mode.
 * Configure a bipolar or unipolar stepper for 4 wire drive mode.
 * @param interface The interface type: FirmataStepper::DRIVER or
 * FirmataStepper::TWO_WIRE
 * @param steps_per_rev The number of steps to make 1 revolution.
 * @param first_pin The direction pin (EasyDriver) or the pin attached to the 
 * 1st motor coil (2 wire drive mode)
 * @param second_pin The step pin (EasyDriver) or the pin attached to the 2nd 
 * motor coil (2 wire drive mode)
 * @param motor_pin_3 The pin attached to the 3rd motor coil
 * @param motor_pin_4 The pin attached to the 4th motor coil 
 */
FirmataStepper::FirmataStepper(byte interface, 
                              int steps_per_rev, 
                              byte pin1, 
                              byte pin2, 
                              byte pin3, 
                              byte pin4) {
  this->step_number = 0;      // which step the motor is on
  this->direction = 0;      // motor direction
  this->last_step_time = 0;    // time stamp in ms of the last step taken
  this->steps_per_rev = steps_per_rev;    // total number of steps for this motor
  this->running = false;
  this->interface = interface; // default to Easy Stepper (or other step + direction driver)

  this->motor_pin_1 = pin1;
  this->motor_pin_2 = pin2;
  this->dir_pin = pin1;
  this->step_pin = pin2;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);

  if (interface == FirmataStepper::FOUR_WIRE) {
    this->motor_pin_3 = pin3;
    this->motor_pin_4 = pin4;    
    pinMode(this->motor_pin_3, OUTPUT);
    pinMode(this->motor_pin_4, OUTPUT);      
  }


  this->alpha = PI_2/this->steps_per_rev;
  this->at_x100 = (long)(this->alpha * T1_FREQ * 100);
  this->ax20000 = (long)(this->alpha*20000);
  this->alpha_x2 = this->alpha * 2;

}

/**
 * Move the stepper a given number of steps at the specified
 * speed (rad/sec), acceleration (rad/sec^2) and deceleration (rad/sec^2).
 *
 * @param steps_to_move The number ofsteps to move the motor
 * @param speed Max speed in 0.01*rad/sec 
 * @param accel [optional] Acceleration in 0.01*rad/sec^2
 * @param decel [optional] Deceleration in 0.01*rad/sec^2
 */
void FirmataStepper::setStepsToMove(long steps_to_move, int speed, int accel, int decel) {
  unsigned long maxStepLimit;
  unsigned long accelerationLimit;

  this->step_number = 0;
  this->lastAccelDelay = 0;
  this->stepCount = 0;
  this->rest = 0;  

  if (steps_to_move < 0) {
    this->direction = FirmataStepper::CCW;
    steps_to_move = -steps_to_move;
  }
  else {
    this->direction = FirmataStepper::CW;
  }

  this->steps_to_move = steps_to_move;

  // set max speed limit, by calc min_delay
  // min_delay = (alpha / tt)/w
  this->min_delay = this->at_x100 / speed;

  // if acceleration or deceleration are not defined
  // start in RUN state and do no decelerate
  if (accel == 0 || decel == 0) {
    this->step_delay = this->min_delay;

    this->decel_start = steps_to_move;
    this->run_state = FirmataStepper::RUN;
    this->accel_count = 0;
    this->running = true;

    return;
  }

  // if only moving 1 step
  if (steps_to_move == 1) {

    // move one step
    this->accel_count = -1;
    this->run_state = FirmataStepper::DECEL;

    this->step_delay = this->min_delay;
    this->running = true;
  }
  else if (steps_to_move != 0) {
    // set initial step delay
    // step_delay = 1/tt * sqrt(2*alpha/accel)
    // step_delay = ( tfreq*0.676/100 )*100 * sqrt( (2*alpha*10000000000) / (accel*100) )/10000
    this->step_delay = (long)((T1_FREQ_148 * sqrt(alpha_x2/accel)) * 1000);   

    // find out after how many steps does the speed hit the max speed limit.
    // maxSpeedLimit = speed^2 / (2*alpha*accel)
    maxStepLimit = (long)speed*speed/(long)(((long)this->ax20000*accel)/100);

    // if we hit max spped limit before 0.5 step it will round to 0.
    // but in practice we need to move at least 1 step to get any speed at all.
    if (maxStepLimit == 0) {
      maxStepLimit = 1;
    }  

    // find out after how many steps we must start deceleration.
    // n1 = (n1+n2)decel / (accel + decel)
    accelerationLimit = (long)((steps_to_move*decel) / (accel+decel));

    // we must accelerate at least 1 step before we can start deceleration
    if (accelerationLimit == 0) {
      accelerationLimit = 1;
    }  

    // use the limit we hit first to calc decel
    if (accelerationLimit <= maxStepLimit) {
      this->decel_val = accelerationLimit - steps_to_move;
    }
    else {
      this->decel_val = -(long)(maxStepLimit*accel)/decel;
    }

    // we must decelerate at least 1 step to stop
    if(this->decel_val == 0) {
      this->decel_val = -1;
    }    

    // find step to start deceleration
    this->decel_start = steps_to_move + this->decel_val;

    // if the max spped is so low that we don't need to go via acceleration state.
    if (this->step_delay <= this->min_delay) {
      this->step_delay = this->min_delay;
      this->run_state = FirmataStepper::RUN;
    }
    else {
      this->run_state = FirmataStepper::ACCEL;
    }

    // reset counter
    this->accel_count = 0;
    this->running = true;
  }
}


bool FirmataStepper::update() {
  bool done = false;
  long newStepDelay;

  unsigned long curTimeVal = micros();
  long timeDiff = curTimeVal - this->last_step_time;

  if (this->running == true && timeDiff >= this->step_delay) {

    this->last_step_time = curTimeVal;

    switch(this->run_state) {
      case FirmataStepper::STOP:
        this->stepCount = 0;
        this->rest = 0;    
        if (this->running) {
          done = true;
        }
        this->running = false;
      break;

      case FirmataStepper::ACCEL:
        updateStepPosition();
        this->stepCount++;
        this->accel_count++;
        newStepDelay = this->step_delay - (((2 * (long)this->step_delay) + this->rest)/(4 * this->accel_count + 1));
        this->rest = ((2 * (long)this->step_delay)+this->rest)%(4 * this->accel_count + 1);

        // check if we should start deceleration
        if (this->stepCount >= this->decel_start) {
          this->accel_count = this->decel_val;
          this->run_state = FirmataStepper::DECEL;
          this->rest = 0;
        }
        // check if we hit max speed
        else if (newStepDelay <= this->min_delay) {
          this->lastAccelDelay = newStepDelay;
          newStepDelay = this->min_delay;
          this->rest = 0;
          this->run_state = FirmataStepper::RUN;
        }
      break;

      case FirmataStepper::RUN:
        updateStepPosition();
        this->stepCount++;
        newStepDelay = this->min_delay;

        // if no accel or decel was specified, go directly to STOP state
        if (stepCount >= this->steps_to_move) {
          this->run_state = FirmataStepper::STOP;
        }
        // check if we should start deceleration
        else if (this->stepCount >= this->decel_start) {
          this->accel_count = this->decel_val;
          // start deceleration with same delay that accel ended with
          newStepDelay = this->lastAccelDelay;
          this->run_state = FirmataStepper::DECEL;
        }
      break;

      case FirmataStepper::DECEL:
        updateStepPosition();
        this->stepCount++;
        this->accel_count++;  

        newStepDelay = this->step_delay - (((2 * (long)this->step_delay) + this->rest)/(4 * this->accel_count + 1));
        this->rest = ((2 * (long)this->step_delay)+this->rest)%(4 * this->accel_count + 1);

        if (newStepDelay < 0) newStepDelay = -newStepDelay;
        // check if we ar at the last step
        if (this->accel_count >= 0) {
          this->run_state = FirmataStepper::STOP;
        }

      break;
    }

    this->step_delay = newStepDelay;

  }

  return done;

}

/**
 * Update the step position.
 * @private
 */
void FirmataStepper::updateStepPosition() {
  // increment or decrement the step number,
  // depending on direction:
  if (this->direction == FirmataStepper::CW) {
    this->step_number++;
    if (this->step_number >= this->steps_per_rev) {
      this->step_number = 0;
    }
  } else {
    if (this->step_number <= 0) {
      this->step_number = this->steps_per_rev;
    }
    this->step_number--;
  }

  // step the motor to step number 0, 1, 2, or 3:
  stepMotor(this->step_number % 4, this->direction);
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
