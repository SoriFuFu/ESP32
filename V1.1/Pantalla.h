#ifndef PANTALLA_H
#define PANTALLA_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define LCD_ADDR 0x27 // DIRECCIÓN DE LA PANTALLA LCD
#define SDA 21        // PIN SDA PARA LA COMUNICACIÓN I2C
#define SCL 22        // PIN SCL PARA LA COMUNICACIÓN I2C
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

  bool initDisplay() // INICIALIZA LA PANTALLA LCD
  {
    lcd.init();      // Inicializa la pantalla LCD
    lcd.backlight(); // Enciende la retroiluminación de la pantalla
    lcd.clear();     // Limpia la pantalla

    // Intentamos escribir un mensaje de bienvenida
    lcd.setCursor(0, 0);
    lcd.print("BaBu Control V1.0");

    // Esperamos un momento para verificar si la pantalla está funcionando
    delay(1000);

    // Ahora intentamos leer la dirección de la pantalla para verificar si está presente
    Wire.beginTransmission(LCD_ADDR);
    if (Wire.endTransmission() == 0)
    {
      return true; // La pantalla está presente y funcional
    }
    else
    {
      return false; // La pantalla no respondió, lo que indica que hay un problema
    }
  }
  void printTime(int hour, int minute, int second) // MUESTRA LA HORA
  {
    lcd.setCursor(0, 0);
    lcd.print(hour < 10 ? "0" + String(hour) : String(hour)); // Mostrar horas con dos dígitos
    lcd.print(":");
    lcd.print(minute < 10 ? "0" + String(minute) : String(minute)); // Mostrar minutos con dos dígitos
    lcd.print(":");
    lcd.print(second < 10 ? "0" + String(second) : String(second)); // Mostrar segundos con dos dígitos
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

  void printText(const char *text, int column, int row) // MUESTRA UN TEXTO EN LA PANTALLA
  {
    lcd.setCursor(column, row);
    lcd.print(text);
  }

  void printMemory(int freeMemory) // MUESTRA LA MEMORIA LIBRE
  {
    lcd.setCursor(14, 0);
    lcd.print(freeMemory);
    lcd.print(" %");
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
    lcd.setCursor(10, 0);           // Posición para imprimir el símbolo del corazón
    lcd.write((uint8_t)3);          // Imprime el símbolo del corazón
  }

  void clearAp() // LIMPIA EL ICONO DE AP
  {
    lcd.setCursor(10, 0); // Posición para imprimir el símbolo del corazón
    lcd.print(" ");       // Imprime el símbolo del corazón
  }

  void clearDisplay() // LIMPIA LA PANTALLA
  {
    lcd.clear();
  }

  void offDisplay() // APAGA LA PANTALLA
  {
    lcd.noBacklight();
  }

  void onDisplay() // ENCIENDE LA PANTALLA
  {
    lcd.backlight(); // ENCIENDE LA RETROILUMINACIÓN DE LA PANTALLA
  }

  void clearLine(int line) // LIMPIA UNA LÍNEA DE LA PANTALLA
  {
    lcd.setCursor(0, line);
    lcd.print("                    ");
  }

  void page(String name, unsigned long time) // MUESTRA LA PRIMERA PÁGINA
  {
    clearLine(1);
    clearLine(2);
    clearLine(3);
    printName(name);
    countPage(time);
  }
  void printName(String name) // MUESTRA EL NOMBRE DE LA PÁGINA
  {
    int lineLength = 20;
    int nameLength = name.length();
    int spaces = (lineLength - nameLength) / 2;
    clearLine(1);
    lcd.setCursor(0, 2);
    for (int i = 0; i < spaces; i++)
    {
      lcd.print(" ");
    }

    lcd.print(name);
  }

  void countPage(unsigned long time) // MUESTRA LA PRIMERA PÁGINA
  {
    int segundosTotales = time / 1000;
    int horas = segundosTotales / 3600;             // Obtener las horas completas
    int segundosRestantes = segundosTotales % 3600; // Obtener los segundos restantes después de las horas
    int minutos = segundosRestantes / 60;           // Obtener los minutos restantes
    int segundos = segundosRestantes % 60;          // Obtener los segundos restantes después de los minutos
                                                    // Formatear y mostrar en el LCD
    lcd.setCursor(6, 3);
    lcd.print(horas < 10 ? "0" + String(horas) : String(horas)); // Mostrar horas con dos dígitos
    lcd.setCursor(8, 3);
    lcd.print(":");
    lcd.setCursor(9, 3);
    lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos)); // Mostrar minutos con dos dígitos
    lcd.setCursor(11, 3);
    lcd.print(":");
    lcd.setCursor(12, 3);
    lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos)); // Mostrar segundos con dos dígitos
  }

  void cleanCountPage() // LIMPIA EL TIEMPO DE LA PRIMERA PÁGINA
  {
    lcd.setCursor(6, 3);
    lcd.print("  ");
    lcd.setCursor(8, 3);
    lcd.print(":");
    lcd.setCursor(9, 3);
    lcd.print("  ");
    lcd.setCursor(11, 3);
    lcd.print(":");
    lcd.setCursor(12, 3);
    lcd.print("  ");
  }

  void menu(int option) // MUESTRA EL MENÚ PRINCIPAL
  {
    if (option == 1)
    {
      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor(2, 1);
      lcd.print("Conectividad");
    }
    else if (option == 2)
    {
      lcd.setCursor(0, 2);
      lcd.print(">");
      lcd.setCursor(2, 2);
      lcd.print("Equipo");
    }
    else if (option == 3)
    {
      lcd.setCursor(0, 3);
      lcd.print(">");
      lcd.setCursor(2, 3);
      lcd.print("Volver");
    }
  }

  void subMenu(int subMenu, int option) // MUESTRA EL SUBMENÚ
  {
    if (subMenu == 1)
    {
      if (option == 1)
      {
        lcd.setCursor(0, 1);
        lcd.print(">");
        lcd.setCursor(2, 1);
        lcd.print("Conectar WiFi");
      }
      else if (option == 2)
      {
        lcd.setCursor(0, 2);
        lcd.print(">");
        lcd.setCursor(2, 2);
        lcd.print("Conectar AP");
      }
      else if (option == 3)
      {
        lcd.setCursor(0, 3);
        lcd.print(">");
        lcd.setCursor(2, 3);
        lcd.print("Volver");
      }
    }
    else if (subMenu == 2)
    {
      if (option == 1)
      {
        lcd.setCursor(0, 1);
        lcd.print(">");
        lcd.setCursor(2, 1);
        lcd.print("Reiniciar");
      }
      else if (option == 2)
      {
        lcd.setCursor(0, 2);
        lcd.print(">");
        lcd.setCursor(2, 2);
        lcd.print("Volver");
      }
    }
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
