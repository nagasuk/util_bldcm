#ifndef VAL_PWMDUTY_HPP
#define VAL_PWMDUTY_HPP

#include <libbldcm.hpp>
#include <boost/any.hpp>
#include <string>
#include <vector>

namespace parameter {
namespace arg_types {

// Trigger class to select validator
class PwmDuty {
	public:
		PwmDuty(const int &val) : _arg(val) {}
		const int &arg() const { return this->_arg; }

	private:
		int _arg;
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
	parameter::arg_types::PwmDuty *,
	int
);

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

#endif // End of "#ifndef VAL_PWMDUTY_HPP"

