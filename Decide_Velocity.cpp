#include "Decide_Velocity.h"

void Decide_Velocity::Decide_targetvelocity() {
	car.canditate_velocity = std::vector<int>(constants.N, 0);
	_ApplyRestriction();
	for (int ID = 0; ID < constants.N; ++ID) {
		int v = car.velocity.current[ID];
		car.S[ID] = constants.S;
		v = _rule1(ID, v);
		v = _rule2(ID, v);
		v = _rule3(ID, v);
		car.canditate_velocity[ID] = _rule4(ID, v);
	}
}

int Decide_Velocity::_rule1(int ID, int v) {
	int VelocityDifference = car.velocity.current[car.around.preceding.current[ID]] - car.velocity.current[ID];
	bool isPrecedingCarFasterorSame = false;
	if (VelocityDifference >= 0) isPrecedingCarFasterorSame = true;
	if (car.headway.current[ID] >= constants.G || isPrecedingCarFasterorSame == true) {
		v = std::min(car.Vmax[ID], v + 1);
	}
	return v;
}

int Decide_Velocity::_rule2(int ID, int v) {
	if (random->random(1.0) <= constants.q) {
		if (random->random(1.0) > constants.r) car.S[ID] = 1;
		int previous_preceedingcarID = car.around.preceding.previous[ID];
		int s = 1;
		for (s; s < car.S[ID]; s++) previous_preceedingcarID = car.around.preceding.previous[previous_preceedingcarID];
		int distance = car.position.previous[previous_preceedingcarID] - car.position.previous[ID];
		distance -= car.S[ID];
		if (distance < 0) distance += constants.lanelength;
		v = std::min(v, distance);
	}
	if (v < 0) {
		std::cout << "error V2 " << ID << std::endl;
		std::cout << v << std::endl;
		std::getchar();
	}
	return v;
}

int Decide_Velocity::_rule3(int ID, int v) {
	if (random->random(1.0) > constants.r) car.S[ID] = 1;
	int preceedingcarID = car.around.preceding.current[ID];
	int s = 1;
	for (s; s < car.S[ID]; s++) preceedingcarID = car.around.preceding.current[preceedingcarID];
	int distance = car.position.current[preceedingcarID] - car.position.current[ID];
	distance -= car.S[ID];
	if (distance < 0) distance += constants.lanelength;
	v = std::min(v, distance);
	//std::cout << ID << ":" << v << "," << distance << std::endl;
	if (v < 0) {
		std::cout << "error V3 " << ID << std::endl;
		std::cout << v << std::endl;
		std::getchar();
	}
	return v;
}

int Decide_Velocity::_rule4(int ID, int v) {
	p = constants.p1;
	int preceedingcarID = car.around.preceding.current[ID];
	int VelocityDifference = car.velocity.current[preceedingcarID] - car.velocity.current[ID];
	if (car.headway.current[ID] < constants.G) {
		if (VelocityDifference > 0) p = constants.p2;
		if (VelocityDifference == 0) p = constants.p3;
		if (VelocityDifference < 0) p = constants.p4;
	}
	if (random->random(1.0) < 1 - p) {
		if (v == 0) v = 0;
		else if (v - 1 > 1) v -= 1;
		else v = 1;
	}
	if (v < 0) {
		std::cout << "error V4 " << ID << std::endl;
		std::cout << v << std::endl;
		std::getchar();
	}
	return v;
}

void Decide_Velocity::_ApplyRestriction() {
	if (CountedJam.empty()) return;
	for (int Lane = 0; Lane < constants.Numberoflane; ++Lane) {
		if (CountedJam[Lane].empty()) continue;
		for (auto& Jam : CountedJam[Lane]) { //for (int i = 0; i < CountedJam[Lane].size(); ++i)?????????
			for (int i = 1; i <= 200; ++i) {
				int specific = car.position.current[Jam.Rear] - i;//??????????????????
				if (specific < 0) specific += constants.lanelength;//?????????????????????Constants
				int Number = -1;
				if (map.recorded.existence.current[Lane][specific]) {//map.recorded.existence.current[??????][????????????]
					Number = map.recorded.ID.current[Lane][specific];
				}
				else continue;//?????????????????????????????????????????????????????????
				int Distance = i - 1;//jam???????????????????????????
				int RestrictedVmax = 5;
				//if (Distance <= 50) RestrictedVmax = 2;
				if (Distance <= 100) RestrictedVmax = 3;
				else if (Distance <= 200) RestrictedVmax = 4;
				car.Vmax[Number] = std::min(RestrictedVmax, car.Vmax[Number]);
			}
		}
	}
	/*for (int ID = 0;ID < constants.lanelength;++ID) {
		bool isPrecedingVehicleEndofJam = true;
		int NumberofVehicletoCheck = 4;
		int PrecedingVehicleID = car.around.preceding.current[ID];
		int CurrentPosition = car.position.current[PrecedingVehicleID];
		for (int i = 0; i < NumberofVehicletoCheck; ++i) {
			if (car.headway.current[PrecedingVehicleID] > 0 or car.velocity.current[PrecedingVehicleID] > 0) {
				isPrecedingVehicleEndofJam = false;
				break;
			}
			PrecedingVehicleID = car.around.preceding.current[PrecedingVehicleID];
		}
		if (!isPrecedingVehicleEndofJam) return;
		for (int i = 1; i <= 200; ++i) {
			int specific = CurrentPosition - i;
			if (specific < 0)specific += constants.lanelength;
			int Number = -1;
			if (map.recorded.existence.current[car.lanenumber.current[ID]][specific]) {//map.recorded.existence.current[????????][???Z??????????????]
				Number = map.recorded.ID.current[car.lanenumber.current[ID]][specific];
			}
			else continue;//?????????????????????????????????@??????????????????????????????????????????????
			int Distance = i - 1;//jam????????????????????????????????????
			int RestrictedVmax = 5;
			if (Distance <= 50) RestrictedVmax = 2;
			else if (Distance <= 100) RestrictedVmax = 3;
			else if (Distance <= 200) RestrictedVmax = 4;
			car.Vmax[Number] = std::min(RestrictedVmax, car.Vmax[Number]);
		}
	}*/
}
 