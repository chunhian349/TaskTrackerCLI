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

set(UNITTEST_PROJECT_NAME TestTaskTracker)

set(UNITTEST_SOURCES
	tests/TestTaskTracker.cpp
	src/Utils.cpp
	src/TaskTracker.cpp
)

add_executable(${UNITTEST_PROJECT_NAME} ${UNITTEST_SOURCES})

target_link_libraries(${UNITTEST_PROJECT_NAME} GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(${UNITTEST_PROJECT_NAME})