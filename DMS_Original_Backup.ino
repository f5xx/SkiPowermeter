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
