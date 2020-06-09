// Dependencies
#include <Arduino.h>
#include <ArtnetWifi.h>
#include <FastLED.h>
#include <esp_wifi.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>

// LED settings
const uint8_t kMatrixWidth = 15;
const uint8_t kMatrixHeight = 9;
const int numLeds = kMatrixWidth * kMatrixHeight;
const int numberOfChannels = numLeds * 3; // (1 led = 3 channels)
const byte dataPin = 4;
CRGB leds[numLeds];

// Art-Net settings
ArtnetWifi artnet;
const int startUniverse = 0; 
const int maxUniverses = numberOfChannels / 512 + ((numberOfChannels % 512) ? 1 : 0);
bool universesReceived[maxUniverses];
bool sendFrame = 1;
int previousDataLength = 0;

// Security Settings, used both for WiFi portal as well as OTA-update authentication
#define DEVICE_NAME "bloclock"
#define PASSWORD "block1234"

unsigned long lastFrameReceived = 0;

/**
 * Function to test all LEDs, called after connection is made
*/
void initTest()
{
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB(127, 0, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB(0, 127, 0);
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB(0, 0, 127);
  }
  FastLED.show();
  delay(500);
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show();
}
/*
* XY Map For serpentine layout, generated with https://macetech.github.io/FastLED-XY-Map-Generator/
*/
uint8_t XY(uint8_t x, uint8_t y)
{
  if ((x >= kMatrixWidth) || (y >= kMatrixHeight))
  {
    return (numLeds + 2);
  }

  const uint8_t XYTable[] = {
      0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
      29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15,
      30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
      59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,
      60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
      89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75,
      90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104,
      119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, 108, 107, 106, 105,
      120, 121, 122, 123, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134};

  uint8_t i = (y * kMatrixWidth) + x;
  uint8_t j = XYTable[i];
  return j;
}

/**
 * Handles a Art-net DMX frame, sets all LEDs to the received color
*/
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t *data)
{
  sendFrame = 1;
  //Command to set brightness of the whole strip
  if (universe == 15)
  {
    FastLED.setBrightness(data[0]);
    FastLED.show();
  }

  // Store which universe has got in
  if ((universe - startUniverse) < maxUniverses)
  {
    universesReceived[universe - startUniverse] = 1;
  }

  for (int i = 0; i < maxUniverses; i++)
  {
    if (universesReceived[i] == 0)
    {
      sendFrame = 0;
      break;
    }
  }

  // Read universe and put into the right part of the display buffer
  for (int i = 0; i < length / 3; i++)
  {
    int led = i + (universe - startUniverse) * (previousDataLength / 3);
    if (led < numLeds)
      leds[led] = CRGB(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
  }
  previousDataLength = length;

  if (sendFrame)
  {
    FastLED.show();

    // Reset universeReceived to 0
    memset(universesReceived, 0, maxUniverses);
  }

  lastFrameReceived = millis();
}

void setupOTA(){

  // Set up Arduino OTA update system
  ArduinoOTA.setHostname(DEVICE_NAME);
  ArduinoOTA.setPassword(PASSWORD);

  // Register Callbacks
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR)
      Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR)
      Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR)
      Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR)
      Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR)
      Serial.println("End Failed");
  });
  ArduinoOTA.begin();
  Serial.println("Ready of OTA updates");
}

void setup() {

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  esp_wifi_set_ps(WIFI_PS_NONE);

  Serial.begin(115200);
  Serial.println("Booting...");

  // Set up FastLED and test all lights
  FastLED.addLeds<WS2812B, dataPin, GRB>(leds, numLeds);
  initTest();

  // Set up WiFi Manager
  WiFiManager wm;
//  wm.resetSettings();
  wm.setClass("invert");

  if (wm.autoConnect(DEVICE_NAME, PASSWORD)){

    Serial.println("Connected with IP address:");
    Serial.println(WiFi.localIP());

    initTest();

    artnet.begin();
    artnet.setArtDmxCallback(onDmxFrame);
    setupOTA();
  }
}

void loop() {
  ArduinoOTA.handle();
  artnet.read();
  unsigned long lastFrameMsAgo = millis() - lastFrameReceived;

// Reset display if no frames are received for a second
if (lastFrameMsAgo >= 5000)
  {
    FastLED.clear();
    FastLED.show();
  }
}