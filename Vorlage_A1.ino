/*
Diese Vorlage soll Ihnen helfen einen Einstieg in die Programmierung des Arduinos 
zu finden und Sie bei der Bewältigung der ersten Aufgabe unterstützen. Die notwendigen 
Befehle zur Umsetzung Ihrer Aufgabe finden Sie in der offiziellen Dokumantation von Arduino unter: 

//www.arduino.cc/reference/en/language/functions/communication/serial/
//www.arduino.cc/reference/en/language/functions/analog-io/analogread/
//www.arduino.cc/reference/en/language/functions/time/delay/
*/

#include <Arduino.h>

pin_size_t analogInputPin = A1;   //Pin für analoges Signaleingang
float value = 0;        //Wert für abgetestetes digitales Signal

float u_in  = 0;      //gesuchter Messwert in Volt
float u_inmu  = 0;    //gesuchter Messwert in Mikrovolt


void setup() {
  // Dieser Teil Ihres Programms wird zu Beginn einmailg ausgeführt

  // Initialisieren Sie eine serielle Kommunikation mit einer Baudrate von 115200
  Serial.begin(115200); 

  // Ändern Sie die Auflösung des ADCs auf 12 Bit
  analogReadResolution(12);

}

void loop() {
  // Dieser Teil wird in einer Schleife wiederholt ausgeführt

  // Lesen Sie den Wert des ADC auf Pin A1 aus
  value = analogRead(analogInputPin);

  // Berechnen Sie den Wert der Eingangsspannung am ADC in V
    //𝑈𝑎 = 𝐺 ⋅ (𝑈𝑖𝑛+ − 𝑈𝑖𝑛−) + V𝑐𝑐/2 - Formel aus Anleitung S5. 2.2.2 - EKG Schaltung Sparkfunk AD8232 
    // -> 𝑈𝑎 ist das abgetastete Signal am Pin A1
    //     Es muss noch umgerechnet werden: Die Aufläsung des ADC an Pin A1 wurde von 10 Bit auf 12 Bit erhöht
    //     Daraus folgt nach Angabe der Webseite Werte zwischen 0 und 4095 am Pin (2^12 = 4096 mögliche Werte).
    //     Der Wertebereich  am ADC muss auf den Bereich von 0 bis 3,3V skaliert werden.
    //     => n_value = (value/4096)*3.3
    // -> G ist der Verstärkungsfaktor mit 1100
    // -> (𝑈𝑖𝑛+ − 𝑈𝑖𝑛−) ist das gesuchte Signal
    // ->  V𝑐𝑐/2 Mitte des Messbereiches von 0<->3.3V
  //u_in = (((value/4096)*3.300)-(3.300/2))/1100;
  u_in = ((value/4095)*3.300-(3.300 /2))/1100;

  // Berechnen Sie den Wert der differentiellen Eingansspanung am AFE in µV
  u_inmu = u_in*1000*1000;

  // Senden Sie den Wert der differentiellen Eingansspanung über die serielle Schnittstelle
  Serial.println(u_inmu);
  // Pausieren Sie das Programm, so dass die Abtastrate des ADC 200 Hz beträgt (Vernachlässigen Sie den Einfluss der anderen Befehle)
  delay(5);
}
