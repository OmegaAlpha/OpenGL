@echo off
REM Navigate to the directory containing CMakeLists.txt (expands to current directory)
cd /d %~dp0

REM Create a build directory (optional, if not exists)
echo Creating build directory...
if not exist "build" mkdir build

REM Navigate into the build directory
cd build

echo Running CMake with verbose output...

REM Run CMake to generate Visual Studio 2022 project files with verbose output
cmake .. -G "Visual Studio 17 2022" --log-level=VERBOSE

REM Optional: Open the generated Visual Studio solution file
rem start "OpenGLTest.sln" REM Replace with your project name if different

pause

REM End the batch file
exit