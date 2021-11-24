#include "Detect_Jams.h"

void Detect_Jams::DetectJam() {
	CountedJam.clear();
	for (int Lane = 0; Lane < constants.Numberoflane; ++Lane) {
		isChecked.assign(constants.lanelength, false);
		CountedJam.emplace_back();
		if (constants.N == constants.lanelength * constants.Numberoflane) {
			JamInformation Filled;
			Filled.Front = Filled.Rear = car.leadingvehicle[Lane].ID;
			Filled.Length = constants.lanelength;
			CountedJam[Lane].emplace_back(Filled);
			continue;
		}
		_InitialDetect(Lane);
		_ExpandJam(Lane);
	}
}

void Detect_Jams::_InitialDetect(int Lane) {
	int InitialID = car.leadingvehicle[Lane].ID;
	//while (car.headway.current[InitialID] != 0) InitialID = car.around.preceding.current[InitialID];
	int FocalVehicleID = car.around.preceding.current[InitialID];
	JamInformation temporayJam;
	while(FocalVehicleID != InitialID) {
		int cnt = 1;
		temporayJam.Front = temporayJam.Rear = FocalVehicleID;
		while (car.headway.current[FocalVehicleID] == 0 and car.velocity.current[FocalVehicleID] == 0) {
			FocalVehicleID = car.around.preceding.current[FocalVehicleID];
			temporayJam.Front = FocalVehicleID;
			cnt++;
			if (FocalVehicleID == InitialID) break;
		}
		if (cnt >= 5) {
			temporayJam.Length = cnt;
			CandidateJamLeader.emplace_back(temporayJam);
		}
		if (FocalVehicleID == InitialID) break;
		FocalVehicleID = car.around.preceding.current[FocalVehicleID];
	}
	std::sort(CandidateJamLeader.begin(), CandidateJamLeader.end(), [](const JamInformation& fr, const JamInformation& se) {
		return fr.Length < se.Length; //‚ ‚ç‚©‚¶‚ß¸‡‚Åsort‚µ‚Ä‚¨‚¢‚ÄAÅ‰‚Ìa‘Ø’·‚ª’·‚¢“z‚ç‚©‚çˆ—
	});
}

void Detect_Jams::_ExpandJam(int Lane) {
	while (!CandidateJamLeader.empty()) { //Œó•âŽÔ—¼‚ª0‚É‚È‚é‚Ü‚Å’Tõ‚ð‚·‚é
		auto FocalJam = CandidateJamLeader.back(); CandidateJamLeader.pop_back();
		if (isChecked[car.position.current[FocalJam.Front]] or isChecked[car.position.current[FocalJam.Rear]]) continue; //Å‰‚ÉisChecked‚Åæ“ªŽÔ—¼‚Ìposition‚©‚ç”»•ÊÏ‚Ý‚©‚Ç‚¤‚©‚ð”»’è,‚·‚Å‚É’TõÏ‚Ý‚Ìê‡‚Ískip
		isChecked[car.position.current[FocalJam.Front]] = true;
		isChecked[car.position.current[FocalJam.Rear]] = true;
		double MinimumDensity = 0.85; 
		int IncludedVehicle = car.position.current[FocalJam.Front] - car.position.current[FocalJam.Rear] + 1;
		if (IncludedVehicle < 0) IncludedVehicle += constants.lanelength;
		int CurrentJamLength = IncludedVehicle;
		bool isForward = true;
		bool isBackward = true;
		do {
			int PrecedingVehicleID = car.around.preceding.current[FocalJam.Front];
			int FollowingVehicleID = car.around.following.current[FocalJam.Rear];
			if (isForward) {
				if (isChecked[car.position.current[PrecedingVehicleID]]) isForward = false;
				else if ((IncludedVehicle + 1) / ((double)CurrentJamLength + car.headway.current[FocalJam.Front] + 1) >= MinimumDensity and car.velocity.current[PrecedingVehicleID] <= 1) {
					isChecked[car.position.current[PrecedingVehicleID]] = true;
					FocalJam.Front = PrecedingVehicleID;
					CurrentJamLength += car.headway.current[FocalJam.Front] + 1;
					++IncludedVehicle;
					PrecedingVehicleID = car.around.preceding.current[PrecedingVehicleID];
				}
				else isForward = false;
			}
			if (isBackward) {
				if (isChecked[car.position.current[FollowingVehicleID]]) isBackward = false;
				else if ((IncludedVehicle + 1) / ((double)CurrentJamLength + car.headway.current[FollowingVehicleID] + 1) >= MinimumDensity and car.velocity.current[FollowingVehicleID] <= 1) {
					isChecked[car.position.current[FollowingVehicleID]] = true;
					FocalJam.Rear = FollowingVehicleID;
					CurrentJamLength += car.headway.current[FollowingVehicleID] + 1;
					++IncludedVehicle;
					FollowingVehicleID = car.around.preceding.current[FollowingVehicleID];
				}
				else isBackward = false;
			}
		} while ((isForward and isBackward) and IncludedVehicle <= map.eachlanevehicle[Lane]);
		FocalJam.Length = CurrentJamLength;
		CountedJam[Lane].emplace_back(FocalJam);
	}
	std::sort(CountedJam[Lane].begin(), CountedJam[Lane].end(), [](const JamInformation& fr, const JamInformation& se) {
		return fr.Length < se.Length; //ƒ‰ƒ€ƒ_Ž®ƒ\[ƒgA”äŠrŠÖ”‚ð’è‹`‚µ‚È‚­‚Ä‚à‚æ‚¢B
	});
}

Detect_Jams::JamStatistics Detect_Jams::CountJam() {
	JamStatistics ThisTimeStatistics;
	std::vector<int> hantei(constants.lanelength, 0);
	for (int Lane = 0; Lane < constants.Numberoflane; ++Lane) for (auto& Jam : CountedJam[Lane]) {
		int ID = Jam.Rear;
		while (ID != Jam.Front) {
			hantei[ID]++;
			ID = car.around.preceding.current[ID];
		}
		ThisTimeStatistics.AverageJamLength += Jam.Length;
		ThisTimeStatistics.MaximumJamLength = std::max(ThisTimeStatistics.MaximumJamLength, Jam.Length);
		++ThisTimeStatistics.NumberofJam;
	}
	return ThisTimeStatistics;
}