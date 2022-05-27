
#include "motor_mng.hpp"
#include "parameter.hpp"

#include <libbldcm.hpp>
#include <libfpgasoc.hpp>

#include <mutex>
#include <memory>
#include <array>
#include <stdexcept>
#include <functional>

using std::mutex;
using std::lock_guard;
using std::shared_ptr;
using std::unique_ptr;
using std::array;
using std::function;
using std::out_of_range;

namespace motor_mng {
//===== [Labels] =====
constexpr int minMotorId = static_cast<int>(0);


//===== [Objects] ===== 
// [External objects]
// Nothing

// [Filescoped objects]
namespace {
	mutex mtx{};
	shared_ptr<Fpgasoc> fpgaPtr{};
	array<unique_ptr<bldcm::Motor>, BldcmIpNum> motorPtrs{};
} // End of filescoped objects


//===== Prototypes =====
// [External prototypes]
// Nothing

// [Filescoped prototypes]
namespace {
	// Nothing
} // End of filescoped prototypes


// ===== Definitions of Function =====
// [External Functions]
void init()
{
	// Get motors' parameter
	const parameter::MotorsParameter &param = parameter::motorsParameter();

	// Initialize motor objects
	// 1st; Create Fpgasoc pointer object
	fpgaPtr.reset(new Fpgasoc);

	// 2nd; Create motor pointers object array and initialize
	for (decltype(motorPtrs)::size_type i = static_cast<int>(0); i < motorPtrs.size(); i++) {
		if (param.ip[i].use) {
			auto &motorPtr = motorPtrs[i];

			// Create motor object
			motorPtr.reset(new bldcm::Motor(fpgaPtr, param.ip[i].clockFreq, param.ip[i].baseAddr));

			// 1st; Disable output
			motorPtr->outputEnable(false);
			motorPtr->rotationalSpeed(bldcm::Rps(0));

			// 2nd; Set period and duty
			motorPtr->pwmDuty(param.ip[i].initDuty);
			motorPtr->pwmPeriod(param.ip[i].period, param.ip[i].prscSel);

			// 3rd; Initialize phase
			motorPtr->phase(static_cast<int>(0));
		}
	}
}

void deinit()
{
	// Reset parameters
	for (auto &motorPtr : motorPtrs) {
		if (motorPtr) {
			// 1st; Disable output
			motorPtr->outputEnable(false);
			motorPtr->rotationalSpeed(bldcm::Rps(0));

			// 2nd; Set period and duty
			motorPtr->pwmDuty(static_cast<int>(0));
			motorPtr->pwmPeriod(std::chrono::nanoseconds(2621400), static_cast<int>(0));

			// 3rd; Initialize phase
			motorPtr->phase(static_cast<int>(0));
		}
	}
}

bool isAvailable(int motorId)
{
	bool ret = false;

	if ((motorId >= minMotorId) && (motorId < static_cast<int>(motorPtrs.size()))) {
		if (motorPtrs[motorId]) { // Is motor pointer valid?
			ret = true;
		}
	}

	return ret;
}

bldcm::Motor &motor(int motorId)
{
	if ((motorId < minMotorId) || (motorId >= static_cast<int>(motorPtrs.size()))) {
		throw out_of_range("Motor ID is out of range.");
	}

	return *(motorPtrs[motorId]);
}

void atomicEnv(const function<void()> &proc)
{
	lock_guard<mutex> lock{mtx};

	proc();
}

// [Filescoped Functions]
namespace {
	// Nothing
} // End of filescoped functions

} // End of "namespace motor_mng"

