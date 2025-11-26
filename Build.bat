# Build.bat - Compile the project
@echo off
echo Building Transport-Sim...

REM Find Visual Studio installation
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo ERROR: Visual Studio not found!
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (`"%VSWHERE%" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
    set MSBUILD=%%i
)

if not defined MSBUILD (
    echo ERROR: MSBuild not found!
    exit /b 1
)

echo Using MSBuild: %MSBUILD%
"%MSBUILD%" Transport-Sim.sln /p:Configuration=Debug /p:Platform=x64 /m

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo Build successful!
