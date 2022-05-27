#ifndef MOTOR_MNG_HPP
#define MOTOR_MNG_HPP

#include <libbldcm.hpp>
#include <chrono>
#include <functional>

namespace motor_mng {

// Labels
constexpr int      BldcmIpNum = static_cast<int>(4);

// Functions
void init();
void deinit();
bool isAvailable(int motorId);
bldcm::Motor &motor(int motorId);
void atomicEnv(const std::function<void()> &proc);

} // End of "namespace motor_mng"

#endif // End of "#ifndef MOTOR_MNG_HPP"
