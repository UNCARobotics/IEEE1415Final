//EtchaSketch.cpp - library for playing EtchaSketch
//Created by Gabriel Earley on March 28, 2015

#include "Arduino.h"
#include "EtchaSketch.h"

EtchaSketch::EtchaSketch()
{
	//Sets the pins to power the step pin on each chip  
#define LeftKnobPin 29
#define RightKnobPin 26
#define MovementPin 24
	//Sets the pins to power the direction pin on each chip
#define LeftKnobDirectionPin 28
#define RightKnobDirectionPin 25
#define MovementDirectionPin 22
	//Sets the pins to power the sleep pins
#define LeftKnobSleep 30
#define RightKnobSleep 27
#define MovementSleep 23
	//Sets a macro that controls the direction of motor
#define Clockwise LOW
#define CounterClockwise HIGH
#define UP LOW
#define DOWN HIGH
	//Defines steps
#define FullStep 60
#define HalfStep 30
#define QuaterStep 15
#define UPStep1 5000 //Steps to top range 4900-5400
#define UPStep2 450 //Steps Halrange 300-500
#define DOWNStep1 3600//range 3500-3800
#define DOWNStep2 1000//range 800-1200

	//Sets rest for the signal created
	//sleep is the length of the rising edge of the pwm for the two knob motors
#define sleep 1
	//sleep2 is the rest between each motor movement
#define sleep2 1
	//sleep3 is the length of the rising edge of the pwm for the up and down motor 
#define sleep3 200
#define Wakeup 1
#define MotorSleep 1

	pinMode(LeftKnobPin, OUTPUT);
	pinMode(RightKnobPin, OUTPUT);
	pinMode(MovementPin, OUTPUT);
	pinMode(LeftKnobDirectionPin, OUTPUT);
	pinMode(RightKnobDirectionPin, OUTPUT);
	pinMode(MovementDirectionPin, OUTPUT);
	pinMode(LeftKnobSleep, OUTPUT);
	pinMode(RightKnobSleep, OUTPUT);
	pinMode(MovementSleep, OUTPUT);
}

void EtchaSketch::LowerMechanismToKnobs()
{
	RotateMotorUPDOWN(MovementPin, MovementDirectionPin, DOWN, MovementSleep, DOWNStep1);
	return;
}

void EtchaSketch::LowerMechanismOnKnobs()
{
	RotateMotorUPDOWN(MovementPin, MovementDirectionPin, DOWN, MovementSleep, DOWNStep2);
	return;
}

void EtchaSketch::RaiseMechanismHalfwayUpKnobs()
{
	RotateMotorUPDOWN(MovementPin, MovementDirectionPin, UP, MovementSleep, UPStep2);
	return;
}

void EtchaSketch::RaiseMechanismToTop()
{
	RotateMotorUPDOWN(MovementPin, MovementDirectionPin, UP, MovementSleep, UPStep1);
	return;

}

void EtchaSketch::PlayEtchaSketch()
{
	LowerMechanismOnKnobs();
	RaiseMechanismHalfwayUpKnobs();

	////Start of First I 

	rest(MotorSleep);
	// BEGIN I Step Right 1 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, FullStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue I Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue I Step UP 1 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, Clockwise, FullStep, RightKnobSleep);

	rest(MotorSleep);
	// Continue I Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Finish I Step Right 1 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, FullStep, LeftKnobSleep);

	rest(MotorSleep);
	// Head towards the First E Step Right 1/4 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, QuaterStep, LeftKnobSleep);


	////Start of First E

	rest(MotorSleep);
	// Start First E Step Down 1/2 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, CounterClockwise, HalfStep, RightKnobSleep);

	rest(MotorSleep);
	// Complete middle of First E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Complete middle of First E Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue First E Step Down 1/2 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, CounterClockwise, HalfStep, RightKnobSleep);

	rest(MotorSleep);
	//Finish the bottem of the First E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	//Finish the bottem of the First E Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Go back to Top of First E Step UP 1 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, Clockwise, FullStep, RightKnobSleep);

	rest(MotorSleep);
	// Head Towards Second E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Head towards the Second E Step Right 1/4 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, QuaterStep, LeftKnobSleep);

	////Start of Second E

	rest(MotorSleep);
	// Start Second E Step Down 1/2 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, CounterClockwise, HalfStep, RightKnobSleep);

	rest(MotorSleep);
	// Complete middle of Second E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Complete middle of Second E Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue Second E Step Down 1/2 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, CounterClockwise, HalfStep, RightKnobSleep);

	rest(MotorSleep);
	//Finish the bottem of the Second E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	//Finish the bottem of the Second E Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Go back to Top of Second E Step UP 1 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, Clockwise, FullStep, RightKnobSleep);

	rest(MotorSleep);
	// Head Towards the Third E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Head towards the Third E Step Right 1/4 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, QuaterStep, LeftKnobSleep);

	////Start of Third E

	rest(MotorSleep);
	// Start the Third E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue the Third E Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue Third E Step Down 1/2 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, CounterClockwise, HalfStep, RightKnobSleep);

	rest(MotorSleep);
	// Complete middle of Third E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Complete middle of Third E Step Left 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, CounterClockwise, HalfStep, LeftKnobSleep);

	rest(MotorSleep);
	// Continue Third E Step Down 1/2 unit steps:
	RotateMotor(RightKnobPin, RightKnobDirectionPin, CounterClockwise, HalfStep, RightKnobSleep);

	rest(MotorSleep);
	//Finish the bottem of the Third E Step Right 1/2 unit steps:
	RotateMotor(LeftKnobPin, LeftKnobDirectionPin, Clockwise, HalfStep, LeftKnobSleep);

	RaiseMechanismToTop();
	return;
}

void EtchaSketch::RotateMotor(int MotorPin, int DirectionPin, int Direction, int Steps, int SleepPin)
{
	rest(sleep2);
	digitalWrite(SleepPin, HIGH);
	rest(sleep2);
	digitalWrite(DirectionPin, Direction);
	rest(sleep2);
	int i = 0;
	while (i < Steps){
		digitalWrite(MotorPin, HIGH);
		rest(sleep);
		digitalWrite(MotorPin, LOW);
		rest(sleep);
		i++;
	}
	digitalWrite(SleepPin, LOW);
	return;
}

void EtchaSketch::RotateMotorUPDOWN(int MotorPin, int DirectionPin, int Direction, int SleepPin, int Steps)
{
	int i = 0;
	rest(sleep2);
	digitalWrite(SleepPin, HIGH);
	rest(sleep2);
	digitalWrite(DirectionPin, Direction);
	rest(sleep2);
	while (Steps > i){
		digitalWrite(MotorPin, HIGH);
		microrest(sleep3);
		digitalWrite(MotorPin, LOW);
		microrest(sleep3);
		i++;
	}
	digitalWrite(SleepPin, LOW);
	return;
}


void EtchaSketch::rest(int restTime){
	unsigned long timer;
	timer = millis();
	while (millis() < timer + restTime){
	}
	return;
}

void EtchaSketch::microrest(int microresttime){
	unsigned long microtimer;
	microtimer = micros();
	while (micros() < microtimer + microresttime){
	}
	return;
}














