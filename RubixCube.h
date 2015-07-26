//RubicCube header- library for running Rubix Cube
//

#pragma once
#include "Arduino.h"

class RubixCube
{
	public:
	RubixCube();
	void playRubixCube();
	private:
	void rest(int);
};