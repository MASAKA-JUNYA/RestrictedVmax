#pragma once
#include "Update_Position.h"

class Multilane_TrafficFlow {
private:
	void _DoSimulation(Update_Position* DoSim,int time);
public:
	void Calculation(int filenumber, int lanelength, int Numberofvehicle, int Numberoflane);
};