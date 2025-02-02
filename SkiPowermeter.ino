// main.ino
#include "BLEHelper.h"
#include "Accelerometer.h"

#include "Adafruit_TinyUSB.h"
#include "HX711.h"
#include <float.h>

BLEHelper bleHelper; //BLE
HX711 dms; //DMS
Accelerometer Acc;
// Definiere Pins für den HX711
const int LOADCELL_DOUT_PIN = 3; // Daten-Pin (DT) --> DMS
const int LOADCELL_SCK_PIN = 2;  // Takt-Pin (SCK) --> DMS


short power;
int revolutions = 0;
unsigned short timestamp = 0;

void setup() {
  //BLE Setup
  Serial.begin(115200);
  delay(2000);
  Serial.println("Starting BLE Power Meter...");

  // Initialize BLEHelper
  bleHelper.begin();

  //DMS Setup
  // Initialisiere HX711 mit den definierten Pins
  dms.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  dms.set_scale();    // Standard-Kalibrierfaktor, ggf. anpassen
  dms.tare();         // Setzt den Nullpunkt (Tara)

  Serial.println("HX711 Module ready");

  // Accelerometer setup
  // Initialize I2C
  Wire.begin();
  delay(20);
  Acc.begin();
  delay(100);
}

void loop() {
  
  //collect data and eavluate
  collectDataAndEvaluate();

  //TestBLE();
  //TestDMS();
  //TestAccel();
  //delay(100);
}


// Test Functions
void TestDMS()
{
  float force_reading = dms.get_value(1);     // get latest value

  Serial.print("dms reading: ");
  Serial.println(force_reading, 2);             // round to two decimals
}

void TestBLE() 
{
  // Simulate dynamic power data
  power = 200 + (millis() / 1000) % 100;  // Power in watts
  revolutions += 2;  // Increment revolutions
  timestamp = (timestamp + 1024) % 65536;  // Increment timestamp with rollover every 64 seconds

  // Generate plausible example data for optional characteristics
  uint8_t batteryLevel = random(50, 101);                                       // Battery level between 50% and 100%
  unsigned short cumulativeCrankRevolutions = revolutions*1.5;                  // Use revolutions for cumulative crank revolutions
  unsigned short lastCrankEventTime = (timestamp + random(100, 500)) % 65536;   // Last crank event time with some offset
  unsigned short maxForceMagnitude = random(300, 600);                          // Maximum force magnitude between 300 and 600
  unsigned short minForceMagnitude = random(100, 299);                          // Minimum force magnitude between 100 and 299
  unsigned short maxAngle = random(180, 360);                                   // Maximum angle between 180 and 360 degrees
  unsigned short minAngle = random(0, 179);                                     // Minimum angle between 0 and 179 degrees

  // Update power data with all characteristics
  bleHelper.updatePowerData(power, revolutions, timestamp, batteryLevel, 1, cumulativeCrankRevolutions, lastCrankEventTime, maxForceMagnitude, minForceMagnitude, maxAngle, minAngle);

}

void TestAccel()
{
  int16_t ax, ay, az;
  Acc.readAcceleration(ax, ay, az);

  // Verify the BMI160 chip ID
  uint8_t chipId = Acc.readRegister(BMI160_REG_CHIP_ID);
  if (chipId != BMI160_CHIP_ID) {
    Serial.println("BMI160 not detected. Check wiring.");
  }

  // Convert raw data to g
  float accelX = ax / 16384.0;
  float accelY = ay / 16384.0;
  float accelZ = az / 16384.0;

  float accel = pow(pow(accelX,2)+pow(accelY,2)+pow(accelZ,2), 0.5);
  // Serial.print("Accel X: ");
  // Serial.print(accelX);
  // Serial.print(" g, Y: ");
  // Serial.print(accelY);
  // Serial.print(" g, Z: ");
  // Serial.print(accelZ);
  // Serial.println(" g");
  Serial.print(accel);
  Serial.println(" g");
}


// Example function to collect DMS and Accel data for 1s, then evaluate & publish

unsigned short getTimestamp() {
  // (uint32_t) cast to avoid overflow during multiplication
  uint32_t timeIn1024ths = ((uint32_t)millis() * 1024UL) / 1000UL; 
  return (unsigned short)(timeIn1024ths & 0xFFFF);
}

void collectDataAndEvaluate() {
  // essentially updates power and revolutions every 5s
  // uses 
  int16_t simulatedPower;

  for (int i = 0; i < 10; i++) 
  {
    const unsigned long collectionTime = 500; // 1 second
    const unsigned long startTime = millis();
    
    // sampling every 10 ms => ~50 samples in 500ms second
    const unsigned long sampleInterval = 10;  // in  ms

    // Prepare buffers
    const int numSamples = 50;
    float dmsBuffer[numSamples];
    float accelBuffer[numSamples];

    int sampleIndex = 0;

    // Collect data for up to 1 second or until we fill the buffer
    while (millis() - startTime < collectionTime && sampleIndex < numSamples) {
      // 1) Read DMS (strain-gauge) data
      float force_reading = dms.get_value(2);  // use 2 the avarage of two readings of the DMS
      
      // 2) Read accelerometer data
      int16_t ax, ay, az;
      Acc.readAcceleration(ax, ay, az);

      // Convert raw accel to g
      float accelX = ax / 16384.0;
      float accelY = ay / 16384.0;
      float accelZ = az / 16384.0;
      float accelMag = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);
      
      // Store in buffers
      dmsBuffer[sampleIndex]   = force_reading;
      accelBuffer[sampleIndex] = accelMag;
      sampleIndex++;

      // Wait for next sampling interval
      delay(sampleInterval);
    }
    // buffer filled! Lets evaluate...

    //Chat-Gpt...
    // Evaluate the collected data—here’s a simple example (avg, min, max).
    float dmsSum = 0;
    float dmsMin = FLT_MAX;
    float dmsMax = -FLT_MAX;
    
    float accelSum = 0;
    float accelMin = FLT_MAX;
    float accelMax = -FLT_MAX;

    for (int i = 0; i < sampleIndex; i++) {
      // DMS stats
      float val = dmsBuffer[i];
      dmsSum += val;
      if (val < dmsMin) dmsMin = val;
      if (val > dmsMax) dmsMax = val;

      // Accelerometer stats
      float aVal = accelBuffer[i];
      accelSum += aVal;
      if (aVal < accelMin) accelMin = aVal;
      if (aVal > accelMax) accelMax = aVal;
    }

    float dmsAvg   = (sampleIndex > 0) ? (dmsSum / sampleIndex) : 0;
    float accelAvg = (sampleIndex > 0) ? (accelSum / sampleIndex) : 0;

    // Print results for debugging
    Serial.println("=== 1-second data summary ===");
    Serial.print("DMS   Avg: "); Serial.print(dmsAvg, 2);
    Serial.print("  Min: "); Serial.print(dmsMin, 2);
    Serial.print("  Max: "); Serial.println(dmsMax, 2);

    Serial.print("Accel Avg: "); Serial.print(accelAvg, 2);
    Serial.print("  Min: "); Serial.print(accelMin, 2);
    Serial.print("  Max: "); Serial.println(accelMax, 2);
    Serial.println("=============================\n");

    // publish results over BLE
    simulatedPower = abs((int16_t)dmsAvg);         // dms-avg ~ power
    // calculate revolutions based on accel-range
    timestamp = millis() % 65536;
    if (accelMax - accelMin > 2) revolutions +=1;
  }
  Serial.print("revs "); Serial.print(revolutions, 2);
  bleHelper.updateRevolutionData(simulatedPower, revolutions, getTimestamp());
}

