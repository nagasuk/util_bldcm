#ifndef VAL_PWMPERIOD_HPP
#define VAL_PWMPERIOD_HPP

#include <boost/any.hpp>
#include <chrono>
#include <string>
#include <vector>

namespace parameter {
namespace arg_types {

// Trigger class to select validator
class PwmPeriod {
	public:
		PwmPeriod(const std::chrono::nanoseconds &val) : _arg(val) {}
		const std::chrono::nanoseconds &arg() const { return this->_arg; }

	private:
		std::chrono::nanoseconds _arg;
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
	parameter::arg_types::PwmPeriod *,
	int
);

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

#endif // End of "#ifndef VAL_PWMPERIOD_HPP"

