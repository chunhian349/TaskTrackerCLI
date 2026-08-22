cmake_minimum_required(VERSION 3.15)

# 1. Name your project (replace "MyProject" with your actual project name)
project(TaskTrackerCLI VERSION 1.0 LANGUAGES CXX)

# 2. Set the C++ standard (change to 11, 14, 17, or 20 depending on your code)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Tell the compiler where to find your header files
include_directories(include)

# 4. Explicitly list all your source files
set(SOURCES 
	src/TaskTrackerCLI.cpp
	src/Utils.cpp
	src/TaskTracker.cpp
)

# 5. Define the final executable name and its source files
add_executable(${PROJECT_NAME} ${SOURCES})

# --- GOOGLETEST INTEGRATION ---
enable_testing()

# 1. Download GoogleTest safely from GitHub
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.15.2  # Uses a stable, fixed release version
)
# For Windows compatibility, prevent gtest from overriding compiler flag settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 2. Define your test runner executable
add_executable(TestTaskTracker 
    tests/TestTaskTracker.cpp
	src/Utils.cpp
	src/TaskTracker.cpp
)

# 3. Link GoogleTest to your test runner
target_link_libraries(TestTaskTracker GTest::gtest_main)

# 4. Let CMake find and organize all your individual gtest macros automatically
include(GoogleTest)
gtest_discover_tests(TestTaskTracker)