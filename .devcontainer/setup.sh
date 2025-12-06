#!/bin/bash

echo "Setting up Arduino development environment..."

# Update board index
arduino-cli core update-index

# Install cores based on arduino-cli.yaml or common ones
if [ -f "arduino-cli.yaml" ]; then
    echo "Using arduino-cli.yaml configuration..."
    # Install ESP8266 core (adjust as needed for your projects)
    arduino-cli core install esp8266:esp8266
    arduino-cli core install arduino:avr
else
    echo "No arduino-cli.yaml found, installing common cores..."
    arduino-cli core install arduino:avr
fi

# Install common libraries
arduino-cli lib install "FastLED"
arduino-cli lib install "PubSubClient"

# Set up USB permissions
sudo usermod -a -G dialout vscode 2>/dev/null || true

echo "Setup complete!"
