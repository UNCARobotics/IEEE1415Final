// 
// 
// 

#include "SimonActuator.h"
#include "Servo.h"

SimonActuator::SimonActuator()
{
	setDebounce(1);
	setDirections(180, 0, 90);
	setThreshold(30);

}

SimonActuator::SimonActuator(int pin)
{
	this->servoPin = pin;
	this->myServo.attach(this->servoPin);
	setDebounce(1);
	setDirections(180, 0, 90);
	setThreshold(30);
}

void SimonActuator::attachHall(hallPosition hPos, int pin)
{
	if (hPos == SimonActuator::TOP)
		this->hallTopPin = pin;
	else if (hPos == SimonActuator::BOTTOM)
		this->hallBotPin = pin;
}

void SimonActuator::attachLux(int pin)
{
	this->luxPin = pin;
}

float SimonActuator::calibrate()
{
	float val = 0;
	for (int n = 0; n < 1000; n++)
	{
		val += this->readLux();
	}
	val = val / 1000;
	return setCalib(RawToLux(val));
}

void SimonActuator::extend()
{
	this->myServo.write(this->extendVal);
}

float SimonActuator::getCalib()
{
	return this->calibVal;
}

float SimonActuator::getThreshold()
{
	return this->threshold;
}

void SimonActuator::init(int pin)
{
	this->servoPin = pin;
	this->myServo.attach(this->servoPin);
}

bool SimonActuator::isActive()
{
	float val = RawToLux(this->readLux());

	if (val > (this->calibVal + this->threshold)){

		unsigned long lastRead = millis();
		while (millis() < (lastRead + this->debounce))
		{
			val = RawToLux(this->readLux());
			if (val < (this->calibVal + this->threshold)){
				return false;
			}
		}
		return true;
	}
	return false;
}

bool SimonActuator::isHome()
{
	return !(digitalRead(hallTopPin));
}

bool SimonActuator::isHomeLow()
{
	return !(digitalRead(hallBotPin));
}

float SimonActuator::RawToLux(long raw){
	float rawRange = 1024; // 3.3v
	float logRange = 5.0; // 3.3v = 10^5 lux
	float logLux = raw * logRange / rawRange;
	return pow(10, logLux);
}

float SimonActuator::readLux()
{
	return analogRead(this->luxPin);
}

void SimonActuator::retract()
{
	this->myServo.write(this->retractVal);
}

float SimonActuator::setCalib(float calib)
{
	this->calibVal = calib;
	return (this->calibVal);
}

void SimonActuator::setDebounce(unsigned long mill)
{
	this->debounce = mill;
}

void SimonActuator::setDirections(int retract, int extend, int still)
{
	this->extendVal = extend;
	this->retractVal = retract;
	this->stillVal = still;
}

float SimonActuator::setThreshold(float thresh)
{
	if (thresh >= 0)
		this->threshold = thresh;
	return (this->threshold);
}

void SimonActuator::stop()
{
	this->myServo.write(this->stillVal);
}