#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

#define RIEGO_EXTERIOR 21 // PIN PARA EL RIEGO EXTERIOR
#define RIEGO_INTERIOR 19 // PIN PARA EL RIEGO INTERIOR

// INTERVALO DE LECTURA CADA 1 SEGUNDO
#define INTERVALODELECTURA 1000
#define BMP280_ADDR 0x76 // DIRECCIÓN DEL SENSOR BMP280
#define SDA 2            // PIN SDA PARA LA COMUNICACIÓN I2C
#define SCL 4            // PIN SCL PARA LA COMUNICACIÓN I2C

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

WebSocketsServer webSocketSensors = WebSocketsServer(81);
WebSocketsServer webSocketInterior = WebSocketsServer(82);
WebSocketsServer webSocketExterior = WebSocketsServer(83);

AsyncWebServer server(80); // Objeto del servidor HTTP

void webSocketEventSensors(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    readBPM280();
}

void webSocketEventInterior(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Este canal es para controlar el riego interior
    // Implementa la lógica de control de riego interior aquí

    if (type == WStype_TEXT)
    {
        String message = String((char *)payload);
        if (message == "ON")
        {
            digitalWrite(RIEGO_INTERIOR, HIGH);
        }
        else if (message == "OFF")
        {
            digitalWrite(RIEGO_INTERIOR, LOW);
        }
    }
}

void webSocketEventExterior(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    // Este canal es para controlar el riego exterior
    // Implementa la lógica de control de riego exterior aquí

    if (type == WStype_TEXT)
    {
        String message = String((char *)payload);
        if (message == "ON")
        {
            digitalWrite(RIEGO_EXTERIOR, HIGH);
        }
        else if (message == "OFF")
        {
            digitalWrite(RIEGO_EXTERIOR, LOW);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    Wire.begin(SDA, SCL);
    WiFi.begin(ssid, password);

    // Inicializar SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("Error al montar SPIFFS");
        return;
    }
    Serial.println("SPIFFS montado correctamente");

    // SENSOR BMP280
    if (!bmp.begin(BMP280_ADDR))
    {
        Serial.println("No se ha detectado el sensor BMP280");
    }

    pinMode(RIEGO_EXTERIOR, OUTPUT);
    pinMode(RIEGO_INTERIOR, OUTPUT);

    webSocketSensors.begin();
    webSocketSensors.onEvent(webSocketEventSensors);

    webSocketInterior.begin();
    webSocketInterior.onEvent(webSocketEventInterior);

    webSocketExterior.begin();
    webSocketExterior.onEvent(webSocketEventExterior);

    // Maneja las solicitudes GET para la página de inicio
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", String(), false); });
    // Inicia el servidor HTTP
    server.begin();
    Serial.println("Servidor HTTP iniciado");
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

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
        pantallaLCD.intermitenteWifi();
    }
    Serial.println("Connected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  // Realizar la lectura solo si ha pasado el intervalo de tiempo
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
  {
    ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
    webSocketSensors.loop();
  }
    webSocketInterior.loop();
    webSocketExterior.loop();
}

void readBPM280()
{
    // LECTURA DE SENSORES
    float temperature = bmp.readTemperature();
    float pressure = bmp.readPressure();
    float altitude = bmp.readAltitude(1013.25);
    
    // ENVÍO DE DATOS A LOS CLIENTES
    String temperatureMsg = "Temperatura: " + String(temperature) + " C";
    String pressureMsg = "Presion: " + String(pressure) + " Pa";
    String altitudeMsg = "Altitud: " + String(altitude) + " m";

    webSocketSensors.broadcastTXT(temperatureMsg);
    webSocketSensors.broadcastTXT(pressureMsg);
    webSocketSensors.broadcastTXT(altitudeMsg);
}

