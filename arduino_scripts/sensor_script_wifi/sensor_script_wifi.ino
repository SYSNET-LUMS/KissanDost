#include <WiFi.h>
#include <HTTPClient.h>
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

bool new_data = 0; // Flag to send data only once

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

// const char* ssid = "TP-Link_7F22";
// const char* password = "66157748";

const char* ssid = "Ufone-LTE-FBD7D";
const char* password = "7A5FBD7D";

void setup() {
  // Init Serial Monitor
  Serial.begin(9600);
  SerialPort.begin(4800, SERIAL_8N1, 25, 27); // Change the pins according to your ESP32's hardware serial pins

  // Setup the wake-up source as timer
  esp_sleep_enable_timer_wakeup(600000000); 
  Serial.println("Setup ESP32 to sleep for every " + String(600000000) +
  " microSeconds");
}

void loop()
{
  // Read NPK values
  printNPKValues();
  delay(1000); // Adjust delay according to your needs

  // Read pH value
  printPHValue();
  delay(1000); // Adjust delay according to your needs

  // Read soil moisture
  printSoilMoisture();
  delay(1000); // Adjust delay according to your needs

  // Read soil temperature
  printSoilTemperature();
  delay(1000); // Adjust delay according to your needs

  // Read conductivity value
  printConductivity();
  delay(1000); // Adjust delay according to your needs

  // Get Battery Percentage of Power Supply
  printBatteryPercentage();
  delay(1000);

  // Send message via ESP-NOW
  if (new_data == 0){
    myData.ID = espID;
    sendDataToFastAPI();
    new_data = 1;
  }

  // Enter deep sleep
  Serial.println("Going to sleep now");
  delay(1000);
  Serial.flush(); 
  esp_deep_sleep_start();
  Serial.println("This will never be printed");
}

void printNPKValues() {
  // Send query to the sensor
  SerialPort.write(npkQuery, sizeof(npkQuery));

  // Wait for response from the sensor
  delay(100); // Adjust delay as needed

  // Check if there's data available to read
  if (SerialPort.available() >= 11) { // Adjust the number based on the expected response length
    // Read the respons

    uint8_t npkResponse[11]; // Adjust the size based on the expected response length
    SerialPort.readBytes(npkResponse, sizeof(npkResponse));

    // Extract N, P, and K values from the received response
    uint16_t nitrogen = (npkResponse[3] << 8) | npkResponse[4];
    uint16_t phosphorus = (npkResponse[5] << 8) | npkResponse[6];
    uint16_t potassium = (npkResponse[7] << 8) | npkResponse[8];

    // Adding to Struct
    myData.nitrogen = nitrogen;
    myData.phosphorous = phosphorus;
    myData.potassium = potassium;

    // Print the values
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
  // Send query to the sensor
  SerialPort.write(phQuery, sizeof(phQuery));

  // Wait for response from the sensor
  delay(100); // Adjust delay as needed

  // Check if there's data available to read
  if (SerialPort.available() >= 7) { // Adjust the number based on the expected response length
    // Read the response
    uint8_t phResponse[7]; // Adjust the size based on the expected response length
    SerialPort.readBytes(phResponse, sizeof(phResponse));

    // Extract pH value from the received response
    uint16_t phValue = (phResponse[3] << 8) | phResponse[4];
    float pH = phValue / 10.0; // Convert to decimal

    // Adding to Struct
    myData.pH = pH;

    // Print the value
    Serial.println("pH Value:");
    Serial.println(pH);
  }
}

void printSoilMoisture() {
  // Send query to the sensor
  SerialPort.write(soilMoistureQuery, sizeof(soilMoistureQuery));

  // Wait for response from the sensor
  delay(100); // Adjust delay as needed

  // Check if there's data available to read
  if (SerialPort.available() >= 7) { // Adjust the number based on the expected response length
    // Read the response
    uint8_t moistureResponse[7]; // Adjust the size based on the expected response length
    SerialPort.readBytes(moistureResponse, sizeof(moistureResponse));

    // Extract soil moisture value from the received response
    uint16_t moistureValue = (moistureResponse[3] << 8) | moistureResponse[4];
    float moisture = moistureValue / 10.0; // Convert to decimal

    // Adding To Struct
    myData.moisture = moisture;

    // Print the value
    Serial.println("Soil Moisture:");
    Serial.println(moisture);
  }
}

void printSoilTemperature() {
  // Send query to the sensor
  SerialPort.write(soilTemperatureQuery, sizeof(soilTemperatureQuery));

  // Wait for response from the sensor
  delay(100); // Adjust delay as needed

  // Check if there's data available to read
  if (SerialPort.available() >= 7) { // Adjust the number based on the expected response length
    // Read the response
    uint8_t temperatureResponse[7]; // Adjust the size based on the expected response length
    SerialPort.readBytes(temperatureResponse, sizeof(temperatureResponse));

    // Extract soil temperature value from the received response
    uint16_t temperatureValue = (temperatureResponse[3] << 8) | temperatureResponse[4];
    float temperature = temperatureValue / 10.0; // Convert to decimal

    // Adding to Struct
    myData.temp = temperature;

    // Print the value
    Serial.println("Soil Temperature:");
    Serial.println(temperature);
  }
}

void printConductivity() {
  // Send query to the sensor
  SerialPort.write(conductivityQuery, sizeof(conductivityQuery));

  // Wait for response from the sensor
  delay(100); // Adjust delay as needed

  // Check if there's data available to read
  if (SerialPort.available() >= 7) { // Adjust the number based on the expected response length
    // Read the response
    uint8_t conductivityResponse[7]; // Adjust the size based on the expected response length
    SerialPort.readBytes(conductivityResponse, sizeof(conductivityResponse));

    // Extract conductivity value from the received response
    uint16_t conductivityValue = (conductivityResponse[3] << 8) | conductivityResponse[4];

    myData.conductivity = conductivityValue;

    // Print the value
    Serial.println("Conductivity:");
    Serial.println(conductivityValue);
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
  new_data = 0;

  // Calculate and print the battery percentage
  Serial.print("Battery Percentage: ");
  Serial.print(batteryPercentage);
  Serial.print("%");
  Serial.println();
}

void sendDataToFastAPI() {
  // Connect to WiFi
  delay(100);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  unsigned long startAttemptTime = millis(); // Record the start time
  unsigned long timeout = 60000; // Set timeout to 60 seconds (1 minute)

  while (WiFi.status() != WL_CONNECTED) {
    // Check if the time since starting is greater than the timeout
    if (millis() - startAttemptTime >= timeout) {
      Serial.println(" Failed to connect within 60 seconds.");
      return; // Exit the function if timeout is reached
    }
    
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Connected to WiFi");

  
  if (WiFi.status() == WL_CONNECTED) { // Check WiFi connection status
    HTTPClient http; // Declare object of class HTTPClient

    // Construct the URL with query parameters
    String url = "https://pandas-production-7843.up.railway.app/addReadingRecord?";
    url += "pH=" + String(myData.pH, 2) + "&";
    url += "nitrogen=" + String(myData.nitrogen, 2) + "&";
    url += "phosphorous=" + String(myData.phosphorous, 2) + "&";
    url += "potassium=" + String(myData.potassium, 2) + "&";
    url += "temperature=" + String(myData.temp, 2) + "&";
    url += "moisture=" + String(myData.moisture, 2) + "&";
    url += "conductivity=" + String(myData.conductivity, 2) + "&";
    url += "battery=" + String(myData.battery, 2) + "&";
    url += "user_id=" + String(myData.ID);

    http.begin(url); // Specify the URL with parameters

    // Send HTTP POST request
    int httpResponseCode = http.POST(""); // Send an empty body

    // Print response
    if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println(httpResponseCode); // Print return code
        Serial.println(response); // Print request answer
    } else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}