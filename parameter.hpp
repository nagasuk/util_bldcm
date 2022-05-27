#ifndef PARAMETER_HPP
#define PARAMETER_HPP

#include "monitor.hpp"

#include <libbldcm.hpp>
#include <chrono>
#include <vector>

namespace parameter {
//===== [Classes] =====
// [External classes(implementation)]
class MonitorParameter {
	public:
		// Sub class
		class InitDispCfg {
			public:
				monitor::RotationalSpeedUnit rsu;
				monitor::PeriodUnit          pu;
		};

		InitDispCfg initDisp;
};

class MotorsParameter {
	public:
		class IpCfg {
			public:
				IpCfg(
					const bool &use_,
					const uint32_t &baseAddr_,
					const bldcm::Hz &clockFreq_,
					const int &prscSel_,
					const std::chrono::nanoseconds &period_,
					const int &initDuty_
				) :
					use(use_), baseAddr(baseAddr_), clockFreq(clockFreq_),
					prscSel(prscSel_), period(period_), initDuty(initDuty_) {}

				bool                     use;
				uint32_t                 baseAddr;
				bldcm::Hz                clockFreq;
				int                      prscSel;
				std::chrono::nanoseconds period;
				int                      initDuty;
		};

		std::vector<IpCfg> ip;
		decltype(ip)::size_type numIp() const { return this->ip.size(); }
};

// [Filescoped classes]
// Nothing

// ===== Definitions of Function =====
// [External Functions]
bool loadCfg();
const MonitorParameter &monitorParameter();
const MotorsParameter &motorsParameter();

} // End of "namespace parameter"

#endif // End of "#ifndef PARAMETER_HPP"

