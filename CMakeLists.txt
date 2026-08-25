# Project configuration
cmake_minimum_required(VERSION 3.15)

project(TaskTrackerCLI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

set(SOURCES 
	src/TaskTrackerCLI.cpp
	src/Utils.cpp
	src/TaskTracker.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Test configuration
enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2  
)

# For Windows compatibility, prevent gtest from overriding compiler flag settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(UNIT_TEST TestTaskTracker)
set(UNIT_TEST_SOURCES
	tests/TestTaskTracker.cpp
	src/Utils.cpp
	src/TaskTracker.cpp
)
add_executable(${UNIT_TEST} ${UNIT_TEST_SOURCES})
target_link_libraries(${UNIT_TEST} GTest::gtest_main)

set(CLI_TEST TestTaskTrackerCLI)
set(CLI_TEST_SOURCES tests/TestTaskTrackerCLI.cpp)
add_executable(${CLI_TEST} ${CLI_TEST_SOURCES})
target_compile_definitions(${CLI_TEST} PRIVATE TASKTRACKERCLI_EXE_PATH="$<TARGET_FILE:${PROJECT_NAME}>")
target_link_libraries(${CLI_TEST} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${UNIT_TEST})
gtest_discover_tests(${CLI_TEST})