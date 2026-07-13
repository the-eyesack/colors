#include <FastLED.h>
#include <esp_now.h>
#include <WiFi.h>

// LED Configuration
#define DATA_PIN    0    
#define NUM_LEDS    11  
#define BRIGHTNESS  50   
#define LED_TYPE    WS2812B 
#define COLOR_ORDER GRB   

CRGB leds[NUM_LEDS];

// MATCHING STRUCTURE: Must be identical to the base code!
typedef struct struct_message {
    float distance;
    bool active;
} struct_message;

struct_message receivedData; // Holds incoming data packet

const int START_R = 0,   START_G = 0,   START_B = 255; // Blue at Position 0.0
const int END_R   = 255, END_G   = 127, END_B = 0;   // Orange at Position 1.0

void setup() {
    Serial.begin(115200);
    delay(2000); 
    
    // Initialize FastLED
    FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clear();
    FastLED.show();

    // Set Wi-Fi mode
    WiFi.mode(WIFI_STA);

    // Initialize ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
   
    // Register the receiver callback function
}

void loop() {
  fill_solid(leds, NUM_LEDS, CHSV(255, 255, 255));
  FastLED.show();
  delay(100); 
    
}