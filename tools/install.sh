SCRIPT_DIR=$(cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P)
PROJECT_ROOT="$SCRIPT_DIR/.."
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_SCRIPT="$SCRIPT_DIR/build.sh"

echo "--- Running Build... ---"
"$BUILD_SCRIPT" "$@"
if [ $? -ne 0 ]; then exit 1; fi

echo "--- Installing SVCS (May require sudo) ---"

cd "$BUILD_DIR" || { echo "ERROR: Build directory not found. Cannot install. 🛑"; exit 1; }

sudo cmake --install .
INSTALL_RESULT=$?

cd "$PROJECT_ROOT"

if [ $INSTALL_RESULT -ne 0 ]; then
    echo "ERROR: Installation failed. 🛑"
    exit 1
fi
echo "--- SVCS installation succeeded. 🚀 ---"