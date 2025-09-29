echo "--- Setting Execution Permissions for all Shell Scripts ---"

find . -name "*.sh" -exec chmod +x {} \;

echo "Permissions set successfully. All .sh files are now executable. ✅"
