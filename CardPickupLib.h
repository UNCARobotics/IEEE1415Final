// CardPickupLib.h

#pragma once
#include "Servo.h"
#include "SimonActuator.h"

class CardPickupLib
{
 protected:
	 //SimonActuator cardActuator;

 public:
         SimonActuator cardActuator;
	 CardPickupLib();
	 void init();

	 void pickupCard();
};
