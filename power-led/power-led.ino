#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include "secrets.h"
// ==== Configuration ====
// Wi-Fi credentials
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

// MQTT broker settings
const char* mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT;
const char* mqtt_user = MQTT_USER;
const char* mqtt_pass = MQTT_PASS;
const char* mqtt_topic = "led/control";

// LED settings
#define DATA_PIN    D7
#define CLK_PIN     D6
#define NUM_LEDS    59
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
CRGB leds[NUM_LEDS];

// Wi-Fi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// ==== Functions ====

void setup_wifi() {
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
    delay(500);
    Serial.print(".");
    retries++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  // Handle simple messages
  if (msg == "off") {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
  } else if (msg == "red") {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
    FastLED.show();
  } else if (msg == "blue") {
    fill_solid(leds, NUM_LEDS, CRGB::Blue);
    FastLED.show();
  } else if (msg == "green") {
    fill_solid(leds, NUM_LEDS, CRGB::Green);
    FastLED.show();
  } else {
    Serial.println("Unknown command");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
      client.subscribe(mqtt_topic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

// ==== Main Setup ====

void setup() {
  Serial.begin(115200);
  delay(100);

  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(128);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

// ==== Main Loop ====

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

