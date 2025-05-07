#!/bin/bash

# Convert relative path to absolute (cross-platform)
if command -v realpath >/dev/null 2>&1; then
    ABS_PATH=$(realpath "$1")
else
    ABS_PATH=$(cd "$1" && pwd)
fi

echo "central logging to $ABS_PATH"

# Run docker with the resolved path
#docker build -t vector-logger vector-logger
docker run -d --rm --name vector-logger -p 8686:8686 -v "$ABS_PATH":/var/log vector-logger
