// SimonActuator.h

#pragma once
#include <Arduino.h>
#include "Servo.h"

class SimonActuator
{
protected:
	Servo myServo;
	int servoPin;

	int extendVal;
	int retractVal;
	int stillVal;

	float calibVal;
	float threshold;
	unsigned long debounce;

public:
	typedef enum { TOP, BOTTOM } hallPosition;

	void init(int);

	int hallTopPin;
	int hallBotPin;

	SimonActuator();
	SimonActuator(int servoPin);

	void attachHall(hallPosition, int hallPin);
	void attachLux(int luxPin);
	float calibrate();
	float RawToLux(long);
	float readLux();

	void setDebounce(unsigned long millis);
	void setDirections(int retract, int extend, int still);

	void extend();
	void retract();
	void stop();

	float getCalib();
	float setCalib(float calib);
	float getThreshold();
	float setThreshold(float thresh);

	bool isActive();
	bool isHome();
	bool isHomeLow();
	int luxPin;
};