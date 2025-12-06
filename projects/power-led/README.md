# Power LED Controller for LOLIN D1 Mini (ESP8266)

This project is an Arduino sketch designed to control an APA102 LED strip using a LOLIN D1 Mini (ESP8266) board. It connects to WiFi, communicates with an MQTT broker, and visualizes power, price, tariff, room temperature, and garage door status using addressable LEDs.

## Features

- **WiFi Connectivity:** Automatically connects to a configured WiFi network using credentials from `secrets.h`.
- **MQTT Communication:** Subscribes to multiple MQTT topics to receive updates about power usage, price, tariff, room temperature, and garage door state.
- **LED Visualization:** Uses FastLED to control an APA102 LED strip, displaying different statuses and values with colors and patterns.
- **Status Indication:** LEDs indicate WiFi/MQTT connection status and garage door state, with blinking for issues.

## File Structure

- `power-led.ino`: Main Arduino sketch containing all logic for WiFi, MQTT, and LED control.
- `secrets.h`: Stores sensitive information such as WiFi and MQTT credentials (not included in version control).

## Main Components

### Configuration
- WiFi and MQTT credentials are loaded from `secrets.h`.
- LED strip is defined with 59 APA102 LEDs, with specific indices for status, garage, tariff, and temperature indicators.

### Functions
- `setLedHSV(index, hue, sat, val)`: Sets a single LED to a specific HSV color.
- `setRangeHSV(start, end, hue, sat, val)`: Sets a range of LEDs to a specific HSV color.
- `handlePower()`: Visualizes current power usage on the LED strip.
- `handlePrice(msg)`: Updates the color hue based on received price.
- `handleTariff(msg)`: Updates tariff LEDs based on received tariff status.
- `handleRumstemp(msg)`: Updates room temperature LED.
- `handleGaragedoor(msg)`: Updates garage door LED.
- `callback(topic, payload, length)`: Handles incoming MQTT messages and dispatches them to the appropriate handler.
- `blinkIfIssue()`: Blinks status LEDs if there are issues with WiFi, MQTT, or garage door.
- `setup_wifi()`: Connects to WiFi and updates status LED.
- `connect_mqtt()`: Connects to MQTT broker and subscribes to topics.

### Main Loop
- Maintains WiFi and MQTT connections.
- Processes incoming MQTT messages and updates LEDs accordingly.

## Usage

1. **Configure WiFi and MQTT credentials** in `secrets.h`.
2. **Compile and upload** the sketch to the LOLIN D1 Mini using the provided VS Code tasks or Arduino CLI commands.
3. **Connect the APA102 LED strip** to the board (DATA_PIN: D7, CLK_PIN: D6).
4. **Monitor serial output** for debugging and status updates.

## MQTT Topics
- `led/control`: Simple color commands ("off", "red", "blue", "green").
- `powerprice`: Power price updates.
- `powerreport/low_res_power`: Power usage updates.
- `powerstate/tariff`: Tariff status updates.
- `powerstate/rumstemp`: Room temperature status updates.
- `garage/door_state`: Garage door state updates.

## Dependencies
- [ESP8266WiFi](https://arduino-esp8266.readthedocs.io/)
- [PubSubClient](https://pubsubclient.knolleary.net/)
- [FastLED](http://fastled.io/)
- [Ticker](https://github.com/espressif/arduino-esp8266/tree/master/libraries/Ticker)

## License

This project is open source and available under the MIT License.
