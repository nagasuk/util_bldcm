#include "val_ipBaseAddr.hpp"

#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

using std::string;
using std::stoul;
using std::vector;

namespace po = boost::program_options;

// Custom validator
namespace boost {
namespace program_options {
namespace validators {

void validate(
	boost::any &v,
	const vector<string> &values,
	parameter::arg_types::IpBaseAddr *,
	int
)
{
	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const string &s = po::validators::get_single_string(values);

	try {
		uint32_t val = static_cast<uint32_t>(stoul(s, nullptr, static_cast<unsigned long>(0U)));
		v = boost::any(parameter::arg_types::IpBaseAddr(val));
	} catch (...) {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

