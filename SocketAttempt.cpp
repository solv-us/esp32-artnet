#include <Arduino.h>
#include <WiFi.h>
#include <SocketIoClient.h>
#include <FastLED.h>

/// WIFI Settings ///
const char *ssid = "daniel";
const char *password = "hallohallo";

/// Socket.IO Settings ///
char host[] = "192.168.178.12";                 // Socket.IO Server Address
int port = 8080;                                 // Socket.IO Port Address
char path[] = "/socket.io/?transport=websocket"; // Socket.IO Base Path
bool useSSL = true;                             // Use SSL Authentication
const char *sslFingerprint = "";                 // SSL Certificate Fingerprint
bool useAuth = false;                            // use Socket.IO Authentication
const char *serverUsername = "socketIOUsername";
const char *serverPassword = "socketIOPassword";

/// Pin Settings ///
int LEDPin = BUILTIN_LED;
int buttonPin = 0;

CRGB color = CRGB(180, 160, 140);
const int numberOfLeds = 60;     // The amount of LEDS in the strip
const int ledDataPin = 4;        // The PIN no. to which the strips data cable is connected
const int masterBrightness = 255; // Overall brightness of the lamp (0-255)
CRGB ledArray[numberOfLeds];

/////////////////////////////////////
////// ESP32 Socket.IO Client //////
///////////////////////////////////

SocketIoClient webSocket;
WiFiClient client;

bool LEDState = false;
bool Color = false;
int Beat = 1;

void socket_Connected(const char *payload, size_t length)
{
  Serial.println("Socket.IO Connected!");
}

void socket_event(const char *payload, size_t length)
{
  Serial.print("Received time from server: ");
  Serial.println(payload);
  for (int i = 0; i < numberOfLeds; i++)
  {
    ledArray[i] = color;

    int iMin = Beat*15 -15;
    int iMax = Beat*15;
    if(i >= iMin && i <= iMax){
      //
    }else{
      ledArray[i].fadeLightBy(240);
    }
  }

  Beat += 1;
  if(Beat > 4){
    Beat = 1;
  }

  FastLED.show();
}

void changeColor(const char *payload, size_t length)
{
  Serial.print("Received color from server: ");
  Serial.println(payload);
  
  
  if(Color){
    color= CRGB(30, 100, 255);
  } else {
    color  = CRGB(255, 0, 140);
  }
  fill_solid(ledArray, numberOfLeds, color);
  Color = !Color;
}

  void setup()
  {
    Serial.begin(115200);
    // Initialize the FastLED library
    FastLED.addLeds<NEOPIXEL, ledDataPin>(ledArray, numberOfLeds);
    FastLED.setBrightness(masterBrightness);
    fill_solid(ledArray, numberOfLeds, color);

    delay(10);

    pinMode(LEDPin, OUTPUT);
    pinMode(buttonPin, INPUT);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Setup 'on' listen events
    webSocket.on("connect", socket_Connected);
    webSocket.on("beat", socket_event);
    webSocket.on("color", changeColor);

    // Setup Connection
    if (useSSL)
    {
      webSocket.beginSSL(host, port, path, sslFingerprint);
    }
    else
    {
      webSocket.begin(host, port, path);
    }

    // Handle Authentication
    if (useAuth)
    {
      webSocket.setAuthorization(serverUsername, serverPassword);
    }
  }

  void loop()
  {
    webSocket.loop();
    FastLED.show();

    FastLED.delay(5);
  }