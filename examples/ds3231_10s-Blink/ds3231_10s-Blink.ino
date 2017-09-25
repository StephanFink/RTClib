#include <Wire.h> // Die Datums- und Zeit-Funktionen der DS3231 RTC werden über das I2C aufgerufen.
#include "RTClib.h"

RTC_DS3231 rtc;

char daysOfTheWeek[7][12] = {"Sonntag", "Montag", "Dienstag", "Mittwoch", "Donnerstag", "Freitag", "Samstag"};
bool syncOnFirstStart = false; // true, falls die Zeitinformationen der RTC mit dem PC synchronisiert werden sollen.
                               // sollte standardmäßig auf false stehen

void setup () {
  pinMode(LED_BUILTIN, OUTPUT);

#ifndef ESP8266
  while (!Serial); // für Leonardo/Micro/Zero relevant
#endif

  Serial.begin(9600);

  delay(3000); // Warte auf Terminal

  if (! rtc.begin()) {
    Serial.println("Kann RTC nicht finden");
    while (1);
  }

  if (rtc.lostPower() || syncOnFirstStart) {
    Serial.println("Die RTC war vom Strom getrennt. Die Zeit wird neu synchronisiert.");
    // Über den folgenden Befehl wird die die RTC mit dem Zeitstempel versehen, zu dem der
    // Kompilierungsvorgang gestartet wurde, beginnt aber erst mit dem vollständigen Upload
    // selbst mit zählen. Daher geht die RTC von Anfang an wenige Sekunden nach.
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // Ein fest definierter Startzeitpunkt kann alternativ nach dem Schema
    // (DateTime(Jahr,Tag,Monat,Stunde,Minute,Sekunde)) festgelegt werden, z.B.:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}

void loop () {
  DateTime now = rtc.now();

  // immer, wenn die RTC auf 0, 10, 20, 30, 40 oder 50 volle Sekunden hochgezählt hat, sollen datum und Zeit
  // angegeben werden und die boardinterne LED für eine Sekunde leuchten.
  if (now.second() % 10 == 0) {
    digitalWrite(LED_BUILTIN, HIGH);
    
    Serial.print(now.year(), DEC);
    Serial.print('/');
    Serial.print(now.month(), DEC);
    Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" (");
    Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
    Serial.print(") ");
    Serial.print(now.hour(), DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println();
    
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(8000);  // Unterbrechung der Zeitabfrage bis kurz vor dem nächsten 10s-Wert
  }
}