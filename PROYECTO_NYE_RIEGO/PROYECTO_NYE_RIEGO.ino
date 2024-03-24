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
const int PINES_ENTRADA[] = {14, 15, 12, 32, 26, 22};
// ENUMERACIÓN PARA INDEXAR LOS PINES DE ENTRADA
enum IndiceEntrada {
  ARRIBA,
  ABAJO,
  IZQUIERDA,
  DERECHA,
  BOTON_INTERIOR,
  BOTON_EXTERIOR
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
unsigned long ultimoTiempo = 0;
const unsigned long intervaloLectura = 2000;  // Intervalo de tiempo entre lecturas en milisegundos

// FUNCIONES
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
  pinMode(PINES_ENTRADA[ARRIBA], INPUT_PULLDOWN);
  pinMode(PINES_ENTRADA[ABAJO], INPUT_PULLDOWN);
  pinMode(PINES_ENTRADA[IZQUIERDA], INPUT_PULLDOWN);
  pinMode(PINES_ENTRADA[DERECHA], INPUT_PULLDOWN);
  pinMode(PINES_ENTRADA[BOTON_EXTERIOR], INPUT_PULLDOWN);
  pinMode(PINES_ENTRADA[BOTON_INTERIOR], INPUT_PULLDOWN);
  pinMode(PINES_SALIDA[RIEGO_EXTERIOR], OUTPUT);
  pinMode(PINES_SALIDA[RIEGO_INTERIOR], OUTPUT);
  pinMode(PINES_SALIDA[LIBRE_1], OUTPUT);
  pinMode(PINES_SALIDA[LIBRE_2], OUTPUT);
  // SE APAGAN LAS SALIDAS
  digitalWrite(PINES_SALIDA[RIEGO_EXTERIOR], LOW);
  digitalWrite(PINES_SALIDA[RIEGO_INTERIOR], LOW);
  digitalWrite(PINES_SALIDA[LIBRE_1], LOW);
  digitalWrite(PINES_SALIDA[LIBRE_2], LOW);
  // SE MUESTRA UN MENSAJE DE BIENVENIDA
  imprimirMensaje("Iniciando sistema...");
  delay(2000);
  lcd.clear();
}


void loop() {
  unsigned long tiempoActual = millis();

  // Realizar la lectura solo si ha pasado el intervalo de tiempo
  if (tiempoActual - ultimoTiempo >= intervaloLectura) {
    ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura

    // Lee el estado del botón de reset
    int estadoBoton = digitalRead(RESET_BUTTON_PIN);

    // Si el botón de reset está presionado (estado bajo)
    if (estadoBoton == LOW) {
      Serial.println("Boton de reset presionado. Reiniciando...");
      ESP.restart(); // Reinicia la placa ESP32
    }

    // Imprimir lecturas de temperatura
    lcd.setCursor(12, 0);
    lcd.print(bmp.readTemperature());
    lcd.print(" C");
  }

  // Realizar la lectura de los botones
  int estadoBotonArriba = digitalRead(PINES_ENTRADA[ARRIBA]);
  int estadoBotonAbajo = digitalRead(PINES_ENTRADA[ABAJO]);
  int estadoBotonIzquierda = digitalRead(PINES_ENTRADA[IZQUIERDA]);
  int estadoBotonDerecha = digitalRead(PINES_ENTRADA[DERECHA]);
  int estadoBotonExterior = digitalRead(PINES_ENTRADA[BOTON_EXTERIOR]);
  int estadoBotonInterior = digitalRead(PINES_ENTRADA[BOTON_INTERIOR]);

  // Si el botón de arriba está presionado
  if (estadoBotonArriba == HIGH) {
    lcd.clear();
    imprimirMensaje("Arriba presionado");
  }else if (estadoBotonAbajo == HIGH) {
    lcd.clear();
    imprimirMensaje("Abajo presionado");
  }else if (estadoBotonIzquierda == HIGH) {
    imprimirMensaje("Izquierda presionado");
  }else if (estadoBotonDerecha == HIGH) {
    lcd.clear();
    imprimirMensaje("Derecha presionado");
  }else if (estadoBotonExterior == HIGH) {
    lcd.clear();
    imprimirMensaje("Exterior presionado");
  }else if (estadoBotonInterior == HIGH) {
    lcd.clear();
    imprimirMensaje("Interior presionado");
  }else {
    // imprimirMensaje("Esperando...");
  }
}

void imprimirMensaje(String mensaje) {
  lcd.setCursor(0, 1);
  lcd.print(mensaje);
  }
 
