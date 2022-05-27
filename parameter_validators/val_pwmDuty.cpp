#include "val_pwmDuty.hpp"

#include <string>
#include <vector>
#include <stdexcept>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

using std::string;
using std::stoi;
using std::vector;
using std::range_error;

namespace po = boost::program_options;

// Custom validator
namespace boost {
namespace program_options {
namespace validators {

void validate(
	boost::any &v,
	const vector<string> &values,
	parameter::arg_types::PwmDuty *,
	int
)
{
	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const string &s = po::validators::get_single_string(values);

	try {
		const int val = stoi(s, nullptr, static_cast<unsigned long>(0U));

		if ((val < static_cast<int>(0)) || (val > static_cast<int>(100))) {
			throw range_error{R"("pwmDuty" is out of range. [0-100])"};
		}

		v = boost::any(parameter::arg_types::PwmDuty(val));

	} catch (...) {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

