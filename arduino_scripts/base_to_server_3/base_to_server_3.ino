#include <esp_now.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <esp_wifi.h>

// Define the Serial port to communicate with ESP32
#define SerialPort Serial2 // Change this if you're using a different serial port

int espID = 1; // Adjust Accordingly

// Define the queries to send to the sensor
uint8_t soilMoistureQuery[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x01, 0x84, 0x0A}; // Read Humidity (works)
uint8_t soilTemperatureQuery[] = {0x01, 0x03, 0x00, 0x01, 0x00, 0x01, 0xD5, 0xCA}; // Read Temperature (works)
uint8_t conductivityQuery[] = {0x01, 0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xCA}; // Read Conductivity (works)
uint8_t phQuery[] = {0x01, 0x03, 0x00, 0x03, 0x00, 0x01, 0x74, 0x0A}; 
uint8_t npkQuery[] = {0x01, 0x03, 0x00, 0x04, 0x00, 0x03, 0x44, 0x0a}; 

// For Battery Percentage
int sensorPin = 35;   // select the input pin for the potentiometer
float sensorValue = 0;  // variable to store the value coming from the sensor
int count = 0; 

bool new_data = 0; // Flag to send data only once

// Universal MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Must match the receiver structure
typedef struct sensor_data {
  int16_t nitrogen;
  int16_t phosphorous;
  int16_t potassium;
  int16_t moisture;     // Scaled to one decimal place (e.g., 65.4% -> 654)
  int16_t temp;         // Scaled to one decimal place (e.g., 23.7°C -> 237)
  int16_t pH;           // Scaled to one decimal place (e.g., 7.1 -> 71)
  int16_t conductivity; // Same as above
  uint8_t battery;      // 0 to 100 fits in 1 byte
  uint8_t ID;           // 1 to 9 fits in 1 byte
} sensor_data;

sensor_data myData;
sensor_data receivedData;

#define ARRAY_SIZE 4  // Define the size of the array

sensor_data dataArray[ARRAY_SIZE];
int storedDataCount = 0;

bool flag = false; 

const char* ssid = "Ufone-LTE-FBD7D";
const char* password = "7A5FBD7D";

// Create peer interface
esp_now_peer_info_t peerInfo;

// Callback For Sending Data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void sendDataToFastAPI() {
  
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_STA);

  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B);

  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected to WiFi");
  
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Construct the URL with query parameters
    String url = "https://pandas-production-7843.up.railway.app/addReadingRecord?";
    url += "pH=" + String(receivedData.pH, 1) + "&";
    url += "nitrogen=" + String(receivedData.nitrogen, 1) + "&";
    url += "phosphorous=" + String(receivedData.phosphorous, 1) + "&";
    url += "potassium=" + String(receivedData.potassium, 1) + "&";
    url += "temperature=" + String(receivedData.temp, 1) + "&";
    url += "moisture=" + String(receivedData.moisture, 1) + "&";
    url += "conductivity=" + String(receivedData.conductivity, 1) + "&";
    url += "battery=" + String(receivedData.battery, 1) + "&";
    url += "user_id=" + String(receivedData.ID);

    http.begin(url);

    // Send HTTP POST request
    int httpResponseCode = http.POST(""); // Send an empty body

    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode); // Print return code
        Serial.println(response); // Print request answer
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    http.end(); // Free resources
  } else {
    Serial.println("WiFi Disconnected");
  }

  // Turn WiFi off again to free resources for ESP-NOW
  WiFi.mode(WIFI_OFF);
  delay(100);
  WiFi.mode(WIFI_STA);

  esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);

  // Reinitialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Re-register callback functions
  esp_now_register_send_cb(OnDataSent);
  esp_now_register_recv_cb(OnDataRecv);
}

// Callback for Receiving Data
void OnDataRecv(const esp_now_recv_info * mac, const uint8_t *incomingData, int len) {
  memcpy(&receivedData, incomingData, sizeof(receivedData));

  // Print to Serial Monitor
  Serial.print("Bytes received: ");
  Serial.println(len);
  
  Serial.println("NPK Values:");
  Serial.print("Nitrogen: ");
  Serial.println(receivedData.nitrogen);
  Serial.print("Phosphorus: ");
  Serial.println(receivedData.phosphorous);
  Serial.print("Potassium: ");
  Serial.println(receivedData.potassium);

  Serial.println("pH Value:");
  Serial.println(receivedData.pH);

  Serial.println("Soil Moisture:");
  Serial.println(receivedData.moisture);

  Serial.println("Soil Temperature:");
  Serial.println(receivedData.temp);

  Serial.println("Conductivity:");
  Serial.println(receivedData.conductivity);

  Serial.print("Battery Percentage: ");
  Serial.print(receivedData.battery);
  Serial.print("%");

  Serial.println();
  Serial.println("ID:");
  Serial.println(receivedData.ID);
  Serial.println();

  flag = true; 
}

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

	esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR);
  delay(100);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Send Callback Function
  esp_now_register_send_cb(OnDataSent);

  // Receive Callback Function
  esp_now_register_recv_cb(OnDataRecv);

  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  if (flag == true) {
    sendDataToFastAPI();
    flag = false;
  }

  delay(100);
}
