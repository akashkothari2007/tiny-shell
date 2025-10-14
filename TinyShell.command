#!/bin/bash
PROFILE="TinyShell"
BUILD_DIR="/Users/akashkothari/Desktop/MyShell/build"
APP_NAME="TinyShell"

# Launch a new Terminal window using your custom profile
osascript <<EOF
tell application "Terminal"
    do script "cd $BUILD_DIR; ./tiny-shell"
    set current settings of front window to settings set "$PROFILE"
    set custom title of front window to "$APP_NAME"
    activate
end tell
EOF