//RubixCube.cpp - library for playing EtchaSketch
//Created by Gabriel Earley on April 4, 2015

#include "Arduino.h"
#include "RubixCube.h"

RubixCube::RubixCube()
{
	pinMode(4, OUTPUT);
}

void RubixCube::playRubixCube()
{

	digitalWrite(4, HIGH);
	rest(175);
	digitalWrite(4, LOW);
	rest(1000);
}

void RubixCube::rest(int restTime){
	unsigned long timer;
	timer = millis();
	while (millis() < timer + restTime){
	}
	return;
}
