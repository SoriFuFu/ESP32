#include <WiFi.h>
#include <WebSocketsServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include "time.h"
#include <Bounce2.h>
#include "Pantalla.h"

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

bool wifiConnect = false;

WebSocketsServer webSocketSensors = WebSocketsServer(81);
WebSocketsServer webSocketInterior = WebSocketsServer(82);
WebSocketsServer webSocketExterior = WebSocketsServer(83);

AsyncWebServer server(80); // Objeto del servidor HTTP

void setup()
{
    // INICIALIZAR PUERTO SERIAL
    Serial.begin(115200);
    // INICIALIZAR WIFI
    WiFi.begin(ssid, password);
    // INICIALIZAR PANTALLA LCD
    pantallaLCD.inicializar();
    // INICIALIZAR SPIFFS
    if (!SPIFFS.begin())
    {
        Serial.println("Error al montar SPIFFS");
        return;
    }
    Serial.println("SPIFFS montado correctamente");
    // INICIALIZAR EL SENSOR BMP280
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
    // CONFIGURAR EL CANAL WEBSOCKET PARA LOS SENSORES
    webSocketSensors.begin();
    webSocketSensors.onEvent(webSocketEventSensors);
    // CONFIGURAR EL CANAL WEBSOCKET PARA EL RIEGO INTERIOR
    webSocketInterior.begin();
    webSocketInterior.onEvent(webSocketEventInterior);
    // CONFIGURAR EL CANAL WEBSOCKET PARA EL RIEGO EXTERIOR
    webSocketExterior.begin();
    webSocketExterior.onEvent(webSocketEventExterior);

    // CONFIGURAR EL SERVIDOR HTTP PARA LA PÁGINA WEB
    // server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    //           { request->send(SPIFFS, "/index.html", String(), false); });
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send(SPIFFS, "/index.html", "text/html"); });
    server.serveStatic("/", SPIFFS, "/");
    // INICIAR EL SERVIDOR HTTP
    server.begin();
    Serial.println("Servidor HTTP iniciado");

    // ESPERAR A QUE SE CONECTE A LA RED WIFI
    int intentos = 0;
    while (WiFi.status() != WL_CONNECTED && wifiConnect == false)
    {
        Serial.println("Connecting to WiFi...");
        pantallaLCD.intermitenteWifi();
        pantallaLCD.imprimirTexto("Conectando WiFi...");
        intentos++;

        // Verificar si se han realizado 10 intentos
        if (intentos == 10)
        {
            Serial.println("No se pudo conectar a la red WiFi");
            pantallaLCD.imprimirTexto("Error de conexión WiFi");
            break; // Salir del bucle si se alcanza el límite de intentos
        }
    }

    // Si se estableció la conexión, imprimir la información de la conexión
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Conectado a WiFi");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        pantallaLCD.imprimirTexto("Conectado a WiFi");
        char ip[20];
        sprintf(ip, "IP: %s", WiFi.localIP().toString().c_str());
        pantallaLCD.imprimirTexto(ip);
        // IMPIMIMOS LA PANTALLA DE INICIO
        pantallaLCD.panelRiego();
        pantallaLCD.imprimirWifi();
        pantallaLCD.MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
        pantallaLCD.MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
    }

    // CONFIGURAR LA HORA CON UN SERVIDOR NTP
    configTime(3600, 0, "pool.ntp.org");

    sendExteriorState();
    sendInteriorState();
}

// MANEJO DE LOS EVENTOS DE LOS WEBSOCKETS
void webSocketEventSensors(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_TEXT)
    {
        String mensaje = String((char *)payload);
        Serial.print("Sensores:");
        Serial.println(mensaje);
    }
}

void webSocketEventInterior(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_TEXT)
    {
        String message = String((char *)payload);
        if (message == "1")
        {
            manejoRiegoInterior();
        }else if (message == "APAGADO")
        {
            apagarRiegoInt();
        }
    }
}

void webSocketEventExterior(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
    if (type == WStype_TEXT)
    {
        String message = String((char *)payload);
        if (message == "1")
        {
            manejoRiegoExterior();
        }else if (message == "APAGADO")
        {
            apagarRiegoExt();
        }
        
    }
}



// ENVIO DE MENSAJES A LOS CLIENTES

void sendSensorData()
{
    // Enviar datos de los sensores a los clientes
    float temperature = bmp.readTemperature();
    String temperatureMsg = "Temperature:" + String(temperature);
    webSocketSensors.broadcastTXT(temperatureMsg);
}

void sendInteriorState()
{
    String state;
    switch (estadoRiegoInterior)
    {
    case INACTIVO:
        state = "INACTIVO";
        break;
    case ESPERA:
        state = "ESPERA";
        break;
    case ACTIVO:
        state = "ACTIVO";
        break;
    case PAUSA:
        state = "PAUSA";
        break;
    case APAGADO:
        state = "APAGADO";
        break;
    default:
        state = "DESCONOCIDO";
        break;
    }
    webSocketInterior.broadcastTXT(state);
}

void sendExteriorState()
{
    String state;
    switch (estadoRiegoExterior)
    {
    case INACTIVO:
        state = "INACTIVO";
        break;
    case ESPERA:
        state = "ESPERA";
        break;
    case ACTIVO:
        state = "ACTIVO";
        break;
    case PAUSA:
        state = "PAUSA";
        break;
    case APAGADO:
        state = "APAGADO";
        break;
    default:
        state = "DESCONOCIDO";
        break;
    }
    webSocketExterior.broadcastTXT(state);
}

void loop()
{
    // RETARDO DE 1 SEGUNDO
    unsigned long tiempoActual = millis();
    static unsigned long ultimoTiempo = 0;
    if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
    {
        ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
        readBPM280();
        horaActual();
        temperaturaActual();
        // MOSTRAR CONTADOR DE RIEGO EXTERIOR
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
        // MOSTRAR CONTADOR DE RIEGO INTERIOR
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

    // RETARDO DE 1/2 SEGUNDO
    static bool state = false; // Declaración estática de state
    static unsigned long repeticion500 = 0;
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
        }
    }

    // GESTIONAR LOS EVENTOS DE LOS WEBSOCKETS

    webSocketSensors.loop();
    webSocketInterior.loop();
    webSocketExterior.loop();

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
        manejoRiegoExterior();
    }
    else if (btnExterior.read() == HIGH && btnExterior.currentDuration() == 3000)
    {
        apagarRiegoExt();
    }

    // RIEGO INTERIOR
    if (btnInterior.fell())
    {
        manejoRiegoInterior();
    }
    else if (btnInterior.read() == HIGH && btnInterior.currentDuration() == 3000)
    {
        apagarRiegoInt();
    }
}

void horaActual()
{
    // Obtiene la hora actual del RTC
    time_t now = time(nullptr);
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    // Formatea la hora para mostrarla en la pantalla LCD
    char timeStr[20];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    float temperatura = bmp.readTemperature();
    pantallaLCD.imprimirHora(timeStr);
}

void temperaturaActual()
{
    // Obtiene la temperatura actual del sensor BMP280
    float temperatura = bmp.readTemperature();
    pantallaLCD.imprimirTemp(temperatura);
}

void readBPM280()
{
    // LECTURA DE SENSORES
    float temperatura = bmp.readTemperature();
    // ENVÍO DE DATOS A LOS CLIENTES
    String temperatureMsg = "Temperatura:" + String(temperatura);
    webSocketSensors.broadcastTXT(temperatureMsg);
}

// MANEJOS DE LOS RIEGOS
void manejoRiegoExterior()
{
    switch (estadoRiegoExterior)
    {
    case INACTIVO:
        if (estadoRiegoExterior == INACTIVO)
        {
            contadorExterior = 1;
            estadoRiegoExterior = ESPERA;
            sendExteriorState();
        }
        break;
    case ESPERA:
        contadorExterior = 0;
        tiempoRestanteRiegoExterior = tiempoSeleccionadoRiegoExterior;
        digitalWrite(RIEGO_EXTERIOR, HIGH);
        estadoRiegoExterior = ACTIVO;
        sendExteriorState();
        break;
    case ACTIVO:
        digitalWrite(RIEGO_EXTERIOR, LOW);
        estadoRiegoExterior = PAUSA;
        sendExteriorState();
        break;
    case PAUSA:
        digitalWrite(RIEGO_EXTERIOR, HIGH);
        estadoRiegoExterior = ACTIVO;
        sendExteriorState();
        break;
    case APAGADO:
        estadoRiegoExterior = INACTIVO;
        sendExteriorState();
        break;
    }
}

void manejoRiegoInterior()
{
    switch (estadoRiegoInterior)
    {
    case INACTIVO:
        if (estadoRiegoInterior == INACTIVO)
        {
            contadorInterior = 1;
            estadoRiegoInterior = ESPERA;
            sendInteriorState();
        }
        break;
    case ESPERA:
        contadorInterior = 0;
        tiempoRestanteRiegoInterior = tiempoSeleccionadoRiegoInterior;
        digitalWrite(RIEGO_INTERIOR, HIGH);
        estadoRiegoInterior = ACTIVO;
        sendInteriorState();
        break;
    case ACTIVO:
        digitalWrite(RIEGO_INTERIOR, LOW);
        estadoRiegoInterior = PAUSA;
        sendInteriorState();
        break;
    case PAUSA:
        digitalWrite(RIEGO_INTERIOR, HIGH);
        estadoRiegoInterior = ACTIVO;
        sendInteriorState();
        break;
    case APAGADO:
        estadoRiegoInterior = INACTIVO;
        sendInteriorState();
        break;
    }
}

void apagarRiegoExt()
{
    digitalWrite(RIEGO_EXTERIOR, LOW);
    estadoRiegoExterior = APAGADO;
    pantallaLCD.MostrarContadorExterior(tiempoSeleccionadoRiegoExterior);
    sendExteriorState();
}

void apagarRiegoInt()
{
    digitalWrite(RIEGO_INTERIOR, LOW);
    estadoRiegoInterior = APAGADO;
    pantallaLCD.MostrarContadorInterior(tiempoSeleccionadoRiegoInterior);
    sendInteriorState();
}