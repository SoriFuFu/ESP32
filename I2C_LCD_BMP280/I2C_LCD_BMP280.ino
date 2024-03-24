
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#define LCD_ADDR 0x27
#define BMP280_ADDR 0x76
#define BMP_SDA 2          // Pin SDA (data) para la comunicación I2C
#define BMP_SCL 4          // Pin SCL (clock) para la comunicación I2C

// Tamaño de la pantalla LCD (20 columnas x 4 filas)
#define LCD_COLS 20
#define LCD_ROWS 4
// Inicialización de la pantalla LCD
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Adafruit_BMP280 bmp;

void setup() {
  Serial.begin(9600);
  Serial.println(F("BMP280 test"));

  // Inicializar la comunicación I2C con los pines SDA y SCL especificados
  Wire.begin(BMP_SDA, BMP_SCL);
  // Inicializa la pantalla LCD
  lcd.init();
  
  // Enciende la retroiluminación de la pantalla LCD
  lcd.backlight();

  lcd.clear();
  
  // Inicializar el sensor BMP280 con la dirección I2C determinada
  if (!bmp.begin(BMP280_ADDR)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }
}


void loop() {
  // Imprimir lecturas de temperatura, presión y altitud
  lcd.setCursor(0, 0);
  lcd.print("ESTACION");

  lcd.setCursor(0, 1);
  lcd.print("Temp =");
  lcd.print(bmp.readTemperature());
  lcd.print(" *C");

  lcd.setCursor(0, 2);
  lcd.print("Press = ");
  lcd.print(bmp.readPressure());
  lcd.print(" Pa");

  lcd.setCursor(0, 3);
  lcd.print("ALT = ");
  lcd.print(bmp.readAltitude(1013.25)); // Ajusta esta presión al nivel del mar local
  lcd.print(" m");
  
  delay(2000);
}
