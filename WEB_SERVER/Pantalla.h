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

  void inicializar()
  {
    lcd.init();      // INICIALIZA LA PANTALLA LCD
    lcd.backlight(); // ENCIENDE LA RETROILUMINACIÓN DE LA PANTALLA
    lcd.clear();     // LIMPIA LA PANTALLA
                     // SE MUESTRA UN MENSAJE DE BIENVENIDA
    imprimirTexto("Iniciando sistema...");
  }

  void imprimirTexto(const char *texto)
  {
    lcd.setCursor(0, 1);
    lcd.print(texto);
    delay(2000);
    limpiarFila(1);
  }

  void limpiarFila(int fila)
  {
    lcd.setCursor(0, fila);
    lcd.print("                    ");
  } 

  void limpiarPantalla()
  {
    lcd.clear();
  }

  void menuPrincipal()
  {
    lcd.setCursor(0, 1);
    lcd.print("Menu Principal");
    lcd.setCursor(4, 1);
    lcd.print("Wifi");
    lcd.setCursor(4, 2);
    lcd.print("Reset");
    lcd.setCursor(4, 3);
    lcd.print("Volver");
  }
  void imprimirHora(char *hora)
  {
    // Imprimir hora
    lcd.setCursor(0, 0);
    lcd.print(hora);
  }

  void imprimirTemp(float temp)
  {
    // Imprimir lecturas de temperatura
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

  void panelRiego()
  {
    lcd.setCursor(0, 1);
    lcd.print("  Riego  ");
    lcd.createChar(2, separationSymbol); // Crea el símbolo del wifi
    lcd.setCursor(9, 1);
    lcd.write((uint8_t)2);               // Imprime el símbolo del wifi
    lcd.createChar(2, separationSymbol); // Crea el símbolo del wif
    lcd.setCursor(10, 1);
    lcd.write((uint8_t)2); // Imprime el símbolo del wifi
    lcd.setCursor(13, 1);
    lcd.print("Riego");

    lcd.setCursor(0, 2);
    lcd.print("Interior");
    lcd.createChar(2, separationSymbol); // Crea el símbolo del wifi
    lcd.setCursor(9, 2);
    lcd.write((uint8_t)2);               // Imprime el símbolo del wifi
    lcd.createChar(2, separationSymbol); // Crea el símbolo del wifi
    lcd.setCursor(10, 2);
    lcd.write((uint8_t)2); // Imprime el símbolo del wifi
    lcd.setCursor(12, 2);
    lcd.print("Exterior");

    lcd.createChar(2, separationSymbol); // Crea el símbolo del wifi
    lcd.setCursor(9, 3);
    lcd.write((uint8_t)2);               // Imprime el símbolo del wifi
    lcd.createChar(2, separationSymbol); // Crea el símbolo del wifi
    lcd.setCursor(10, 3);
    lcd.write((uint8_t)2); // Imprime el símbolo del wifi
  }
  void subMenuWifi()
  {
  }

  void MostrarContadorExterior(int TiempoRestanteRiegoExterior)
  {
    // Convertir milisegundos a segundos
    int segundosTotales = TiempoRestanteRiegoExterior / 1000;

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
  void MostrarContadorInterior(int TiempoRestanteRiegoInterior)
  {
    // Convertir milisegundos a segundos
    int segundosTotales = TiempoRestanteRiegoInterior / 1000;

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
  void LimpiarContadorExterior()
  {
    // Formatear y mostrar en el LCD
    lcd.setCursor(12, 3);
    lcd.print("  "); // Mostrar horas con dos dígitos
    lcd.setCursor(14, 3);
    lcd.print(":");
    lcd.setCursor(15, 3);
    lcd.print("  "); // Mostrar minutos con dos dígitos
    lcd.setCursor(17, 3);
    lcd.print(":");
    lcd.setCursor(18, 3);
    lcd.print("  "); // Mostrar segundos con dos dígitos
  }
  void LimpiarContadorInterior()
  {

    // Formatear y mostrar en el LCD
    lcd.setCursor(0, 3);
    lcd.print("  "); // Mostrar horas con dos dígitos
    lcd.setCursor(2, 3);
    lcd.print(":");
    lcd.setCursor(3, 3);
    lcd.print("  "); // Mostrar minutos con dos dígitos
    lcd.setCursor(5, 3);
    lcd.print(":");
    lcd.setCursor(6, 3);
    lcd.print("  "); // Mostrar segundos con dos dígitos
  }

  void Reset()
  {
    Serial.println("Reiniciando...");
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("Reiniciando...");
    delay(2000);
    ESP.restart(); // Reinicia la placa ESP32
  }

  void imprimirCorazon()
  {
    lcd.createChar(3, heartSymbol); // Crea el símbolo del corazón
    lcd.setCursor(10, 0);           // Posición para imprimir el símbolo del corazón
    lcd.write((uint8_t)3);          // Imprime el símbolo del corazón
  }
  void limpiarCorazon()
  {
    lcd.setCursor(10, 0); // Posición para imprimir el símbolo del corazón
    lcd.print(" ");       // Imprime el símbolo del corazón
  }

  void imprimirWifi()
  {
    lcd.createChar(1, wifiSymbol); // Crea el símbolo del wifi
    lcd.setCursor(9, 0);           // Posición para imprimir el símbolo del wifi
    lcd.write((uint8_t)1);         // Imprime el símbolo del wifi
  }
  void limpiarWifi()
  {
    lcd.setCursor(9, 0); // Posición para imprimir el símbolo del wifi
    lcd.print(" ");      // Imprime el símbolo del wifi
  }
  void intermitenteWifi()
  {
    unsigned long tiempoActual = millis(); // Obtener el tiempo actual

    // Verificar si ha pasado medio segundo desde la última vez que se cambió el estado del icono de WiFi
    if (tiempoActual - tiempoAnteriorWifi >= 500)
    {
      tiempoAnteriorWifi = tiempoActual; // Actualizar el tiempo anterior

      lcd.setCursor(13, 0); // Posición para imprimir el símbolo del WiFi

      // Alternar el estado del icono del WiFi
      wifiEncendido = !wifiEncendido;

      if (wifiEncendido)
      {
        imprimirWifi(); // Imprimir el símbolo del WiFi si está encendido
      }
      else
      {
        limpiarWifi(); // Limpiar el espacio si está apagado
      }
    }
  }
};

#endif
