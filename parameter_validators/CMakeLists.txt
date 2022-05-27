cmake_minimum_required(VERSION 3.13)

cmake_policy(SET CMP0076 NEW)

# [ For building this projects ]
target_sources(
	bldcmmon PRIVATE
	val_rotationalSpeedUnit.cpp
	val_periodUnit.cpp
	val_ipBaseAddr.cpp
	val_clock.cpp
	val_prscSel.cpp
	val_pwmPeriod.cpp
	val_pwmDuty.cpp
)

