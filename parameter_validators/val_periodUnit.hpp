#ifndef VAL_PERIODUNIT_HPP
#define VAL_PERIODUNIT_HPP

#include "../monitor.hpp"
#include <boost/any.hpp>
#include <string>
#include <vector>

namespace parameter {
namespace arg_types {

// Torigger class to select validator
class PeriodUnit {
	public:
		PeriodUnit(const monitor::PeriodUnit &val)
			: _arg(val) {}
		const monitor::PeriodUnit &arg() const { return this->_arg; }

	private:
		monitor::PeriodUnit _arg;
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
	parameter::arg_types::PeriodUnit *,
	int
);

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

#endif // End of "#ifndef VAL_PERIODUNIT_HPP"

