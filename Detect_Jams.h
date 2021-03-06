#pragma once
#include "Initialize.h"

class Detect_Jams :public Initialize {
public:
	struct JamInformation {
		int Front;
		int Rear;
		int Length;
	};
	struct JamStatistics {
		int MaximumJamLength;
		double AverageJamLength;
		int NumberofJam;
		JamStatistics operator+(const JamStatistics& other) {
			this->AverageJamLength += other.AverageJamLength;
			this->NumberofJam += other.NumberofJam;
			return *this;
		}
		JamStatistics() {
			this->AverageJamLength = 0;
			this->MaximumJamLength = 0;
			this->NumberofJam = 0;
		}
	};
	std::vector<std::vector<JamInformation>> CountedJam;
	void DetectJam();
	JamStatistics CountJam();
private:
	std::vector<JamInformation> CandidateJamLeader; //IDを挿入
	std::vector<bool> isChecked; //座標を基準に渋滞判定済みかを管理(重複時に対応するため)
	void _InitialDetect(int Lane);
	void _ExpandJam(int Lane);
};

