@echo off
setlocal enabledelayedexpansion

echo =============================
echo Cleaning old generated files...
echo =============================

rd /s /q bin 2>nul
rd /s /q build 2>nul
del /f /q Transport-Sim.sln 2>nul
del /f /q *.vcxproj* 2>nul

echo =============================
echo Building GLFW as static lib...
=============================

cd vendor\glfw
rd /s /q build 2>nul
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64 -DBUILD_SHARED_LIBS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY="MultiThreaded" > cmake_log.txt
cmake --build . --config Debug > build_log.txt
cd ..\..\..

echo =============================
echo Running Premake to generate .sln...
echo =============================

vendor\Premake\premake5.exe vs2022

echo =============================
echo Generation Complete.
echo =============================

pause
