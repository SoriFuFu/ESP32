#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27      // Dirección de la pantalla LCD
#define BMP280_ADDR 0x76   // Dirección del sensor BMP280 (temperatura y presión)
#define SDA 2              // Pin SDA para la comunicación I2C
#define SCL 4              // Pin SCL para la comunicación I2C
#define RESET_BUTTON_PIN 33 // Pin para el botón de reset

// Tamaño de la pantalla LCD (20 columnas x 4 filas)
#define LCD_COLS 20
#define LCD_ROWS 4

// Inicialización de la librería de la pantalla LCD
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Adafruit_BMP280 bmp;

// VARIABLES
// WIFI
const char *ssid = "Tarta-D";
const char *password = "FUFU2022...";
// PINES DE ENTRADA
const int PINES_ENTRADA[] = {14, 15, 13, 32, 35, 39};
// ENUMERACIÓN PARA INDEXAR LOS PINES DE ENTRADA
enum IndiceEntrada {
  ARRIBA,
  ABAJO,
  IZQUIERDA,
  DERECHA,
  BOTON_EXTERIOR,
  BOTON_INTERIOR
};
// PINES DE SALIDA
const int PINES_SALIDA[] = {21, 19, 18, 5};
// ENUMERACIÓN PARA INDEXAR LOS PINES DE SALIDA
enum IndiceSalida {
  RIEGO_EXTERIOR,
  RIEGO_INTERIOR,
  LIBRE_1,
  LIBRE_2
};
// GLOBALES

void setup() {
  Serial.begin(115200);
  // Inicializar la comunicación I2C con los pines SDA y SCL especificados
  Wire.begin(SDA, SCL);

  // SE INICIALIZA LA CONEXIÓN WIFI
  // WiFi.begin(ssid, password);
  // Serial.println("Connecting to WiFi..");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println();
  // Serial.print("Connected to the WiFi network with IP Address: ");
  // Serial.println(WiFi.localIP());
  // DISPLAY
  // SE INICIALIZA EL DISPLAY
  lcd.init();
  // ENCIENDE LA RETROILUMINACIÓN DE LA PANTALLA
  lcd.backlight();
  // LIMPIA LA PANTALLA
  lcd.clear();
  // SENSOR BMP280
  if (!bmp.begin(BMP280_ADDR)) {
    Serial.println("Could not find a valid BMP280 sensor, check wiring!");
    while (1);
  }

  // Especificación del PIN como entrada o salida
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // Lee el estado del botón de reset
  int estadoBoton = digitalRead(RESET_BUTTON_PIN);

  // Si el botón de reset está presionado (estado bajo)
  if (estadoBoton == LOW) {
    Serial.println("Boton de reset presionado. Reiniciando...");
    delay(100); // Debounce - espera un breve periodo para evitar rebotes
    ESP.restart(); // Reinicia la placa ESP32
  }

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

