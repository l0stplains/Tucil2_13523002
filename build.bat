@echo off
setlocal EnableDelayedExpansion

REM Create and enter the build directory
if not exist build (
    mkdir build
)
cd build

REM Check if g++ is available in the PATH
where g++ >nul 2>&1
if %errorlevel%==0 (
    echo g++ found. Configuring with MinGW Makefiles...
    REM Configure using MinGW Makefiles and explicitly set g++ as the C++ compiler
    cmake -G "MinGW Makefiles" -DCMAKE_CXX_COMPILER=g++ ..
) else (
    echo g++ not found. Using default CMake configuration...
    cmake ..
)

REM Check if the CMake configuration succeeded
if errorlevel 1 (
    echo Error: CMake configuration failed.
    exit /b %errorlevel%
)

REM Build the project
cmake --build .
if errorlevel 1 (
    echo Error: Build failed.
    exit /b %errorlevel%
)

echo Build succeeded.

