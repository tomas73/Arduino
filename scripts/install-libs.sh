#!/bin/bash

# Install common Arduino libraries
echo "Installing Arduino libraries..."

arduino-cli lib install "FastLED"
arduino-cli lib install "PubSubClient"

# Add more libraries as needed for your projects
# arduino-cli lib install "ArduinoJson"
# arduino-cli lib install "Adafruit NeoPixel"

echo "✓ Libraries installed!"
