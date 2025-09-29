SCRIPT_DIR=$(cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P)

PROJECT_ROOT="$SCRIPT_DIR/.."

BUILD_DIR="$PROJECT_ROOT/build"

echo "--- Creating/Entering Build Directory ($BUILD_DIR) ---"

mkdir -p "$BUILD_DIR"

cd "$BUILD_DIR" || { echo "ERROR: Failed to enter directory $BUILD_DIR"; exit 1; }

echo "--- Configuring CMake ---"

cmake "$PROJECT_ROOT" "$@"

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed. ❌"
    exit 1
fi

echo "--- Starting Build ---"

cmake --build .

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed. ❌"
    exit 1
fi

echo "--- SVCS Build Succeeded! 🎉 ---"