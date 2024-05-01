#include "config.h"
#include "Pantalla.h"
#include "time.h"
#include <Bounce2.h>
#include "wifi_config.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

ConfigManager config;
WifiConfig wifiConfig;

#define INTERVALODELECTURA 1000 // INTERVALO DE ACTULIZACIÓN
#define BMP280_ADDR 0x76        // DIRECCIÓN DEL SENSOR BMP280
// VARIABLES DE ENTRADA
#define OK 33     // PIN PARA EL BOTÓN DE OK
#define UP 14     // PIN PARA EL BOTÓN DE UP
#define DOWN 15   // PIN PARA EL BOTÓN DE DOWN
#define LEFT 12   // PIN PARA EL BOTÓN DE LEFT
#define RIGHT 32  // PIN PARA EL BOTÓN DE DERECHA
#define BTN_K1 22 // PIN PARA EL BOTÓN DE K1
#define BTN_K2 13 // PIN PARA EL BOTÓN DE K2
#define BTN_K3 27 // PIN PARA EL BOTÓN DE K3
#define BTN_K4 26 // PIN PARA EL BOTÓN DE K4
#define RESET 33  // PIN PARA EL BOTÓN DE RESET

// VARIABLES DE SALIDA
#define K1 21 // relay PARA EL RIEGO EXTERIOR
#define K2 19 // relay PARA EL RIEGO INTERIOR
#define K3 18 // relay PARA UNA SALIDA LIBRE
#define K4 5  // relay PARA UNA SALIDA LIBRE

// CREAR OBJETOS DE LA CLASE BOUNCE PARA CADA BOTÓN
Bounce btnOk = Bounce();
Bounce btnUp = Bounce();
Bounce btnDown = Bounce();
Bounce btnLeft = Bounce();
Bounce btnRight = Bounce();
Bounce btnK1 = Bounce();
Bounce btnK2 = Bounce();
Bounce btnK3 = Bounce();
Bounce btnK4 = Bounce();
Bounce btnReset = Bounce(); // BOTÓN DE RESET

Pantalla displayLCD(LCD_ADDR, SDA, SCL);
Adafruit_BMP280 bmp;

WebSocketsServer webSocketConfig = WebSocketsServer(81);
AsyncWebServer server(80); // Objeto del servidor HTTP

// ESTADOS DEL RIEGO
enum RelayState
{
  INACTIVE,
  WAITING,
  ACTIVE,
  PAUSED,
  OFF
};

// ESTABLECER EL ESTADO INICIAL DE CADA RIEGO
RelayState K1State = INACTIVE;
RelayState K2State = INACTIVE;
RelayState K3State = INACTIVE;
RelayState K4State = INACTIVE;
bool K1Timer = false;
bool K2Timer = false;
bool K3Timer = false;
bool K4Timer = false;
unsigned long remainingTimeK1 = 0;
unsigned long remainingTimeK2 = 0;
unsigned long remainingTimeK3 = 0;
unsigned long remainingTimeK4 = 0;
unsigned long selectedTimeK1 = 0;
unsigned long selectedTimeK2 = 0;
unsigned long selectedTimeK3 = 0;
unsigned long selectedTimeK4 = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  SPIFFS.begin();      // Inicializar el sistema de archivos SPIFFS
  config.loadConfig(); // Cargar la configuración al inicio

  //***** CONFIGURACIÓN DE LA PANTALLA LCD
  displayLCD.initDisplay();

  //***** INICIALIZAR LA CONEXIÓN WIFI Y AP
  bool wifiActive = config.getWifiActive(); // Verificar si el WiFi está activado
  bool apActive = config.getApActive();     // Verificar si el AP está activado
  if (wifiActive && apActive)               // Inicializar WiFi y AP
  {
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();
    IPAddress ip = config.getWifiIP();
    IPAddress subnet = config.getWifiSubnet();
    IPAddress gateway = config.getWifiGateway();
    wifiConfig.initWifiPlus(wifiSSID.c_str(), wifiPassword.c_str(), ip, subnet, gateway, apSSID.c_str(), apPassword.c_str());
    displayLCD.printWifi();
    displayLCD.printAp();
  }
  else if (wifiActive && !apActive) // Inicializar WiFi
  {
    String wifiSSID = config.getWifiSSID();
    String wifiPassword = config.getWifiPassword();
    IPAddress ip = config.getWifiIP();
    IPAddress subnet = config.getWifiSubnet();
    IPAddress gateway = config.getWifiGateway();
    wifiConfig.initWifi(wifiSSID.c_str(), wifiPassword.c_str(), ip, gateway, subnet);
    displayLCD.printWifi();
  }
  else if (!wifiActive && apActive) // Inicializar AP
  {
    String apSSID = config.getApSSID();
    String apPassword = config.getApPassword();
    wifiConfig.initAP(apSSID.c_str(), apPassword.c_str());
    IPAddress ipAddress = wifiConfig.getAPIP();
    displayLCD.printAp();
  }

  //***** CONFIGURAR EL CANAL WEBSOCKET PARA LA CONFIGURACIÓN
  webSocketConfig.begin();
  webSocketConfig.onEvent(webSocketEventConfig);
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/");

  //***** INICIAR EL SERVIDOR HTTP
  server.begin();
  Serial.println("Servidor HTTP iniciado");

  //***** SENSOR BMP280
  if (!bmp.begin(BMP280_ADDR))
  {
    Serial.println("No se ha detectado el sensor BMP280");
  }
  else
  {
    Serial.println("Sensor BMP280 detectado");
  }

  //***** CONFIGURACIÓN DE LOS BOTONES DE ENTRADA
  btnOk.attach(OK, INPUT_PULLUP);         // RESET COMO PULLUP
  btnUp.attach(UP, INPUT_PULLDOWN);       // UP COMO PULLDOWN
  btnDown.attach(DOWN, INPUT_PULLDOWN);   // DOWN COMO PULLDOWN
  btnLeft.attach(LEFT, INPUT_PULLDOWN);   // LEFT COMO PULLDOWN
  btnRight.attach(RIGHT, INPUT_PULLDOWN); // DERECHA COMO PULLDOWN
  btnK1.attach(BTN_K1, INPUT_PULLDOWN);   // ENTRADA BOTON K1 COMO PULLDOWN
  btnK2.attach(BTN_K2, INPUT_PULLDOWN);   // ENTRADA BOTON K2 COMO PULLDOWN
  btnK3.attach(BTN_K3, INPUT_PULLDOWN);   // ENTRADA BOTON K3 COMO PULLDOWN
  btnK4.attach(BTN_K4, INPUT_PULLDOWN);   // ENTRADA BOTON K4 COMO PULLDOWN
  btnReset.attach(RESET, INPUT_PULLUP);   // ENTRADA BOTON RESET COMO PULLUP

  //***** ESTABLECER EL TIEMPO DE REBOTE DE CADA BOTÓN
  btnOk.interval(20);
  btnUp.interval(10);
  btnDown.interval(20);
  btnLeft.interval(20);
  btnRight.interval(20);
  btnK1.interval(20);
  btnK2.interval(20);
  btnK3.interval(20);
  btnK4.interval(20);
  btnReset.interval(20);

  //***** CONFIGURACIÓN DE LOS RELÉS DE SALIDA Y ESTABLECERLOS EN BAJO
  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  digitalWrite(K1, LOW);
  digitalWrite(K2, LOW);
  digitalWrite(K3, LOW);
  digitalWrite(K4, LOW);

  //***** CONFIGURAR LA HORA CON UN SERVIDOR NTP
  configTime(3600, 0, "pool.ntp.org");
}

// Función para manejar los eventos del WebSocket de configuración
void webSocketEventConfig(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_TEXT)
  {
    String message = String((char *)payload);
    DynamicJsonDocument data(1024);
    deserializeJson(data, message);

    String action = data["action"];
    //****CONFIGURACIÓN WIFI Y AP****//
    if (action == "getConfig") // Enviar la configuracion al cliente
    {                          //
      updateClientConfig();
    }

    else if (action == "getNetworks") // Buscar redes WiFi
    {
      Serial.println("Buscando redes WiFi");
      String networksJson = wifiConfig.searchNetworks();
      webSocketConfig.sendTXT(num, networksJson);
    }

    else if (action == "setWifiConfig") // ACTIVAR O DESACTIVAR WIFI
    {
      bool wifiActive = data["wifiActive"];
      if (!wifiActive)
      {
        config.setWifiActive(false);
        config.setWifiStatus(false);
        config.setWifiSSID("");
        config.setWifiPassword("");
        config.setWifiIP("");
        config.setWifiSubnet("");
        config.setWifiGateway("");
        wifiConfig.disconnectWifi();
      }
      else
      {
        String ssid = data["ssid"];
        String password = data["password"];
        String ipConfig = data["ip"];
        String subnetConfig = data["subnet"];
        String gatewayConfig = data["gateway"];

        IPAddress ip;
        IPAddress subnet;
        IPAddress gateway;

        wifiConfig.initWifi(ssid.c_str(), password.c_str(), ip, gateway, subnet);

        bool WifiConnection = wifiConfig.verifyWifiConnection(); // Verificar la conexión
        if (WifiConnection)
        {
          config.setWifiActive(true);
          config.setWifiStatus(true);
          config.setWifiSSID(ssid);
          config.setWifiPassword(password);
          IPAddress ip = wifiConfig.getIPAddress();
          IPAddress subnet = wifiConfig.getSubnetMask();
          IPAddress gateway = wifiConfig.getGatewayIP();
          config.setWifiIP(ip.toString());
          config.setWifiSubnet(subnet.toString());
          config.setWifiGateway(gateway.toString());
          String message = "{ \"status\": \"true\", \"ip\": \"" + ip.toString() + "\", \"subnet\": \"" + subnet.toString() + "\", \"gateway\": \"" + gateway.toString() + "\" }";
          webSocketConfig.sendTXT(num, message);
          Serial.print("IP Wifi: ");
          Serial.println(ip);
        }

        else
        {
          webSocketConfig.sendTXT(num, "false");
        }
      }
    }

    else if (action == "setApConfig") // Configurar AP
    {
      String active = data["active"];
      if (active == "true")
      {
        String ssid = config.getApSSID();
        String password = config.getApPassword();
        wifiConfig.initAP(ssid.c_str(), password.c_str());
        config.setApActive(true);
        config.setApStatus(true);
        IPAddress ip = wifiConfig.getAPIP();
      }
      else if (active == "false")
      {
        config.setApActive(false);
        config.setApStatus(false);
        wifiConfig.stopAP();
      }
    }

    //****CONFIGURACIÓN RELÉS****//
    if (action == "setRelayActive") // Configurar relés
    {
      String relay = data["relay"];

      if (relay == "K1")
      {
        bool K1Active = data["K1Active"];
        config.setK1Active(K1Active);
      }
      else if (relay == "K2")
      {
        bool K2Active = data["K2Active"];
        config.setK2Active(K2Active);
      }
      else if (relay == "K3")
      {
        bool K3Active = data["K3Active"];
        config.setK3Active(K3Active);
      }
      else if (relay == "K4")
      {
        bool K4Active = data["K4Active"];
        config.setK4Active(K4Active);
      }
    }
    else if (action == "setRelayName")
    {
      String relay = data["relay"];
      String relayName = data["relayName"];
      if (relay == "K1")
      {
        config.setK1Name(relayName);
      }
      else if (relay == "K2")
      {
        config.setK2Name(relayName);
      }
      else if (relay == "K3")
      {
        config.setK3Name(relayName);
      }
      else if (relay == "K4")
      {
        config.setK4Name(relayName);
      }
    }
  }
  else if (type == WStype_CONNECTED)
  {
    Serial.println("Cliente conectado");
  }
  else if (type == WStype_DISCONNECTED)
  {
    Serial.println("Cliente desconectado");
  }
}

void updateClientConfig()
{
  // Enviar la configuración actual al cliente
  String configJson;
  serializeJson(config.getConfigDoc(), configJson);
  webSocketConfig.broadcastTXT(configJson);
  Serial.println("Enviando configuración al cliente");
}
void sendMessage(String message)
{
  webSocketConfig.broadcastTXT(message);
}
void loop()
{
  webSocketConfig.loop();
  // RETARDO DE 1 SEGUNDO
  unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA)
  {
    ultimoTiempo = tiempoActual; // Actualizar el tiempo de la última lectura
                                 //*** ***//
    getTime();                   // ACTUALIZAR LA HORA
    getTemperature();            // ACTUALIZAR LA TEMPERATURA
  }
  // ACTUALIZAR EL ESTADO DE CADA BOTÓN
  btnOk.update();
  btnUp.update();
  btnDown.update();
  btnLeft.update();
  btnRight.update();
  btnK1.update();
  btnK2.update();
  btnK3.update();
  btnK4.update();
  btnReset.update();

  if (btnOk.fell()) // CONFIRMACIÓN DE LA INSERCION DEL TIEMPO DE RIEGO MANUAL
  {
    if (K1Timer == 1)
    {
      K1Timer = 0;
      remainingTimeK1 = selectedTimeK1;
      Serial.print("Tiempo seleccionado en K1: ");
      Serial.println(selectedTimeK1);
    }
    else if (K2Timer == 1)
    {
      K2Timer = 0;
      remainingTimeK2 = selectedTimeK2;
      Serial.print("Tiempo seleccionado en K2: ");
      Serial.println(selectedTimeK2);
    }
    else if (K3Timer == 1)
    {
      K3Timer = 0;
      remainingTimeK3 = selectedTimeK3;
      Serial.print("Tiempo seleccionado en K3: ");
      Serial.println(selectedTimeK3);
    }
    else if (K4Timer == 1)
    {
      K4Timer = 0;
      remainingTimeK4 = selectedTimeK4;
      Serial.print("Tiempo seleccionado en K4: ");
      Serial.println(selectedTimeK4);
    }
  }

  if (btnUp.fell()) // BOTON UP AUMENTA EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    if (K1Timer == 1)
    {
      selectedTimeK1 += 60000;
    }
    else if (K2Timer == 1)
    {
      selectedTimeK2 += 60000;
    }
    else if (K3Timer == 1)
    {
      selectedTimeK3 += 60000;
    }
    else if (K4Timer == 1)
    {
      selectedTimeK4 += 60000;
    }
  }

  if (btnDown.fell()) // BOTON DOWN DISMINUYE EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    // Botón DOWN presionado
    if (K1Timer == 1)
    {
      selectedTimeK1 -= 60000;
      Serial.print("Tiempo seleccionado K1: ");
      Serial.println(selectedTimeK1);
    }
    else if (K2Timer == 1)
    {
      selectedTimeK2 -= 60000;
      Serial.print("Tiempo seleccionado K2: ");
      Serial.println(selectedTimeK2);
    }
    else if (K3Timer == 1)
    {
      selectedTimeK3 -= 60000;
      Serial.print("Tiempo seleccionado K3: ");
      Serial.println(selectedTimeK3);
    }
    else if (K4Timer == 1)
    {
      selectedTimeK4 -= 60000;
      Serial.print("Tiempo seleccionado K4: ");
      Serial.println(selectedTimeK4);
    }
  }

  if (btnLeft.fell()) // BOTON LEFT ESTÁ INACTIVO
  {
    // Botón LEFT presionado
    Serial.println("Botón LEFT presionado");
  }

  if (btnRight.fell()) // BOTON DERECHA ESTÁ INACTIVO
  {
    // Botón DERECHA presionado
    Serial.println("Botón DERECHA presionado");
  }

  if (btnReset.rose()) // BOTÓN DE RESET
  {
    reset();
  }
  else if (btnK1.read() == HIGH && btnK1.currentDuration() == 10000) // RESTABLECER LA CONFIGURACIÓN DE FÁBRICA DESPUÉS DE 10 SEGUNDOS
  {
    factoryReset();
  }

  if (btnK1.fell()) // BOTÓN K1
  {
    switch (K1State)
    {
    case INACTIVE:
      setTimeRelay("K1");
      break;
    case WAITING:
      relayStart(K1, K1State);
      break;
    case ACTIVE:
      relayPause(K1, K1State);
      break;
    case PAUSED:
      relayContinue(K1, K1State);
      break;
    case OFF:
      K1State = INACTIVE;
      break;
    }
  }
  else if (btnK1.read() == HIGH && btnK1.currentDuration() == 3000) // APAGAR K1 DESPUÉS DE 3 SEGUNDOS
  {
    relayStop(K1, K1State);
  }

  if (btnK2.fell()) // BOTÓN K2
  {
    switch (K2State)
    {
    case INACTIVE:
      setTimeRelay("K2");
      break;
    case WAITING:
      relayStart(K2, K2State);
      break;
    case ACTIVE:
      relayPause(K2, K2State);
      break;
    case PAUSED:
      relayContinue(K2, K2State);
      break;
    case OFF:
      K2State = INACTIVE;
      break;
    }
  }
  else if (btnK2.read() == HIGH && btnK2.currentDuration() == 3000) // APAGAR K2 DESPUÉS DE 3 SEGUNDOS
  {
    relayStop(K2, K2State);
  }
}

// FUNCIONES PARA EL MANEJO DE LOS RELÉS
void setTimeRelay(String relay) // ESTABLECER EL TIEMPO DE RIEGO
{
  if (relay == "K1")
  {
    K1Timer = 1;
    K1State = WAITING;
  }
  else if (relay == "K2")
  {
    K2Timer = 1;
    K2State = WAITING;
  }
  else if (relay == "K3")
  {
    K3Timer = 1;
    K3State = WAITING;
  }
  else if (relay == "K4")
  {
    K4Timer = 1;
    K4State = WAITING;
  }
}
void relayStart(int relay, RelayState &state) // ACTIVAR EL RELÉ
{
  digitalWrite(relay, HIGH);
  state = ACTIVE;
}
void relayStop(int relay, RelayState &state) // APAGAR EL RELÉ
{
  digitalWrite(relay, LOW);
  state = OFF;
}
void relayPause(int relay, RelayState &state) // PAUSAR EL RELÉ
{
  digitalWrite(relay, LOW);
  state = PAUSED;
}
void relayContinue(int relay, RelayState &state) // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
{
  digitalWrite(relay, HIGH);
  state = ACTIVE;
}

// FUNCIONES D LOOP
void getTime() // ACTUALIZAR LA HORA Y LA TEMPERATURA EN LA PANTALLA
{
  time_t now = time(nullptr);
  struct tm timeinfo;
  localtime_r(&now, &timeinfo);
  // Formatea la hora para mostrarla en la pantalla LCD
  char timeStr[20];
  strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
  displayLCD.printTime(timeStr);
}

void getTemperature() // OBTENER LA TEMPERATURA DEL SENSOR BMP280
{
  float temperature = bmp.readTemperature();
  displayLCD.printTemp(temperature);
}
// FUNCIONES DEL SISTEMA
void reset() // REINICIAR EL DISPOSITIVO
{
  Serial.println("Reiniciando...");
  ESP.restart();
}

void factoryReset() // RESTABLECER LA CONFIGURACIÓN DE FÁBRICA
{
  Serial.println("Restableciendo la configuración de fábrica...");
  // config.factoryReset();
  // reset();
}
