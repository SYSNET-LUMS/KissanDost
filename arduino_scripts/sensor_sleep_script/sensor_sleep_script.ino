#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_sleep.h>

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

bool new_data = false; // Flag to send data only once
bool new_rec_data = false;

// Universal MAC Address
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

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

bool rec_data = false;

// Global array to store the hash value
unsigned long hashArray[50];
int hashCount = 0;

// DJB2 hash function
unsigned long djb2Hash(unsigned char *str, unsigned int len) {
  unsigned long hash = 5381;
  for (unsigned int i = 0; i < len; i++) {
    hash = ((hash << 5) + hash) + str[i]; // hash * 33 + str[i]
  }
  return hash;
}

// Function to calculate and store the hash value of the sensor_data
unsigned long calculateHash(sensor_data data) {
  unsigned char *byteData = (unsigned char*)&data;
  unsigned int len = sizeof(data);
  unsigned long hashValue = djb2Hash(byteData, len);
  
  return hashValue;
}

// Create peer interface
esp_now_peer_info_t peerInfo;

// Callback For Sending Data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
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

  delay(100);
}

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  SerialPort.begin(4800, SERIAL_8N1, 25, 27); // Change the pins according to your ESP32's hardware serial pins

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Set maximum transmission power (84 corresponds to 20.5 dBm)
  esp_wifi_set_max_tx_power(84);

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

  Serial.println("Sensor ESP going to sleep for 30 minutes...");
  esp_sleep_enable_timer_wakeup(30 * 60 * 1000000); // 30 minutes in microseconds
}

void loop()
{
  // Read all sensors
  printNPKValues();
  // delay(100);

  printPHValue();
  // delay(100);

  printSoilMoisture();
  // delay(100);

  printSoilTemperature();
  // delay(100);

  printConductivity();
  // delay(100);

  printBatteryPercentage();
  // delay(100);

  // if (hashCount == 0){
  //   new_rec_data = true;

  //   // Calculating Hash
  //   unsigned long hash = calculateHash(receivedData);

  //   hashArray[hashCount] = hash;
  //   hashCount++;
  // }
  // else{
  //   int flag = 0;
  //   for (int i = 0; i < hashCount; i++){
  //     if(hashArray[i] == calculateHash(receivedData)){
  //       flag = 1;
  //     }
  //   }

  //   if (flag == 0){
  //     new_rec_data = true;

  //     // Calculating Hash
  //     unsigned long hash = calculateHash(receivedData);

  //     hashArray[hashCount] = hash;
  //     hashCount++;

  //     if (hashCount == 50){
  //       hashCount = 0;
  //     }

  //     Serial.println("hashes:");
  //     Serial.println(hashArray[hashCount - 1]);
  //   }
  // }

  // Wait for new data from ESP-NOW
  // while (!new_data && !new_rec_data) {
  //   delay(1000); // Wait for a second before checking again
  // }

  // Transmit the data 5 times over 1 minute
  // for (int i = 0; i < 5; i++) {
  //   // Transmit myData and receivedData
  //   myData.ID = espID;
  //   esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  //   if (rec_data){
  //     result = esp_now_send(broadcastAddress, (uint8_t *)&receivedData, sizeof(receivedData));
  //   }

  //   if (result == ESP_OK) {
  //     Serial.println("Successfully Sent Data");
  //   } else {
  //     Serial.println("Error Sending Data");
  //   }

  //   delay(12000); // Wait for 12 seconds before sending again
  // }

  // Transmit myData and receivedData
  myData.ID = espID;
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
  if (rec_data){
    result = esp_now_send(broadcastAddress, (uint8_t *)&receivedData, sizeof(receivedData));
  }

  if (result == ESP_OK) {
    Serial.println("Successfully Sent Data");
  } else {
    Serial.println("Error Sending Data");
  }

  delay(1000);

  rec_data = false;

  // Enter deep sleep
  Serial.println("Going to sleep now");

  esp_now_deinit(); // Properly deinitialize ESP-NOW
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF); // Turn off Wi-Fi completely

  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void printNPKValues() {
  // Send query to the sensor
  SerialPort.write(npkQuery, sizeof(npkQuery));

  // Wait for response from the sensor
  delay(100);

  // Check if there's data available to read
  if (SerialPort.available() >= 11) {
    uint8_t npkResponse[11];
    SerialPort.readBytes(npkResponse, sizeof(npkResponse));

    uint16_t nitrogen = (npkResponse[3] << 8) | npkResponse[4];
    uint16_t phosphorus = (npkResponse[5] << 8) | npkResponse[6];
    uint16_t potassium = (npkResponse[7] << 8) | npkResponse[8];

    myData.nitrogen = nitrogen;
    myData.phosphorous = phosphorus;
    myData.potassium = potassium;

    Serial.println("NPK Values:");
    Serial.print("Nitrogen: ");
    Serial.println(nitrogen);
    Serial.print("Phosphorus: ");
    Serial.println(phosphorus);
    Serial.print("Potassium: ");
    Serial.println(potassium);
  } 
}

void printPHValue() {
  SerialPort.write(phQuery, sizeof(phQuery));
  delay(100);

  if (SerialPort.available() >= 7) {
    uint8_t phResponse[7];
    SerialPort.readBytes(phResponse, sizeof(phResponse));

    uint16_t phValue = (phResponse[3] << 8) | phResponse[4];
    float pH = phValue / 10.0;

    myData.pH = pH;

    Serial.println("pH Value:");
    Serial.println(pH);
  }
}

void printSoilMoisture() {
  SerialPort.write(soilMoistureQuery, sizeof(soilMoistureQuery));
  delay(100);

  if (SerialPort.available() >= 7) {
    uint8_t moistureResponse[7];
    SerialPort.readBytes(moistureResponse, sizeof(moistureResponse));

    uint16_t moistureValue = (moistureResponse[3] << 8) | moistureResponse[4];
    float moisture = moistureValue / 10.0;

    myData.moisture = moisture;

    Serial.println("Soil Moisture:");
    Serial.println(moisture);
  }
}

void printSoilTemperature() {
  SerialPort.write(soilTemperatureQuery, sizeof(soilTemperatureQuery));
  delay(100);

  if (SerialPort.available() >= 7) {
    uint8_t temperatureResponse[7];
    SerialPort.readBytes(temperatureResponse, sizeof(temperatureResponse));

    uint16_t temperatureValue = (temperatureResponse[3] << 8) | temperatureResponse[4];
    float temperature = temperatureValue / 10.0;

    myData.temp = temperature;

    Serial.println("Soil Temperature:");
    Serial.println(temperature);
  }
}

void printConductivity() {
  SerialPort.write(conductivityQuery, sizeof(conductivityQuery));
  delay(100);

  if (SerialPort.available() >= 7) {
    uint8_t conductivityResponse[7];
    SerialPort.readBytes(conductivityResponse, sizeof(conductivityResponse));

    uint16_t conductivityValue = (conductivityResponse[3] << 8) | conductivityResponse[4];
    float conductivity = conductivityValue;

    myData.conductivity = conductivity;

    Serial.println("Conductivity:");
    Serial.println(conductivity);
  }
}

int getBatteryPercentage(float voltage) {
    if (voltage >= 12.89) return 100.0;
    else if (voltage <= 11.63) return 0.0;
    
    // Array of voltage and capacity pairs
    float voltageLevels[] = {12.89, 12.78, 12.65, 12.51, 12.41, 12.23, 12.11, 11.96, 11.81, 11.70, 11.63};
    float capacities[] = {100.0, 90.0, 80.0, 70.0, 60.0, 50.0, 40.0, 30.0, 20.0, 10.0, 0.0};

    // Find the appropriate interval for interpolation
    for (int i = 0; i < 10; i++) {
        if (voltage >= voltageLevels[i+1]) {
            // Linear interpolation formula
            return capacities[i] + (voltage - voltageLevels[i]) * (capacities[i+1] - capacities[i]) / (voltageLevels[i+1] - voltageLevels[i]);
        }
    }
    return 0.0;  // Default case (shouldn't reach here)
}

void printBatteryPercentage() {
  float sensorValue = 0;
  const int numReadings = 100;

  // Take 100 readings in a for loop
  for (int i = 0; i < numReadings; i++) {
    sensorValue += analogRead(sensorPin);
  }

  // Average the sensor readings
  sensorValue = sensorValue / numReadings;

  // Print the raw sensor value
  Serial.print("Raw: ");
  Serial.print(sensorValue);
  Serial.println();

  int batteryPercentage = getBatteryPercentage(7.08234 * ((sensorValue / 4096.0) * 3.3));

  myData.battery = batteryPercentage;
  new_data = true;

  // Calculate and print the battery percentage
  Serial.print("Battery Percentage: ");
  Serial.print(batteryPercentage);
  Serial.print("%");
  Serial.println();
}
