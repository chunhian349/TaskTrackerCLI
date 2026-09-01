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
* **CLI Tests**: `build/Release/TestTaskTrackerCLI.exe`

## Running Tests
* Navigate into your `build` directory and run:
```bash
ctest -C Release
```

## Example Commands and Usage
* Navigate into `build/Release` directory:
```bash
# Adding a new task
./TaskTrackerCLI.exe add "Buy groceries"
# Updating and deleting tasks
./TaskTrackerCLI.exe update 1 "Buy groceries and cook dinner"
./TaskTrackerCLI.exe delete 1
# Marking a task as in progress or done
./TaskTrackerCLI.exe mark-in-progress 1
./TaskTrackerCLI.exe mark-done 1
# Listing all tasks
./TaskTrackerCLI.exe list
# Listing tasks by status
./TaskTrackerCLI.exe list done
./TaskTrackerCLI.exe list todo
./TaskTrackerCLI.exe list in-progress
```
