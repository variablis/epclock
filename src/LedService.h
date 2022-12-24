#ifdef ESP32
#include <HTTPClient.h>
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#endif

#include <FastLED.h>
#include <ArduinoJson.h>

#define NUM_LEDS 144 // leds in ledstrip
#define DATA_PIN 15
#define UPD_LEDS 20000 // 1000ms = 1s
#define UPD_JSON 60000

class LedService {
    CRGB leds[NUM_LEDS];
    unsigned long stime1, stime2, stime3;
    int colors[6] = {CRGB::Black, CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::OrangeRed, CRGB::Red};

    StaticJsonDocument<4096> doc;
    String payload;

    public:
        LedService();
        void setup();
        void updateLeds();
        void getJson();
};
