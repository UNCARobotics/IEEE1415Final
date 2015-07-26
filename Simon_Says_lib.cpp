#include <Arduino.h>
#include "Simon_Says_lib.h"
#include "SimonActuator.h"

SimonActuator* actuators[NUM_BUTTONS];
int numActs = 0;

Simon_Says_lib::Simon_Says_lib(int pTime, int rTime)
{
	this->setRetractTime(rTime);
	this->setPauseTime(pTime);
}


void Simon_Says_lib::setup()
{
	analogReference(EXTERNAL);

	pinMode(37, OUTPUT);
	pinMode(40, OUTPUT);
	pinMode(43, OUTPUT);
	pinMode(46, OUTPUT);
	pinMode(34, OUTPUT);

	pinMode(35, INPUT_PULLUP);
	pinMode(36, INPUT_PULLUP);
	pinMode(38, INPUT_PULLUP);
	pinMode(39, INPUT_PULLUP);
	pinMode(41, INPUT_PULLUP);
	pinMode(42, INPUT_PULLUP);
	pinMode(44, INPUT_PULLUP);
	pinMode(45, INPUT_PULLUP);

	redAct.init(40);	   // Attach pins
	blueAct.init(46);
	greenAct.init(37);
	yellowAct.init(43);
	startAct.init(34);

	this->addActuator(&redAct, Simon_Says_lib::RED);	// Add our actuator to the simon object, in the RED position.
	this->addActuator(&blueAct, Simon_Says_lib::BLUE);
	this->addActuator(&greenAct, Simon_Says_lib::GREEN);
	this->addActuator(&yellowAct, Simon_Says_lib::YELLOW);
	this->addActuator(&startAct, Simon_Says_lib::START);

	redAct.attachHall(SimonActuator::TOP, 39);			// The top hall sensor for our actuator is on pin 3.
	redAct.attachHall(SimonActuator::BOTTOM, 38);	    // The bottom hall sensor for our actuator is on pin 4.
	redAct.attachLux(A8);							// The luminosity sensor for our actuator is on analog pin 0.
	redAct.setThreshold(60);

	blueAct.attachHall(SimonActuator::TOP, 45);
	blueAct.attachHall(SimonActuator::BOTTOM, 44);
	blueAct.attachLux(A9);
	blueAct.setThreshold(60);

	greenAct.attachHall(SimonActuator::TOP, 36);
	greenAct.attachHall(SimonActuator::BOTTOM, 35);
	greenAct.attachLux(A5);
	greenAct.setThreshold(60);

	yellowAct.attachHall(SimonActuator::TOP, 42);
	yellowAct.attachHall(SimonActuator::BOTTOM, 41);
	yellowAct.attachLux(A6);
	yellowAct.setThreshold(40);
}

bool Simon_Says_lib::addActuator(SimonActuator* act, buttons color)
{
	if (numActs < NUM_BUTTONS){
		actuators[color] = act;
		numActs++;
		return true;
	}
	else
		return false;
}

void Simon_Says_lib::calibrateSensors(){
	for (int i = RED; i < NUM_BUTTONS - 1; i++)
	{
		actuators[i]->calibrate();
	}
}

void Simon_Says_lib::homeTop(unsigned long timeout)
{
	int homing = NUM_BUTTONS - 1;
	int home[4] = { 1, 1, 1, 1 };
	for (int i = RED; i < NUM_BUTTONS - 1; i++)
	{
		actuators[i]->retract();
		actuators[START]->retract();
	}
	unsigned long timer = millis();
	while (homing > 0 && millis() < (timer + timeout))
	{
		for (int i = RED; i < NUM_BUTTONS - 1; i++)
		{
			if (home[i])
				if (actuators[i]->isHome())
				{
					actuators[START]->stop();
					actuators[i]->stop();
					homing--;
					home[i] = 0;
				}
		}
	}
	for (int i = RED; i < NUM_BUTTONS - 1; i++)     
	{
		actuators[i]->stop();
		
	}
}

void Simon_Says_lib::homeLow(unsigned long timeout)
{
	int homing = NUM_BUTTONS - 1;
	int home[4] = { 1, 1, 1, 1 };
	for (int i = RED; i < NUM_BUTTONS - 1; i++)
	{
		actuators[i]->extend();
	}
	
	unsigned long timer = millis();
	while (homing > 0 && millis() < (timer + timeout))
	{
		for (int i = RED; i < NUM_BUTTONS - 1; i++)
		{
			if (home[i])
				if (actuators[i]->isHomeLow())
				{
					actuators[i]->stop();
					homing--;
					home[i] = 0;
				}
		}
	}
	for (int i = RED; i < NUM_BUTTONS - 1; i++)
	{
		actuators[i]->stop();
	}
}

void Simon_Says_lib::oldSequence(buttons* sequence, int nRound)
{
	nRound++;
	for (int i = 0; i < nRound; i++){
		buttons light = readNextLight();
		if (light == NONE)
		{
			// Do nothing
		}
		else if (light != buttons(sequence[i])){
			// Do nothing
		}
	}
}

void Simon_Says_lib::playSequence(buttons* sequence, int numRounds)
{
	unsigned long timer;
	for (int i = 0; i <= numRounds; i++)
	{
		timer = millis();
		while (millis() < (timer + PAUSE_TIME)){
			// wait to extend for pause time
		}
		actuators[sequence[i]]->extend();
		int count = 0;
		while (!(actuators[sequence[i]]->isActive())){
			// Continue to extend until the light turns on
			count++;
		}
		Serial.print("Saw light after:\t");
		Serial.println(count);
		actuators[sequence[i]]->retract();
		timer = millis();
		while (millis() < (timer + RETRACT_TIME)){
			// Retract for RETRACT_TIME milliseconds
		}
		actuators[sequence[i]]->stop();
	}
}

bool Simon_Says_lib::playSimon(unsigned long timeOut){
	for (int i = 0; i < NUM_BUTTONS; i++)
	{
		actuators[i]->stop();
	}
	this->homeTop(5000);
	this->homeLow(5000);

	calibrateSensors();

	Simon_Says_lib::buttons sequence[NUM_ROUNDS];
	Simon_Says_lib::buttons light;

	int nRound = 0;
	long complete = millis();
	while (millis() < (complete + timeOut)){
		if (nRound == 0)
			light = this->start(actuators[START]);
		else 
			light = this->readNextLight();

		if (light != Simon_Says_lib::NONE){
			sequence[nRound] = light;					// Add the new light to the sequence.
			this->playSequence(sequence, nRound);		// Plays the game sequence.
			this->oldSequence(sequence, nRound);		// Reads the sequence up to the current round as the game plays it back.
		}
		else{
			this->setPauseTime(0);
			this->playSequence(sequence, nRound-1);		// If we miss the light, play the previous sequence then quit (Max time)
			this->homeTop(5000);
			return 0;
		}
		nRound++;
	}
	this->homeTop(5000);
	return 1;
}


Simon_Says_lib::buttons Simon_Says_lib::readNextLight()
{
	unsigned long timer = millis();
	while (millis() < (timer + MAX_LIGHT_WAIT)){							// Don't block the whole robot if we miss the light.  We've lost :(
		for (int i = RED; i < NUM_BUTTONS - 1; i++){

			if (actuators[i]->isActive()){									// Read sensor, check if it's above threshhold.
				timer = millis();
				while (actuators[i]->isActive() && (millis() < (timer + MAX_LIGHT_OFF))){
					// Wait for the light to turn off.
				}
				return buttons(i);
			}
		}
	}
	return NONE;
}
void Simon_Says_lib::setRetractTime(unsigned long time)
{
	this->RETRACT_TIME = time;
}

void Simon_Says_lib::setPauseTime(unsigned long time)
{
	this->PAUSE_TIME = time;
}

Simon_Says_lib::buttons Simon_Says_lib::start(SimonActuator* startAct)
{
	unsigned long timer;
	calibrateSensors();
	startAct->extend();
	buttons val = readNextLight();
	startAct->retract();
	timer = millis();
	while (millis() < (timer + 2 * (RETRACT_TIME))){
		// Retract for RETRACT_TIME milliseconds
	}
	startAct->stop();
	return val;
}

Simon_Says_lib::~Simon_Says_lib()
{
	analogReference(DEFAULT);
}
