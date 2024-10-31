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