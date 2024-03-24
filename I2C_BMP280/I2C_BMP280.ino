#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>

#define BMP280_ADDR 0x76  // Dirección I2C del sensor BMP280
#define BMP_SDA 2          // Pin SDA (data) para la comunicación I2C
#define BMP_SCL 4          // Pin SCL (clock) para la comunicación I2C

Adafruit_BMP280 bmp;      // Instancia del sensor BMP280

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));

  // Inicializar la comunicación I2C con los pines SDA y SCL especificados
  Wire.begin(BMP_SDA, BMP_SCL);

  // Inicializar el sensor BMP280 con la dirección I2C determinada
  if (!bmp.begin(BMP280_ADDR)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}

void loop() {
  // Imprimir lecturas de temperatura, presión y altitud
  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  Serial.print("Approx altitude = ");
  Serial.print(bmp.readAltitude(1013.25)); // Ajusta esta presión al nivel del mar local
  Serial.println(" m");
  Serial.println();
  
  delay(2000);
}

