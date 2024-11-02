// main.ino
#include "BLEHelper.h"

#include "Adafruit_TinyUSB.h"   // DMS
#include "HX711.h"              // DMS

BLEHelper bleHelper; //BLE
HX711 scale; //DMS


// Definiere Pins für den HX711
const int LOADCELL_DOUT_PIN = 3; // Daten-Pin (DT) --> DMS
const int LOADCELL_SCK_PIN = 2;  // Takt-Pin (SCK) --> DMS


short power;
unsigned short cadence;
unsigned short revolutions = 0;
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
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale();    // Standard-Kalibrierfaktor, ggf. anpassen
  scale.tare();         // Setzt den Nullpunkt (Tara)

  Serial.println("DMS und HX711-Modul bereit.");
}

void loop() {
  
  TestBLE();
  TestDMS();
  delay(1000);  // Send data every second
}


void TestDMS()
{
  float gewicht = scale.get_units(10);  // Durchschnitt über 10 Messungen

  Serial.print("Gewicht / Dehnung: ");
  Serial.print(gewicht, 2);             // Zwei Dezimalstellen anzeigen
  Serial.println(" Einheiten");
}

void TestBLE() 
{
  // Simulate dynamic power data
  power = 200 + (millis() / 1000) % 100;  // Power in watts
  revolutions += 2;  // Increment revolutions
  timestamp = (timestamp + 1024) % 65536;  // Increment timestamp with rollover every 64 seconds

  // Generate plausible example data for optional characteristics
  uint8_t batteryLevel = random(50, 101);  // Battery level between 50% and 100%
  unsigned short cumulativeCrankRevolutions = revolutions*1.5;  // Use revolutions for cumulative crank revolutions
  unsigned short lastCrankEventTime = (timestamp + random(100, 500)) % 65536;  // Last crank event time with some offset
  unsigned short maxForceMagnitude = random(300, 600);  // Maximum force magnitude between 300 and 600
  unsigned short minForceMagnitude = random(100, 299);  // Minimum force magnitude between 100 and 299
  unsigned short maxAngle = random(180, 360);  // Maximum angle between 180 and 360 degrees
  unsigned short minAngle = random(0, 179);  // Minimum angle between 0 and 179 degrees

  // Update power data with all characteristics
  bleHelper.updatePowerData(power, revolutions, timestamp, batteryLevel, 1, cumulativeCrankRevolutions, lastCrankEventTime, maxForceMagnitude, minForceMagnitude, maxAngle, minAngle);

}
