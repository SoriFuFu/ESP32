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
WebSocketsServer webSocketData = WebSocketsServer(82);   // OBJETO DEL SERVIDOR WEBSOCKET PARA LOS DATOS
WebSocketsServer webSocketRelay = WebSocketsServer(83);  // OBJETO DEL SERVIDOR WEBSOCKET PARA LOS RELÉS

AsyncWebServer server(80); // OBJETO DEL SERVIDOR HTTP

#define INTERVALODELECTURA2MIN 120000 // INTERVALO DE ACTULIZACIÓN DE 2 MINUTOS
#define INTERVALODELECTURA10SEG 10000 // INTERVALO DE ACTULIZACIÓN DE 10 SEGUNDOS
#define INTERVALODELECTURA1SEG 1000   // INTERVALO DE ACTULIZACIÓN DE 1 SEGUNDO
#define INTERVALODELECTURA05SEG 500   // INTERVALO DE ACTULIZACIÓN
#define BMP280_ADDR 0x76              // DIRECCIÓN DEL SENSOR BMP280
#define LED 25                        // PIN PARA EL LED
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
bool timer[4] = {false, false, false, false};
bool stateTimer[4] = {false, false, false, false};

// VARIABLES PARA EL TIEMPO RESTANTE
unsigned long remainingTime[4] = {0, 0, 0, 0};
unsigned long timeOnDisplay = 0;
bool displayState = true;
int menuPage = 1; // PÁGINA DEL DISPLAY

uint8_t clientData = 0; // Variable para rastrear el cliente conectado
uint8_t clientConfig = 1;
uint8_t clientRelay = 1;

// SETUP DE LA APLICACIÓN PRINCIPAL
void setup()
{
  Serial.begin(115200);
  delay(1000);
  SPIFFS.begin();      // Inicializar el sistema de archivos SPIFFS
  config.loadConfig(); // Cargar la configuración al inicio
configTime(3600, 0, "pool.ntp.org");
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
  config.setMacAddress(wifiConfig.getMACAddress()); // GUARDAR LA DIRECCIÓN MAC

  //***** CONFIGURAR EL CANAL WEBSOCKET PARA LA CONFIGURACIÓN
  webSocketConfig.begin();
  webSocketData.begin();
  webSocketRelay.begin();

  //***** CONFIGURAR LOS EVENTOS DEL WEBSOCKET
  webSocketConfig.onEvent(webSocketEventConfig);
  webSocketData.onEvent(webSocketEventData);
  webSocketRelay.onEvent(webSocketEventRelay);

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

  config.setAllRelayState("INACTIVE");

  setMenuPage();
  getConfigData();
}

void getConfigData()
{
  remainingTime[0] = config.getRelayTimerSelected("K1");
  remainingTime[1] = config.getRelayTimerSelected("K2");
  remainingTime[2] = config.getRelayTimerSelected("K3");
  remainingTime[3] = config.getRelayTimerSelected("K4");

  config.setRelayRemainingTime("K1", remainingTime[0]);
  config.setRelayRemainingTime("K2", remainingTime[1]);
  config.setRelayRemainingTime("K3", remainingTime[2]);
  config.setRelayRemainingTime("K4", remainingTime[3]);

  LCD_UpdateTimer("K1");
  LCD_UpdateTimer("K2");
  LCD_UpdateTimer("K3");
  LCD_UpdateTimer("K4");
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
    String K1Name = config.getRelayName("K1");
    String K2Name = config.getRelayName("K2");
    displayLCD.K1Name(K1Name);
    displayLCD.K2Name(K2Name);
    displayLCD.K1Timer(remainingTime[0]);
    displayLCD.K2Timer(remainingTime[1]);
  }
  else if (menuPage == 2)
  {
    displayLCD.clearLine(1);
    displayLCD.clearLine(2);
    displayLCD.clearLine(3);
    displayLCD.relaySeparation();
    String K3Name = config.getRelayName("K3");
    String K4Name = config.getRelayName("K4");
    displayLCD.K3Name(K3Name);
    displayLCD.K4Name(K4Name);
    displayLCD.K3Timer(remainingTime[2]);
    displayLCD.K4Timer(remainingTime[3]);
  }
  else if (menuPage == 3)
  {
    displayLCD.clearLine(1);
    displayLCD.clearLine(2);
    displayLCD.clearLine(3);
    displayLCD.menu();
  }
}

void LCD_UpdateTimer(String relay) // ACTUALIZAR EL TEMPORIZADOR EN LA PANTALLA
{
  if (relay == "K1" && menuPage == 1)
  {
    displayLCD.K1Timer(remainingTime[0]);
  }
  else if (relay == "K2" && menuPage == 1)
  {
    displayLCD.K2Timer(remainingTime[1]);
  }
  else if (relay == "K3" && menuPage == 2)
  {
    displayLCD.K3Timer(remainingTime[2]);
  }
  else if (relay == "K4" && menuPage == 2)
  {
    displayLCD.K4Timer(remainingTime[3]);
  }
}

void LCD_OFF() // APAGAR LA PANTALLA
{
  displayLCD.offDisplay();
}
void LCD_ON() // ENCENDER LA PANTALLA
{
  timeOnDisplay = millis();
  displayLCD.onDisplay();
}
//***** FUNCIONES PARA EL WEB SERVER
void webSocketEventData(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE DATOS
{
  if (type == WStype_CONNECTED)
  {
    if (clientData == 0)
    {
      clientData = num;
      Serial.print("Cliente ");
      Serial.print(num);
      Serial.println(" conectado al servidor de datos");
    }
    else
    {
      disconnectClient(clientData, webSocketData, "del servidor de datos"); // Desconectar al segundo cliente intentando conectarse
    }
  }
  else if (type == WStype_TEXT)
  {

    String message = String((char *)payload);
    DynamicJsonDocument data(1024);
    deserializeJson(data, message);

    String action = data["action"];
    Serial.print("Acción: ");
    Serial.println(action);
    if (action == "getConfig") // Enviar la configuracion al cliente
    {                          //
      updateClientConfig();
    }
  }
  else if (type == WStype_DISCONNECTED)
  {
    if (num == clientData)
    {
      clientData = 0; // Restablecer la variable para permitir una nueva conexión
      Serial.print("Cliente ");
      Serial.print(num);
      Serial.println(" desconectado del servidor de datos");
    }
  }
}

void updateClientConfig() // ACTUALIZAR LA CONFIGURACIÓN DEL CLIENTE
{
  // Enviar la configuración actual al cliente
  String configJson;
  serializeJson(config.getConfigDoc(), configJson);
  webSocketData.broadcastTXT(configJson);
  Serial.println("Configuración enviada al cliente");
}

bool sendMessageClient(String message, uint8_t num) // ENVIAR MENSAJE AL CLIENTE
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["message"] = message;
  char jsonString[100];
  serializeJson(jsonDocument, jsonString);
  // Envía la cadena JSON a través del WebSocket
  webSocketConfig.sendTXT(num, jsonString);
  return true;
}

void sendRemainingTime() // ENVIAR EL TIEMPO RESTANTE
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["action"] = "UPDATE_TIMER";
  jsonDocument["timeK1"] = remainingTime[0];
  jsonDocument["timeK2"] = remainingTime[1];
  jsonDocument["timeK3"] = remainingTime[2];
  jsonDocument["timeK4"] = remainingTime[3];

  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocketRelay.broadcastTXT(jsonString);
}

void sendTimerStatus(String relay, bool timerState) // ENVIAR EL ESTADO DEL TEMPORIZADOR
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["relay"] = relay;
  jsonDocument["action"] = "UPDATE_TIMER_STATUS";
  jsonDocument["status"] = timerState;

  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocketRelay.broadcastTXT(jsonString);
}

void sendRelayStatus(String relay, String status) // ENVIAR EL ESTADO DEL RELÉ
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["relay"] = relay;
  jsonDocument["action"] = status;
  char jsonString[100];
  serializeJson(jsonDocument, jsonString);

  // Envía la cadena JSON a través del WebSocket
  webSocketRelay.broadcastTXT(jsonString);
}

void disconnectClient(uint8_t num, WebSocketsServer &webSocket, String origin) // DESCONECTAR AL CLIENTE
{
  webSocket.disconnect(num);
  Serial.print("Cliente ");
  Serial.print(num);
  Serial.print(" desconectado de ");
  Serial.println(origin);
}

void webSocketEventConfig(uint8_t num, WStype_t type, uint8_t *payload, size_t length) // FUNCION PARA MANEJAR LOS EVENTOS DEL WEBSOCKET DE CONFIGURACIÓN
{
  if (type == WStype_CONNECTED)
  {
    disconnectClient(clientConfig, webSocketConfig, "configuración"); // Desconectar al segundo cliente intentando conectarse
    clientData = num;
    Serial.print("Cliente ");
    Serial.print(num);
    Serial.println(" conectado a la configuración");
  }
  else if (type == WStype_TEXT)
  {
    String message = String((char *)payload);
    DynamicJsonDocument data(1024);
    deserializeJson(data, message);
    String action = data["action"];

    //****CONFIGURACIÓN WIFI Y AP****//
    if (action == "getNetworks") // Buscar redes WiFi
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
    else if (action == "Reset") // Restablecer la configuración
    {
      ESP.restart();
    }

    //****CONFIGURACIÓN RELÉS****//
    if (action == "setRelayActive") // Configurar relés
    {
      String relay = data["relay"];

      if (relay == "K1")
      {
        bool K1Active = data["K1Active"];
        config.setRelayActive("K1", K1Active);
      }
      else if (relay == "K2")
      {
        bool K2Active = data["K2Active"];
        config.setRelayActive("K2", K2Active);
      }
      else if (relay == "K3")
      {
        bool K3Active = data["K3Active"];
        config.setRelayActive("K3", K3Active);
      }
      else if (relay == "K4")
      {
        bool K4Active = data["K4Active"];
        config.setRelayActive("K4", K4Active);
      }
    }
    else if (action == "setRelayName")
    {
      String relay = data["relay"];
      String relayName = data["relayName"];

      config.setRelayName(relay, relayName);
    }
  }

  else if (type == WStype_DISCONNECTED)
  {
    if (num == clientConfig)
    {
      clientConfig = 1; 
      Serial.print("Cliente ");
      Serial.print(num);
      Serial.println(" se ha desconectado de la configuración");
    }
  }
}

void handleAction(const int relayNumber, const String &relay, const String &action, const DynamicJsonDocument &data, RelayState &state)
{
  // Manejar la acción para el relé especificado
  int remainingNumber = relay == "K1" ? 0 : relay == "K2" ? 1
                                        : relay == "K3"   ? 2
                                                          : 3;
  if (action == "SET_TIMER")
  {
    bool timerState = data["timerState"];
    unsigned long selectedTime = data["selectedTime"];

    if (timerState == false)
    {
      remainingTime[remainingNumber] = 0;
      config.setRelayRemainingTime(relay, 0);
      config.setRelayTimerState(relay, false);
      sendRemainingTime();
      LCD_UpdateTimer(relay);
      setState(relay, INACTIVE);
      Serial.print("timerState: ");
      Serial.println(timerState);
    }
    else
    {
      Serial.print("selectedTime de ");
      Serial.print(relay);
      Serial.print(": ");
      Serial.println(selectedTime);

      remainingTime[remainingNumber] = selectedTime;
      config.setRelayRemainingTime(relay, selectedTime);
      config.setRelayTimerSelected(relay, selectedTime);
      config.setRelayTimerState(relay, true);
      sendRemainingTime();
      sendTimerStatus(relay, true);
      LCD_UpdateTimer(relay);
      setState(relay, WAITING);
    }
  }
  else if (action == "ON")
  {
    relayStart(relayNumber, state); // Corregido
    config.setRelayState(relay, "ACTIVE");
    sendRelayStatus(relay, "ON");
  }
  else if (action == "ACTIVE")
  {

    relayStart(relayNumber, state); // Corregido
    config.setRelayState(relay, "ACTIVE");
    sendRelayStatus(relay, "ACTIVE");
  }
  else if (action == "OFF")
  {

    relayStop(relayNumber, state); // Corregido
    config.setRelayState(relay, "INACTIVE");
    setState(relay, INACTIVE);
    sendRelayStatus(relay, "OFF");
  }
  else if (action == "INACTIVE")
  {

    relayInactive(relayNumber, state); // Corregido
    config.setRelayState(relay, "INACTIVE");
    remainingTime[remainingNumber] = config.getRelayTimerSelected(relay);
    config.setRelayRemainingTime(relay, remainingTime[remainingNumber]);
    LCD_UpdateTimer(relay);
    sendRemainingTime();
    setState(relay, INACTIVE);
    sendRelayStatus(relay, "INACTIVE");
  }
  else if (action == "PAUSE")
  {

    relayPause(relayNumber, state); // Corregido
    config.setRelayState(relay, "PAUSE");
    sendRelayStatus(relay, "PAUSE");
  }
  else if (action == "CONTINUE")
  {
    relayContinue(relayNumber, state); // Corregido
    config.setRelayState(relay, "ACTIVE");
    sendRelayStatus(relay, "ACTIVE");
  }
}

void setState(const String &relay, RelayState state)
{
  if (relay == "K1")
  {
    K1State = state;
  }
  else if (relay == "K2")
  {
    K2State = state;
    Serial.println("K2: " + state);
  }
  else if (relay == "K3")
  {
    K3State = state;
  }
  else if (relay == "K4")
  {
    K4State = state;
  }
}

void webSocketEventRelay(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  if (type == WStype_CONNECTED)
  {
    disconnectClient(clientRelay, webSocketRelay, "control de relés"); // Desconectar al segundo cliente intentando conectarse
    clientRelay = num;
    Serial.print("Cliente ");
    Serial.print(num);
    Serial.println(" conectado al servidor de relés");
  }
  else if (type == WStype_TEXT)
  {
    String message = String((char *)payload);
    DynamicJsonDocument data(1024);
    if (deserializeJson(data, message) != DeserializationError::Ok)
    {
      Serial.println("Error al deserializar JSON");
      return;
    }
    String relay = data["relay"];
    int relayNumber = relay == "K1" ? K1 : relay == "K2" ? K2
                                       : relay == "K3"   ? K3
                                                         : K4;

    String action = data["action"];

    if (relay == "K1")
    {
      handleAction(relayNumber, relay, action, data, K1State);
    }
    else if (relay == "K2")
    {
      handleAction(relayNumber, relay, action, data, K2State);
    }
    else if (relay == "K3")
    {
      handleAction(relayNumber, relay, action, data, K3State);
    }
    else if (relay == "K4")
    {
      handleAction(relayNumber, relay, action, data, K4State);
    }
    else
    {
      Serial.println("Relé no válido");
    }
  }
  else if (type == WStype_DISCONNECTED)
  {
    if (num == clientRelay)
    {
      clientRelay = 1; // Restablecer la variable para permitir una nueva conexión
      Serial.print("Cliente ");
      Serial.print(num);
      Serial.println(" se ha desconectado del servidor de relés");
    }
  }
}

//***** FUNCIONES PARA EL MANEJO DE LOS RELÉS
void setTimeRelay(String relay) // ESTABLECER EL TIEMPO DE RIEGO
{
  if (relay == "K1")
  {
    timer[0] = 1;
    K1State = WAITING;
  }
  else if (relay == "K2")
  {
    timer[1] = 1;
    K2State = WAITING;
  }
  else if (relay == "K3")
  {
    timer[2] = 1;
    K3State = WAITING;
  }
  else if (relay == "K4")
  {
    timer[3] = 1;
    K4State = WAITING;
  }
}
void relayOn(int relay, RelayState &state) // ENCENDER EL RELÉ
{
  Serial.print("Relé: ");
  Serial.print(relay);
  Serial.println(" ON");

  digitalWrite(relay, HIGH);
  state = ON;
}
void relayStart(int relay, RelayState &state) // ACTIVAR EL RELÉ
{
  Serial.print("Relé: ");
  Serial.print(relay);
  Serial.println(" ACTIVE");
  digitalWrite(relay, HIGH);
  state = ACTIVE;
}
void relayPause(int relay, RelayState &state) // PAUSAR EL RELÉ
{
  Serial.print("Relé: ");
  Serial.print(relay);
  Serial.println(" PAUSE");
  digitalWrite(relay, LOW);
  state = PAUSE;
}
void relayContinue(int relay, RelayState &state) // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
{
  Serial.print("Relé: ");
  Serial.print(relay);
  Serial.println(" CONTINUE");
  digitalWrite(relay, HIGH);
  state = ACTIVE;
}
void relayStop(int relay, RelayState &state) // APAGAR EL RELÉ
{
  Serial.print("Relé: ");
  Serial.print(relay);
  Serial.println(" OFF");
  digitalWrite(relay, LOW);
  state = INACTIVE;
}
void relayInactive(int relay, RelayState &state) // APAGAR EL RELÉ
{
  Serial.print("Relé: ");
  Serial.print(relay);
  Serial.println(" INACTIVE");
  digitalWrite(relay, LOW);
  state = OFF;
}
void setManualTimer() // MOSTRAR PARPADEO
{
  if (timer[0] == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K1
  {
    if (stateTimer[0] == false)
    {
      displayLCD.cleanK1Timer();
      stateTimer[0] = true;
    }
    else
    {
      displayLCD.K1Timer(remainingTime[0]);
      stateTimer[0] = false;
    }
  }
  else if (timer[1] == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K2
  {
    if (stateTimer[1] == false)
    {
      displayLCD.cleanK2Timer();
      stateTimer[1] = true;
    }
    else
    {
      displayLCD.K2Timer(remainingTime[1]);
      stateTimer[1] = false;
    }
  }
  else if (timer[2] == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K3
  {
    if (stateTimer[2] == false)
    {
      displayLCD.cleanK3Timer();
      stateTimer[2] = true;
    }
    else
    {
      displayLCD.K3Timer(remainingTime[2]);
      stateTimer[2] = false;
    }
  }
  else if (timer[3] == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K4
  {
    if (stateTimer[3] == false)
    {
      displayLCD.cleanK4Timer();
      stateTimer[3] = true;
    }
    else
    {
      displayLCD.K4Timer(remainingTime[3]);
      stateTimer[3] = false;
    }
  }
}

void finallyRelayTimer(int relay, String relayNumber, RelayState &state, unsigned long &remainingTime) // FINALIZAR EL TEMPORIZADOR DEL RELÉ
{
  relayInactive(relay, state);
  remainingTime = config.getRelayTimerSelected(relayNumber);
  config.setRelayRemainingTime(relayNumber, remainingTime);
  sendRemainingTime();
  sendRelayStatus(relayNumber, "INACTIVE");
  LCD_UpdateTimer(relayNumber);
}

void updateTimeRelay()
{
  sendRemainingTime();
  LCD_UpdateTimer("K1");
  LCD_UpdateTimer("K2");
  LCD_UpdateTimer("K3");
  LCD_UpdateTimer("K4");
}
//***** FUNCIONES DE LOOP
void loop()
{
  webSocketConfig.loop();
  webSocketData.loop();
  webSocketRelay.loop();

  // RETARDO DE 1 SEGUNDO
  unsigned long tiempoActual = millis();
  static unsigned long ultimoTiempo = 0;
  if (tiempoActual - ultimoTiempo >= INTERVALODELECTURA1SEG)
  {
    ultimoTiempo = tiempoActual; // ACTUALIZAR EL TIEMPO DE LA ÚLTIMA LECTURA
    getTime();                   // ACTUALIZAR LA HORA

    // getTemperature(); // ACTUALIZAR LA TEMPERATURA
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

    if (K1State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K1
    {
      remainingTime[0] -= 10000;
      config.setRelayRemainingTime("K1", remainingTime[0]);
      updateTimeRelay();
      if (remainingTime[0] == 0)
      {
        finallyRelayTimer(K1, "K1", K1State, remainingTime[0]);
      }
    }
    if (K2State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K2
    {
      remainingTime[1] -= 10000;
      config.setRelayRemainingTime("K2", remainingTime[1]);
      updateTimeRelay();
      if (remainingTime[1] == 0)
      {
        finallyRelayTimer(K2, "K2", K2State, remainingTime[1]);
      }
    }
    if (K3State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K3
    {
      remainingTime[2] -= 10000;
      config.setRelayRemainingTime("K3", remainingTime[2]);
      updateTimeRelay();
      if (remainingTime[2] == 0)
      {
        finallyRelayTimer(K3, "K3", K3State, remainingTime[2]);
      }
    }
    if (K4State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K4
    {
      remainingTime[3] -= 10000;
      config.setRelayRemainingTime("K4", remainingTime[3]);
      updateTimeRelay();
      if (remainingTime[3] == 0)
      {
        finallyRelayTimer(K4, "K4", K4State, remainingTime[3]);
      }
    }
  }

  if (tiempoActual - timeOnDisplay >= INTERVALODELECTURA2MIN)
  {
    LCD_OFF();
    displayState = false;
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

    if (timer[0] == 1)
    {
      if (remainingTime[0] >= 60000)
      {
        config.setRelayTimerState("K1", true);
        sendTimerStatus("K1", true);
      }
      else
      {
        config.setRelayTimerState("K1", false);
        remainingTime[0] = 0;
        sendTimerStatus("K1", false);
      }
      sendRemainingTime();
      config.setRelayTimerSelected("K1", remainingTime[0]);
      config.setRelayRemainingTime("K1", remainingTime[0]);
      LCD_UpdateTimer("K1");
      timer[0] = 0;
    }
    else if (timer[1] == 1)
    {
      if (remainingTime[1] >= 60000)
      {
        config.setRelayTimerState("K2", true);
        sendTimerStatus("K2", true);
      }
      else
      {
        config.setRelayTimerState("K2", false);
        remainingTime[1] = 0;
        sendTimerStatus("K2", false);
      }
      sendRemainingTime();
      config.setRelayTimerSelected("K2", remainingTime[1]);
      config.setRelayRemainingTime("K2", remainingTime[1]);
      LCD_UpdateTimer("K2");
      timer[1] = 0;
    }
    else if (timer[2] == 1)
    {
      if (remainingTime[2] >= 60000)
      {
        config.setRelayTimerState("K3", true);
        sendTimerStatus("K3", true);
      }
      else
      {
        config.setRelayTimerState("K3", false);
        remainingTime[2] = 0;
        sendTimerStatus("K3", false);
      }
      sendRemainingTime();
      config.setRelayTimerSelected("K3", remainingTime[2]);
      config.setRelayRemainingTime("K3", remainingTime[2]);
      LCD_UpdateTimer("K3");
      timer[2] = 0;
    }
    else if (timer[3] == 1)
    {
      if (remainingTime[3] >= 60000)
      {
        config.setRelayTimerState("K4", true);
        sendTimerStatus("K4", true);
      }
      else
      {
        config.setRelayTimerState("K4", false);
        remainingTime[3] = 0;
        sendTimerStatus("K4", false);
      }
      sendRemainingTime();
      config.setRelayTimerSelected("K4", remainingTime[3]);
      config.setRelayRemainingTime("K4", remainingTime[3]);
      LCD_UpdateTimer("K4");
      timer[3] = 0;
    }
  }else if (btnOk.read() == HIGH && btnOk.currentDuration() <= 2000) // RESTABLECER LA CONFIGURACIÓN DE FÁBRICA DESPUÉS DE 10 SEGUNDOS
  {
    timeOnDisplay = millis();
    if (displayState == false)
    {
      displayLCD.onDisplay();
      setMenuPage();
      displayState = true;
    }


  }

  if (btnUp.fell()) // BOTON UP AUMENTA EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    if (timer[0] == 1)
    {
      remainingTime[0] += 60000;
    }
    else if (timer[1] == 1)
    {
      remainingTime[1] += 60000;
    }
    else if (timer[2] == 1)
    {
      remainingTime[2] += 60000;
    }
    else if (timer[3] == 1)
    {
      remainingTime[3] += 60000;
    }
  }

  if (btnDown.fell()) // BOTON DOWN DISMINUYE EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    // Botón DOWN presionado
    if (timer[0] == 1)
    {
      remainingTime[0] -= 60000;
    }
    else if (timer[1] == 1)
    {
      remainingTime[1] -= 60000;
    }
    else if (timer[2] == 1)
    {
      remainingTime[2] -= 60000;
    }
    else if (timer[3] == 1)
    {
      remainingTime[3] -= 60000;
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
    timeOnDisplay = millis();
       if (displayState == false)
    {
      displayLCD.onDisplay();
      setMenuPage();
      displayState = true;
    }
    bool K1Active = config.getRelayActive("K1");

    if (K1Active) // Si el relé K1 está inactivo
    {
      switch (K1State)
      {
      case INACTIVE:
        setTimeRelay("K1");
        break;
      case WAITING:
        if (timer[0] == 0)
        {
          relayStart(K1, K1State);
          sendRelayStatus("K1", "ACTIVE");
        }
        else
        {
          Serial.println("El tiempo de riego no está confirmado");
        }
        break;
      case ACTIVE:
        relayPause(K1, K1State);
        sendRelayStatus("K1", "PAUSE");
        break;
      case PAUSE:
        relayContinue(K1, K1State);
        sendRelayStatus("K1", "ACTIVE");
        break;
      case OFF:
        K1State = INACTIVE;
        sendRelayStatus("K1", "INACTIVE");
        break;
      }
    }
  }
  else if (btnK1.read() == HIGH && btnK1.currentDuration() == 3000) // APAGAR K1 DESPUÉS DE 3 SEGUNDOS
  {
    remainingTime[0] = config.getRelayTimerSelected("K1");
    relayInactive(K1, K1State);
    LCD_UpdateTimer("K1");
    sendRelayStatus("K1", "INACTIVE");
    sendRemainingTime();
  }

  if (btnK2.fell()) // BOTÓN K2
  {
    timeOnDisplay = millis();
        if (displayState == false)
    {
      displayLCD.onDisplay();
      setMenuPage();
      displayState = true;
    }
    bool K2Active = config.getRelayActive("K2");
    if (K2Active) // Si el relé K2 está activo
    {
      switch (K2State)
      {
      case INACTIVE:
        setTimeRelay("K2");
        break;
      case WAITING:
        if (timer[1] == 0)
        {
          relayStart(K2, K2State);
          sendRelayStatus("K2", "ACTIVE");
        }
        else
        {
          Serial.println("El tiempo de riego no está confirmado");
        }
        break;
      case ACTIVE:
        relayPause(K2, K2State);
        sendRelayStatus("K2", "PAUSE");
        break;
      case PAUSE:
        relayContinue(K2, K2State);
        sendRelayStatus("K2", "ACTIVE");
        break;
      case OFF:
        K2State = INACTIVE;
        sendRelayStatus("K2", "INACTIVE");
        break;
      }
    }
  }
  else if (btnK2.read() == HIGH && btnK2.currentDuration() == 3000) // APAGAR K2 DESPUÉS DE 3 SEGUNDOS
  {
    remainingTime[1] = config.getRelayTimerSelected("K2");
    relayInactive(K2, K2State);
    LCD_UpdateTimer("K2");
    sendRelayStatus("K2", "INACTIVE");
    sendRemainingTime();
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

//***** FUNCIONES DE OBTENCIÓN DE DATOS

void getTime() {
    static bool initialized = false;
    static unsigned long lastAttemptTime = 0;
    const unsigned long interval = 30000; // Intervalo de reintento en milisegundos

    if (!initialized || millis() - lastAttemptTime >= interval) {
        if (!initialized) {
            Serial.println("Configurando la hora...");
            initialized = true;
        }
        // Intenta configurar la hora
        configTime(3600, 0, "pool.ntp.org");
        lastAttemptTime = millis();
    }

    // Verifica si se ha configurado la hora correctamente
    time_t now = time(nullptr);
    if (now > 0) {
        // La hora se ha configurado correctamente
        struct tm timeinfo;
        localtime_r(&now, &timeinfo);
        
        // Formatea la hora para mostrarla en la pantalla LCD
        char timeStr[20];
        strftime(timeStr, sizeof(timeStr), "%H:%M", &timeinfo);
        
        
        // Muestra la hora en la pantalla LCD
        displayLCD.printTime(timeStr);
    } else {
        // La hora todavía no se ha configurado
        Serial.println("Error al configurar la hora. Reintentando...");
    }
}


void getTemperature() // OBTENER LA TEMPERATURA DEL SENSOR BMP280
{
  float temperature = bmp.readTemperature();
  displayLCD.printTemp(temperature);
}
