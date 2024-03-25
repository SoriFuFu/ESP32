#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"

#define LCD_ADDR 0x27        // Dirección de la pantalla LCD
#define BMP280_ADDR 0x76     // Dirección del sensor BMP280 (temperatura y presión)
#define SDA 2                // Pin SDA para la comunicación I2C
#define SCL 4                // Pin SCL para la comunicación I2C
#define RESET_BUTTON_PIN 33  // Pin para el botón de reset

// Tamaño de la pantalla LCD (20 columnas x 4 filas)
#define LCD_COLS 20
#define LCD_ROWS 4

// Inicialización de la librería de la pantalla LCD
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Adafruit_BMP280 bmp;

// VARIABLES
// WIFI
const char *ssid = "Tarta-D";
const char *password = "OliviaPampo2024...";
// PINES DE ENTRADA
const int PINES_ENTRADA[] = { 14, 15, 12, 32, 26, 22 };
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
const int PINES_SALIDA[] = { 21, 19, 18, 5 };
// ENUMERACIÓN PARA INDEXAR LOS PINES DE SALIDA
enum IndiceSalida {
  RIEGO_EXTERIOR,
  RIEGO_INTERIOR,
  LIBRE_1,
  LIBRE_2
};
// GLOBALES
unsigned long ultimoTiempo = 0;
const unsigned long intervaloLectura = 1000;  // Intervalo de tiempo entre lecturas en milisegundos

// FUNCIONES
void setup() {
  Serial.begin(115200);
  // Inicializar la comunicación I2C con los pines SDA y SCL especificados
  Wire.begin(SDA, SCL);
  configTime(3600, 0, "pool.ntp.org"); // Configura el desplazamiento horario a +1 hora (3600 segundos)

  
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
    while (1)
      ;
  }

  // Especificación del PIN como entrada o salida
  pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
  for (int i = 0; i < sizeof(PINES_ENTRADA) / sizeof(PINES_ENTRADA[0]); ++i) {
    pinMode(PINES_ENTRADA[i], INPUT_PULLDOWN);
  }
  for (int i = 0; i < sizeof(PINES_SALIDA) / sizeof(PINES_SALIDA[0]); ++i) {
    pinMode(PINES_SALIDA[i], OUTPUT);
    digitalWrite(PINES_SALIDA[i], LOW);  // Apagar la salida
  }
  // SE MUESTRA UN MENSAJE DE BIENVENIDA
  imprimirMensaje(0, 1, "Iniciando sistema...");
  delay(2000);
  lcd.clear();

  //SE INICIALIZA LA CONEXIÓN WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
  lcd.setCursor(1, 1);
  lcd.print("Conectando WiFi...");
  }
  lcd.clear();
  lcd.setCursor(5, 1);
  lcd.print("Conectado");
  lcd.setCursor(2, 2);
  lcd.print("IP: ");
  lcd.print(WiFi.localIP().toString());
  delay(5000);
  lcd.clear();
}


void loop() {
  unsigned long tiempoActual = millis();
  // Realizar la lectura solo si ha pasado el intervalo de tiempo
  if (tiempoActual - ultimoTiempo >= intervaloLectura) {
    ultimoTiempo = tiempoActual;  // Actualizar el tiempo de la última lectura

    int estadoBoton = digitalRead(RESET_BUTTON_PIN);  // Lee el estado del botón de reset

    if (estadoBoton == LOW) {  // Si el botón de reset está presionado (estado bajo)
      Serial.println("Boton de reset presionado. Reiniciando...");
      ESP.restart();  // Reinicia la placa ESP32
    }
    menuPrincipal();
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
    clearLCDLine(1);
    imprimirMensaje(0, 2, "Arriba presionado");
  } else if (estadoBotonAbajo == HIGH) {
    clearLCDLine(1);
    imprimirMensaje(0, 2, "Abajo presionado");
  } else if (estadoBotonIzquierda == HIGH) {
    imprimirMensaje(0, 2, "Izquierda presionado");
  } else if (estadoBotonDerecha == HIGH) {
    clearLCDLine(1);
    imprimirMensaje(0, 2, "Derecha presionado");
  } else if (estadoBotonExterior == HIGH) {
    clearLCDLine(1);
    imprimirMensaje(0, 2, "Exterior presionado");
  } else if (estadoBotonInterior == HIGH) {
    clearLCDLine(1);
    imprimirMensaje(0, 2, "Interior presionado");
  } else {
    // imprimirMensaje("Esperando...");
  }
}

void imprimirMensaje(int COL, int ROW, String mensaje) {
    static unsigned long tiempoInicioLimpiarLinea = 0;

    unsigned long tiempoActual = millis();

    lcd.setCursor(COL, ROW);
    lcd.print(mensaje);

    // Si han pasado 3 segundos desde la última impresión, limpia la línea
    if (tiempoActual - tiempoInicioLimpiarLinea >= 3000) {
        clearLCDLine(ROW); // Limpia la línea especificada
    }

    // Actualiza el tiempo de inicio para el próximo temporizador
    tiempoInicioLimpiarLinea = tiempoActual;
}

void clearLCDLine(int ROW) {
  lcd.setCursor(0, ROW); // Mueve el cursor al inicio de la línea 'ROW'
  for (int n = 0; n < 20; n++) { // Suponiendo que es un LCD de 20 caracteres de ancho
    lcd.print(" "); // Imprime un espacio en blanco en cada posición
  }
}
void menuPrincipal() {
// Obtiene la hora actual del RTC
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);

    // Formatea la hora para mostrarla en la pantalla LCD
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    

    // Muestra la hora en la pantalla LCD
    lcd.setCursor(0, 0);
    lcd.print(timeStr);

    // Imprimir lecturas de temperatura
    lcd.setCursor(13, 0);
    lcd.print(bmp.readTemperature());
    lcd.print(" C");

    // imprimir Riego exterior
    lcd.setCursor(0, 1);
    lcd.print("  Riego  ");
    lcd.setCursor(9, 1);
    lcd.print("|");
    lcd.setCursor(10, 1);
    lcd.print("|");
    lcd.setCursor(13, 1);
    lcd.print("Riego");

    lcd.setCursor(0, 2);
    lcd.print("Exterior");
    lcd.setCursor(9, 2);
    lcd.print("|");
    lcd.setCursor(10, 2);
    lcd.print("|");
    lcd.setCursor(12 , 2);
    lcd.print("Interior");

    // Imprimir contador de riego
    lcd.setCursor(0, 3);
    lcd.print("00:00:00");
    lcd.setCursor(10, 3);
    lcd.print(" ");
        lcd.setCursor(9, 3);
    lcd.print("|");
    lcd.setCursor(10, 3);
    lcd.print("|");
    lcd.setCursor(12, 3);
    lcd.print("00:00:00");

}