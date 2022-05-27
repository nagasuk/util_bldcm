#ifndef VAL_IPBASEADDR_HPP
#define VAL_IPBASEADDR_HPP

#include "../monitor.hpp"
#include <boost/any.hpp>
#include <string>
#include <vector>

namespace parameter {
namespace arg_types {

// Trigger class to select validator
class IpBaseAddr {
	public:
		IpBaseAddr(const uint32_t &val) : _arg(val) {}
		const uint32_t &arg() const { return this->_arg; }

	private:
		uint32_t _arg;
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
	parameter::arg_types::IpBaseAddr *,
	int
);

} // End of "namespace validators"
} // End of "namespace program_options"
} // End of "namespace boost"

#endif // End of "#ifndef VAL_IPBASEADDR_HPP"

