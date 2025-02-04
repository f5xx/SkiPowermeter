# 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
// main.ino
# 3 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino" 2
# 4 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino" 2

# 6 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino" 2
# 7 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino" 2
# 8 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino" 2


# 9 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\SkiPowermeter.ino"
BLEHelper bleHelper; //BLE
HX711 dms; //DMS
Accelerometer Acc;
// Definiere Pins für den HX711
const int LOADCELL_DOUT_PIN = 3; // Daten-Pin (DT) --> DMS
const int LOADCELL_SCK_PIN = 2; // Takt-Pin (SCK) --> DMS


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
  dms.set_scale(); // Standard-Kalibrierfaktor, ggf. anpassen
  dms.tare(); // Setzt den Nullpunkt (Tara)

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
  float force_reading = dms.get_value(1); // get latest value

  Serial.print("dms reading: ");
  Serial.println(force_reading, 2); // round to two decimals
}

void TestBLE()
{
  // Simulate dynamic power data
  power = 200 + (millis() / 1000) % 100; // Power in watts
  revolutions += 2; // Increment revolutions
  timestamp = (timestamp + 1024) % 65536; // Increment timestamp with rollover every 64 seconds

  // Generate plausible example data for optional characteristics
  uint8_t batteryLevel = random(50, 101); // Battery level between 50% and 100%
  unsigned short cumulativeCrankRevolutions = revolutions*1.5; // Use revolutions for cumulative crank revolutions
  unsigned short lastCrankEventTime = (timestamp + random(100, 500)) % 65536; // Last crank event time with some offset
  unsigned short maxForceMagnitude = random(300, 600); // Maximum force magnitude between 300 and 600
  unsigned short minForceMagnitude = random(100, 299); // Minimum force magnitude between 100 and 299
  unsigned short maxAngle = random(180, 360); // Maximum angle between 180 and 360 degrees
  unsigned short minAngle = random(0, 179); // Minimum angle between 0 and 179 degrees

  // Update power data with all characteristics
  bleHelper.updatePowerData(power, revolutions, timestamp, batteryLevel, 1, cumulativeCrankRevolutions, lastCrankEventTime, maxForceMagnitude, minForceMagnitude, maxAngle, minAngle);

}

void TestAccel()
{
  int16_t ax, ay, az;
  Acc.readAcceleration(ax, ay, az);

  // Verify the BMI160 chip ID
  uint8_t chipId = Acc.readRegister(0x00);
  if (chipId != 0xD1) {
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
    const unsigned long sampleInterval = 10; // in  ms

    // Prepare buffers
    const int numSamples = 50;
    float dmsBuffer[numSamples];
    float accelBuffer[numSamples];

    int sampleIndex = 0;

    // Collect data for up to 1 second or until we fill the buffer
    while (millis() - startTime < collectionTime && sampleIndex < numSamples) {
      // 1) Read DMS (strain-gauge) data
      float force_reading = dms.get_value(2); // use 2 the avarage of two readings of the DMS

      // 2) Read accelerometer data
      int16_t ax, ay, az;
      Acc.readAcceleration(ax, ay, az);

      // Convert raw accel to g
      float accelX = ax / 16384.0;
      float accelY = ay / 16384.0;
      float accelZ = az / 16384.0;
      float accelMag = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

      // Store in buffers
      dmsBuffer[sampleIndex] = force_reading;
      accelBuffer[sampleIndex] = accelMag;
      sampleIndex++;

      // Wait for next sampling interval
      delay(sampleInterval);
    }
    // buffer filled! Lets evaluate...

    //Chat-Gpt...
    // Evaluate the collected data—here’s a simple example (avg, min, max).
    float dmsSum = 0;
    float dmsMin = 3.4028234663852886e+38F;
    float dmsMax = -3.4028234663852886e+38F;

    float accelSum = 0;
    float accelMin = 3.4028234663852886e+38F;
    float accelMax = -3.4028234663852886e+38F;

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

    float dmsAvg = (sampleIndex > 0) ? (dmsSum / sampleIndex) : 0;
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
    simulatedPower = (((int16_t)dmsAvg)>0?((int16_t)dmsAvg):-((int16_t)dmsAvg)); // dms-avg ~ power
    // calculate revolutions based on accel-range
    timestamp = millis() % 65536;
    if (accelMax - accelMin > 0.5) revolutions +=1;
  }
  Serial.print("revs "); Serial.print(revolutions, 2);
  bleHelper.updateRevolutionData(simulatedPower, revolutions, getTimestamp());
}
# 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\Backup.ino"
//#include <bluefruit.h>

// // BLE service and characteristic for Cycling Speed and Cadence (CSC)
// BLEService cscService = BLEService(0x1816);  // CSC service UUID
// BLECharacteristic cscMeasurementChar = BLECharacteristic(0x2A5B);  // CSC Measurement characteristic UUID

// uint16_t crankRevolutions = 0;  // Total crank revolutions
// uint16_t cadence = 90;          // Initial cadence value (in RPM)
// bool isCadence90 = true;        // Toggle between 90 and 80 RPM

// void setup() {
//   Serial.begin(115200);  // Start serial communication for debugging
//   delay(2000);           // Wait for Serial to initialize
//   Serial.println("Starting BLE Cadence Sensor...");

//   // Initialize Bluefruit BLE module
//   Bluefruit.begin();
//   Bluefruit.setTxPower(8);    // Max transmission power
//   Bluefruit.setName("BikeCadenceSensor");

//   // Start the Cycling Speed and Cadence (CSC) service
//   cscService.begin();
//   Serial.println("CSC Service started...");

//   // Configure the CSC Measurement characteristic
//   cscMeasurementChar.setProperties(CHR_PROPS_NOTIFY);  // Notify property
//   cscMeasurementChar.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);  // No authentication needed
//   cscMeasurementChar.setFixedLen(5);  // 5 bytes for CSC measurement data (flags + 16-bit crank data)
//   cscMeasurementChar.begin();
//   Serial.println("CSC Measurement characteristic started...");

//   // Start advertising the service
//   startAdvertising();
//   Serial.println("Advertising started...");
// }

// void loop() {
//   // Toggle cadence between 90 and 80 RPM every 2 seconds
//   if (isCadence90) {
//     cadence = 90;
//   } else {
//     cadence = 80;
//   }

//   // Update the crank revolutions based on cadence (this is a simplified model)
//   crankRevolutions += cadence / 30;  // Update crank revolution count based on cadence

//   // Prepare CSC measurement packet (5 bytes total)
//   uint8_t cscMeasurementData[5];

//   // Byte 0: Flags (0x02 means only crank data present)
//   cscMeasurementData[0] = 0x02;

//   // Byte 1-2: Cumulative crank revolutions (16-bit)
//   cscMeasurementData[1] = crankRevolutions & 0xFF;        // Lower byte of crank revolutions
//   cscMeasurementData[2] = (crankRevolutions >> 8) & 0xFF; // Upper byte of crank revolutions

//   // Byte 3-4: Last crank event time in 1/1024 seconds (simplified)
//   uint16_t eventTime = (millis() / 1000.0) * 1024;
//   cscMeasurementData[3] = eventTime & 0xFF;         // Lower byte of event time
//   cscMeasurementData[4] = (eventTime >> 8) & 0xFF;  // Upper byte of event time

//   // Notify the connected device with the CSC data
//   cscMeasurementChar.notify(cscMeasurementData, sizeof(cscMeasurementData));

//   // Toggle cadence value
//   isCadence90 = !isCadence90;

//   // Wait for 2 seconds before sending the next value
//   delay(2000);
// }

// void startAdvertising() {
//   // Set up the advertising packet with flags and service information
//   Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
//   Bluefruit.Advertising.addTxPower();  // Include TX power level in the advertising packet
//   Bluefruit.Advertising.addService(cscService);  // Include the CSC service

//   // Include the device name in the scan response
//   Bluefruit.ScanResponse.addName();

//   // Start advertising
//   Bluefruit.Advertising.restartOnDisconnect(true);  // Restart advertising on disconnect
//   Bluefruit.Advertising.setInterval(32, 64);        // Advertising interval (32ms to 64ms)
//   Bluefruit.Advertising.setFastTimeout(30);         // Fast advertising mode timeout
//   Bluefruit.Advertising.start(0);                   // Start advertising indefinitely

//   Serial.println("Started advertising CSC service.");
// }
# 1 "C:\\Users\\janve\\OneDrive\\Dokumente\\Arduino\\SkiPowermeter\\DMS_Original_Backup.ino"
// #include "Adafruit_TinyUSB.h"  // Stelle sicher, dass du diese Bibliothek installiert hast
// #include "HX711.h"

// // Definiere Pins für den HX711
// const int LOADCELL_DOUT_PIN = 3; // Daten-Pin (DT)
// const int LOADCELL_SCK_PIN = 2;  // Takt-Pin (SCK)

// // Erstelle eine Instanz für den HX711
// HX711 scale;

// void setup() {
//   // Initialisiere USB-Serial-Verbindung
//   Serial.begin(9600);
//   while (!Serial) delay(10);  // Warte, bis die serielle Verbindung aufgebaut ist

//   // Initialisiere HX711 mit den definierten Pins
//   scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
//   scale.set_scale();    // Standard-Kalibrierfaktor, ggf. anpassen
//   scale.tare();         // Setzt den Nullpunkt (Tara)

//   Serial.println("DMS und HX711-Modul bereit.");
// }

// void loop() {
//   float gewicht = scale.get_units(10);  // Durchschnitt über 10 Messungen

//   Serial.print("Gewicht / Dehnung: ");
//   Serial.print(gewicht, 2);             // Zwei Dezimalstellen anzeigen
//   Serial.println(" Einheiten");

//   delay(500);  // Halbe Sekunde Pause zwischen den Messungen
// }
