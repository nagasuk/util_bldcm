#include "val_clock.hpp"

#include <regex>
#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

using std::string;
using std::stoul;
using std::vector;

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
	parameter::arg_types::Clock *,
	int
)
{
	static const regex re{R"((\d+) *(|k|M)Hz)"};
	smatch m;
	bldcm::Hz val;

	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const string &s = po::validators::get_single_string(values);

	if (!regex_match(s, m, re)) {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}

	if (m[2].str() == "") {
		val = bldcm::Hz(stoul(m[1].str()));
	} else if (m[2].str() == "k") {
		val = bldcm::clockFreq_cast<bldcm::Hz>(bldcm::KHz(stoul(m[1].str())));
	} else if (m[2].str() == "M") {
		val = bldcm::clockFreq_cast<bldcm::Hz>(bldcm::MHz(stoul(m[1].str())));
	} else {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}

	v = boost::any(parameter::arg_types::Clock(val));
}

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

