#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include "Temporizador.h"
#include "Pantalla.h"

#define LCD_ADDR 0x27       // Dirección de la pantalla LCD
#define BMP280_ADDR 0x76    // Dirección del sensor BMP280 (temperatura y presión)
#define SDA 2               // Pin SDA para la comunicación I2C
#define SCL 4               // Pin SCL para la comunicación I2C
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
enum IndiceEntrada
{
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
enum IndiceSalida
{
  RIEGO_EXTERIOR,
  RIEGO_INTERIOR,
  LIBRE_1,
  LIBRE_2
};

Temporizador oTemporizadorExterno(0);                                                    // Inicializar con el tiempo inicial (en segundos)
Temporizador oTemporizadorInterno(0);                                                    // Inicializar con el tiempo inicial (en segundos)
Pantalla pantalla(LCD_ADDR, LCD_COLS, LCD_ROWS, fila_riego_externo, fila_riego_interno); // Inicializar la pantalla LCD

void setup()
{
  // Inicialización de los pines de entrada
  for (int pin : PINES_ENTRADA)
  {
    pinMode(pin, INPUT);
  }

  // Inicialización de los pines de salida
  for (int pin : PINES_SALIDA)
  {
    pinMode(pin, OUTPUT);
  }

  // Inicialización de la pantalla LCD
  lcd.init();
  lcd.backlight();

  // Inicialización del sensor BMP280
  if (!bmp.begin(BMP280_ADDR))
  {
    Serial.println("Error al iniciar el sensor BMP280. Verifique la conexión y reinicie el dispositivo.");
    while (1)
      ;
  }

  // Inicialización de la conexión WiFi
  Serial.println("Conectando a la red WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Conectando...");
  }
  Serial.println("Conexión exitosa. Dirección IP asignada: ");
  Serial.println(WiFi.localIP());
}

// Variables para controlar el estado de los botones de riego interno y externo
bool botonInternoPresionado = false;
bool botonExternoPresionado = false;

// Contadores para llevar la cuenta de cuántas veces se ha presionado cada botón
int contadorBotonInterno = 0;
int contadorBotonExterno = 0;

// Temporizador para medir el tiempo que se mantiene presionado un botón
unsigned long tiempoInicioPresionado = 0;

// Constante para definir el tiempo mínimo de presión para reiniciar
const unsigned long TIEMPO_MINIMO_REINICIO = 3000; // 3 segundos

void loop()
{
  riegoInterior();
  riegoExterior();

}

void riegoInterior()
{
  int tiempoRiegoInterno; // Variable para almacenar el tiempo de riego interno

  if (digitalRead(PINES_ENTRADA[BOTON_INTERIOR]) == HIGH)
  {
    botonInternoPresionado = true;
    tiempoInicioPresionado = millis();

    if (botonInternoPresionado && digitalRead(PINES_ENTRADA[BOTON_INTERIOR]) == LOW)
    {
      botonInternoPresionado = false;
      unsigned long tiempoPresionado = millis() - tiempoInicioPresionado;
      contadorBotonInterno++;

      if (tiempoPresionado >= TIEMPO_MINIMO_REINICIO)
      {
        contadorBotonInterno = 0;
        // reiniciar todo 
      }

      if (contadorBotonInterno == 1) // elige
      {
        tiempoRiegoInterno = Serial.parseInt();
        oTemporizadorInterno(); // construimos el TEMP con el tiempo recibido y Convertir a milisegundos
      }
      if (contadorBotonInterno == 2) // inicia
      {
        oTemporizadorInterno.iniciarConteoRegresivo(tiempoRiegoInterno * 1000);
        pantalla.iniciarConteo(;
      }
      else if (contadorBotonInterno % 2 != 0) // pausa
      {
        oTemporizadorInterno.pausarConteo();
        pantalla.pausar();
      }
      else
        oTemporizadorInterno.reanudar(); // reanuda
        pantalla.reanudar();
    }
  }
  else
  {
    oTemporizadorInterno.mostrarConteo(lcd);
  }
}
void pausarPantalla()
{
  if (!enPausa)
  {
    tiempoPausado = millis();
    enPausa = true;
  }
}

void reanudarTiempoPantalla()
{
  temporizador->reanudar();
  pausado = false;
}
