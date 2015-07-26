// 
// 
// 

#include "CardPickupLib.h"
#include "SimonActuator.h"

CardPickupLib::CardPickupLib()
{
#define RETURN_TIME 2000
}

void CardPickupLib::init()
{
	pinMode(2, OUTPUT);
	pinMode(3, INPUT_PULLUP);
        cardActuator.init(2);
	cardActuator.attachHall(SimonActuator::BOTTOM, 3);
}

void CardPickupLib::pickupCard()
{
	unsigned long timer;

	
	while (!cardActuator.isHomeLow())
	{
           cardActuator.extend();
	}
	cardActuator.retract();
	timer = millis();
	while (millis() < timer + RETURN_TIME){
	}
	cardActuator.stop();
}
