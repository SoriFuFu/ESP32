#include <LiquidCrystal_I2C.h>

class Pantalla {
private:
  LiquidCrystal_I2C lcd;

public:
  Pantalla(LiquidCrystal_I2C lcd) : lcd(lcd) {}

// Método para iniciar el conteo en la pantalla
void iniciarConteo(int tiempoProgramado, bool esRiegoInterno) {
  lcd.clear(); // Limpiar la pantalla LCD
  
  if (esRiegoInterno) {
    // Mostrar el tiempo programado para el riego interno en la fila 3 y columnas 1 a 8
    lcd.setCursor(1, 3);
    lcd.print(tiempoProgramado);
  } else {
    // Mostrar el tiempo programado para el riego externo en la fila 3 y columnas 11 a 18
    lcd.setCursor(11, 3);
    lcd.print(tiempoProgramado);
  }
}


void pausar() {
  
}
