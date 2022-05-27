#ifndef VAL_ROTATIONALSPEEDUNIT_HPP
#define VAL_ROTATIONALSPEEDUNIT_HPP

#include "../monitor.hpp"
#include <boost/any.hpp>
#include <string>
#include <vector>

namespace parameter {
namespace arg_types {

// Torigger class to select validator
class RotationalSpeedUnit {
	public:
		RotationalSpeedUnit(const monitor::RotationalSpeedUnit &val)
			: _arg(val) {}
		const monitor::RotationalSpeedUnit &arg() const { return this->_arg; }

	private:
		monitor::RotationalSpeedUnit _arg;
};


} // End of "namespace arg_types"
} // End of "namespace parameter"

// Custom validator
namespace boost {
namespace program_options {
namespace validators {

void validate(
	boost::any &v,
	const std::vector<std::string> &values,
	parameter::arg_types::RotationalSpeedUnit *,
	int
);

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

#endif // End of "#ifndef VAL_ROTATIONALSPEEDUNIT_HPP"

