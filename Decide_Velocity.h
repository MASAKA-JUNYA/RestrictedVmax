#pragma once
#include "Detect_Jams.h"

class Decide_Velocity :public Detect_Jams {
private:
	int _rule1(int ID, int temporaryVelocity);
	int _rule2(int ID, int temporaryVelocity);
	int _rule3(int ID, int temporaryVelocity);
	int _rule4(int ID, int temporaryVelocity);
	void _ApplyRestriction();
	double p = 0;
public:
	void Decide_targetvelocity();
};

