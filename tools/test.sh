SCRIPT_DIR=$(cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P)
PROJECT_ROOT="$SCRIPT_DIR/.."
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_SCRIPT="$SCRIPT_DIR/build.sh"

echo "--- Running Build... ---"
"$BUILD_SCRIPT" "$@"
if [ $? -ne 0 ]; then exit 1; fi

echo "--- Running CTest ---"

cd "$BUILD_DIR" || { echo "ERROR: Build directory not found. Cannot run tests. ❌"; exit 1; }

ctest --verbose
TEST_RESULT=$?

cd "$PROJECT_ROOT"

if [ $TEST_RESULT -ne 0 ]; then
    echo "ERROR: Tests failed. ❌"
    exit 1
fi
echo "--- All tests passed! ✅ ---"
