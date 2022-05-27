#include "commander.hpp"

#include "monitor.hpp"
#include "motor_mng.hpp"
#include "display.hpp"

#include <thread>
#include <chrono>

#include <libbldcm.hpp>

using std::chrono::milliseconds;
using std::chrono::nanoseconds;
using std::chrono::steady_clock;
using std::chrono::duration_cast;
using std::this_thread::sleep_until;

namespace commander {

//===== [Labels] =====
// [Filescoped labels]
namespace {
	constexpr milliseconds commandDuration(10);
}

//===== [Objects] ===== 
// [External objects]
// Nothing

// [Filescoped objects]
namespace {
	std::thread th{};
	bool isTermRequest{false};
} // End of filescoped objects


//===== Prototypes =====
// [External prototypes]
// Nothing

// [Filescoped prototypes]
namespace {
	void main();
	void thread();
} // End of filescoped prototypes


// ===== Definitions of Function =====
// [External Functions]
void start()
{
	if (!th.joinable()) {
		th = std::thread{thread}; // Thread: create and move
	}
}

void waitFinish()
{
	if (th.joinable()) {
		th.join();
	}
}


// [Filescoped Functions]
namespace {

void thread()
{
	while (true) {
		// Get base time point
		steady_clock::time_point baseTp{monitor::measureStartCommanderLoad()};

		// Input main
		main();

		// Get mutex
		if (isTermRequest) {
			break;
		}

		// Get end time point
		monitor::measureStopCommanderLoad();

		sleep_until(baseTp + commandDuration);
	}
}

void main()
{
	char cmd{};
	int dispMID_latch{};

	monitor::atomicEnv([&dispMID_latch](){
		dispMID_latch = monitor::dispMotorId();
	});

	if (display::getKey(cmd)) {
		switch (cmd) {
			case 'w':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const bldcm::Rps curRps{motor.rotationalSpeed<bldcm::Rps>()};
						motor.rotationalSpeed(curRps + bldcm::Rps(1));
					});
				}
				break;
			case 'z':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const bldcm::Rps curRps{motor.rotationalSpeed<bldcm::Rps>()};
						if (curRps > bldcm::Rps(0)) {
							motor.rotationalSpeed(curRps - bldcm::Rps(1));
						}
					});
				}
				break;
			case 'W':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const bldcm::Rps curRps{motor.rotationalSpeed<bldcm::Rps>()};
						motor.rotationalSpeed(curRps + bldcm::Rps(6));
					});
				}
				break;
			case 'Z':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const bldcm::Rps curRps{motor.rotationalSpeed<bldcm::Rps>()};
						if (curRps > bldcm::Rps(6)) {
							motor.rotationalSpeed(curRps - bldcm::Rps(6));
						} else if (curRps > bldcm::Rps(0)) {
							motor.rotationalSpeed(bldcm::Rps(0));
						} else {
							// Do nothing
						}
					});
				}
				break;
			case 'x':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						motor.rotationalSpeed(bldcm::Rps(0));
					});
				}
				break;
			case 's':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const int curDuty{motor.pwmDuty()};

						if (curDuty < static_cast<int>(100)) {
							motor.pwmDuty(curDuty + static_cast<int>(1));
						}
					});
				}
				break;
			case 'a':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const int curDuty{motor.pwmDuty()};

						if (curDuty > static_cast<int>(0)) {
							motor.pwmDuty(curDuty - static_cast<int>(1));
						}
					});
				}
				break;
			case 'k':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						if (motor.isStopping()) {
							const int phase{motor.phase()};
							if (phase < static_cast<int>(5)) {
								motor.phase(phase + static_cast<int>(1));
							} else {
								motor.phase(static_cast<int>(0));
							}
						}
					});
				}
				break;
			case 'j':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						if (motor.isStopping()) {
							const int phase{motor.phase()};
							if (phase > static_cast<int>(0)) {
								motor.phase(phase - static_cast<int>(1));
							} else {
								motor.phase(static_cast<int>(5));
							}
						}
					});
				}
				break;
			case 'o':
				if (motor_mng::isAvailable(dispMID_latch)) {
					motor_mng::atomicEnv([dispMID_latch](){
						bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};
						const bool curOutputEnable{motor.outputEnable()};
						motor.outputEnable(!curOutputEnable);
					});
				}
				break;
			case '0':
			case '1':
			case '2':
			case '3':
				monitor::atomicEnv([cmd](){
					monitor::dispMotorId(static_cast<int>(cmd - '0'));
				});
				break;
			case 'r':
				monitor::atomicEnv([](){
					const monitor::RotationalSpeedUnit curRSU{monitor::dispRotationalSpeedUnit()};
					if (curRSU == monitor::RotationalSpeedUnit::rps) {
						monitor::dispRotationalSpeedUnit(monitor::RotationalSpeedUnit::rpm);
					} else if (curRSU == monitor::RotationalSpeedUnit::rpm) {
						monitor::dispRotationalSpeedUnit(monitor::RotationalSpeedUnit::rps);
					} else {
						// Do nothing.
					}
				});
				break;
			case 'p':
				monitor::atomicEnv([](){
					const monitor::PeriodUnit curPU{monitor::dispPeriodUnit()};
					if (curPU == monitor::PeriodUnit::ns) {
						monitor::dispPeriodUnit(monitor::PeriodUnit::us);
					} else if (curPU == monitor::PeriodUnit::us) {
						monitor::dispPeriodUnit(monitor::PeriodUnit::ms);
					} else if (curPU == monitor::PeriodUnit::ms) {
						monitor::dispPeriodUnit(monitor::PeriodUnit::s);
					} else if (curPU == monitor::PeriodUnit::s) {
						monitor::dispPeriodUnit(monitor::PeriodUnit::ns);
					} else {
						// Do nothing.
					}
				});
				break;
			case 'q':
				monitor::termRequest();
				isTermRequest = true;
				break;
			case 'h':
				monitor::atomicEnv([](){
					const monitor::Page curPage{monitor::dispPage()};
					if (curPage == monitor::Page::monitor) {
						monitor::dispPage(monitor::Page::help);
					} else if (curPage == monitor::Page::help) {
						monitor::dispPage(monitor::Page::monitor);
					} else {
						// Do nothing.
					}
				});
				break;
			default:
				// Do nothing
				break;
		}
	}
}

} // End of filescoped functions

} // End of "namespace commander"

