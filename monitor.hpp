#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <functional>
#include <chrono>

namespace monitor {

// Type
enum class RotationalSpeedUnit {
	rps,
	rpm
};

enum class PeriodUnit {
	ns,
	us,
	ms,
	s
};

enum class Page {
	monitor,
	help
};

// Functions
void start();
void waitFinish();
void dispMotorId(int motorId);
int dispMotorId();
void dispRotationalSpeedUnit(RotationalSpeedUnit rsu);
RotationalSpeedUnit dispRotationalSpeedUnit();
void dispPeriodUnit(PeriodUnit pu);
PeriodUnit dispPeriodUnit();
void dispPage(Page pg);
Page dispPage();
void atomicEnv(const std::function<void()> &proc);
void termRequest();
const std::chrono::steady_clock::time_point &measureStartCommanderLoad();
void measureStopCommanderLoad();

} // End of "namespace monitor"

#endif // End of "#ifndef MONITOR_HPP"

