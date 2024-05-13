#include "config.h"
#include "Pantalla.h"
#include "time.h"
#include <Bounce2.h>
#include "wifi_config.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>

ConfigManager config;                                    // OBJETO DE LA CLASE ConfigManager
WifiConfig wifiConfig;                                   // OBJETO DE LA CLASE WifiConfig
Pantalla displayLCD(LCD_ADDR, SDA, SCL);                 // OBJETO DE LA PANTALLA LCD
Adafruit_BMP280 bmp;                                     // OBJETO DEL SENSOR BMP280
WebSocketsServer webSocketConfig = WebSocketsServer(81); // OBJETO DEL SERVIDOR WEBSOCKET PARA LA CONFIGURACIÓN
WebSocketsServer webSocketData = WebSocketsServer(82);   // OBJETO DEL SERVIDOR WEBSOCKET PARA LOS DATOS DE SENSORES Y HORA
WebSocketsServer webSocketK1 = WebSocketsServer(83);     // OBJETO DEL SERVIDOR WEBSOCKET PARA K1
WebSocketsServer webSocketK2 = WebSocketsServer(84);     // OBJETO DEL SERVIDOR WEBSOCKET PARA K2
WebSocketsServer webSocketK3 = WebSocketsServer(85);     // OBJETO DEL SERVIDOR WEBSOCKET PARA K3
WebSocketsServer webSocketK4 = WebSocketsServer(86);     // OBJETO DEL SERVIDOR WEBSOCKET PARA K4
AsyncWebServer server(80);                               // OBJETO DEL SERVIDOR HTTP

#define INTERVALODELECTURA10SEG 10000 // INTERVALO DE ACTULIZACIÓN DE 1 SEGUNDO
#define INTERVALODELECTURA1SEG 1000 // INTERVALO DE ACTULIZACIÓN DE 1 SEGUNDO
#define INTERVALODELECTURA05SEG 500 // INTERVALO DE ACTULIZACIÓN
#define BMP280_ADDR 0x76            // DIRECCIÓN DEL SENSOR BMP280
#define LED 25                      // PIN PARA EL LED
// VARIABLES DE ENTRADA
#define OK 35     // PIN PARA EL BOTÓN DE OK
#define UP 12     // PIN PARA EL BOTÓN DE UP
#define DOWN 32   // PIN PARA EL BOTÓN DE DOWN
#define LEFT 15   // PIN PARA EL BOTÓN DE LEFT
#define RIGHT 14  // PIN PARA EL BOTÓN DE DERECHA
#define BTN_K1 23 // PIN PARA EL BOTÓN DE K1
#define BTN_K2 27 // PIN PARA EL BOTÓN DE K2
#define BTN_K3 36 // PIN PARA EL BOTÓN DE K3
#define BTN_K4 34 // PIN PARA EL BOTÓN DE K4
#define RESET 33  // PIN PARA EL BOTÓN DE RESET
#define D1 22     // PIN PARA EL BOTÓN DATA 1
#define D2 26     // PIN PARA EL BOTÓN DATA 2
#define D3 13     // PIN PARA EL BOTÓN DATA 3

// VARIABLES DE SALIDA
#define K1 21 // RELÉ K1
#define K2 19 // RELÉ K2
#define K3 18 // RELÉ K3
#define K4 5  // RELÉ K4

// CREAR OBJETOS DE LA CLASE BOUNCE PARA CADA BOTÓN
Bounce btnOk = Bounce();    // OK
Bounce btnUp = Bounce();    // ARRIBA
Bounce btnDown = Bounce();  // ABAJO
Bounce btnLeft = Bounce();  // IZQUIERDA
Bounce btnRight = Bounce(); // DERECHA
Bounce btnK1 = Bounce();    // BOTÓN DE K1
Bounce btnK2 = Bounce();    // BOTÓN DE K2
Bounce btnK3 = Bounce();    // BOTÓN DE K3
Bounce btnK4 = Bounce();    // BOTÓN DE K4
Bounce btnReset = Bounce(); // BOTÓN DE RESET

// ESTADOS DEL RIEGO
enum RelayState
{
  INACTIVE,
  WAITING,
  ACTIVE,
  PAUSE,
  ON,
  OFF
};

// ESTABLECER EL ESTADO INICIAL DE CADA RIEGO
RelayState K1State = INACTIVE;
RelayState K2State = INACTIVE;
RelayState K3State = INACTIVE;
RelayState K4State = INACTIVE;

// VARIABLES PARA EL CONTADOR APAGADO/ENCENDIDO
bool K1Timer = false;
bool K2Timer = false;
bool K3Timer = false;
bool K4Timer = false;

bool stateK1Timer = false;
bool stateK2Timer = false;
bool stateK3Timer = false;
bool stateK4Timer = false;
// VARIABLES PARA EL TIEMPO RESTANTE
unsigned long remainingTimeK1 = 0;
unsigned long remainingTimeK2 = 0;
unsigned long remainingTimeK3 = 0;
unsigned long remainingTimeK4 = 0;

int menuPage = 1; // PÁGINA DEL DISPLAY

void getConfigData()
{
  remainingTimeK1 = config.getK1TimerSelected();
  remainingTimeK2 = config.getK2TimerSelected();
  remainingTimeK3 = config.getK3TimerSelected();
  remainingTimeK4 = config.getK4TimerSelected();
  config.setK1Timer(remainingTimeK1);
  config.setK2Timer(remainingTimeK2);
  config.setK3Timer(remainingTimeK3);
  config.setK4Timer(remainingTimeK4);
  LCD_UpdateK1Timer();
  LCD_UpdateK2Timer();
  LCD_UpdateK3Timer();
  LCD_UpdateK4Timer();
}

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
    wifiConfig.initWifi(wifiSSID.c_str(), wifiPassword.c_str(), ip, subnet, gateway);
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
  webSocketData.begin();
  webSocketK1.begin();
  webSocketK2.begin();
  webSocketK3.begin();
  webSocketK4.begin();

  //***** CONFIGURAR LOS EVENTOS DEL WEBSOCKET
  webSocketConfig.onEvent(webSocketEventConfig);
  webSocketData.onEvent(webSocketEventData);
  webSocketK1.onEvent(webSocketEventK1);
  webSocketK2.onEvent(webSocketEventK2);
  webSocketK3.onEvent(webSocketEventK3);
  webSocketK4.onEvent(webSocketEventK4);

  //***** CONFIGURAR EL SERVIDOR HTTP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.onNotFound([](AsyncWebServerRequest *request)
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
  btnOk.attach(OK, INPUT_PULLDOWN);       // RESET COMO PULLUP
  btnUp.attach(UP, INPUT_PULLDOWN);       // UP COMO PULLDOWN
  btnDown.attach(DOWN, INPUT_PULLDOWN);   // DOWN COMO PULLDOWN
  btnLeft.attach(LEFT, INPUT_PULLDOWN);   // LEFT COMO PULLDOWN
  btnRight.attach(RIGHT, INPUT_PULLDOWN); // DERECHA COMO PULLDOWN
  btnK1.attach(BTN_K1, INPUT_PULLDOWN);   // ENTRADA BOTON K1 COMO PULLDOWN
  btnK2.attach(BTN_K2, INPUT_PULLDOWN);   // ENTRADA BOTON K2 COMO PULLDOWN
  btnK3.attach(BTN_K3, INPUT_PULLDOWN);   // ENTRADA BOTON K3 COMO PULLDOWN
  btnK4.attach(BTN_K4, INPUT_PULLDOWN);   // ENTRADA BOTON K4 COMO PULLDOWN
  btnReset.attach(RESET, INPUT_PULLDOWN); // ENTRADA BOTON RESET COMO PULLUP

  //***** ESTABLECER EL TIEMPO DE REBOTE DE CADA BOTÓN
  btnOk.interval(50);
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
  pinMode(LED, OUTPUT);
  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  digitalWrite(LED, HIGH);
  digitalWrite(K1, LOW);
  digitalWrite(K2, LOW);
  digitalWrite(K3, LOW);
  digitalWrite(K4, LOW);
  config.setK1State("INACTIVE");
  config.setK2State("INACTIVE");
  config.setK3State("INACTIVE");
  config.setK4State("INACTIVE");

  //***** CONFIGURAR LA HORA CON UN SERVIDOR NTP
  configTime(3600, 0, "pool.ntp.org");

  setMenuPage();
  getConfigData();

  config.setMacAddress(wifiConfig.getMACAddress());


}

//***** FUNCIONES PARA EL WEB SERVER
void updateClientConfig() // ACTUALIZAR LA CONFIGURACIÓN DEL CLIENTE
{
  // Enviar la configuración actual al cliente
  String configJson;
  serializeJson(config.getConfigDoc(), configJson);
  webSocketConfig.broadcastTXT(configJson);
}

void sendRemainingTime(long remainingTime, WebSocketsServer &webSocket) // ENVIAR EL TIEMPO RESTANTE
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["action"] = "UPDATE_TIMER";
  jsonDocument["time"] = remainingTime;

  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocket.broadcastTXT(jsonString);
}
void sendTimerStatus(bool timerStatus, WebSocketsServer &webSocket) // ENVIAR EL ESTADO DEL TEMPORIZADOR
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["action"] = "UPDATE_TIMER_STATUS";
  jsonDocument["status"] = timerStatus;

  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocket.broadcastTXT(jsonString);
}
void sendRelayStatus(String status, WebSocketsServer &webSocket) // ENVIAR EL ESTADO DEL RELÉ
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["action"] = status;
  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocket.broadcastTXT(jsonString);
}
void sendData() // ENVIAR LA SEÑAL DE WIFI
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["action"] = "UPDATE_WIFI_RSSI";
  jsonDocument["rssi"] = wifiConfig.getWifiSignal();
  jsonDocument["temperature"] = bmp.readTemperature();

  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocketData.broadcastTXT(jsonString);
}

void webSocketEventConfig(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE CONFIGURACIÓN
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
}

void webSocketEventData(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE DATOS
{
  if (type == WStype_CONNECTED)
  {
    Serial.println("Cliente conectado");
  }
}

void webSocketEventK1(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE K1
{
  if (type == WStype_CONNECTED)
  {
    // Serial.println("Cliente K1 conectado");
  }
  if (type == WStype_TEXT)
  {
    String message = String((char *)payload); // RECIBIR EL MENSAJE DEL CLIENTE
    DynamicJsonDocument data(1024);           // CREAR UN DOCUMENTO JSON
    deserializeJson(data, message);           // DESERIALIZAR EL MENSAJE
    String action = data["action"];           // RECIBIR LA ACCIÓN
    Serial.print("Acción K1: ");
    Serial.println(action);

    if (action == "SET_TIMER")
    {
      bool timerStatus = data["timerStatus"]; // RECIBIR EL ESTADO DEL TEMPORIZADOR
      if (timerStatus == false)
      {
        remainingTimeK1 = 0;                             // ESTABLECER EL TIEMPO RESTANTE EN 0
        config.setK1Timer(remainingTimeK1);              // GUARDAR EL TIEMPO RESTANTE
        config.setK1TimerSelected(remainingTimeK1);      // GUARDAR EL TIEMPO SELECCIONADO
        config.setK1TimerStatus(timerStatus);            // GUARDAR EL ESTADO DEL TEMPORIZADOR
        sendRemainingTime(remainingTimeK1, webSocketK1); // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        LCD_UpdateK1Timer();                             // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K1State = INACTIVE;                              // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN INACTIVO
      }
      else if (timerStatus == true)
      {
        unsigned long selectedTime = data["selectedTime"]; // RECIBIR EL TIEMPO SELECCIONADO
        config.setK1TimerSelected(selectedTime);           // RECIBIR EL TIEMPO SELECCIONADO
        remainingTimeK1 = selectedTime;                    // RECIBIR EL TIEMPO SELECCIONADO
        config.setK1TimerStatus(timerStatus);              // GUARDAR EL ESTADO DEL TEMPORIZADOR
        config.setK1Timer(selectedTime);                   // GUARDAR EL TIEMPO RESTANTE
        sendRemainingTime(remainingTimeK1, webSocketK1);   // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        sendTimerStatus(timerStatus, webSocketK1);
        LCD_UpdateK1Timer(); // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K1State = WAITING;
      }
    }
    else if (action == "ON") // ENCENDER EL RELÉ
    {
      relayOn(K1, K1State);
      config.setK1State("ON");
      sendRelayStatus("ON", webSocketK1);
    }
    else if (action == "OFF") // APAGAR EL RELÉ
    {
      relayStop(K1, K1State);
      config.setK1State("INACTIVE");
      K1State = INACTIVE;
      sendRelayStatus("OFF", webSocketK1);
    }
    else if (action == "INACTIVE") // ESTABLECER EL ESTADO DEL RELÉ EN INACTIVO
    {
      relayStop(K1, K1State);
      config.setK1State("INACTIVE");
      remainingTimeK1 = config.getK1TimerSelected();
      LCD_UpdateK1Timer();
      sendRelayStatus("INACTIVE", webSocketK1);
      sendRemainingTime(remainingTimeK1, webSocketK1);
    }
    else if (action == "ACTIVE") // ACTIVAR EL RELÉ
    {
      relayStart(K1, K1State);
      config.setK1State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK1);
    }
    else if (action == "PAUSE") // PAUSAR EL RELÉ
    {
      relayPause(K1, K1State);
      config.setK1State("PAUSE");
      sendRelayStatus("PAUSE", webSocketK1);
    }
    else if (action == "CONTINUE") // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
    {
      relayContinue(K1, K1State);
      config.setK1State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK1);
    }
  }
  // else if (type == WStype_DISCONNECTED)
  // {
  //   Serial.println("Cliente K3 desconectado");
  // }
}

void webSocketEventK2(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE K2
{
  if (type == WStype_CONNECTED)
  {
    // Serial.println("Cliente K2 conectado");
  }

  if (type == WStype_TEXT)
  {
    String message = String((char *)payload); // RECIBIR EL MENSAJE DEL CLIENTE
    DynamicJsonDocument data(1024);           // CREAR UN DOCUMENTO JSON
    deserializeJson(data, message);           // DESERIALIZAR EL MENSAJE
    String action = data["action"];           // RECIBIR LA ACCIÓN

    if (action == "SET_TIMER")
    {
      bool timerStatus = data["timerStatus"]; // RECIBIR EL ESTADO DEL TEMPORIZADOR
      if (timerStatus == false)
      {
        remainingTimeK2 = 0;                             // ESTABLECER EL TIEMPO RESTANTE EN 0
        config.setK2Timer(remainingTimeK2);              // GUARDAR EL TIEMPO RESTANTE
        config.setK2TimerSelected(remainingTimeK2);      // GUARDAR EL TIEMPO SELECCIONADO
        config.setK2TimerStatus(timerStatus);            // GUARDAR EL ESTADO DEL TEMPORIZADOR
        sendRemainingTime(remainingTimeK2, webSocketK2); // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        LCD_UpdateK2Timer();                             // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K2State = INACTIVE;                              // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN INACTIVO
      }
      else if (timerStatus == true)
      {
        unsigned long selectedTime = data["selectedTime"]; // RECIBIR EL TIEMPO SELECCIONADO
        config.setK2TimerSelected(selectedTime);           // GUARDAR EL TIEMPO SELECCIONADO
        remainingTimeK2 = selectedTime;                    // ESTABLECER EL TIEMPO RESTANTE
        config.setK2TimerStatus(timerStatus);              // GUARDAR EL ESTADO DEL TEMPORIZADOR
        config.setK2Timer(selectedTime);                   // GUARDAR EL TIEMPO RESTANTE
        sendRemainingTime(remainingTimeK2, webSocketK2);   // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        sendTimerStatus(timerStatus, webSocketK2);         // ENVIAR EL ESTADO DEL TEMPORIZADOR AL CLIENTE
        LCD_UpdateK2Timer();                               // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K2State = WAITING;                                 // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN ESPERA
      }
    }
    else if (action == "ON") // ENCENDER EL RELÉ
    {
      relayOn(K2, K2State);
      config.setK2State("ON");
      sendRelayStatus("ON", webSocketK2);
    }
    else if (action == "OFF") // APAGAR EL RELÉ
    {
      relayStop(K2, K2State);
      config.setK2State("INACTIVE");
      K2State = INACTIVE;
      sendRelayStatus("OFF", webSocketK2);
    }
    else if (action == "INACTIVE") // ESTABLECER EL ESTADO DEL RELÉ EN INACTIVO
    {
      relayStop(K2, K2State);
      config.setK2State("INACTIVE");
      remainingTimeK2 = config.getK2TimerSelected();
      LCD_UpdateK2Timer();
      sendRelayStatus("INACTIVE", webSocketK2);
      sendRemainingTime(remainingTimeK2, webSocketK2);
    }
    else if (action == "ACTIVE") // ACTIVAR EL RELÉ
    {
      relayStart(K2, K2State);
      config.setK2State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK2);
    }
    else if (action == "PAUSE") // PAUSAR EL RELÉ
    {
      relayPause(K2, K2State);
      config.setK2State("PAUSE");
      sendRelayStatus("PAUSE", webSocketK2);
    }
    else if (action == "CONTINUE") // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
    {
      relayContinue(K2, K2State);
      config.setK2State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK2);
    }
  }
}

void webSocketEventK3(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE K3
{
  if (type == WStype_CONNECTED)
  {
    // Serial.println("Cliente K3 conectado");
  }

  if (type == WStype_TEXT)
  {
    String message = String((char *)payload); // RECIBIR EL MENSAJE DEL CLIENTE
    DynamicJsonDocument data(1024);           // CREAR UN DOCUMENTO JSON
    deserializeJson(data, message);
    String action = data["action"];

    if (action == "SET_TIMER")
    {
      bool timerStatus = data["timerStatus"]; // RECIBIR EL ESTADO DEL TEMPORIZADOR
      if (timerStatus == false)
      {
        remainingTimeK3 = 0;                             // ESTABLECER EL TIEMPO RESTANTE EN 0
        config.setK3Timer(remainingTimeK3);              // GUARDAR EL TIEMPO RESTANTE
        config.setK3TimerSelected(remainingTimeK3);      // GUARDAR EL TIEMPO SELECCIONADO
        config.setK3TimerStatus(timerStatus);            // GUARDAR EL ESTADO DEL TEMPORIZADOR
        sendRemainingTime(remainingTimeK3, webSocketK3); // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        LCD_UpdateK3Timer();                             // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K3State = INACTIVE;                              // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN INACTIVO
      }
      else if (timerStatus == true)
      {
        unsigned long selectedTime = data["selectedTime"]; // RECIBIR EL TIEMPO SELECCIONADO
        config.setK3TimerSelected(selectedTime);           // GUARDAR EL TIEMPO SELECCIONADO
        remainingTimeK3 = selectedTime;                    // ESTABLECER EL TIEMPO RESTANTE
        config.setK3TimerStatus(timerStatus);              // GUARDAR EL ESTADO DEL TEMPORIZADOR
        config.setK3Timer(selectedTime);                   // GUARDAR EL TIEMPO RESTANTE
        sendRemainingTime(remainingTimeK3, webSocketK3);   // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        sendTimerStatus(timerStatus, webSocketK3);         // ENVIAR EL ESTADO DEL TEMPORIZADOR AL CLIENTE
        LCD_UpdateK3Timer();                               // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K3State = WAITING;                                 // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN ESPERA
      }
    }
    else if (action == "ON") // ENCENDER EL RELÉ
    {
      relayOn(K3, K3State);
      config.setK3State("ON");
      sendRelayStatus("ON", webSocketK3);
    }
    else if (action == "OFF") // APAGAR EL RELÉ
    {
      relayStop(K3, K3State);
      config.setK3State("INACTIVE");
      K3State = INACTIVE;
      sendRelayStatus("OFF", webSocketK3);
    }
    else if (action == "INACTIVE") // ESTABLECER EL ESTADO DEL RELÉ EN INACTIVO
    {
      relayStop(K3, K3State);
      config.setK3State("INACTIVE");
      remainingTimeK3 = config.getK3TimerSelected();
      LCD_UpdateK3Timer();
      sendRelayStatus("INACTIVE", webSocketK3);
      sendRemainingTime(remainingTimeK3, webSocketK3);
    }
    else if (action == "ACTIVE") // ACTIVAR EL RELÉ
    {
      relayStart(K3, K3State);
      config.setK3State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK3);
    }
    else if (action == "PAUSE") // PAUSAR EL RELÉ
    {
      relayPause(K3, K3State);
      config.setK3State("PAUSE");
      sendRelayStatus("PAUSE", webSocketK3);
    }
    else if (action == "CONTINUE") // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
    {
      relayContinue(K3, K3State);
      config.setK3State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK3);
    }
  }
}

void webSocketEventK4(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE K4
{
  if (type == WStype_CONNECTED)
  {
    // Serial.println("Cliente K4 conectado");
  }

  if (type == WStype_TEXT)
  {
    String message = String((char *)payload); // RECIBIR EL MENSAJE DEL CLIENTE
    DynamicJsonDocument data(1024);           // CREAR UN DOCUMENTO JSON
    deserializeJson(data, message);
    String action = data["action"];

    if (action == "SET_TIMER")
    {
      bool timerStatus = data["timerStatus"]; // RECIBIR EL ESTADO DEL TEMPORIZADOR
      if (timerStatus == false)
      {
        remainingTimeK4 = 0;                             // ESTABLECER EL TIEMPO RESTANTE EN 0
        config.setK4Timer(remainingTimeK4);              // GUARDAR EL TIEMPO RESTANTE
        config.setK4TimerSelected(remainingTimeK4);      // GUARDAR EL TIEMPO SELECCIONADO
        config.setK4TimerStatus(timerStatus);            // GUARDAR EL ESTADO DEL TEMPORIZADOR
        sendRemainingTime(remainingTimeK4, webSocketK4); // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        LCD_UpdateK4Timer();                             // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K4State = INACTIVE;                              // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN INACTIVO
      }
      else if (timerStatus == true)
      {
        unsigned long selectedTime = data["selectedTime"]; // RECIBIR EL TIEMPO SELECCIONADO
        config.setK4TimerSelected(selectedTime);           // GUARDAR EL TIEMPO SELECCIONADO
        remainingTimeK4 = selectedTime;                    // ESTABLECER EL TIEMPO RESTANTE
        config.setK4TimerStatus(timerStatus);              // GUARDAR EL ESTADO DEL TEMPORIZADOR
        config.setK4Timer(selectedTime);                   // GUARDAR EL TIEMPO RESTANTE
        sendRemainingTime(remainingTimeK4, webSocketK4);   // ENVIAR EL TIEMPO RESTANTE AL CLIENTE
        sendTimerStatus(timerStatus, webSocketK4);         // ENVIAR EL ESTADO DEL TEMPORIZADOR AL CLIENTE
        LCD_UpdateK4Timer();                               // ACTUALIZAR EL TIEMPO EN LA PANTALLA LCD
        K4State = WAITING;                                 // ESTABLECER EL ESTADO DEL TEMPORIZADOR EN ESPERA
      }
    }
    else if (action == "ON") // ENCENDER EL RELÉ
    {
      relayOn(K4, K4State);
      config.setK4State("ON");
      sendRelayStatus("ON", webSocketK4);
    }
    else if (action == "OFF") // APAGAR EL RELÉ
    {
      relayStop(K4, K4State);
      config.setK4State("INACTIVE");
      K4State = INACTIVE;
      sendRelayStatus("OFF", webSocketK4);
    }
    else if (action == "INACTIVE") // ESTABLECER EL ESTADO DEL RELÉ EN INACTIVO
    {
      relayStop(K4, K4State);
      config.setK4State("INACTIVE");
      remainingTimeK4 = config.getK4TimerSelected();
      LCD_UpdateK4Timer();
      sendRemainingTime(remainingTimeK4, webSocketK4);
      sendRelayStatus("INACTIVE", webSocketK4);
    }
    else if (action == "ACTIVE") // ACTIVAR EL RELÉ
    {
      relayStart(K4, K4State);
      config.setK4State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK4);
    }
    else if (action == "PAUSE") // PAUSAR EL RELÉ
    {
      relayPause(K4, K4State);
      config.setK4State("PAUSE");
      sendRelayStatus("PAUSE", webSocketK4);
    }
    else if (action == "CONTINUE") // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
    {
      relayContinue(K4, K4State);
      config.setK4State("ACTIVE");
      sendRelayStatus("ACTIVE", webSocketK4);
    }
  }
}

//***** FUNCIONES PARA EL MANEJO DE LOS RELÉS
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
void relayOn(int relay, RelayState &state) // ENCENDER EL RELÉ
{
  digitalWrite(relay, HIGH);
  state = ON;
}
void relayStart(int relay, RelayState &state) // ACTIVAR EL RELÉ
{
  digitalWrite(relay, HIGH);
  state = ACTIVE;
}
void relayPause(int relay, RelayState &state) // PAUSAR EL RELÉ
{
  digitalWrite(relay, LOW);
  state = PAUSE;
}
void relayContinue(int relay, RelayState &state) // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
{
  digitalWrite(relay, HIGH);
  state = ACTIVE;
}
void relayStop(int relay, RelayState &state) // APAGAR EL RELÉ
{
  digitalWrite(relay, LOW);
  state = INACTIVE;
}
void relayInactive(int relay, RelayState &state) // APAGAR EL RELÉ
{
  digitalWrite(relay, LOW);
  state = OFF;
}
void setManualTimer() // MOSTRAR PARPADEO
{
  if (K1Timer == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K1
  {
    if (stateK1Timer == false)
    {
      displayLCD.cleanK1Timer();
      stateK1Timer = true;
    }
    else
    {
      displayLCD.K1Timer(remainingTimeK1);
      stateK1Timer = false;
    }
  }
  else if (K2Timer == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K2
  {
    if (stateK2Timer == false)
    {
      displayLCD.cleanK2Timer();
      stateK2Timer = true;
    }
    else
    {
      displayLCD.K2Timer(remainingTimeK2);
      stateK2Timer = false;
    }
  }
  else if (K3Timer == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K3
  {
    if (stateK3Timer == false)
    {
      displayLCD.cleanK3Timer();
      stateK3Timer = true;
    }
    else
    {
      displayLCD.K3Timer(remainingTimeK3);
      stateK3Timer = false;
    }
  }
  else if (K4Timer == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K4
  {
    if (stateK4Timer == false)
    {
      displayLCD.cleanK4Timer();
      stateK4Timer = true;
    }
    else
    {
      displayLCD.K4Timer(remainingTimeK4);
      stateK4Timer = false;
    }
  }
}

void finallyK1Timer() // FINALIZAR EL TEMPORIZADOR DE K1
{
    relayInactive(K1, K1State);
    remainingTimeK1 = config.getK1TimerSelected();
    config.setK1Timer(remainingTimeK1);
    sendRemainingTime(remainingTimeK1, webSocketK1);
    sendRelayStatus("INACTIVE", webSocketK1);
    LCD_UpdateK1Timer();
}

void finallyK2Timer() // FINALIZAR EL TEMPORIZADOR DE K2
{
    relayInactive(K2, K2State);
    remainingTimeK2 = config.getK2TimerSelected();
    config.setK2Timer(remainingTimeK2);
    sendRemainingTime(remainingTimeK2, webSocketK2);
    sendRelayStatus("INACTIVE", webSocketK2);
    LCD_UpdateK2Timer();
}

void finallyK3Timer() // FINALIZAR EL TEMPORIZADOR DE K3
{
    relayInactive(K3, K3State);
    remainingTimeK3 = config.getK3TimerSelected();
    config.setK3Timer(remainingTimeK3);
    sendRemainingTime(remainingTimeK3, webSocketK3);
    sendRelayStatus("INACTIVE", webSocketK3);
    LCD_UpdateK3Timer();
}

void finallyK4Timer() // FINALIZAR EL TEMPORIZADOR DE K4
{
    relayInactive(K4, K4State);
    remainingTimeK4 = config.getK4TimerSelected();
    config.setK4Timer(remainingTimeK4);
    sendRemainingTime(remainingTimeK4, webSocketK4);
    sendRelayStatus("INACTIVE", webSocketK4);
    LCD_UpdateK4Timer();
}


//***** FUNCIONES DE LOOP
void loop()
{
  webSocketConfig.loop();
  webSocketData.loop();
  webSocketK1.loop();
  webSocketK2.loop();
  webSocketK3.loop();
  webSocketK4.loop();

  // RETARDO DE 1 SEGUNDO
  unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA1SEG)
  {
    ultimoTiempo = tiempoActual; // ACTUALIZAR EL TIEMPO DE LA ÚLTIMA LECTURA
    getTime();                   // ACTUALIZAR LA HORA

    getTemperature(); // ACTUALIZAR LA TEMPERATURA

    if (K1State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K1
    {
      remainingTimeK1 -= 1000;
      config.setK1Timer(remainingTimeK1);
      LCD_UpdateK1Timer();
      sendRemainingTime(remainingTimeK1, webSocketK1);
      if (remainingTimeK1 == 0)
      {
        finallyK1Timer();
      }
    }
    if (K2State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K2
    {
      remainingTimeK2 -= 1000;
      config.setK2Timer(remainingTimeK2);
      LCD_UpdateK2Timer();
      sendRemainingTime(remainingTimeK2, webSocketK2);
      if (remainingTimeK2 == 0)
      {
        finallyK2Timer();
      }
    }
    if (K3State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K3
    {
      remainingTimeK3 -= 1000;
      config.setK3Timer(remainingTimeK3);
      LCD_UpdateK3Timer();
      sendRemainingTime(remainingTimeK3, webSocketK3);
      if (remainingTimeK3 == 0)
      {
        finallyK3Timer();
      }
    }
    if (K4State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K4
    {
      remainingTimeK4 -= 1000;
      config.setK4Timer(remainingTimeK4);
      LCD_UpdateK4Timer();
      sendRemainingTime(remainingTimeK4, webSocketK4);
      if (remainingTimeK4 == 0)
      {
        finallyK4Timer();
      }
    }
  }
  // RETARDO DE 0,5 SEGUNDOS
  static unsigned long ultimoTiempo2 = 0;
  if (tiempoActual - ultimoTiempo2 >= INTERVALODELECTURA05SEG)
  {
    ultimoTiempo2 = tiempoActual; 
                                  //*** ***//
    setManualTimer();
  }

  // RETARDO DE 10 SEGUNDOS
  static unsigned long ultimoTiempo3 = 0;
  if (tiempoActual - ultimoTiempo3 >= INTERVALODELECTURA10SEG)
  {
    ultimoTiempo3 = tiempoActual; 
    sendData();                   // ENVIAR DATOS
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

  if (btnOk.read() == HIGH && btnOk.currentDuration() == 2000) // CONFIRMACIÓN DE LA INSERCION DEL TIEMPO DE RIEGO MANUAL
  {

    if (K1Timer == 1)
    {
      if (remainingTimeK1 >= 60000)
      {
        config.setK1TimerStatus(true);
        config.setK1TimerSelected(remainingTimeK1);
        config.setK1Timer(remainingTimeK1);
        sendTimerStatus(true, webSocketK1);
        sendRemainingTime(remainingTimeK1, webSocketK1);
      }
      else
      {
        config.setK1TimerStatus(false);
        remainingTimeK1 = 0;
        sendTimerStatus(false, webSocketK1);
        sendRemainingTime(remainingTimeK1, webSocketK1);
        config.setK1Timer(remainingTimeK1);
        config.setK1TimerSelected(remainingTimeK1);
      }
      LCD_UpdateK1Timer();
      K1Timer = 0;
    }
    else if (K2Timer == 1)
    {
      if (remainingTimeK2 >= 60000)
      {
        config.setK2TimerStatus(true);
        config.setK2TimerSelected(remainingTimeK2);
        config.setK2Timer(remainingTimeK2);
        sendTimerStatus(true, webSocketK2);
        sendRemainingTime(remainingTimeK2, webSocketK2);
      }
      else
      {
        config.setK2TimerStatus(false);
        remainingTimeK2 = 0;
        sendTimerStatus(false, webSocketK2);
        sendRemainingTime(remainingTimeK2, webSocketK2);
        config.setK2Timer(remainingTimeK2);
        config.setK2TimerSelected(remainingTimeK2);
      }
      LCD_UpdateK2Timer();
      K2Timer = 0;
    }
    else if (K3Timer == 1)
    {
      if (remainingTimeK3 >= 60000)
      {
        config.setK3TimerStatus(true);
        config.setK3TimerSelected(remainingTimeK3);
        config.setK3Timer(remainingTimeK3);
        sendTimerStatus(true, webSocketK3);
        sendRemainingTime(remainingTimeK3, webSocketK3);
      }
      else
      {
        config.setK3TimerStatus(false);
        remainingTimeK3 = 0;
        sendTimerStatus(false, webSocketK3);
        sendRemainingTime(remainingTimeK3, webSocketK3);
        config.setK3Timer(remainingTimeK3);
        config.setK3TimerSelected(remainingTimeK3);
      }
      LCD_UpdateK3Timer();
      K3Timer = 0;
    }
    else if (K4Timer == 1)
    {
      if (remainingTimeK4 >= 60000)
      {
        config.setK4TimerStatus(true);
        config.setK4TimerSelected(remainingTimeK4);
        config.setK4Timer(remainingTimeK4);
        sendTimerStatus(true, webSocketK4);
        sendRemainingTime(remainingTimeK4, webSocketK4);
      }
      else
      {
        config.setK4TimerStatus(false);
        remainingTimeK4 = 0;
        sendRemainingTime(remainingTimeK4, webSocketK4);
        config.setK4Timer(remainingTimeK4);
        sendTimerStatus(false, webSocketK4);
        config.setK4TimerSelected(remainingTimeK4);
      }
      LCD_UpdateK4Timer();
      K4Timer = 0;
    }
  }

  if (btnUp.fell()) // BOTON UP AUMENTA EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    if (K1Timer == 1)
    {
      remainingTimeK1 += 60000;
    }
    else if (K2Timer == 1)
    {
      remainingTimeK2 += 60000;
    }
    else if (K3Timer == 1)
    {
      remainingTimeK3 += 60000;
    }
    else if (K4Timer == 1)
    {
      remainingTimeK4 += 60000;
    }
  }

  if (btnDown.fell()) // BOTON DOWN DISMINUYE EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    // Botón DOWN presionado
    if (K1Timer == 1)
    {
      remainingTimeK1 -= 60000;
    }
    else if (K2Timer == 1)
    {
      remainingTimeK2 -= 60000;
    }
    else if (K3Timer == 1)
    {
      remainingTimeK3 -= 60000;
    }
    else if (K4Timer == 1)
    {
      remainingTimeK4 -= 60000;
    }
  }

  if (btnLeft.fell()) // BOTON LEFT ESTÁ INACTIVO
  {
    if (menuPage > 1)
    {
      menuPage -= 1;
      setMenuPage();
    }
  }

  if (btnRight.fell()) // BOTON DERECHA ESTÁ INACTIVO
  {
    if (menuPage < 4)
    {
      menuPage += 1;
      setMenuPage();
    }
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

    bool K1Active = config.getK1Active();

    if (K1Active) // Si el relé K1 está inactivo
    {
      switch (K1State)
      {
      case INACTIVE:
        setTimeRelay("K1");
        break;
      case WAITING:
        if (K1Timer == 0)
        {
          relayStart(K1, K1State);
          sendRelayStatus("ACTIVE", webSocketK1);
        }
        else
        {
          Serial.println("El tiempo de riego no está confirmado");
        }
        break;
      case ACTIVE:
        relayPause(K1, K1State);
        sendRelayStatus("PAUSE", webSocketK1);
        break;
      case PAUSE:
        relayContinue(K1, K1State);
        sendRelayStatus("ACTIVE", webSocketK1);
        break;
      case OFF:
        K1State = INACTIVE;
        sendRelayStatus("INACTIVE", webSocketK1);
        break;
      }
    }
  }
  else if (btnK1.read() == HIGH && btnK1.currentDuration() == 3000) // APAGAR K1 DESPUÉS DE 3 SEGUNDOS
  {
    remainingTimeK1 = config.getK1TimerSelected();
    relayInactive(K1, K1State);
    LCD_UpdateK1Timer();
    sendRelayStatus("INACTIVE", webSocketK1);
    sendRemainingTime(remainingTimeK1, webSocketK1);
  }

  if (btnK2.fell()) // BOTÓN K2
  {
    bool K2Active = config.getK2Active();
    if (K2Active) // Si el relé K2 está activo
    {
      switch (K2State)
      {
      case INACTIVE:
        setTimeRelay("K2");
        break;
      case WAITING:
        if (K2Timer == 0)
        {
          relayStart(K2, K2State);
          sendRelayStatus("ACTIVE", webSocketK2);
        }
        else
        {
          Serial.println("El tiempo de riego no está confirmado");
        }
        break;
      case ACTIVE:
        relayPause(K2, K2State);
        sendRelayStatus("PAUSE", webSocketK2);
        break;
      case PAUSE:
        relayContinue(K2, K2State);
        sendRelayStatus("ACTIVE", webSocketK2);
        break;
      case OFF:
        K2State = INACTIVE;
        sendRelayStatus("INACTIVE", webSocketK2);
        break;
      }
    }
  }
  else if (btnK2.read() == HIGH && btnK2.currentDuration() == 3000) // APAGAR K2 DESPUÉS DE 3 SEGUNDOS
  {
    remainingTimeK2 = config.getK2TimerSelected();
    relayInactive(K2, K2State);
    LCD_UpdateK2Timer();
    sendRelayStatus("INACTIVE", webSocketK2);
    sendRemainingTime(remainingTimeK2, webSocketK2);
  }
}

//***** FUNCIONES DEL SISTEMA
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

//***** FUNCIONES DE LA PANTALLA LCD
void setMenuPage() // ESTABLECER LA PÁGINA DEL MENÚ EN LA PANTALLA
{

  if (menuPage == 1)
  {
    displayLCD.clearLine(1);
    displayLCD.clearLine(2);
    displayLCD.clearLine(3);
    displayLCD.relaySeparation();
    String K1Name = config.getK1Name();
    String K2Name = config.getK2Name();
    displayLCD.K1Name(K1Name);
    displayLCD.K2Name(K2Name);
    displayLCD.K1Timer(remainingTimeK1);
    displayLCD.K2Timer(remainingTimeK2);
  }
  else if (menuPage == 2)
  {
    displayLCD.clearLine(1);
    displayLCD.clearLine(2);
    displayLCD.clearLine(3);
    displayLCD.relaySeparation();
    String K3Name = config.getK3Name();
    String K4Name = config.getK4Name();
    displayLCD.K3Name(K3Name);
    displayLCD.K4Name(K4Name);
    displayLCD.K3Timer(remainingTimeK3);
    displayLCD.K4Timer(remainingTimeK4);
  }
  else if (menuPage == 3)
  {
    displayLCD.clearLine(1);
    displayLCD.clearLine(2);
    displayLCD.clearLine(3);
    displayLCD.menu();
  }
}

void LCD_UpdateK1Timer() // ACTUALIZAR EL TEMPORIZADOR EN LA PANTALLA
{
  if (menuPage == 1)
  {
    displayLCD.K1Timer(remainingTimeK1);
  }
}
void LCD_UpdateK2Timer() // ACTUALIZAR EL TEMPORIZADOR EN LA PANTALLA
{
  if (menuPage == 1)
  {
    displayLCD.K2Timer(remainingTimeK2);
  }
}
void LCD_UpdateK3Timer() // ACTUALIZAR EL TEMPORIZADOR EN LA PANTALLA
{
  if (menuPage == 2)
  {
    displayLCD.K3Timer(remainingTimeK3);
  }
   
}
void LCD_UpdateK4Timer() // ACTUALIZAR EL TEMPORIZADOR EN LA PANTALLA
{
  if (menuPage == 2)
  {
    displayLCD.K4Timer(remainingTimeK4);
  }
}

//***** FUNCIONES DE OBTENCIÓN DE DATOS

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
