#!/bin/bash

# Generic upload script for Arduino projects
# Usage: ./scripts/upload.sh <project-name> <fqbn> [port]

PROJECT=$1
FQBN=$2
PORT=${3:-/dev/ttyUSB0}

if [ -z "$PROJECT" ] || [ -z "$FQBN" ]; then
    echo "Usage: $0 <project-name> <fqbn> [port]"
    echo "Example: $0 power-led esp8266:esp8266:d1_mini /dev/ttyUSB0"
    exit 1
fi

PROJECT_PATH="$(dirname "$0")/../projects/$PROJECT"

if [ ! -d "$PROJECT_PATH" ]; then
    echo "Error: Project '$PROJECT' not found in projects/"
    exit 1
fi

echo "Uploading $PROJECT to $PORT..."
arduino-cli upload -p "$PORT" --fqbn "$FQBN" "$PROJECT_PATH"

if [ $? -eq 0 ]; then
    echo "✓ Upload successful!"
else
    echo "✗ Upload failed!"
    exit 1
fi
