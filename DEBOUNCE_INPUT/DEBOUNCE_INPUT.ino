#include <Wire.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>
#include "time.h"
#include <Bounce2.h>

#define LCD_ADDR 0x27     // DIRECCIÓN DE LA PANTALLA LCD
#define BMP280_ADDR 0x76  // DIRECCIÓN DEL SENSOR BMP280
#define SDA 2             // PIN SDA PARA LA COMUNICACIÓN I2C
#define SCL 4             // PIN SCL PARA LA COMUNICACIÓN I2C

// TAMAÑO DE LA PANTALLA LCD (20 COLUMNAS x 4 FILAS)
#define LCD_COLS 20
#define LCD_ROWS 4

// INTERVALO DE LECTURA CADA 1 SEGUNDO
#define INTERVALODELECTURA 1000

// CREAR OBJETOS DE LAS CLASES LiquidCrystal_I2C y Adafruit_BMP280
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);
Adafruit_BMP280 bmp;

// VARIABLES DE ENTRADA
#define RESET_BUTTON_PIN 33  // PIN PARA EL BOTÓN DE RESET
#define ARRIBA 14            // PIN PARA EL BOTÓN DE ARRIBA
#define ABAJO 15             // PIN PARA EL BOTÓN DE ABAJO
#define IZQUIERDA 12         // PIN PARA EL BOTÓN DE IZQUIERDA
#define DERECHA 32           // PIN PARA EL BOTÓN DE DERECHA
#define BOTON_EXTERIOR 22    // PIN PARA EL BOTÓN DE RIEGO EXTERIOR
#define BOTON_INTERIOR 13    // PIN PARA EL BOTÓN DE RIEGO INTERIOR

// VARIABLES DE SALIDA
#define RIEGO_EXTERIOR 21  // PIN PARA EL RIEGO EXTERIOR
#define RIEGO_INTERIOR 19  // PIN PARA EL RIEGO INTERIOR
#define LIBRE_1 18         // PIN PARA UNA SALIDA LIBRE
#define LIBRE_2 5          // PIN PARA UNA SALIDA LIBRE

// WIFI
const char *ssid = "Tarta-D";
const char *password = "OliviaPampo2024...";

// CREAR OBJETOS DE LA CLASE BOUNCE PARA CADA BOTÓN
Bounce btnReset = Bounce();
Bounce btnArriba = Bounce();
Bounce btnAbajo = Bounce();
Bounce btnIzquierda = Bounce();
Bounce btnDerecha = Bounce();
Bounce btnExterior = Bounce();
Bounce btnInterior = Bounce();

// ESTADOS DEL RIEGO
enum RiegoState {
  INACTIVO,
  ESPERA,
  ACTIVO,
  PAUSA
};
// ESTABLECER EL ESTADO INICIAL DE CADA RIEGO
RiegoState estadoRiegoExterior = INACTIVO;
int tiempoSeleccionadoRiegoExterior = 0;
int tiempoRestanteRiegoExterior = 0;
RiegoState estadoRiegoInterior = INACTIVO;
int tiempoSeleccionadoRiegoInterior = 0;
int tiempoRestanteRiegoInterior = 0;
RiegoState estadoRiegoLibre1 = INACTIVO;
RiegoState estadoRiegoLibre2 = INACTIVO;

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA, SCL);  // INICIALIZAR LA COMUNICACIÓN I2C

  lcd.init();       // INICIALIZA LA PANTALLA LCD
  lcd.backlight();  // ENCIENDE LA RETROILUMINACIÓN DE LA PANTALLA
  lcd.clear();      // LIMPIA LA PANTALLA

  // SE MUESTRA UN MENSAJE DE BIENVENIDA
  imprimirMensaje(0, 1, "Iniciando sistema...");
  delay(2000);
  lcd.clear();

  // SENSOR BMP280
  if (!bmp.begin(BMP280_ADDR)) {
    Serial.println("No se ha detectado el sensor BMP280");
  }

  // CONFIGURACIÓN DE LOS BOTONES DE ENTRADA
  btnReset.attach(RESET_BUTTON_PIN, INPUT_PULLUP);     // RESET COMO PULLUP
  btnArriba.attach(ARRIBA, INPUT_PULLDOWN);            // ARRIBA COMO PULLDOWN
  btnAbajo.attach(ABAJO, INPUT_PULLDOWN);              // ABAJO COMO PULLDOWN
  btnIzquierda.attach(IZQUIERDA, INPUT_PULLDOWN);      // IZQUIERDA COMO PULLDOWN
  btnDerecha.attach(DERECHA, INPUT_PULLDOWN);          // DERECHA COMO PULLDOWN
  btnExterior.attach(BOTON_EXTERIOR, INPUT_PULLDOWN);  // RIEGO EXTERIOR COMO PULLDOWN
  btnInterior.attach(BOTON_INTERIOR, INPUT_PULLDOWN);  // RIEGO INTERIOR COMO PULLDOWN

  // ESTABLECER EL TIEMPO DE REBOTE DE CADA BOTÓN
  btnReset.interval(20);
  btnArriba.interval(10);
  btnAbajo.interval(20);
  btnIzquierda.interval(20);
  btnDerecha.interval(20);
  btnExterior.interval(20);
  btnInterior.interval(20);

  // CONFIGURACIÓN DE LOS PINES DE SALIDA
  pinMode(RIEGO_EXTERIOR, OUTPUT);
  pinMode(RIEGO_INTERIOR, OUTPUT);
  pinMode(LIBRE_1, OUTPUT);
  pinMode(LIBRE_2, OUTPUT);
  // APAGAR TODAS LAS SALIDAS
  digitalWrite(RIEGO_EXTERIOR, LOW);
  digitalWrite(RIEGO_INTERIOR, LOW);
  digitalWrite(LIBRE_1, LOW);
  digitalWrite(LIBRE_2, LOW);

  // SE INICIALIZA LA CONEXIÓN WIFI
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

  configTime(3600, 0, "pool.ntp.org");  // CONFIGURAR LA HORA CON UN SERVIDOR NTP
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
  lcd.setCursor(12, 2);
  lcd.print("Interior");

  // Imprimir contador de riego
  lcd.setCursor(8, 3);
  lcd.print(" ");
  lcd.setCursor(9, 3);
  lcd.print("|");
  lcd.setCursor(10, 3);
  lcd.print("|");
  MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
  MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
}

void loop() {
  unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  // Realizar la lectura solo si ha pasado el intervalo de tiempo
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA) {
    ultimoTiempo = tiempoActual;  // Actualizar el tiempo de la última lectura
    menuPrincipal();

    if (estadoRiegoExterior == ACTIVO) {
      ContadorExterior(tiempoRestanteRiegoExterior);
    }


    if (estadoRiegoInterior == ACTIVO) {
      ContadorInterior(tiempoRestanteRiegoInterior);
    }

  }
  // ACTUALIZAR EL ESTADO DE CADA BOTÓN
  btnReset.update();
  btnArriba.update();
  btnAbajo.update();
  btnIzquierda.update();
  btnDerecha.update();
  btnExterior.update();
  btnInterior.update();

  if (btnReset.read() == LOW && btnReset.currentDuration() >= 5000) {
    // Botón RESET presionado durante 3 segundos
    if (btnReset.rose()) {
      Serial.println("Boton de reset presionado. Reiniciando...");
      ESP.restart();  // Reinicia la placa ESP32
    }
  }

  if (btnArriba.fell()) {
    // Botón ARRIBA presionado
    Serial.println("Botón ARRIBA presionado");
  }

  if (btnAbajo.fell()) {
    // Botón ABAJO presionado
    Serial.println("Botón ABAJO presionado");
  }

  if (btnIzquierda.fell()) {
    // Botón IZQUIERDA presionado
    Serial.println("Botón IZQUIERDA presionado");
  }

  if (btnDerecha.fell()) {
    // Botón DERECHA presionado
    Serial.println("Botón DERECHA presionado");
  }

  if (btnExterior.fell()) {

    // Serial.println("Botón de riego exterior presionado");
    switch (estadoRiegoExterior) {
      case INACTIVO:
        estadoRiegoExterior = ESPERA;
        configTemp("RIEGO_EXTERIOR");
        break;
      case ESPERA:
        iniciarRiego("RIEGO_EXTERIOR");
        break;
      case ACTIVO:
        pausarRiego("RIEGO_EXTERIOR");
        break;
      case PAUSA:
        continuarRiego("RIEGO_EXTERIOR");
        break;
    }
  } else if (btnExterior.read() == HIGH && btnExterior.currentDuration() >= 3000) {
    if (btnExterior.fell()) {
      apagarRiego("RIEGO_EXTERIOR");
    }
  }

  if (btnInterior.fell()) {

    // Serial.println("Botón de riego interior presionado");
    switch (estadoRiegoInterior) {
      case INACTIVO:
        configTemp("RIEGO_INTERIOR");
        break;
      case ESPERA:
        iniciarRiego("RIEGO_INTERIOR");
        break;
      case ACTIVO:
        pausarRiego("RIEGO_INTERIOR");
        break;
      case PAUSA:
        continuarRiego("RIEGO_INTERIOR");
        break;
    }
  } else if (btnInterior.read() == HIGH && btnInterior.currentDuration() >= 3000) {
    estadoRiegoInterior = INACTIVO;
    Serial.println("Riego interior desactivado");
  }

  if (estadoRiegoExterior == ACTIVO) {
    iniciarRiego("RIEGO_EXTERIOR");
  } else if (estadoRiegoExterior == PAUSA) {
    digitalWrite(RIEGO_EXTERIOR, LOW);
    Serial.println("Riego exterior pausado");
  }
}
void configTemp(String riego) {
  if (riego == "RIEGO_EXTERIOR") {
    tiempoSeleccionadoRiegoExterior = solicitarTiempo(riego);
    estadoRiegoExterior = ESPERA;
    tiempoRestanteRiegoExterior = tiempoSeleccionadoRiegoExterior;
    MostrarContadorExterior(tiempoRestanteRiegoExterior);
  } else if (riego == "RIEGO_INTERIOR") {
    tiempoSeleccionadoRiegoInterior = solicitarTiempo(riego);
    estadoRiegoInterior = ESPERA;
    tiempoRestanteRiegoInterior = tiempoSeleccionadoRiegoInterior;
    MostrarContadorInterior(tiempoRestanteRiegoInterior);
  }
}
int solicitarTiempo(String riego) {
  Serial.print("Ingrese el tiempo de ");
  Serial.print(riego);
  Serial.println(" en milisegundos:");

  while (!Serial.available()) {
    // Esperar a que esté disponible la entrada serial
  }
  int tiempo = Serial.parseInt();  // Leer el tiempo ingresado
  Serial.println(tiempo);          // Imprimir el tiempo ingresado

  return tiempo;
}
void imprimirMensaje(int COL, int ROW, String mensaje) {
  static unsigned long tiempoInicioLimpiarLinea = 0;

  unsigned long tiempoActual = millis();

  lcd.setCursor(COL, ROW);
  lcd.print(mensaje);

  // Si han pasado 3 segundos desde la última impresión, limpia la línea
  if (tiempoActual - tiempoInicioLimpiarLinea >= 3000) {
    clearLCDLine(ROW);  // Limpia la línea especificada
  }

  // Actualiza el tiempo de inicio para el próximo temporizador
  tiempoInicioLimpiarLinea = tiempoActual;
}
void clearLCDLine(int ROW) {
  lcd.setCursor(0, ROW);          // Mueve el cursor al inicio de la línea 'ROW'
  for (int n = 0; n < 20; n++) {  // Suponiendo que es un LCD de 20 caracteres de ancho
    lcd.print(" ");               // Imprime un espacio en blanco en cada posición
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
}

void iniciarRiego(String riego) {
  if (riego == "RIEGO_EXTERIOR") {
    digitalWrite(RIEGO_EXTERIOR, HIGH);
    estadoRiegoExterior = ACTIVO;
    Serial.println("Riego exterior iniciado");
  } else if (riego == "RIEGO_INTERIOR") {
    digitalWrite(RIEGO_INTERIOR, HIGH);
    estadoRiegoInterior = ACTIVO;
    Serial.println("Riego interior iniciado");
  }
}
void pausarRiego(String riego) {
  if (riego == "RIEGO_EXTERIOR") {
    digitalWrite(RIEGO_EXTERIOR, LOW);
    estadoRiegoExterior = PAUSA;
    Serial.println("Riego exterior pausado");
  } else if (riego == "RIEGO_INTERIOR") {
    digitalWrite(RIEGO_INTERIOR, LOW);
    estadoRiegoInterior = PAUSA;
    Serial.println("Riego interior pausado");
  }
}
void continuarRiego(String riego) {
  if (riego == "RIEGO_EXTERIOR") {
    digitalWrite(RIEGO_EXTERIOR, HIGH);
    estadoRiegoExterior = ACTIVO;
    Serial.println("Riego exterior continuado");
  } else if (riego == "RIEGO_INTERIOR") {
    digitalWrite(RIEGO_INTERIOR, HIGH);
    estadoRiegoInterior = ACTIVO;
    Serial.println("Riego interior continuado");
  }
}
void apagarRiego(String riego) {
  if (riego == "RIEGO_EXTERIOR") {
    digitalWrite(RIEGO_EXTERIOR, LOW);
    estadoRiegoExterior = INACTIVO;
    Serial.println("Riego exterior apagado");
    MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
  } else if (riego == "RIEGO_INTERIOR") {
    digitalWrite(RIEGO_INTERIOR, LOW);
    estadoRiegoInterior = INACTIVO;
    Serial.println("Riego interior apagado");
    MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
  }
}

void ContadorExterior(int TiempoRestanteRiegoExterior) {
  tiempoRestanteRiegoExterior = TiempoRestanteRiegoExterior - 1000;
  MostrarContadorExterior(tiempoRestanteRiegoExterior);
  if (tiempoRestanteRiegoExterior == 0) {
    apagarRiego("RIEGO_EXTERIOR");
  }
}
void MostrarContadorExterior(int TiempoRestanteRiegoExterior) {
  // Convertir milisegundos a segundos
  int segundosTotales = TiempoRestanteRiegoExterior / 1000;

  // Calcular horas, minutos y segundos
  int horas = segundosTotales / 3600;              // Obtener las horas completas
  int segundosRestantes = segundosTotales % 3600;  // Obtener los segundos restantes después de las horas
  int minutos = segundosRestantes / 60;            // Obtener los minutos restantes
  int segundos = segundosRestantes % 60;           // Obtener los segundos restantes después de los minutos

  // Formatear y mostrar en el LCD
  lcd.setCursor(0, 3);
  lcd.print(horas < 10 ? "0" + String(horas) : String(horas));  // Mostrar horas con dos dígitos
  lcd.setCursor(2, 3);
  lcd.print(":");
  lcd.setCursor(3, 3);
  lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos));  // Mostrar minutos con dos dígitos
  lcd.setCursor(5, 3);
  lcd.print(":");
  lcd.setCursor(6, 3);
  lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos));  // Mostrar segundos con dos dígitos
}

void ContadorInterior(int TiempoRestanteRiegoInterior) {
  tiempoRestanteRiegoInterior = TiempoRestanteRiegoInterior - 1000;
  MostrarContadorInterior(tiempoRestanteRiegoInterior);
  if (tiempoRestanteRiegoInterior == 0) {
    apagarRiego("RIEGO_INTERIOR");
  }
}
void MostrarContadorInterior(int TiempoRestanteRiegoInterior) {
  // Convertir milisegundos a segundos
  int segundosTotales = TiempoRestanteRiegoInterior / 1000;

  // Calcular horas, minutos y segundos
  int horas = segundosTotales / 3600;              // Obtener las horas completas
  int segundosRestantes = segundosTotales % 3600;  // Obtener los segundos restantes después de las horas
  int minutos = segundosRestantes / 60;            // Obtener los minutos restantes
  int segundos = segundosRestantes % 60;           // Obtener los segundos restantes después de los minutos

  // Formatear y mostrar en el LCD
  lcd.setCursor(12, 3);
  lcd.print(horas < 10 ? "0" + String(horas) : String(horas));  // Mostrar horas con dos dígitos
  lcd.setCursor(14, 3);
  lcd.print(":");
  lcd.setCursor(15, 3);
  lcd.print(minutos < 10 ? "0" + String(minutos) : String(minutos));  // Mostrar minutos con dos dígitos
  lcd.setCursor(17, 3);
  lcd.print(":");
  lcd.setCursor(18, 3);
  lcd.print(segundos < 10 ? "0" + String(segundos) : String(segundos));  // Mostrar segundos con dos dígitos
}
