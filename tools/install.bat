@echo off
SETLOCAL EnableDelayedExpansion

SET "SCRIPT_DIR=%~dp0"

SET "PROJECT_ROOT=%SCRIPT_DIR%.."

SET "BUILD_DIR=%PROJECT_ROOT%\build"

SET "BUILD_SCRIPT=%SCRIPT_DIR%build.bat"

echo --- Running Build... ---
CALL "%BUILD_SCRIPT%" %*
IF ERRORLEVEL 1 (
    echo ERROR: Build failed before installation. 🛑
    EXIT /B 1
)

echo --- Installing SVCS (Requires Admin privileges to install globally) ---

cd /D "%BUILD_DIR%"
IF ERRORLEVEL 1 (
    echo ERROR: Build directory not found. Cannot install. 🛑
    EXIT /B 1
)

cmake --install .
SET "INSTALL_RESULT=%ERRORLEVEL%"

cd /D "%PROJECT_ROOT%"

IF %INSTALL_RESULT% NEQ 0 (
    echo ERROR: Installation failed. 🛑
    EXIT /B 1
)

echo --- SVCS installation succeeded. 🚀 ---
ENDLOCAL