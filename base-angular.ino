#include <light_CD74HC4067.h>
#include <esp_now.h>
#include <WiFi.h>
#include <math.h> // Included for cosf, sinf, and atan2f

// REPLACE WITH YOUR RECEIVER MAC Address
uint8_t broadcastAddress[] = {0xA4, 0xCB, 0x8F, 0x22, 0x16, 0x58};

typedef struct struct_message {
    float distance; // Transmits the universal 0.0 to 1.0 ratio
    bool active;
} struct_message;

struct_message myData; 
esp_now_peer_info_t peerInfo;

void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status) {
  // Callback status tracking if needed
}

CD74HC4067 mux(4, 5, 6, 7); // Your verified multiplexer address pins

// Set this to your actual number of physical Hall sensors (e.g., 8 or 16)
const int NUM_CHANNELS = 16;
const int TESTCHANNEL[] = {0, 1, 2, 3, 4, 5, 6, 7,8,9,10,11,12,13,14,15}; 

const int SIGNAL1 = 1; 
const float noiseGate = 160.0;

// Independent baselines for all channels
int baselineADC[NUM_CHANNELS] = {0}; 
int calibrationReadings = 500;

// Pre-calculated angles for each sensor distributed evenly around a circle (in radians)
float CHANNEL_ANGLES[NUM_CHANNELS];

void setup() {
  Serial.begin(115200);
  
  pinMode(SIGNAL1, INPUT);

  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  // Calculate the physical radial angular position for each sensor channel
  for(int i = 0; i < NUM_CHANNELS; i++) {
    CHANNEL_ANGLES[i] = ((float)i * 2.0f * M_PI) / (float)NUM_CHANNELS;
  }

  delay(1000);
  Serial.print("Calibrating all "); Serial.print(NUM_CHANNELS); Serial.println(" individual channels...");
  
  // Calibrate each channel independently
  for(int ch = 0; ch < NUM_CHANNELS; ch++) {
    mux.channel(TESTCHANNEL[ch]);
    delay(50); // Let the channel fully open
    
    long sum = 0;
    for(int i = 0; i < calibrationReadings; i++) {
      sum += analogRead(SIGNAL1);
      delay(2); 
    }
    baselineADC[ch] = sum / calibrationReadings; 
    Serial.print("Channel "); Serial.print(TESTCHANNEL[ch]);
    Serial.print(" Baseline: "); Serial.println(baselineADC[ch]);
  }
}

void loop() {
  float intensities[NUM_CHANNELS];
  for(int i = 0; i < NUM_CHANNELS; i++) intensities[i] = 0.0;

  float totalIntensitySum = 0.0;
  
  // X and Y vector components for 2D circular tracking
  float totalX = 0.0f;
  float totalY = 0.0f;
  
  // 1. Scan active sensors
  for(int channel = 0; channel < NUM_CHANNELS; channel++) {
    mux.channel(TESTCHANNEL[channel]); 
    delayMicroseconds(50); // High-speed hardware settle time (avoids bleeding)
    
    int hallValue = analogRead(SIGNAL1);
    
    // Calculate raw absolute deviation from each unique baseline
    intensities[channel] = abs(hallValue - baselineADC[channel]);
    
    // Sum up the raw values for the physics center-of-mass vector math
    totalIntensitySum += intensities[channel];
    
    // Decompose the active intensity into spatial vector coordinates
    totalX += intensities[channel] * cosf(CHANNEL_ANGLES[channel]);
    totalY += intensities[channel] * sinf(CHANNEL_ANGLES[channel]);
  }
  
  // 2. Process math if a true magnet signal is present (Gate check)
  if (totalIntensitySum > noiseGate) { 
    
    // Calculate continuous radial angle (-PI to +PI)
    float angle = atan2f(totalY, totalX);
    
    // Cleanly wrap the range from [-PI, PI] to [0.0, 2*PI]
    if (angle < 0.0f) {
        angle += 2.0f * M_PI;
    }
    
    // Normalize into a clean, un-snapping 0.0 to 1.0 universal circular distance ratio
    float relDist = angle / (2.0f * M_PI);
    
    // Debug print loop to output readings cleanly
    for(int i = 0; i < NUM_CHANNELS; i++) {
      Serial.print(TESTCHANNEL[i]); 
      Serial.print(": "); 
      Serial.print(intensities[i]); 
      Serial.print(" / ");
    }
    Serial.print(" | Target Pos: "); 
    Serial.println(relDist);
    
    // Pack and send data via ESP-NOW
    myData.distance = relDist; 
    myData.active = true;
  } else {
    Serial.println("No magnet detected within range.");
    myData.active = false;
    myData.distance = 0.0; 
  }

  // Broadcast the data frame to the receiver
  esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  delay(200); // 5Hz transmission frame rate for stable, real-time positional updates
}