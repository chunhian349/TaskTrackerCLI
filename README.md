# TaskTrackerCLI

Sample solution for the [task-tracker](https://roadmap.sh/projects/task-tracker) challenge from [roadmap.sh](https://roadmap.sh/).
A lightweight command-line tool for tracking your daily tasks.

## Prerequisites

Before building this project, make sure you have the following installed:
* **C++ Compiler**: Supports C++11 or higher. Minimum versions:
  * Microsoft Visual Studio 2015 or newer
  * GCC (Linux) 4.8.1 or newer
  * Clang (macOS/Linux) 3.3 or newer
* **CMake**: Version 3.15 or higher
* **Git**: **Crucial.** Must be installed on your system command line, as CMake uses Git to automatically download GoogleTest at build time.

## How to Build and Run

Clone the repository:

```bash
git clone git@github.com:chunhian349/TaskTrackerCLI.git
```

Open Visual Studio, go to the project root directory using Developer PowerShell and run the following commands:

```bash
# 1. Create a clean build directory
mkdir build
cd build

# 2. Configure the project using CMake
cmake ..

# 3. Compile and build the executable
cmake --build . --config Release
```

After a successful build, you can find the final executable files here:
* **Application**: `build/Release/TaskTrackerCLI.exe`
* **Unit Tests**: `build/Release/TestTaskTracker.exe`

## Running the Unit Tests
* Navigate into your `build` directory and run:
```bash
ctest -C Release
```
