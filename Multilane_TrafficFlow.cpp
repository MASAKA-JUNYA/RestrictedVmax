#include "Multilane_TrafficFlow.h"

void Multilane_TrafficFlow::Calculation(int filenumber, int lanelength, int Numberofvehicle, int Numberoflane) {
	int Crate = 0;
	double Cp = (double)Crate / 100;
	int ave = 0;
	std::string filename = "Result/Global" + std::to_string(filenumber) + "_Crate" + std::to_string(Crate) + "_test.dat";
	std::string file_TS = "Result/time-space.csv";
	std::ofstream ofs(filename);
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic)
#endif // _OPENMP
	for (int i = 0; i < 40; ++i) {
		int N = (i + 1) * 75;
		N = 1500;
		int CCar = (int)std::round(Cp * N);
		Update_Position::Measuredinfomation AllResult;
		Update_Position::AverageVelocity AllAverageVelocity;
		Update_Position::JamStatistics AllJamsResult;
		AllAverageVelocity.init();
		Update_Position* DoSim = new Update_Position(filenumber, lanelength, N, Numberoflane, CCar);
		for (int j = 0; j < 1800; ++j) {
			_DoSimulation(DoSim, j);
			AllResult += DoSim->AddThisTimeResult();
			//std::cout << j << std::endl;
		}
		DoSim->DoMeasure = true;
		int JamMaximumcnt = 0;
		for (int j = 0; j < 300; ++j) {
			_DoSimulation(DoSim, j + 1800);
			AllResult += DoSim->AddThisTimeResult();
			AllAverageVelocity += DoSim->AverageVelocityThisTime;
			Update_Position::JamStatistics JamsInfo = DoSim->CountJam();
			AllJamsResult.AverageJamLength += JamsInfo.AverageJamLength;
			AllJamsResult.NumberofJam += JamsInfo.NumberofJam;
			AllJamsResult.MaximumJamLength = std::max(JamsInfo.MaximumJamLength, AllJamsResult.MaximumJamLength);
			//for (int k = 0; k < DoSim->constants.N; ++k) ofsTS << j << "," << DoSim->car.position.current[k] << std::endl;
		}
#ifdef _OPENMP
#pragma omp critical
#endif // _OPENMP
		{
			std::string ResultStr;
			ResultStr += std::to_string(N) + "," + std::to_string((double)N / Numberofvehicle) + "," + std::to_string((double)AllResult.passed / Numberoflane) + "," + std::to_string(AllResult.Lanechange_original) + "," + std::to_string(AllResult.Lanechange_Pushed) + ",";
			ResultStr += std::to_string((double)AllAverageVelocity.Cooperator / (DoSim->Cooperator * 300)) + "," + std::to_string((double)AllAverageVelocity.Defector / (DoSim->Defector * 300)) + ",";
			ResultStr += std::to_string((double)AllJamsResult.NumberofJam / 300) + "," + std::to_string(AllJamsResult.AverageJamLength / AllJamsResult.NumberofJam) + "," + std::to_string(AllJamsResult.MaximumJamLength);
			ofs << ResultStr << std::endl;
			std::cout << ResultStr << std::endl;
		}
		delete DoSim;
	}
}


void Multilane_TrafficFlow::_DoSimulation(Update_Position* DoSim,int time) {
	DoSim->AverageVelocityThisTime.init();
	if (time != 0) DoSim->DetectJam();
	DoSim->Decide_targetvelocity();
	DoSim->car.Fromcurrent_toprevious();
	DoSim->map.Fromcurrent_toprevious();
	DoSim->Update_EachVehiclePosition();
}
;