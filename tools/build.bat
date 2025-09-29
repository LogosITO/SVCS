@echo off
SETLOCAL EnableDelayedExpansion

SET "SCRIPT_DIR=%~dp0"

SET "PROJECT_ROOT=%SCRIPT_DIR%.."

SET "BUILD_DIR=%PROJECT_ROOT%\build"

echo --- Setting up CMake Build for Windows ---

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)

echo.
echo --- Configuring CMake ---

cd /D "%BUILD_DIR%"
IF ERRORLEVEL 1 (
    echo ERROR: Failed to enter directory %BUILD_DIR%
    goto :eof
)

cmake "%PROJECT_ROOT%" %*
IF ERRORLEVEL 1 (
    echo ERROR: CMake configuration failed. ❌
    cd /D "%PROJECT_ROOT%"
    goto :eof
)

echo.
echo --- Starting Build ---
cmake --build .
IF ERRORLEVEL 1 (
    echo ERROR: Build failed. ❌
    cd /D "%PROJECT_ROOT%"
    goto :eof
)

echo.
echo --- SVCS Build Succeeded! 🎉 ---
cd /D "%PROJECT_ROOT%"
ENDLOCAL
