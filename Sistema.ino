#include <LiquidCrystal.h>
#include <TinyGPS.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <SPI.h>

const int irSensorPin = A0;     // Pin del sensor de infrarrojo
const int buzzerPin = 5;        // Pin del buzzer
const int greenLedPin = 6;      // Pin del LED verde

LiquidCrystal lcd(8, 9, 10, 11, 12, 13);
TinyGPS gps;
SoftwareSerial ss(0, 1); // pines digitales empleados por la placa de expansión GPS-SD
File rawdata;

void setup() {
  pinMode(irSensorPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  lcd.begin(16, 2);

  Serial.begin(9600);
   ss.begin(38400);

   // inicializacion de SD CARD
  Serial.print("iniciando SD card...");
  pinMode(10, OUTPUT);
  if (!SD.begin(10)) {
    Serial.println("... inicializacion fallida!");
  }
  else
    Serial.println("...inicializacion realizada.");
}

void loop() {
  bool newData = false;
  unsigned long chars;
  unsigned short sentences, failed;
  // Abriendo el archivo txt
  rawdata = SD.open("rawdata.txt", FILE_WRITE);

  int irValue = analogRead(irSensorPin);
  int distance = irToDistance(irValue);  // Convertir la lectura del sensor a distancia en 10 cm
  if (distance < 10) {
    digitalWrite(buzzerPin, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tacho lleno!");
    saveGPSData();  // Guardar datos de GPS en la tarjeta SD
  } else {
    digitalWrite(buzzerPin, LOW);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Distancia: ");
    lcd.print(distance);
    lcd.print(" cm");
  }

  while (Serial.available() > 0) {
    gps.encode(Serial.read());
  }
  // Se toma los datos cada 10 segundo
 for (unsigned long start = millis(); millis() - start < 10000;)
  {
    while (ss.available())
    {
      char c = ss.read();
       // se envia los datos a la SD
       if(rawdata)
         rawdata.write(c);
      if (gps.encode(c)) 
        newData = true;
    }
  }
  rawdata.write("\r");
  rawdata.close();
}

// Función para convertir la lectura analógica del sensor a distancia en cm
int irToDistance(int irValue) {
  int distance = 9462.0 / (irValue - 16.92);
  return distance;
}

