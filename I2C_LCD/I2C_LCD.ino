#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Dirección I2C de la pantalla LCD
#define LCD_ADDR 0x27

// Tamaño de la pantalla LCD (20 columnas x 4 filas)
#define LCD_COLS 20
#define LCD_ROWS 4

// Configuración de los pines SDA y SCL
#define SDA_PIN 2
#define SCL_PIN 4

// Inicialización de la pantalla LCD
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS); 

void setup() {
  // Inicializa la comunicación I2C con los pines SDA y SCL
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Inicializa la pantalla LCD
  lcd.init();
  
  // Enciende la retroiluminación de la pantalla LCD
  lcd.backlight();

  // Imprime mensajes en diferentes filas de la pantalla LCD
  lcd.setCursor(0, 0);
  lcd.print("Pinche");
  
  lcd.setCursor(0, 1);
  lcd.print("Pampodrila");
  
  lcd.setCursor(0, 2);
  lcd.print("Cancodrila");
  
  lcd.setCursor(0, 3);
  lcd.print("Vampirín");
}

void loop() {
  // No necesitamos hacer nada en el bucle loop para este ejemplo
}

