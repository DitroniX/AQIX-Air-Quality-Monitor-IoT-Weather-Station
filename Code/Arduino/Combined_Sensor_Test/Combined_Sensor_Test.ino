/*
  Dave Williams, DitroniX 2019-2026 (ditronix.net)
  CWX(C6), AQIX and AQIL Compact Weather Station and Air Quality Monitors - ESP32-C6 Series Board
  Features include ESP32-C6, BME280, SCD40/SCD41 Air Quality CO2 Sensor, SFH2440 Light Sensor, CR123A, Battery Sensor, USB ESP32C6 Dev Board  Flashing Compatible

  PCA 1.2603 - CWX(C6), AQIX and AQIL - Basic Bring Up Test Code - March 2026
 
  Remember!
  - Set the BOARD to Use ESP32C6 Dev Module (or similar).
  - You can also set the BAUD rate up to 921600 to speed up flashing.
  - The SDK does NOT need external power to flash.  It will take Power from the USB 5V.
  - The Serial Monitor is configured for BAUD 115200
  
  The purpose of this test code is to cycle through the various main functions of the board as part of bring up testing.
  .
  This test code is OPEN SOURCE and formatted for easier viewing.  Although is is not intended for real world use, it may be freely used, or modified as needed.
  It is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

  Further information, details and examples can be found on our website or github.com/DitroniX

  CWX           https://github.com/DitroniX/CWX-Compact-Weather-Station
  AQIX          https://github.com/DitroniX/AQIX-Air-Quality-Monitor-IoT-Weather-Station
  AQIL (LoRA)   https://github.com/DitroniX/AQIL-ESP32-C6-SX1262-LoRa-Air-Quality-Monitor-IoT-Weather-Station
*/

// Libraries
#include <Arduino.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>
#include <Adafruit_BME280.h>  // Remember to install the Adafruit_BME280 library
#include <Adafruit_Sensor.h>

// ****************  VARIABLES / DEFINES / STATIC ****************

// Variables
char SensorResult[10];
float TemperatureC;
float Humidity;
int Pressure;
float ADC_Voltage;
float ADC_Calibration;
int Light_Percentage;

// Objects
Adafruit_BME280 bme;  //BME280 connect to ESP32-C6 I2C BME280 0x76 | BMP280 0x76 |  BMP180 0x77
SensirionI2cScd4x scd4x;

// Constants
const int LoopDelay = 2;  // Loop Delay in Seconds

// Variables
static char errorMessage[64];
static int16_t error;

// macro definitions
// make sure that we use the proper definition of NO_ERROR
#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

// **************** INPUTS ****************
#define ADC 3  // ADC (12-bit 0–4095 0-3.3V)

// **************** OUTPUTS ****************
#define LED_Red 22    // Red LED
#define LED_Green 21  // Gren LED
#define LED_Blue 23   // Blue LED

// Define I2C (Expansion Port)
#define I2C_SDA 6
#define I2C_SCL 7

// ######### FUNCTIONS #########

void PrintUint64(uint64_t& value) {
  Serial.print("0x");
  Serial.print((uint32_t)(value >> 32), HEX);
  Serial.print((uint32_t)(value & 0xFFFFFFFF), HEX);
}

// Map float values to percentages
float MapValues(float x, float DC_Min, float DC_Max, float Percentage_Min, float Percentage_Max) {
  return (x - DC_Min) * (Percentage_Max - Percentage_Min) / (DC_Max - DC_Min) + Percentage_Min;
}

void InitialiseTemperatureSensor() {
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!  Could eqally be a BMP180?");
    // BME280 0x76 | BMP280 0x76 |  BMP180 0x77
    // while (1);
  }
}

// ######### SETUP #########
void setup() {

  // Stabalise
  delay(250);

  // Initialise UART:
  Serial.begin(115200, SERIAL_8N1);  //115200
  while (!Serial)
    ;
  Serial.println("");

  // Initialize I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  //  Configures the specified LED GPIO as outputs
  pinMode(LED_Red, OUTPUT);
  pinMode(LED_Green, OUTPUT);
  pinMode(LED_Blue, OUTPUT);

  // LEDs Default Off State
  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_Green, LOW);
  digitalWrite(LED_Blue, LOW);

  Serial.println("");

  InitialiseTemperatureSensor();

  uint16_t error;
  char errorMessage[256];

  scd4x.begin(Wire, SCD41_I2C_ADDR_62);

  Serial.println("Checking Sensor");

  uint64_t serialNumber = 0;
  delay(30);
  // Ensure scd4x is in clean state
  error = scd4x.wakeUp();
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute wakeUp(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
  }
  error = scd4x.stopPeriodicMeasurement();
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
  }
  error = scd4x.reinit();
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute reinit(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
  }
  // Read out information about the scd4x
  error = scd4x.getSerialNumber(serialNumber);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute getSerialNumber(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }
  Serial.print("SCD4x Sensor Serial Number:\t 0x");
  PrintUint64(serialNumber);
  Serial.println("\n");
  //
  // If temperature offset and/or scd4x altitude compensation
  // is required, you should call the respective functions here.
  // Check out the header file for the function definitions.

  Serial.println("CWX(C6), AQIX and AQIL Configured...\n");
}

// ######### LOOP #########
void loop() {


  // Test RGB LEDs
  Serial.println("Preparing to Test LEDs");

  // Red
  digitalWrite(LED_Red, HIGH);
  delay(500);
  digitalWrite(LED_Red, LOW);
  delay(100);

  // Green
  digitalWrite(LED_Green, HIGH);
  delay(500);
  digitalWrite(LED_Green, LOW);
  delay(100);

  // Blue
  digitalWrite(LED_Blue, HIGH);
  delay(500);
  digitalWrite(LED_Blue, LOW);
  delay(100);

  // White
  digitalWrite(LED_Red, HIGH);
  digitalWrite(LED_Green, HIGH);
  digitalWrite(LED_Blue, HIGH);
  delay(500);
  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_Green, LOW);
  digitalWrite(LED_Blue, LOW);
  delay(100);


  // Red
  digitalWrite(LED_Red, HIGH);

  // Read BME280 Sensor
  Serial.println("Preparing to Read BME280 Sensor");

  // Get Temperature ºC and round to 1 decimal place
  TemperatureC = bme.readTemperature();
  dtostrf(TemperatureC, 5, 1, SensorResult);
  sprintf(SensorResult, "%s", SensorResult);
  Serial.print("Temperature (ºC): \t\t");
  Serial.println(SensorResult);

  // Get Humity % and round to no decimal places
  Humidity = round(bme.readHumidity());
  dtostrf(Humidity, 5, 0, SensorResult);
  Serial.print("Humidity (%): \t\t\t");
  Serial.println(SensorResult);

  // Get Pressure and round to no decimal places
  Pressure = (bme.readPressure() / 100.0F);
  Pressure = round(Pressure);
  Serial.print("Pressure (hPa): \t\t");
  Serial.println(Pressure);

  Serial.println("\n");

  digitalWrite(LED_Red, LOW);
  digitalWrite(LED_Green, HIGH);

  // Read Light Sensor
  Serial.println("Preparing to Read Light Sensor");

  ADC_Calibration = 3.300;  // Adjust as needed for final calibration of ADC_Voltage.
  ADC_Voltage = (analogRead(ADC) * ADC_Calibration) / 4095;

  // Update the displayed info
  Serial.print("ADC Value \t");
  Serial.print(analogRead(ADC));

  Serial.print("\t\tADC Voltage ");
  Serial.print(ADC_Voltage);
  Serial.print(" V \t");

  Light_Percentage = MapValues(ADC_Voltage, 0, 3.3, 0, 100);  // 3.3V Equaltes to approximately 0.8V (800mV) - Potential Divider

  Serial.print("Light ");
  Serial.print(Light_Percentage);
  Serial.print(" % ");

  if (Light_Percentage > 100) {
    Serial.print("\t (Bright)");
  }

  Serial.println("\n");

  digitalWrite(LED_Green, LOW);
  digitalWrite(LED_Blue, HIGH);

  // Read SCD4x Sensor
  Serial.println("Preparing to Read SCD4x Sensor.  Please wait for CO2 Sampling..");

  uint16_t co2Concentration = 0;
  float temperature = 0.0;
  float relativeHumidity = 0.0;

  // Wake the scd4x up from sleep mode.
  error = scd4x.wakeUp();
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute wakeUp(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }

  // Ignore first measurement after wake up.
  error = scd4x.measureSingleShot();
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute measureSingleShot(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }

  // Perform single shot measurement and read data.
  error = scd4x.measureAndReadSingleShot(co2Concentration, temperature, relativeHumidity);
  if (error != NO_ERROR) {
    Serial.print("Error trying to execute measureAndReadSingleShot(): ");
    errorToString(error, errorMessage, sizeof errorMessage);
    Serial.println(errorMessage);
    return;
  }

  // Display Readings
  Serial.print("CO2 concentration [ppm]:\t ");
  Serial.println(co2Concentration);

  Serial.print("Temperature [°C]:\t\t ");
  Serial.println(temperature);

  Serial.print("Relative Humidity [RH]:\t\t ");
  Serial.println(relativeHumidity);

  Serial.println("\n");
  digitalWrite(LED_Blue, LOW);

  // Sleep
  Serial.println("Sleep for 2 Seconds\n\n");
  delay(LoopDelay * 1000);
}
