#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <FastLED.h>
#include <Ticker.h>
#include "secrets.h"
// ==== Configuration ====
// Wi-Fi credentials
const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

// MQTT broker settings
const char *mqtt_server = MQTT_SERVER;
const int mqtt_port = MQTT_PORT;
const char *mqtt_user = MQTT_USER;
const char *mqtt_pass = MQTT_PASS;
const char *mqtt_topic = "led/control";
const char *mqtt_topic_price = "powerprice";
const char *mqtt_topic_power = "powerreport/low_res_power";
const char *mqtt_topic_tariff = "powerstate/tariff";
const char *mqtt_topic_rumstemp = "powerstate/rumstemp";
const char *mqtt_topic_garagedoor = "garage/door_state";

// LED settings
#define DATA_PIN D7
#define CLK_PIN D6
#define NUM_LEDS 59
#define LED_TYPE APA102
#define COLOR_ORDER BGR
CRGB leds[NUM_LEDS];

#define LED_GARAGE (NUM_LEDS - 1)
#define LED_TARIFF_C (NUM_LEDS - 3)
#define LED_TARIFF_B (NUM_LEDS - 4)
#define LED_TARIFF_A (NUM_LEDS - 5)
#define LED_RUMSTEMP (NUM_LEDS - 6)
#define LED_MAX_POWER (NUM_LEDS - 8)
#define HSV_VAL (128)
#define HSV_VAL_MAX (255)
#define HSV_SAT_WHITE (0)
#define HSV_SAT_COLOR (255)
#define HSV_HUE_RED (0)
#define HSV_HUE_YELLOW (60)
#define HSV_HUE_GREEN (128)
#define HSV_HUE_BLUE (240)
#define HSV_HUE_MAGENTA (300)

// Wi-Fi and MQTT clients
WiFiClient espClient;
PubSubClient client(espClient);

// Globals
Ticker blinker;
bool wifi_ok = false;
bool mqtt_ok = false;
bool blink_on = true;
bool garage_door_open = true;
int price_hue = HSV_HUE_RED;
int current_power = 100;

// ==== Functions ====

void setLedHSV(int index, uint8_t hue, uint8_t sat, uint8_t val) {
  if (index < 0 || index >= NUM_LEDS) return; // bounds check
  leds[index] = CHSV(hue, sat, val);
  FastLED.show();
}

void setRangeHSV(int startIndex, int endIndex, uint8_t hue, uint8_t sat, uint8_t val) {
  if (startIndex < 0) startIndex = 0;
  if (endIndex >= NUM_LEDS) endIndex = NUM_LEDS - 1;
  if (startIndex > endIndex) return;

  for (int i = startIndex; i <= endIndex; i++) {
    leds[i] = CHSV(hue, sat, val);
  }
}

void handlePower()
{
  int disp_power = current_power / 100;
  if (disp_power > LED_MAX_POWER) disp_power = LED_MAX_POWER;

  setRangeHSV(0, LED_MAX_POWER, HSV_HUE_GREEN, HSV_SAT_WHITE, HSV_VAL);
  setRangeHSV(0, disp_power, price_hue, HSV_SAT_COLOR, HSV_VAL);
  FastLED.show();
}

void handlePrice(String msg)
{
  float p=msg.toFloat();
  int current_price=int(p*100);
  Serial.println(current_price);

  if (current_price < 0) current_price=0;
  if (current_price > 512) current_price=512;

  int disp_color = 512 - current_price;
  disp_color = disp_color / 4;
  price_hue=disp_color;
}

void handleTariff(String msg) {
  setRangeHSV(LED_TARIFF_A, LED_TARIFF_C, HSV_HUE_GREEN, HSV_SAT_COLOR, HSV_VAL);
  
  if (msg == "TARIFF_A") setRangeHSV(LED_TARIFF_A, LED_TARIFF_A, HSV_HUE_RED, HSV_SAT_COLOR, HSV_VAL);
  if (msg == "TARIFF_B") setRangeHSV(LED_TARIFF_A, LED_TARIFF_B, HSV_HUE_RED, HSV_SAT_COLOR, HSV_VAL);
  if (msg == "TARIFF_C") setRangeHSV(LED_TARIFF_A, LED_TARIFF_C, HSV_HUE_RED, HSV_SAT_COLOR, HSV_VAL);
  FastLED.show();
}

void handleRumstemp(String msg) {

  int hue = HSV_HUE_RED;
  if (msg == "NORMAL") hue = HSV_HUE_GREEN;
  setLedHSV(LED_RUMSTEMP, hue, HSV_SAT_COLOR, HSV_VAL);
  FastLED.show();
}

void handleGaragedoor(String msg) {

  int hue = HSV_HUE_RED;
  garage_door_open = true;
  if (msg == "CLOSED") {
    hue = HSV_HUE_GREEN;
    garage_door_open = false;
  }
  setLedHSV(LED_GARAGE, hue, HSV_SAT_COLOR, HSV_VAL);
  FastLED.show();
}


void callback(char *top, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(top);
  Serial.print("] ");

  String topic = top;
  String msg;
  for (unsigned int i = 0; i < length; i++)
  {
    msg += (char)payload[i];
  }
  Serial.println(msg);

  if (topic == mqtt_topic_power)
  {
    current_power = msg.toInt();
    handlePower();
  }
  else if (topic == mqtt_topic_price)
  {
    handlePrice(msg);
  }
  else if (topic == mqtt_topic_tariff)
  {
    handleTariff(msg);
  }
  else if (topic == mqtt_topic_rumstemp)
  {
    handleRumstemp(msg);
  }
  else if (topic == mqtt_topic_garagedoor)
  {
    handleGaragedoor(msg);
  }
  else if (topic == mqtt_topic)
  {
    // Handle simple messages
    if (msg == "off")
    {
      fill_solid(leds, NUM_LEDS, CRGB::Black);
      FastLED.show();
    }
    else if (msg == "red")
    {
      fill_solid(leds, NUM_LEDS, CRGB::Red);
      FastLED.show();
    }
    else if (msg == "blue")
    {
      fill_solid(leds, NUM_LEDS, CRGB::Blue);
      FastLED.show();
    }
    else if (msg == "green")
    {
      fill_solid(leds, NUM_LEDS, CRGB::Green);
      FastLED.show();
    }
    else
    {
      Serial.println("Unknown message");
    }
  }
  else
  {
    Serial.println("Unknown topic");
  }
}

void blinkIfIssue(void)
{
  blink_on=!blink_on;

  // Garage Door
  if (garage_door_open) {
    setLedHSV(LED_GARAGE, HSV_HUE_RED, blink_on ? HSV_SAT_WHITE : HSV_SAT_COLOR, HSV_VAL_MAX);
  }

  //WiFi & MQTT
  if (!wifi_ok) {
    setRangeHSV(0, LED_MAX_POWER, blink_on ? HSV_HUE_RED : HSV_HUE_YELLOW, HSV_SAT_COLOR, HSV_VAL);
    FastLED.show();
  } else if (!mqtt_ok) {
    setRangeHSV(0, LED_MAX_POWER, blink_on ? HSV_HUE_MAGENTA : HSV_HUE_BLUE, HSV_SAT_COLOR, HSV_VAL);
    FastLED.show();
  }
}
// ==== Main Setup ====

void setup_wifi()
{
  wifi_ok = false;
  Serial.println();
  WiFi.mode(WIFI_STA);

  int conn_stat = WL_CONNECTED + 1;
  while (conn_stat != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    delay(100);
    WiFi.begin(ssid, password);
    conn_stat = WiFi.waitForConnectResult();
  }
  wifi_ok = true;
  setRangeHSV(0, LED_MAX_POWER, HSV_HUE_RED, HSV_SAT_WHITE, HSV_VAL);
  FastLED.show();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void connect_mqtt()
{
  mqtt_ok = false;
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("PowerLEDClient", mqtt_user, mqtt_pass))
    {
      mqtt_ok = true;
      setRangeHSV(0, LED_MAX_POWER, HSV_HUE_RED, HSV_SAT_WHITE, HSV_VAL);
      FastLED.show();
      Serial.println("connected");
      client.subscribe(mqtt_topic);
      client.subscribe(mqtt_topic_price);
      client.subscribe(mqtt_topic_power);
      client.subscribe(mqtt_topic_tariff);
      client.subscribe(mqtt_topic_rumstemp);
      client.subscribe(mqtt_topic_garagedoor);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(100);

  FastLED.addLeds<LED_TYPE, DATA_PIN, CLK_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  setRangeHSV(0, NUM_LEDS, HSV_HUE_RED, HSV_SAT_WHITE, HSV_VAL);

  FastLED.show();

  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  blinker.attach(0.5, blinkIfIssue); // Call every half second
}

// ==== Main Loop ====

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost, reconnecting...");
    setup_wifi();
  }

  if (!client.connected())
  {
    connect_mqtt();
  }
  client.loop();
}
