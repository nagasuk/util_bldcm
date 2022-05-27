#include "val_rotationalSpeedUnit.hpp"
#include "../monitor.hpp"

#include <unordered_map>

#include <string>
#include <vector>

#include <boost/program_options.hpp>
#include <boost/any.hpp>

using std::string;
using std::vector;
using std::unordered_map;

namespace po = boost::program_options;

// Custom validator
namespace boost {
namespace program_options {
namespace validators {

void validate(
	boost::any &v,
	const vector<string> &values,
	parameter::arg_types::RotationalSpeedUnit *,
	int
)
{
	static const unordered_map<string, monitor::RotationalSpeedUnit> convTbl{
		 {"rpm", monitor::RotationalSpeedUnit::rpm}
		,{"rps", monitor::RotationalSpeedUnit::rps}
	};
	decltype(convTbl)::const_iterator foundItr;

	// Make sure no previous assignment to 'v' was made.
	po::validators::check_first_occurrence(v);

	// Extract the first string from 'values'. If there is more than
	// one string, it's an error, and exception will be thrown.
	const string &s = po::validators::get_single_string(values);

	foundItr = convTbl.find(s);

	if (foundItr != convTbl.cend()) {
		v = boost::any(parameter::arg_types::RotationalSpeedUnit(foundItr->second));
	} else {
		throw po::validation_error(po::validation_error::invalid_option_value);
	}
}

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

