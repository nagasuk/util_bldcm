#include "val_pwmPeriod.hpp"

#include <regex>
#include <string>
#include <vector>
#include <chrono>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

using std::string;
using std::stoul;
using std::vector;

using std::chrono::nanoseconds;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::duration_cast;

using std::regex;
using std::regex_match;
using std::smatch;

namespace po = boost::program_options;

// Custom validator
namespace boost {
namespace program_options {
namespace validators {

void validate(
	boost::any &v,
	const vector<string> &values,
	parameter::arg_types::PwmPeriod *,
	int
)
{
	static const regex re{R"((\d+) *(n|u|m|)s)"};
	smatch m;
	nanoseconds val;

	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const string &s = po::validators::get_single_string(values);

	if (!regex_match(s, m, re)) {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}

	if (m[2].str() == "n") {
		val = nanoseconds(stoul(m[1].str()));
	} else if (m[2].str() == "u") {
		val = duration_cast<nanoseconds>(microseconds(stoul(m[1].str())));
	} else if (m[2].str() == "m") {
		val = duration_cast<nanoseconds>(milliseconds(stoul(m[1].str())));
	} else if (m[2].str() == "") {
		val = duration_cast<nanoseconds>(seconds(stoul(m[1].str())));
	} else {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}

	v = boost::any(parameter::arg_types::PwmPeriod(val));
}

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

