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

// Temporizador oTemporizadorExterno(0);                                                    // Inicializar con el tiempo inicial (en segundos)
// Temporizador oTemporizadorInterno(0);                                                    // Inicializar con el tiempo inicial (en segundos)
// Pantalla pantalla(LCD_ADDR, LCD_COLS, LCD_ROWS, fila_riego_externo, fila_riego_interno); // Inicializar la pantalla LCD

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
int tiempoInicioPulsacionInterior = 0;

enum RiegoState
{
    INACTIVO,
    ESPERA,
    ACTIVO,
    PAUSA
};
RiegoState estadoRiegoInterior = INACTIVO;

void loop()
{
  riegoInterior();
  // riegoExterior();
}

void riegoInterior()
{
  int tiempoRiegoInterno;        // Variable para almacenar el tiempo de riego interno
  int tiempoContador = millis(); // Variable para almacenar el tiempo actual
  unsigned long tiempoInicioPresionado = 0;

  if (digitalRead(BOTON_INTERIOR) == HIGH)
  {
    if (!botonInternoPresionado)
    {
      // Empieza a contar el tiempo solo si el botón no estaba presionado antes
      tiempoInicioPulsacionInterior = millis();
      botonInternoPresionado = true;
      Serial.println("Botón interno presionado");
      Serial.println(botonInternoPresionado);
    }
    else
    {
      if (estadoRiegoInterior != INACTIVO && (tiempoContador - tiempoInicioPresionado >= 3000))
      {
        // Si el botón ha estado presionado durante 3 segundos
        Serial.println("Pulsación larga detectada");
        botonInternoPresionado = false; // Resetea la detección de pulsación
      }
      if (botonInternoPresionado && (tiempoContador - tiempoInicioPresionado <= 500))
      {
        // Si el botón se ha soltado antes de 500 ms
        Serial.println("Pulsación corta detectada");
        // Resetea las variables para la próxima pulsación
        botonInternoPresionado = false;
      }
    }
  }
  else  // Si el botón no está presionado
  {
    botonInternoPresionado = false; // Resetea la detección de pulsación
  }
      // 

  // if (botonInternoPresionado && digitalRead(BOTON_INTERIOR) == LOW)
  // {
  //   botonInternoPresionado = false;
  //   unsigned long tiempoPresionado = millis() - tiempoInicioPresionado;
  //   contadorBotonInterno++;
  //   Serial.println("Boton interno presionado");
  //   Serial.println(tiempoPresionado);
  //   Serial.println(contadorBotonInterno);

  //   if (tiempoContador - tiempoInicioPresionado >= TIEMPO_MINIMO_REINICIO)
  //   {
  //     tiempoInicioPresionado = millis();
  //     contadorBotonInterno = 0;
  //     Serial.println("Reiniciando temporizador interno");
  //     // reiniciar todo
  //   }

  //   if (contadorBotonInterno == 1) // elige
  //   {
  //     tiempoRiegoInterno = Serial.parseInt();
  //     // oTemporizadorInterno(); // construimos el TEMP con el tiempo recibido y Convertir a milisegundos
  //     Serial.println("El tiempo de riego interno es: ");
  //   }
  //   if (contadorBotonInterno == 2) // inicia
  //   {
  //     // oTemporizadorInterno.iniciarConteoRegresivo(tiempoRiegoInterno * 1000);
  //     // pantalla.iniciarConteo();
  //     Serial.println("Iniciando temporizador interno");
  //   }
  //   else if (contadorBotonInterno % 2 != 0) // pausa
  //   {
  //     // oTemporizadorInterno.pausarConteo();
  //     // pantalla.pausar();
  //     Serial.println("Pausando temporizador interno");
  //   }
  //   else
  //     // oTemporizadorInterno.reanudar(); // reanuda
  //     // pantalla.reanudar();
  //     Serial.println("Reanudando temporizador interno");
  // }
}
// void pausarPantalla()
// {
//   if (!enPausa)
//   {
//     tiempoPausado = millis();
//     enPausa = true;
//   }
// }

// void reanudarTiempoPantalla()
// {
//   temporizador->reanudar();
//   pausado = false;
// }

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
  {                 // Suponiendo que es un LCD de 20 caracteres de ancho
    lcd.print(" "); // Imprime un espacio en blanco en cada posición
  }
}