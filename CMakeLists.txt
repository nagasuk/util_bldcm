cmake_minimum_required(VERSION 3.13)

# [ Project Setting ]
## Common
project(util_bldcm VERSION 1.0.0 LANGUAGES CXX)

# [ External libaries ]
## Thread components
find_package(Threads REQUIRED)

## Boost
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS program_options)

## ncurses
find_package(Curses REQUIRED)

## libbldcm
find_package(bldcm 1.0.0)

# [ For building this projects ]
add_executable(bldcmmon)
target_sources(bldcmmon PRIVATE
	main.cpp
	display.cpp
	commander.cpp
	monitor.cpp
	motor_mng.cpp
	parameter.cpp
)
target_include_directories(bldcmmon PRIVATE
	${CURSES_INCLUDE_DIR}
)
target_link_libraries(bldcmmon PRIVATE
	bldcm::bldcm
	Threads::Threads
	${CURSES_LIBRARIES}
	${Boost_LIBRARIES}
)
target_compile_options(bldcmmon PRIVATE -Wall)
target_compile_features(bldcmmon PRIVATE cxx_std_17)

# [ Subdirectories ]
add_subdirectory(parameter_validators)

# [ Installation ]
include(GNUInstallDirs)
install(TARGETS bldcmmon RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES bldcmmon.cfg DESTINATION ${CMAKE_INSTALL_DOCDIR}/bldcm RENAME bldcmmon.sample.cfg)

