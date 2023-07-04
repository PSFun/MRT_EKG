/*
Diese Vorlage soll Sie bei der Abarbeitung der Aufgabe 2 unterstützen. 
Zur Abarbeitung benötigen Sie Befehle die Sie in den folgenden Dokumentationen nachlesen können:
https://www.arduino.cc/en/Reference/ArduinoBLE
https://github.com/arduino-libraries/ArduinoBLE
https://github.com/SofaPirate/Chrono

Aufgabe 2 baut auf der vorherigen Aufgabe auf. Notwendige Schritte aus Aufgabe 1 werden nicht nochmals beschrieben. 
*/

// Biblotheken
// =============================================================
// Binden Sie die benötigten Bibliotheken "ArduinoBLE" und "Chrono" ein

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Chrono.h>
#include <stdio.h>
#include <string.h>

// =============================================================
// Hilfsvariablen
// =============================================================
// Erstellen Sie eine Variable die den analogen Eingangspin des Arduinos definiert
pin_size_t analogInputPin = A1;   //Pin für analoges Signaleingang
float value = 0;        //Wert für abgetestetes digitales Signal
float u_in  = 0;      //gesuchter Messwert in Volt
float u_inmu  = 0;    //gesuchter Messwert in Mikrovolt

unsigned int hr_delay = 5;

// Die Webapp wartet auf einen BLE Servie mit der UUID:
// "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
// Erstellen Sie das BLE Service Objekt mit der entsprechenden UUID
BLEService hr_service("4fafc201-1fb5-459e-8fcc-c5c9c331914b");

// in dieser erwartet sie eine Characteristic mit Spannungswerten (in micro Volt) mit der UUID:
// "4fafc201-1cc4-e7c1-c757-f1267dd021e8"
// Erstellen Sie das BLE Chracteristic Objekt mit der entsprechenden UUID, verwenden Sie den Typ BLEFloatCharacteristic
// Fügen Sie die Characteristic Eigenschaften Read und Notify hinzu
BLEFloatCharacteristic v_characteristic("4fafc201-1cc4-e7c1-c757-f1267dd021e8", BLERead | BLENotify);

// Uhr für die Signalabtastung
// Erstellen Sie ein Chrono Objekt für die regelmäßige Signalabtastung
Chrono timer;

void setup() {
  // put your setup code here, to run once:

  // Initalisieren Sie die serielle Kommunikation und den ADC wie in Aufgabe 1
  Serial.begin(115200);         //Setze baud-Rate
  analogReadResolution(12);     //Erhöhe ADC-Auflösung von 10 auf 12 Bit

  // Initialisieren Sie die BLE Funktionalität, Geben Sie eine Fehlermeldung über die serielle Schnittstelle, 
  // falls die Initialisierung nicht korrekt erfolgt.
  if(!BLE.begin()) {
    Serial.println("BLE konnte nicht initialisiert werden!");
    while(1);
  }

  // Stellen Sie den BLE Gerätename "ECGprojectIBMT"
  BLE.setDeviceName("ECGprojectIBMT");

  // Setzen Sie oben definierten Servicen als Advertised Service
  BLE.setAdvertisedService(hr_service);

  // Fügen Sie dem Service die oben definierte Characteristic hinzu
  hr_service.addCharacteristic(v_characteristic);

  // Fügen sie den definierten Service hinzu
  BLE.addService(hr_service);

  // Setzuen Sie dem Service einen initalen Wert 0
  v_characteristic.writeValue(u_in);

  // Starten Sie das Advertisement
  BLE.advertise();

}

void loop() {
  // put your main code here, to run repeatedly:
  // prüfen Sie ob eine Verbindung zu einen BLE central besteht, wenn ja:
  BLEDevice central = BLE.central();
  
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());
  }
  
  // Fragen Sie alle 5 ms den ADC ab und Schreiben sie den Spannungswert am Eingang des AFE in die Characteristic
  // Nutzen Sie für die zyklische Abfrage die Chrono Bibliothek

  while(central.connected())  {

    if(timer.hasPassed(hr_delay)) {
      value = analogRead(analogInputPin);
      u_in = (((value/4095)*3.300)-(3.300/2))/1100;
      u_inmu = u_in*1000*1000;
      v_characteristic.writeValue(u_inmu);

      timer.restart();
    }
  }
}
