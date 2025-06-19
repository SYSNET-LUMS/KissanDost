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
// uint8_t BaseAddress[] = {}

// Must match the receiver structure
typedef struct sensor_data {
  float nitrogen;
  float phosphorous;
  float potassium;
  float moisture;
  float temp;
  float pH;
  float conductivity;
  float battery;
  int ID;
} sensor_data;

sensor_data myData;
sensor_data receivedData;

#define ARRAY_SIZE 4  // Define the size of the array

sensor_data dataArray[ARRAY_SIZE];
int storedDataCount = 0;

bool flag = false; 
bool data_sent = false;

// Create peer interface
esp_now_peer_info_t peerInfo;

// Callback For Sending Data

// void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
//   Serial.print("\r\nLast Packet Send Status:\t");
//   Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  if (status == ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery Success");
    data_sent = true; // Mark data as sent
  } else {
    Serial.println("Delivery Fail");
  }
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

  flag = true ; 



  delay(2000);
}

void setup() {
  // Init Serial Monito
  Serial.begin(9600);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // int *a = 0;
  // esp_wifi_get_max_tx_power(a);
  // Serial.println(a);
  Serial.println(esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_LR));

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

  // Setup the wake-up source as timer
  // esp_sleep_enable_timer_wakeup(600000000); // 10 minutes in microseconds
  // Serial.println("Setup ESP32 to sleep for every " + String(600000000) +
  // " microSeconds");

  Serial.println("Repeater ESP going to sleep for 8 minutes and 3 seconds...");
  esp_sleep_enable_timer_wakeup((8 * 60) * 1000000); // 8 minutes and 3 seconds in microseconds
}

void loop()
{
  for (int i = 0; i < 10; i++) {
    
    if (flag == true){
      esp_now_send(broadcastAddress, (uint8_t *) &receivedData, sizeof(receivedData));
      flag = false;
    }

    delay(12000); // Wait for 12 seconds before sending again
  }

  flag = false;

  Serial.println("Data sent successfully. Going to sleep now.");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}