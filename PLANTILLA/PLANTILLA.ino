#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"

#define LCD_ADDR 0x27    // Dirección de la pantalla LCD
#define BMP280_ADDR 0x76 // Dirección del sensor BMP280 (temperatura y presión)
#define SDA 2            // Pin SDA para la comunicación I2C
#define SCL 4            // Pin SCL para la comunicación I2C

// Pines de entrada
#define RESET_BUTTON_PIN 33 // Pin para el botón de reset
#define ARRIBA 14           // Pin para el botón de arriba
#define ABAJO 15            // Pin para el botón de abajo
#define IZQUIERDA 12        // Pin para el botón de izquierda
#define DERECHA 32          // Pin para el botón de derecha
#define BOTON_EXTERIOR 22   // Pin para el botón de riego exterior
#define BOTON_INTERIOR 13   // Pin para el botón de riego interior

// Pines de salida
#define RIEGO_EXTERIOR 21 // Pin para el riego exterior
#define RIEGO_INTERIOR 19 // Pin para el riego interior
#define LIBRE_1 18        // Pin para una salida libre
#define LIBRE_2 5         // Pin para otra salida libre

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

void setup()
{
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
    if (!bmp.begin(BMP280_ADDR))
    {
        Serial.println("Could not find a valid BMP280 sensor, check wiring!");
        while (1)
            ;
    }

    // Especificación del PIN como entrada o salida
    pinMode(RESET_BUTTON_PIN, INPUT_PULLUP);
    pinMode(ARRIBA, INPUT_PULLDOWN);
    pinMode(ABAJO, INPUT_PULLDOWN);
    pinMode(IZQUIERDA, INPUT_PULLDOWN);
    pinMode(DERECHA, INPUT_PULLDOWN);
    pinMode(BOTON_EXTERIOR, INPUT_PULLDOWN);
    pinMode(BOTON_INTERIOR, INPUT_PULLDOWN);

    pinMode(RIEGO_EXTERIOR, OUTPUT);
    pinMode(RIEGO_INTERIOR, OUTPUT);
    pinMode(LIBRE_1, OUTPUT);
    pinMode(LIBRE_2, OUTPUT);

    digitalWrite(RIEGO_EXTERIOR, LOW);
    digitalWrite(RIEGO_INTERIOR, LOW);
    digitalWrite(LIBRE_1, LOW);
    digitalWrite(LIBRE_2, LOW);

    // SE MUESTRA UN MENSAJE DE BIENVENIDA
    imprimirMensaje(0, 1, "Iniciando sistema...");
    delay(2000);
    lcd.clear();

    // SE INICIALIZA LA CONEXIÓN WIFI
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
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

void loop{

}

void imprimirMensaje(int COL, int ROW, String mensaje)
{
    static unsigned long tiempoInicioLimpiarLinea = 0;

    unsigned long tiempoActual = millis();

    lcd.setCursor(COL, ROW);
    lcd.print(mensaje);

    // Si han pasado 3 segundos desde la última impresión, limpia la línea
    if (tiempoActual - tiempoInicioLimpiarLinea >= 3000)
    {
        clearLCDLine(ROW); // Limpia la línea especificada
    }

    // Actualiza el tiempo de inicio para el próximo temporizador
    tiempoInicioLimpiarLinea = tiempoActual;
}
void clearLCDLine(int ROW)
{
    lcd.setCursor(0, ROW); // Mueve el cursor al inicio de la línea 'ROW'
    for (int n = 0; n < 20; n++)
    {                   // Suponiendo que es un LCD de 20 caracteres de ancho
        lcd.print(" "); // Imprime un espacio en blanco en cada posición
    }
}
