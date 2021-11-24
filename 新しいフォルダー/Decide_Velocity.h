#pragma once
#include "Initialize.h"

class Decide_Velocity :public Initialize {
private:
	int _rule1(int ID, int temporaryVelocity);
	int _rule2(int ID, int temporaryVelocity);
	int _rule3(int ID, int temporaryVelocity);
	int _rule4(int ID, int temporaryVelocity);
	void _rule5(int ID);
	double p = 0;
public:
	void Decide_targetvelocity();
};

