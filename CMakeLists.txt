cmake_minimum_required(VERSION 4.0)
project(MOS_6502_Emulation)

set(CMAKE_CXX_STANDARD 23)

include(cmake/CPM.cmake)
CPMAddPackage("gh:google/googletest#v1.17.0")

include(GoogleTest)

# Automatically gather all .cpp files in src/M6502Lib/
file(GLOB_RECURSE LIB_SOURCES "src/M6502Lib/*.cpp")

# Create the static library with all source files
add_library(MOS_6502_Lib STATIC ${LIB_SOURCES})
target_include_directories(MOS_6502_Lib PUBLIC include)

# Enable testing
enable_testing()

# Automatically gather all .cpp files in tests/
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

# Create the test executable with all test files
add_executable(CPUtest ${TEST_SOURCES})
target_link_libraries(CPUtest PRIVATE GTest::gtest_main MOS_6502_Lib)
target_include_directories(CPUtest PUBLIC include)

gtest_discover_tests(CPUtest)

