#include <libfpgasoc.hpp>
#include <libbldcm.hpp>

#include <iostream>
#include <thread>
#include <cstdlib>

#include "display.hpp"
#include "commander.hpp"
#include "monitor.hpp"
#include "motor_mng.hpp"
#include "parameter.hpp"

using std::cout;
using std::endl;

// Function definition
int main()
{
	const bool isLoadCfgSuccessful = parameter::loadCfg();
	int retVal = static_cast<int>(EXIT_FAILURE);

	if (isLoadCfgSuccessful) {
		display::init();
		motor_mng::init();

		monitor::start();
		commander::start();

		monitor::waitFinish();
		commander::waitFinish();

		motor_mng::deinit();
		display::deinit();

		cout << "Terminated because 'q' is input." << endl;
	}

	if (isLoadCfgSuccessful) {
		retVal = static_cast<int>(EXIT_SUCCESS);
	}

	return retVal;
}


