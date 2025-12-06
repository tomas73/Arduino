#!/bin/bash

# Generic compile script for Arduino projects
# Usage: ./scripts/compile.sh <project-name> <fqbn>

PROJECT=$1
FQBN=$2

if [ -z "$PROJECT" ] || [ -z "$FQBN" ]; then
    echo "Usage: $0 <project-name> <fqbn>"
    echo "Example: $0 power-led esp8266:esp8266:d1_mini"
    exit 1
fi

PROJECT_PATH="$(dirname "$0")/../projects/$PROJECT"

if [ ! -d "$PROJECT_PATH" ]; then
    echo "Error: Project '$PROJECT' not found in projects/"
    exit 1
fi

echo "Compiling $PROJECT for $FQBN..."
arduino-cli compile --fqbn "$FQBN" "$PROJECT_PATH"

if [ $? -eq 0 ]; then
    echo "✓ Compilation successful!"
else
    echo "✗ Compilation failed!"
    exit 1
fi
