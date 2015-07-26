#pragma once
#include "Servo.h"
#include "SimonActuator.h"

#define NUM_BUTTONS 5
#define NUM_ROUNDS 10
#define MAX_LIGHT_WAIT 5000
#define MAX_LIGHT_OFF 5000

class Simon_Says_lib
{
public:
	typedef enum { RED, GREEN, BLUE, YELLOW, START, NONE } buttons;
	Simon_Says_lib(int, int);

	SimonActuator redAct;
	SimonActuator blueAct;
	SimonActuator yellowAct;
	SimonActuator greenAct;
	SimonActuator startAct;

	bool addActuator(SimonActuator*, buttons);
	bool playSimon(unsigned long timeOut);
	void calibrateSensors();		// Iterate through all actuators and call their "calibrate" methods.
	void oldSequence(buttons*, int);
	void playSequence(buttons*, int);
	char* playTest(buttons*, int);
	void setup();
	buttons readNextLight();
	buttons start(SimonActuator*);

	void homeTop(unsigned long);
	void homeLow(unsigned long);

	void setRetractTime(unsigned long);
	void setPauseTime(unsigned long);

	bool playSimon();

	char* debug();

	~Simon_Says_lib();		// Destructor

	//char* printArray(int* sequence, int numRounds);

private:

	unsigned long RETRACT_TIME;
	unsigned long PAUSE_TIME;
};