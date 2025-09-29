@echo off
SETLOCAL EnableDelayedExpansion

SET "SCRIPT_DIR=%~dp0"

SET "PROJECT_ROOT=%SCRIPT_DIR%.."

SET "BUILD_DIR=%PROJECT_ROOT%\build"

SET "BUILD_SCRIPT=%SCRIPT_DIR%build.bat"


echo --- Running Build... ---
CALL "%BUILD_SCRIPT%" %*
IF ERRORLEVEL 1 (
    echo ERROR: Build failed. Cannot run tests. ❌
    EXIT /B 1
)

echo --- Running CTest ---

cd /D "%BUILD_DIR%"
IF ERRORLEVEL 1 (
    echo ERROR: Build directory not found. Cannot run tests. ❌
    EXIT /B 1
)

ctest --verbose
SET "TEST_RESULT=%ERRORLEVEL%"

cd /D "%PROJECT_ROOT%"

IF %TEST_RESULT% NEQ 0 (
    echo ERROR: Tests failed. ❌
    EXIT /B 1
)

echo --- All tests passed! ✅ ---
ENDLOCAL
