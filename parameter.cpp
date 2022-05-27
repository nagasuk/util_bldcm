#include "parameter.hpp"
#include "parameter_validators/validators.hpp"

#include "motor_mng.hpp"

#include <filesystem>
#include <array>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <exception>
#include <string_view>

#include <boost/program_options.hpp>
#include <boost/exception/exception.hpp>
#include <boost/exception/diagnostic_information.hpp>

namespace fs = std::filesystem;
namespace po = boost::program_options;

using std::cout;
using std::cerr;
using std::endl;
using std::array;
using std::string;
using std::string_view;
using std::stringstream;
using std::setw;
using std::setfill;
using std::replace;

namespace parameter {

//===== [Labels] =====
// [External labels]
// Nothing

// [Filescoped labels]
namespace {
	constexpr char CfgNameMonitorInitRSU[] = "monitor.init.rotationalSpeedUnit";
	constexpr char CfgNameMonitorInitPU[]  = "monitor.init.periodUnit";

	constexpr char CfgSufxMotorIpUse[]       = "ip.use";
	constexpr char CfgSufxMotorIpBaseAddr[]  = "ip.baseAddr";
	constexpr char CfgSufxMotorIpClock[]     = "ip.clock";
	constexpr char CfgSufxMotorIpPrscSel[]   = "ip.prscSel";
	constexpr char CfgSufxMotorIpPwmPeriod[] = "ip.pwmPeriod";
	constexpr char CfgSufxMotorInitPwmDuty[] = "init.pwmDuty";

	constexpr string_view CfgFileName{"bldcmmon.cfg"};
	constexpr string_view CfgSampleFileName{"bldcmmon.sample.cfg"};
}

//===== [Objects] ===== 
// [External objects]
// Nothing

// [Filescoped objects]
namespace {
	fs::path cfgFilePath{};

	po::options_description cfgOpDesc{};
	po::variables_map cfgVm;

	MonitorParameter monitorParam{};
	MotorsParameter  motorsParam{};

} // End of filescoped objects

//===== Prototypes =====
// [External prototypes]
// Nothing

// [Filescoped prototypes]
namespace {
	void initConfigDescription();
	void parseCfgFile();
	bool searchCfgFile();
	bool searchCfgFileCurDir();
	bool searchCfgFileSysDir();
	void storeCfg();

	void err(const string &str);
	void msg(const string &prefix, const string &str, bool isStderr = false);
} // End of filescoped prototypes

// ===== Definitions of Function =====
// [External Functions]
bool loadCfg()
{
	bool isCfgFound{searchCfgFile()};
	bool isParsingSuccess = true;
	bool ret = false;

	if (isCfgFound) {
		try {
			initConfigDescription();
			parseCfgFile();

		} catch (std::exception &e) {
			err(e.what());
			isParsingSuccess = false;

		} catch (boost::exception &e) {
			err(boost::diagnostic_information(e));
			isParsingSuccess = false;
		}

		if (isParsingSuccess) {
			storeCfg();
		}

	} else {
		stringstream ss;
		ss << "Configuration file of \"" << CfgFileName << "\" is NOT found.\n"
		   << "You can copy sample configuration in \"<install root path>/share/doc/bldcm/" << CfgSampleFileName << "\"\n"
		   << "to \"/etc/\" or the current directory and use it.";
		err(ss.str());
	}

	if (isCfgFound && isParsingSuccess) {
		ret = true;
	}

	return ret;
}

const MonitorParameter &monitorParameter()
{
	return monitorParam;
}

const MotorsParameter &motorsParameter()
{
	return motorsParam;
}

// [Filescoped Functions]
namespace {

void initConfigDescription()
{
	stringstream ss{};

	// 1st; Monitor section
	cfgOpDesc.add_options()
		(CfgNameMonitorInitRSU, po::value<arg_types::RotationalSpeedUnit>()->required(), "Initial setting of display rotational speed unit.")
		(CfgNameMonitorInitPU,  po::value<arg_types::PeriodUnit>()->required(),          "Initial setting of display period unit."          )
	;

	// 2nd; Motor section
	for (int i = static_cast<int>(0); i < motor_mng::BldcmIpNum; i++) {
		// Clear ss
		ss.str("");
		ss.clear(stringstream::goodbit);

		// Generate prefix
		ss << "motor." << setw(1) << setfill('0') << i << '.';

		// Add options
		cfgOpDesc.add_options()
			((ss.str() + CfgSufxMotorIpUse).c_str(),       po::value<bool>()->required(),                  "Select to use IP or not to.")
			((ss.str() + CfgSufxMotorIpBaseAddr).c_str(),  po::value<arg_types::IpBaseAddr>()->required(), "Base address of IP."        )
			((ss.str() + CfgSufxMotorIpClock).c_str(),     po::value<arg_types::Clock>()->required(),      "Clock frequency of IP."     )
			((ss.str() + CfgSufxMotorIpPrscSel).c_str(),   po::value<arg_types::PrscSel>()->required(),    "IP setting of PWM_PRSC."    )
			((ss.str() + CfgSufxMotorIpPwmPeriod).c_str(), po::value<arg_types::PwmPeriod>()->required(),  "PWM period."                )
			((ss.str() + CfgSufxMotorInitPwmDuty).c_str(), po::value<arg_types::PwmDuty>()->required(),    "Initial PWM duty."          )
		;
	}
}

void parseCfgFile()
{
	po::store(po::parse_config_file(cfgFilePath.c_str(), cfgOpDesc), cfgVm);
	notify(cfgVm);
}

bool searchCfgFile()
{
	bool isCfgFound{false};

	// 1st; Search cfg file in current directory
	isCfgFound = searchCfgFileCurDir();

	//      Judge foundation.
	if (!isCfgFound) {
		// 2nd; Search cfg file in system directory
		isCfgFound = searchCfgFileSysDir();
	}

	return isCfgFound;
}

bool searchCfgFileCurDir()
{
	const fs::path curCfgPath{fs::current_path() / CfgFileName};
	bool isCfgFileExists{fs::exists(curCfgPath)};

	if (isCfgFileExists) {
		cfgFilePath = curCfgPath;
	}

	return isCfgFileExists;
}

bool searchCfgFileSysDir()
{
	static const array<fs::path,2> cfgPathsInSysDirs{
		 fs::path("/etc")               / CfgFileName // Priority: 1
		,fs::path("/usr/lib/bldcmtest") / CfgFileName // Priority: 2
	};

	bool isCfgFileExists{false};

	for (const auto &cfgPath : cfgPathsInSysDirs) {
		isCfgFileExists = fs::exists(cfgPath);
		if (isCfgFileExists) {
			cfgFilePath = cfgPath;
			break;
		}
	}

	return isCfgFileExists;
}

void storeCfg()
{
	stringstream ss{};

	// Store monitor configuration
	monitorParam.initDisp.rsu = cfgVm[CfgNameMonitorInitRSU].as<arg_types::RotationalSpeedUnit>().arg();
	monitorParam.initDisp.pu  = cfgVm[CfgNameMonitorInitPU].as<arg_types::PeriodUnit>().arg();

	// Store motors configuration
	for (int i = static_cast<int>(0); i < motor_mng::BldcmIpNum; i++) {
		// Clear ss
		ss.str("");
		ss.clear(stringstream::goodbit);

		// Generate prefix
		ss << "motor." << setw(1) << setfill('0') << i << '.';

		// Add options
		motorsParam.ip.emplace_back(
			cfgVm[ss.str() + CfgSufxMotorIpUse].as<bool>(),
			cfgVm[ss.str() + CfgSufxMotorIpBaseAddr].as<arg_types::IpBaseAddr>().arg(),
			cfgVm[ss.str() + CfgSufxMotorIpClock].as<arg_types::Clock>().arg(),
			cfgVm[ss.str() + CfgSufxMotorIpPrscSel].as<arg_types::PrscSel>().arg(),
			cfgVm[ss.str() + CfgSufxMotorIpPwmPeriod].as<arg_types::PwmPeriod>().arg(),
			cfgVm[ss.str() + CfgSufxMotorInitPwmDuty].as<arg_types::PwmDuty>().arg()
		);
	}
}

void err(const string &str)
{
	constexpr char prefix[] = "Error";

	msg(prefix, str, true);
}

void msg(const string &prefix, const string &str, bool isStderr)
{
	const string prefixSpace(prefix.size(), static_cast<char>(' '));

	string outStr{prefix + ": " + str};

	string::size_type pos = outStr.find('\n');

	while (pos != string::npos) {
		outStr.insert(pos + static_cast<string::size_type>(1), prefixSpace + ": ");
		pos = outStr.find('\n', pos + static_cast<string::size_type>(1));
	}

	if (!isStderr) {
		cout << outStr << endl;
	} else {
		cerr << outStr << endl;
	}
}

} // End of filescoped functions

} // End of "namespace parameter"

