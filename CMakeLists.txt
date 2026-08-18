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
