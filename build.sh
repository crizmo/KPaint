#!/bin/bash

# Script to build KPaint

# Path to GBDK directory
GBDK_PATH=~/Documents/GitHub/gbdk
GBDK_PATH_EXPANDED=$(eval echo $GBDK_PATH)

# Check if GBDK exists
if [ ! -d "$GBDK_PATH_EXPANDED" ]; then
    echo "Error: GBDK not found at $GBDK_PATH_EXPANDED"
    exit 1
fi

# Create build directory if it doesn't exist
mkdir -p build

# Compile the project
echo "Compiling KPaint..."
$GBDK_PATH/bin/lcc -Wa-l -Wl-m -Wl-j -DUSE_SFR_FOR_REG -I$GBDK_PATH_EXPANDED/include -o build/kpaint.gb src/main.c

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful! ROM is at build/kpaint.gb"
    ls -l build/kpaint.gb
else
    echo "Build failed!"
    exit 1
fi
