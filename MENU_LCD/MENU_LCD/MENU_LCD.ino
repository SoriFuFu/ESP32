#include "Pantalla.h"
#include "time.h"
#include <Bounce2.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <WiFi.h>

// INTERVALO DE LECTURA CADA 1 SEGUNDO
#define INTERVALODELECTURA 1000
#define BMP280_ADDR 0x76 // DIRECCIÓN DEL SENSOR BMP280

Pantalla pantallaLCD(LCD_ADDR, SDA, SCL);
Adafruit_BMP280 bmp;

// VARIABLES DE ENTRADA
#define RESET_BUTTON_PIN 33 // PIN PARA EL BOTÓN DE RESET
#define ARRIBA 14           // PIN PARA EL BOTÓN DE ARRIBA
#define ABAJO 15            // PIN PARA EL BOTÓN DE ABAJO
#define IZQUIERDA 12        // PIN PARA EL BOTÓN DE IZQUIERDA
#define DERECHA 32          // PIN PARA EL BOTÓN DE DERECHA
#define BOTON_EXTERIOR 22   // PIN PARA EL BOTÓN DE RIEGO EXTERIOR
#define BOTON_INTERIOR 13   // PIN PARA EL BOTÓN DE RIEGO INTERIOR

// VARIABLES DE SALIDA
#define RIEGO_EXTERIOR 21 // PIN PARA EL RIEGO EXTERIOR
#define RIEGO_INTERIOR 19 // PIN PARA EL RIEGO INTERIOR
#define LIBRE_1 18        // PIN PARA UNA SALIDA LIBRE
#define LIBRE_2 5         // PIN PARA UNA SALIDA LIBRE

// WIFI
const char *ssid = "Tarta-D";
const char *password = "OliviaPampo2024...";

// CREAR OBJETOS DE LA CLASE BOUNCE PARA CADA BOTÓN
Bounce btnOk = Bounce();
Bounce btnArriba = Bounce();
Bounce btnAbajo = Bounce();
Bounce btnIzquierda = Bounce();
Bounce btnDerecha = Bounce();
Bounce btnExterior = Bounce();
Bounce btnInterior = Bounce();

// ESTADOS DEL RIEGO
enum RiegoState
{
  INACTIVO,
  ESPERA,
  ACTIVO,
  PAUSA,
  APAGADO
};

// ESTABLECER EL ESTADO INICIAL DE CADA RIEGO
RiegoState estadoRiegoExterior = INACTIVO;
int tiempoSeleccionadoRiegoExterior = 0;
int tiempoRestanteRiegoExterior = 0;
bool contadorExterior = 0;
RiegoState estadoRiegoInterior = INACTIVO;
int tiempoSeleccionadoRiegoInterior = 0;
int tiempoRestanteRiegoInterior = 0;
bool contadorInterior = 0;
RiegoState estadoRiegoLibre1 = INACTIVO;
RiegoState estadoRiegoLibre2 = INACTIVO;

bool conectandoWiFi = false;

void setup()
{
  Serial.begin(115200);

  // Inicializar la pantalla
  pantallaLCD.inicializar();

  // SENSOR BMP280
  if (!bmp.begin(BMP280_ADDR))
  {
    Serial.println("No se ha detectado el sensor BMP280");
  }

  // CONFIGURACIÓN DE LOS BOTONES DE ENTRADA
  btnOk.attach(RESET_BUTTON_PIN, INPUT_PULLUP);       // RESET COMO PULLUP
  btnArriba.attach(ARRIBA, INPUT_PULLDOWN);           // ARRIBA COMO PULLDOWN
  btnAbajo.attach(ABAJO, INPUT_PULLDOWN);             // ABAJO COMO PULLDOWN
  btnIzquierda.attach(IZQUIERDA, INPUT_PULLDOWN);     // IZQUIERDA COMO PULLDOWN
  btnDerecha.attach(DERECHA, INPUT_PULLDOWN);         // DERECHA COMO PULLDOWN
  btnExterior.attach(BOTON_EXTERIOR, INPUT_PULLDOWN); // RIEGO EXTERIOR COMO PULLDOWN
  btnInterior.attach(BOTON_INTERIOR, INPUT_PULLDOWN); // RIEGO INTERIOR COMO PULLDOWN

  // ESTABLECER EL TIEMPO DE REBOTE DE CADA BOTÓN
  btnOk.interval(20);
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

  configTime(3600, 0, "pool.ntp.org"); // CONFIGURAR LA HORA CON UN SERVIDOR NTP
  pantallaLCD.panelRiego();
  pantallaLCD.MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
  pantallaLCD.MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
  // SE INICIALIZA LA CONEXIÓN WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
   pantallaLCD.intermitenteWifi();
  }
  pantallaLCD.imprimirWifi();
 

}

void loop()
{
  //   if (!WiFi.isConnected() && !conectandoWiFi) {
  //   conectarWiFi(); // Intenta conectar al WiFi
  // }
  unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  // Realizar la lectura solo si ha pasado el intervalo de tiempo
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
  {
    ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
    // Obtiene la hora actual del RTC
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    // Formatea la hora para mostrarla en la pantalla LCD
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
    float temperatura = bmp.readTemperature();
    pantallaLCD.imprimirHoraTemp(timeStr, temperatura);
    // Actualizar el tiempo restante de riego
    if (estadoRiegoExterior == ACTIVO)
    {
      tiempoRestanteRiegoExterior -= 1000;
      pantallaLCD.MostrarContadorExterior(tiempoRestanteRiegoExterior);
      if (tiempoRestanteRiegoExterior == 0)
      {
        estadoRiegoExterior = INACTIVO;
        digitalWrite(RIEGO_EXTERIOR, LOW);
        pantallaLCD.MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
      }
    }
    if (estadoRiegoInterior == ACTIVO)
    {
      tiempoRestanteRiegoInterior -= 1000;
      pantallaLCD.MostrarContadorInterior(tiempoRestanteRiegoInterior);
      if (tiempoRestanteRiegoInterior == 0)
      {
        estadoRiegoInterior = INACTIVO;
        digitalWrite(RIEGO_INTERIOR, LOW);
        pantallaLCD.MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
      }
    }
    pantallaLCD.imprimirCorazon();
  }
  static bool state = false; // Declaración estática de state
  static unsigned long repeticion500 = 0;

  // Realizar la lectura solo si ha pasado el intervalo de tiempo
  if (tiempoActual - repeticion500 >= 500)
  {
    repeticion500 = tiempoActual; // Actualizar el tiempo de la última lectura

    if (contadorExterior == 1)
    {
      if (state == false)
      {
        pantallaLCD.MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
        state = true;
      }
      else
      {
        pantallaLCD.LimpiarContadorExterior();
        state = false;
      }
      Serial.println("Contador exterior");
    }

    if (contadorInterior == 1)
    {
      if (state == false)
      {
        pantallaLCD.MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
        state = true;
      }
      else
      {
        pantallaLCD.LimpiarContadorInterior();
        state = false;
      }
      Serial.println("Contador interior");
    }
  }

  // ACTUALIZAR EL ESTADO DE CADA BOTÓN
  btnOk.update();
  btnArriba.update();
  btnAbajo.update();
  btnIzquierda.update();
  btnDerecha.update();
  btnExterior.update();
  btnInterior.update();

  if (btnOk.rose())
  {
    if (contadorInterior == 1)
    {
      contadorInterior = 0;
    }
    else if (contadorExterior == 1)
    {
      contadorExterior = 0;
    }
  }

  if (btnArriba.fell())
  {
    // Botón ARRIBA presionado
    if (contadorInterior == 1)
    {
      tiempoSeleccionadoRiegoInterior += 60000;
    }
    else if (contadorExterior == 1)
    {
      tiempoSeleccionadoRiegoExterior += 60000;
    }
  }

  if (btnAbajo.fell())
  {
    // Botón ABAJO presionado
    if (contadorInterior == 1)
    {
      tiempoSeleccionadoRiegoInterior -= 60000;
    }
    else if (contadorExterior == 1)
    {
      tiempoSeleccionadoRiegoExterior -= 60000;
    }
  }

  if (btnIzquierda.fell())
  {
    // Botón IZQUIERDA presionado
    Serial.println("Botón IZQUIERDA presionado");
  }

  if (btnDerecha.fell())
  {
    // Botón DERECHA presionado
    Serial.println("Botón DERECHA presionado");
  }
  // RIEGO EXTERIOR
  if (btnExterior.fell())
  {
    // Serial.println("Botón de riego exterior presionado");
    switch (estadoRiegoExterior)
    {
    case INACTIVO:
      setTempExt();
      break;
    case ESPERA:
      contadorExterior = 0;
      tiempoRestanteRiegoExterior = tiempoSeleccionadoRiegoExterior;
      iniciarRiegoExt();
      break;
    case ACTIVO:
      pausarRiegoExt();
      break;
    case PAUSA:
      continuarRiegoExt();
      break;
    case APAGADO:
      estadoRiegoExterior = INACTIVO;
      break;
    }
  }
  else if (btnExterior.read() == HIGH && btnExterior.currentDuration() == 3000)
  {
    apagarRiegoExt();
  }

  // RIEGO INTERIOR
  if (btnInterior.fell())
  {
    // Serial.println("Botón de riego interior presionado");
    switch (estadoRiegoInterior)
    {
    case INACTIVO:
      setTempInt();
      break;
    case ESPERA:
      contadorInterior = 0;
      tiempoRestanteRiegoInterior = tiempoSeleccionadoRiegoInterior;
      iniciarRiegoInt();
      break;
    case ACTIVO:
      pausarRiegoInt();
      break;
    case PAUSA:
      continuarRiegoInt();
      break;
    case APAGADO:
      estadoRiegoInterior = INACTIVO;
      break;
    }
  }
  else if (btnInterior.read() == HIGH && btnInterior.currentDuration() == 3000)
  {
    apagarRiegoInt();
  }
}

void setTempExt()
{
  if (estadoRiegoExterior == INACTIVO)
  {
    contadorExterior = 1;
    estadoRiegoExterior = ESPERA;
  }
}

void iniciarRiegoExt()
{
  digitalWrite(RIEGO_EXTERIOR, HIGH);
  estadoRiegoExterior = ACTIVO;
}

void pausarRiegoExt()
{
  digitalWrite(RIEGO_EXTERIOR, LOW);
  estadoRiegoExterior = PAUSA;
}

void continuarRiegoExt()
{
  digitalWrite(RIEGO_EXTERIOR, HIGH);
  estadoRiegoExterior = ACTIVO;
}

void apagarRiegoExt()
{
  digitalWrite(RIEGO_EXTERIOR, LOW);
  estadoRiegoExterior = APAGADO;
  pantallaLCD.MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
}

void setTempInt()
{
  if (estadoRiegoInterior == INACTIVO)
  {
    contadorInterior = 1;
    estadoRiegoInterior = ESPERA;
  }
}

void iniciarRiegoInt()
{
  digitalWrite(RIEGO_INTERIOR, HIGH);
  estadoRiegoInterior = ACTIVO;
}

void pausarRiegoInt()
{
  digitalWrite(RIEGO_INTERIOR, LOW);
  estadoRiegoInterior = PAUSA;
}

void continuarRiegoInt()
{
  digitalWrite(RIEGO_INTERIOR, HIGH);
  estadoRiegoInterior = ACTIVO;
}

void apagarRiegoInt()
{
  digitalWrite(RIEGO_INTERIOR, LOW);
  estadoRiegoInterior = APAGADO;
  pantallaLCD.MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
}

// Función para conectar al WiFi
void conectarWiFi() {
  Serial.println("Conectando al WiFi...");
  conectandoWiFi = true; // Establecer la variable de estado a true para indicar que se está intentando conectar

  // Inicializar la conexión WiFi
  WiFi.begin(ssid, password);

  int intentos = 0;
  // Verificar el estado de la conexión WiFi hasta que se establezca la conexión o se agoten los intentos
  if (WiFi.status() != WL_CONNECTED && intentos < 20) {
    // Mostrar el icono de WiFi intermitente mientras se intenta conectar
    pantallaLCD.intermitenteWifi();
    
    delay(500);
    Serial.print(".");
    intentos++;
  }

  // Verificar si se pudo conectar al WiFi
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("Conexión WiFi establecida");
    Serial.print("Dirección IP: ");
    Serial.println(WiFi.localIP());
    conectandoWiFi = false; // Establecer la variable de estado a false para indicar que se ha establecido la conexión
    pantallaLCD.imprimirWifi();
  } else {
    Serial.println("");
    Serial.println("Error al conectar al WiFi.");
    pantallaLCD.limpiarWifi();
  }
}
