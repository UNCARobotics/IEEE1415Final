//EtchaSketch.h - Library for running EtchaSketch code.
//Created by Gabriel Earley, March 28 2015.

#ifndef EtchaSketch_h
#define EtchaSketch_h
#include "Arduino.h"

class EtchaSketch
{
public:
	EtchaSketch();
	void PlayEtchaSketch();
	void LowerMechanismToKnobs();
private:
	void LowerMechanismOnKnobs();
	void RaiseMechanismToTop();
	void RaiseMechanismHalfwayUpKnobs();
	void rest(int);
	void microrest(int);
	void RotateMotor(int, int, int, int, int);
	void RotateMotorUPDOWN(int, int, int, int, int);
};
#endif
