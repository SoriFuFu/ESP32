#ifndef PANTALLA_H
#define PANTALLA_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27 // DIRECCIÓN DE LA PANTALLA LCD
#define SDA 2         // PIN SDA PARA LA COMUNICACIÓN I2C
#define SCL 4         // PIN SCL PARA LA COMUNICACIÓN I2C
// TAMAÑO DE LA PANTALLA LCD (20 COLUMNAS x 4 FILAS)
#define LCD_COLS 20
#define LCD_ROWS 4
byte wifiSymbol[8] = {

    B11111,
    B00000,
    B01110,
    B00000,
    B00100,
    B00100,
    B00000,
    B00000};

byte separationSymbol[8] = {
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111};

byte heartSymbol[8] = {
    B00000,
    B01010,
    B11111,
    B11111,
    B01110,
    B00100,
    B00000,
    B00000};

class Pantalla
{
private:
  LiquidCrystal_I2C lcd;
  unsigned long tiempoAnteriorWifi = 0; // Variable para almacenar el tiempo anterior
  bool wifiEncendido = false;           // Variable para controlar el estado del icono de WiFi

public:
  Pantalla(uint8_t lcd_addr, uint8_t sda, uint8_t scl)
      : lcd(lcd_addr, LCD_COLS, LCD_ROWS)
  {
    Wire.begin(sda, scl);
  }

  void initDisplay() // INICIALIZA LA PANTALLA LCD
  {
    lcd.init();      // INICIALIZA LA PANTALLA LCD
    lcd.backlight(); // ENCIENDE LA RETROILUMINACIÓN DE LA PANTALLA
    lcd.clear();     // LIMPIA LA PANTALLA
    // SE MUESTRA UN MENSAJE DE BIENVENIDA
    lcd.setCursor(0, 1);
    lcd.print("Iniciando sistema...");
    delay(2000);
    lcd.clear(); // LIMPIA LA PANTALLA
  }

  void printTime(char *hora) // MUESTRA LA HORA
  {
    lcd.setCursor(0, 0);
    lcd.print(hora);
  }

  void printTemp(float temp) // MUESTRA LA TEMPERATURA
  {
    int longitud = String(temp).length(); // Obtener la longitud de la temperatura como cadena
    int columna;
    if (longitud <= 5)
    {
      columna = 13; // Si la temperatura tiene decenas y 2 decimales
    }
    else
    {
      columna = 12; // Si la temperatura tiene centenas
    }
    lcd.setCursor(columna, 0);
    lcd.print(temp);
    lcd.print("C");
    lcd.print((char)223); // Imprimir el símbolo de grado
  }

  void printText(const char *text) // MUESTRA UN TEXTO EN LA PANTALLA
  {
    lcd.setCursor(0, 1);
    lcd.print(text);
  }

  void clearDisplay() // LIMPIA LA PANTALLA
  {
    lcd.clear();
  }

  void clearLine(int line) // LIMPIA UNA LÍNEA DE LA PANTALLA
  {
    lcd.setCursor(0, line);
    lcd.print("                    ");
  }
  void printWifi() // MUESTRA EL ICONO DE WIFI
  {
    lcd.createChar(1, wifiSymbol);
    lcd.setCursor(9, 0);
    lcd.write((uint8_t)1);
  }

  void clearWifi() // LIMPIA EL ICONO DE WIFI
  {
    lcd.setCursor(9, 0); // Posición para imprimir el símbolo del WiFi
    lcd.print(" ");      // Imprime el símbolo del WiFi
  }

  void printAp() // MUESTRA EL ICONO DE AP
  {
    lcd.createChar(3, heartSymbol); // Crea el símbolo del corazón
    lcd.setCursor(6, 0);            // Posición para imprimir el símbolo del corazón
    lcd.write((uint8_t)3);          // Imprime el símbolo del corazón
  }

  void clearAp() // LIMPIA EL ICONO DE AP
  {
    lcd.setCursor(6, 0); // Posición para imprimir el símbolo del corazón
    lcd.print(" ");      // Imprime el símbolo del corazón
  }

  void menu() // MUESTRA EL MENÚ PRINCIPAL
  {
    lcd.setCursor(0, 1);
    lcd.print("Menu Principal");
    lcd.setCursor(0, 2);
    lcd.print("Wifi");
    lcd.setCursor(0, 3);
    lcd.print("Reset");
  }

  void relaySeparation() // MUESTA LA SEPARACIÓN DE LOS RELES
  {
    lcd.createChar(2, separationSymbol);
    lcd.setCursor(9, 1);
    lcd.write((uint8_t)2);
    lcd.createChar(2, separationSymbol);
    lcd.setCursor(10, 1);
    lcd.write((uint8_t)2);
    lcd.createChar(2, separationSymbol);
    lcd.setCursor(9, 2);
    lcd.write((uint8_t)2);
    lcd.createChar(2, separationSymbol);
    lcd.setCursor(10, 2);
    lcd.write((uint8_t)2);
    lcd.createChar(2, separationSymbol);
    lcd.setCursor(9, 3);
    lcd.write((uint8_t)2);
    lcd.createChar(2, separationSymbol);
    lcd.setCursor(10, 3);
    lcd.write((uint8_t)2);
  }

  void K1Name(String name) // MUESTRA EL NOMBRE DEL K1
  {
    lcd.setCursor(0, 1);
    lcd.print("  Riego  ");
    // lcd.print(name);
    lcd.setCursor(0, 2);
    lcd.print("Interior");
  }

  void K2Name(String name) // MUESTRA EL NOMBRE DEL K2
  {
    lcd.setCursor(13, 1);
    lcd.print("Riego");
    // lcd.print(name);
    lcd.setCursor(12, 2);
    lcd.print("Exterior");
  }

  void K3Name(String name) // MUESTRA EL NOMBRE DEL K3
  {
    lcd.setCursor(0, 1);
    lcd.print("  Luz  ");
    // lcd.print(name);
    lcd.setCursor(0, 2);
    lcd.print("Interior");
  }

  void K4Name(String name) // MUESTRA EL NOMBRE DEL K4
  {
    lcd.setCursor(13, 1);
    lcd.print("Luz");
    // lcd.print(name);
    lcd.setCursor(12, 2);
    lcd.print("Exterior");
  }

  void K1Timer(unsigned long remainingTimeK1) // MUESTRA EL TEMPORIZADOR DEL K1
  {
    // Convertir milisegundos a segundos
    int segundosTotales = remainingTimeK1 / 1000;
    // Calcular horas, minutos y segundos
    int horas = segundosTotales / 3600;             // Obtener las horas completas
    int segundosRestantes = segundosTotales % 3600; // Obtener los segundos restantes después de las horas
    int minutos = segundosRestantes / 60;           // Obtener los minutos restantes
    int segundos = segundosRestantes % 60;          // Obtener los segundos restantes después de los minutos
                                                    // Formatear y mostrar en el LCD
    lcd.setCursor(0, 3);
    lcd.print(horas < 10 ? "0" + String(horas) : String(horas)); // Mostrar horas con dos dígitos
    lcd.setCursor(2, 3);
    lcd.print(":");
    lcd.setCursor(3, 3);
    lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos)); // Mostrar minutos con dos dígitos
    lcd.setCursor(5, 3);
    lcd.print(":");
    lcd.setCursor(6, 3);
    lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos)); // Mostrar segundos con dos dígitos
  }

  void K2Timer(unsigned long remainingTimeK2) // MUESTRA EL TEMPORIZADOR DE K2
  {
    // Convertir milisegundos a segundos
    int segundosTotales = remainingTimeK2 / 1000;

    // Calcular horas, minutos y segundos
    int horas = segundosTotales / 3600;             // Obtener las horas completas
    int segundosRestantes = segundosTotales % 3600; // Obtener los segundos restantes después de las horas
    int minutos = segundosRestantes / 60;           // Obtener los minutos restantes
    int segundos = segundosRestantes % 60;          // Obtener los segundos restantes después de los minutos

    // Formatear y mostrar en el LCD
    lcd.setCursor(12, 3);
    lcd.print(horas < 10 ? "0" + String(horas) : String(horas)); // Mostrar horas con dos dígitos
    lcd.setCursor(14, 3);
    lcd.print(":");
    lcd.setCursor(15, 3);
    lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos)); // Mostrar minutos con dos dígitos
    lcd.setCursor(17, 3);
    lcd.print(":");
    lcd.setCursor(18, 3);
    lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos)); // Mostrar segundos con dos dígitos
  }

  void K3Timer(unsigned long remainingTimeK3) // MUESTRA EL TEMPORIZADOR DE K3
  {
    // Convertir milisegundos a segundos
    int segundosTotales = remainingTimeK3 / 1000;

    // Calcular horas, minutos y segundos
    int horas = segundosTotales / 3600;             // Obtener las horas completas
    int segundosRestantes = segundosTotales % 3600; // Obtener los segundos restantes después de las horas
    int minutos = segundosRestantes / 60;           // Obtener los minutos restantes
    int segundos = segundosRestantes % 60;          // Obtener los segundos restantes después de los minutos

    // Formatear y mostrar en el LCD
    lcd.setCursor(0, 3);
    lcd.print(horas < 10 ? "0" + String(horas) : String(horas)); // Mostrar horas con dos dígitos
    lcd.setCursor(2, 3);
    lcd.print(":");
    lcd.setCursor(3, 3);
    lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos)); // Mostrar minutos con dos dígitos
    lcd.setCursor(5, 3);
    lcd.print(":");
    lcd.setCursor(6, 3);
    lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos)); // Mostrar segundos con dos dígitos
  }

  void K4Timer(unsigned long remainingTimeK4) // MUESTRA EL TEMPORIZADOR DE K4
  {
    // Convertir milisegundos a segundos
    int segundosTotales = remainingTimeK4 / 1000;

    // Calcular horas, minutos y segundos
    int horas = segundosTotales / 3600;             // Obtener las horas completas
    int segundosRestantes = segundosTotales % 3600; // Obtener los segundos restantes después de las horas
    int minutos = segundosRestantes / 60;           // Obtener los minutos restantes
    int segundos = segundosRestantes % 60;          // Obtener los segundos restantes después de los minutos

    // Formatear y mostrar en el LCD
    lcd.setCursor(12, 3);
    lcd.print(horas < 10 ? "0" + String(horas) : String(horas)); // Mostrar horas con dos dígitos
    lcd.setCursor(14, 3);
    lcd.print(":");
    lcd.setCursor(15, 3);
    lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos)); // Mostrar minutos con dos dígitos
    lcd.setCursor(17, 3);
    lcd.print(":");
    lcd.setCursor(18, 3);
    lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos)); // Mostrar segundos con dos dígitos
  }

  void cleanK1Timer() // LIMPIA EL TEMPORIZADOR DE K1
  {
    lcd.setCursor(0, 3);
    lcd.print("  ");
    lcd.setCursor(2, 3);
    lcd.print(":");
    lcd.setCursor(3, 3);
    lcd.print("  ");
    lcd.setCursor(5, 3);
    lcd.print(":");
    lcd.setCursor(6, 3);
    lcd.print("  ");
  }

  void cleanK2Timer() // LIMPIA EL TEMPORIZADOR DE K2
  {
    lcd.setCursor(12, 3);
    lcd.print("  ");
    lcd.setCursor(14, 3);
    lcd.print(":");
    lcd.setCursor(15, 3);
    lcd.print("  ");
    lcd.setCursor(17, 3);
    lcd.print(":");
    lcd.setCursor(18, 3);
    lcd.print("  ");
  }

  void cleanK3Timer() // LIMPIA EL TEMPORIZADOR DE K3
  {
    lcd.setCursor(0, 3);
    lcd.print("  ");
    lcd.setCursor(2, 3);
    lcd.print(":");
    lcd.setCursor(3, 3);
    lcd.print("  ");
    lcd.setCursor(5, 3);
    lcd.print(":");
    lcd.setCursor(6, 3);
    lcd.print("  ");
  }

  void cleanK4Timer() // LIMPIA EL TEMPORIZADOR DE K4
  {
    lcd.setCursor(12, 3);
    lcd.print("  ");
    lcd.setCursor(14, 3);
    lcd.print(":");
    lcd.setCursor(15, 3);
    lcd.print("  ");
    lcd.setCursor(17, 3);
    lcd.print(":");
    lcd.setCursor(18, 3);
    lcd.print("  ");
  }

  void Reset()
  {
    Serial.println("Reiniciando...");
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("Reiniciando...");
    delay(2000);
  }
};

#endif
