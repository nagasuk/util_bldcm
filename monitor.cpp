#include "monitor.hpp"
#include "motor_mng.hpp"
#include "display.hpp"
#include "parameter.hpp"

#include <libbldcm.hpp>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <string>
#include <mutex>
#include <functional>
#include <thread>

using std::stringstream;
using std::mutex;
using std::lock_guard;
using std::chrono::steady_clock;
using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::duration_cast;
using std::this_thread::sleep_until;
using std::boolalpha;
using std::fixed;
using std::setw;
using std::setprecision;
using std::string;
using std::function;

namespace monitor {

//===== [Labels] =====
// [External labels]
// Nothing

// [Filescoped labels]
namespace {
	constexpr Page InitialDispPage = Page::monitor;
	constexpr std::chrono::milliseconds monitorDuration(100);
}

//===== [Classes] =====
// [External classes(implementation)]
// Nothing

// [Filescoped classes]
namespace {

class ProcLoad {
	public:
		// Constructors/Destructors
		ProcLoad() {}
		explicit ProcLoad(double load, double maxLoad) noexcept(true)
			: _load(load), _maxLoad(maxLoad) {}
		~ProcLoad() noexcept(true) {}

		// Methods
		const double &load() const noexcept(true)
		{
			return this->_load;
		}

		const double &maxLoad() const noexcept(true)
		{
			return this->_maxLoad;
		}

		void measureStart() noexcept(true)
		{
			this->_startTp = steady_clock::now();
		}

		void measureStop() noexcept(true)
		{
			this->_stopTp = steady_clock::now();
		}

		void updateLoad() noexcept(true)
		{
			nanoseconds elapsedTime{duration_cast<nanoseconds>(this->_stopTp - this->_startTp)};
			this->_load = static_cast<double>(elapsedTime.count() * 100) /
			              static_cast<double>(duration_cast<nanoseconds>(monitorDuration).count());

			if (this->_load > this->_maxLoad) {
				this->_maxLoad = this->_load;
			}
		}

		const steady_clock::time_point &startTp() const noexcept(true)
		{
			return this->_startTp;
		}

		const steady_clock::time_point &stopTp() const noexcept(true)
		{
			return this->_stopTp;
		}

	private:
		double _load = static_cast<double>(0.);
		double _maxLoad = static_cast<double>(0.);
		std::chrono::steady_clock::time_point _startTp;
		std::chrono::steady_clock::time_point _stopTp;
};

} // End of filescoped classes

//===== [Objects] ===== 
// [External objects]
// Nothing

// [Filescoped objects]
namespace {
	mutex mtx{};

	bool isTermRequest{false};

	RotationalSpeedUnit dispRSU{};
	PeriodUnit          dispPU{};
	Page                dispPG{InitialDispPage};
	int                 dispMID{static_cast<int>(0)};

	ProcLoad commanderProcLoad{};
	ProcLoad monitorProcLoad{};

	std::thread th{};
} // End of filescoped objects


//===== Prototypes =====
// [External prototypes]
// Nothing

// [Filescoped prototypes]
namespace {
	void thread();
	void dispMonitor();
	void dispHelp();
} // End of filescoped prototypes


// ===== Definitions of Function =====
// [External Functions]
void start()
{
	// Initialize parameters
	const parameter::MonitorParameter &param = parameter::monitorParameter();

	dispRSU = param.initDisp.rsu;
	dispPU  = param.initDisp.pu;

	// Start monitor thread
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

void dispMotorId(int motorId)
{
	dispMID = motorId;
}

int dispMotorId()
{
	return dispMID;
}

void dispRotationalSpeedUnit(RotationalSpeedUnit rsu)
{
	dispRSU = rsu;
}

RotationalSpeedUnit dispRotationalSpeedUnit()
{
	return dispRSU;
}

void dispPeriodUnit(PeriodUnit pu)
{
	dispPU = pu;
}

PeriodUnit dispPeriodUnit()
{
	return dispPU;
}

void dispPage(Page pg)
{
	dispPG = pg;
}

Page dispPage()
{
	return dispPG;
}

void atomicEnv(const function<void()> &proc)
{
	lock_guard<mutex> lock{mtx};
	proc();
}

const steady_clock::time_point &measureStartCommanderLoad()
{
	commanderProcLoad.measureStart();
	return commanderProcLoad.startTp();
}

void measureStopCommanderLoad()
{
	commanderProcLoad.measureStop();

	{
		lock_guard<mutex> lock{mtx};
		commanderProcLoad.updateLoad();
	}
}

void termRequest()
{
	lock_guard<mutex> lock{mtx};
	isTermRequest = true;
}

// [Filescoped Functions]
namespace {
void thread()
{
	while (true) {
		static int count{static_cast<int>(0)};
		static Page preDisp{InitialDispPage};
		Page disp{};


		// Get base time point
		monitorProcLoad.measureStart();

		{
			lock_guard<mutex> lock{mtx};
			disp = dispPG;
		}

		display::erase();
		// Sometime display clear
		if (count == static_cast<int>(5)) {
			count = static_cast<int>(0);
			display::clear();
		}
		count++;

		// Clear display if page is switched
		if (disp != preDisp) {
			display::clear();
		}
		preDisp = disp;

		if (disp == Page::monitor) {
			dispMonitor(); // Do monitor
		} else if (disp == Page::help) {
			dispHelp(); // Display help
		} else {
			// Do nothing
		}

		display::refresh();

		{
			// Get mutex
			lock_guard<mutex> lock{mtx};

			if (isTermRequest) {
				break;
			}
		}

		// Get end time point
		monitorProcLoad.measureStop();
		monitorProcLoad.updateLoad();

		sleep_until(monitorProcLoad.startTp() + monitorDuration);
	}
}

void dispMonitor()
{
	// For status
	string       hwIpVersion{};
	int          deadtime{};
	bool         isOutputEnable{};
	int64_t      rotSpeed{};
	int64_t      period{};
	int          duty{};
	int          phase{};
	bool         isStopping{};
	bool         isReflectedFreq{};
	stringstream ss{};

	RotationalSpeedUnit dispRSU_latch{};
	PeriodUnit          dispPU_latch{};
	int                 dispMID_latch{};

	ProcLoad commanderProcLoad_latch{};

	// Latch some display data
	{
		std::lock_guard lock{mtx};
		dispRSU_latch = dispRSU;
		dispPU_latch  = dispPU;
		dispMID_latch = dispMID;
		commanderProcLoad_latch = commanderProcLoad;
	}

	// Judge if selected motor is available.
	if (motor_mng::isAvailable(dispMID_latch)) {

		// Fetch information
		motor_mng::atomicEnv([&, dispMID_latch, dispPU_latch, dispRSU_latch](){
			bldcm::Motor &motor{motor_mng::motor(dispMID_latch)};

			hwIpVersion = motor.hwIpVersion();
			deadtime    = motor.deadtime();

			isOutputEnable =  motor.outputEnable();

			if (dispRSU_latch == RotationalSpeedUnit::rps) {
			rotSpeed =  motor.rotationalSpeed<bldcm::Rps>().count();
			} else if (dispRSU_latch == RotationalSpeedUnit::rpm) {
			rotSpeed =  motor.rotationalSpeed<bldcm::Rpm>().count();
			} else {
			rotSpeed = static_cast<int64_t>(-1);
			}

			if (dispPU_latch == PeriodUnit::ns) {
			period = motor.pwmPeriod<nanoseconds>().first.count();
			} else if (dispPU_latch == PeriodUnit::us) {
			period = motor.pwmPeriod<microseconds>().first.count();
			} else if (dispPU_latch == PeriodUnit::ms) {
				period = motor.pwmPeriod<milliseconds>().first.count();
			} else if (dispPU_latch == PeriodUnit::s) {
				period = motor.pwmPeriod<seconds>().first.count();
			} else {
				period = static_cast<int64_t>(-1);
			}

			duty  = motor.pwmDuty();
			phase = motor.phase();
			isStopping =  motor.isStopping();
			isReflectedFreq =  motor.isReflectedFreq();
		});

		// Print motor information
		ss << "====== Selected Motor ID: " << dispMID_latch << " ======\n";
		ss << "[mBldcm IP information]" << '\n';
		ss << " - Hardware IP version  : " << hwIpVersion << '\n';
		ss << " - Implemented dead time: " << deadtime    << '\n';

		// Print motor condition
		ss << "[Motor condition]" << '\n';
		ss << " - Output          : " << boolalpha << isOutputEnable << '\n';

		if (dispRSU_latch == RotationalSpeedUnit::rps) {
			ss << " - Rotational speed: " << rotSpeed << " RPS" << '\n';
		} else if (dispRSU_latch == RotationalSpeedUnit::rpm) {
			ss << " - Rotational speed: " << rotSpeed << " RPM" << '\n';
		} else {
			ss << " - Rotational speed : " << rotSpeed << '\n';
		}

		if (dispPU_latch == PeriodUnit::ns) {
			ss << " - Period          : " << period << " ns" << '\n';
		} else if (dispPU_latch == PeriodUnit::us) {
			ss << " - Period          : " << period << " us" << '\n';
		} else if (dispPU_latch == PeriodUnit::ms) {
			ss << " - Period          : " << period << " ms" << '\n';
		} else if (dispPU_latch == PeriodUnit::s) {
			ss << " - Period          : " << period << " s"  << '\n';
		} else {
			ss << " - Period          : " << period << '\n';
		}

		ss << " - PWM Duty        : " << duty << " %" << '\n';
		ss << " - Phase#          : " << phase << '\n';
		ss << " - Stopping?       : " << boolalpha << isStopping << '\n';
		ss << " - Reflected?      : " << boolalpha << isReflectedFreq << '\n';

		ss << "[Process information]" << '\n';
		ss << " - Commander proc load: " << fixed << setprecision(3) << setw(7) << commanderProcLoad_latch.load() << "% (MAX: " << setw(7) << commanderProcLoad_latch.maxLoad() << "%)" << '\n';
		ss << " - Monitor   proc load: " << fixed << setprecision(3) << setw(7) << monitorProcLoad.load()         << "% (MAX: " << setw(7) << monitorProcLoad.maxLoad()         << "%)" << '\n';
	} else {
		// Print motor information
		ss << "====== Selected Motor ID: " << dispMID_latch << " ======\n";
		ss << "[[[[[[[ THIS MOTOR ID IS NOT USED. ]]]]]]]\n";
	}

	ss << "<<< Type 'h' key to display usage.      >>>" << '\n';
	ss << "<<< Type 'q' key to exit this software. >>>" << '\n';

	display::putStr(ss.str());
}

void dispHelp()
{
	stringstream ss{};

	// Print how to control
	ss << "[How to control]"                                                                   << '\n';
	ss << " - Select motor"                                                                    << '\n';
	ss << "   0-3) Select motor ID"                                                            << '\n';
	ss << " - Speed"                                                                           << '\n';
	ss << "   w) Up by 1RPS"                                                                   << '\n';
	ss << "   z) Down by 1RPS"                                                                 << '\n';
	ss << "   W) Up by 6RPS"                                                                   << '\n';
	ss << "   Z) Down by 6RPS"                                                                 << '\n';
	ss << "   x) Set 0RPS"                                                                     << '\n';
	ss << " - PWM Duty"                                                                        << '\n';
	ss << "   d) Higher"                                                                       << '\n';
	ss << "   s) Lower"                                                                        << '\n';
	ss << " - Increment/decrement rotational phase (This is work if isStopping flag is true.)" << '\n';
	ss << "   k) Incremet"                                                                     << '\n';
	ss << "   j) Decrement"                                                                    << '\n';
	ss << " - Enable output"                                                                   << '\n';
	ss << "   o) Toggle output (\"enable\" <-> \"disable\")"                                   << '\n';
	ss << " - Switch display unit of rotational speed"                                         << '\n';
	ss << "   r) Switch display unit (\"rps\" -> \"rpm\" -> \"rps\" -> ...)"                   << '\n';
	ss << " - Switch display unit of PWM period"                                               << '\n';
	ss << "   p) Switch display unit (\"us\" -> \"ms\" -> \"s\" -> \"ns\" -> \"ms\" -> ...)"   << '\n';
	ss << " - Switch pages"                                                                    << '\n';
	ss << "   h) Toggle pages between \"monitor page\" and \"help page\""                      << '\n';
	ss << " - Exit this software"                                                              << '\n';
	ss << "   q) Exit"                                                                         << '\n';

	display::putStr(ss.str());
}

} // End of filescoped functions


} // End of "namespace monitor"

