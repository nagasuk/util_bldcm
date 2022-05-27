#ifndef VAL_CLOCK_HPP
#define VAL_CLOCK_HPP

#include <libbldcm.hpp>
#include <boost/any.hpp>
#include <string>
#include <vector>

namespace parameter {
namespace arg_types {

// Trigger class to select validator
class Clock {
	public:
		Clock(const bldcm::Hz &val) : _arg(val) {}
		const bldcm::Hz &arg() const { return this->_arg; }

	private:
		bldcm::Hz _arg;
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
	parameter::arg_types::Clock *,
	int
);

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

#endif // End of "#ifndef VAL_CLOCK_HPP"

