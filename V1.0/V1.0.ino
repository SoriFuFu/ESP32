#include "config.h"
#include "Pantalla.h"
#include "log.h"
#include "RTClib.h"
#include <Bounce2.h>
#include "wifi_config.h"
#include <ESPAsyncWebServer.h>
#include <Ticker.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>

ConfigManager config;                    // OBJETO DE LA CLASE ConfigManager
WifiConfig wifiConfig;                   // OBJETO DE LA CLASE WifiConfig
Pantalla displayLCD(LCD_ADDR, SDA, SCL); // OBJETO DE LA PANTALLA LCD                                   // OBJETO DEL SENSOR BMP280
LogManager logSave;                      // OBJETO DE LA CLASE LogManager
RTC_DS3231 rtc;                          // OBJETO DE LA CLASE RTC_DS3231
Ticker tickerButton;
Ticker ticker500ms;
Ticker ticker1s;
Ticker ticker1min;
Ticker ticker5min;
Ticker ticker24h;
Ticker tickerSupervisor;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// VARIABLES DE ENTRADA
#define LEFT 25  // PIN PARA EL BOTÓN DE LEFT
#define UP 26    // PIN PARA EL BOTÓN DE UP
#define RIGHT 27 // PIN PARA EL BOTÓN DE RIGHT
#define DOWN 33  // PIN PARA EL BOTÓN DE DOWN
#define OK 32    // PIN PARA EL BOTÓN DE OK

#define BTN_K1 34 // PIN PARA EL BOTÓN DE K1
#define BTN_K2 35 // PIN PARA EL BOTÓN DE K2

// VARIABLES DE SALIDA
#define K1 15    // RELÉ K1
#define K1ON 5   // LED K1 ON
#define K1OFF 17 // LED K1 OFF
#define K2 2     // RELÉ K2
#define K2ON 19  // LED K2 ON
#define K2OFF 18 // LED K2 OFF
#define K3 4     // RELÉ K3
#define K4 16    // RELÉ K4

// CREAR OBJETOS DE LA CLASE BOUNCE PARA CADA BOTÓN
Bounce btnOk = Bounce();    // OK
Bounce btnUp = Bounce();    // ARRIBA
Bounce btnDown = Bounce();  // ABAJO
Bounce btnLeft = Bounce();  // IZQUIERDA
Bounce btnRight = Bounce(); // DERECHA
Bounce btnK1 = Bounce();    // BOTÓN DE K1
Bounce btnK2 = Bounce();    // BOTÓN DE K2

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

// VARIABLES PARA EL CONTADOR APAGADO/ENCENDIDO
bool timer[4] = {false, false};
bool stateTimer[4] = {false, false};

// VARIABLES PARA EL TIEMPO RESTANTE
unsigned long remainingTime[2] = {0, 0};
unsigned long timeOnDisplay = 0;
bool displayState = true;
int menuPage = 1;

// VARIABLES DE SEGUIMIENTO DE LOS TICKERS
unsigned long ticker500msSupervisor = 0;
unsigned long ticker1sSupervisor = 0;
unsigned long ticker1minSupervisor = 0;
unsigned long ticker5minSupervisor = 0;

int wifiReconnectCount = 0;

bool webSocketStatus = false;

// SETUP DE LA APLICACIÓN PRINCIPAL
void setup()
{
  Serial.begin(115200);
  // // Deshabilitar el watchdog de hardware del núcleo 0
  // disableCore0WDT();

  // // Deshabilitar el watchdog de hardware del núcleo 1
  // disableCore1WDT();

  //***** CONFIGURACIÓN DE LOS BOTONES DE ENTRADA
  btnOk.attach(OK, INPUT_PULLDOWN);       // RESET COMO PULLUP
  btnUp.attach(UP, INPUT_PULLDOWN);       // UP COMO PULLDOWN
  btnDown.attach(DOWN, INPUT_PULLDOWN);   // DOWN COMO PULLDOWN
  btnLeft.attach(LEFT, INPUT_PULLDOWN);   // LEFT COMO PULLDOWN
  btnRight.attach(RIGHT, INPUT_PULLDOWN); // DERECHA COMO PULLDOWN
  btnK1.attach(BTN_K1, INPUT_PULLDOWN);   // ENTRADA BOTON K1 COMO PULLDOWN
  btnK2.attach(BTN_K2, INPUT_PULLDOWN);   // ENTRADA BOTON K2 COMO PULLDOWN
  // btnK3.attach(BTN_K3, INPUT_PULLDOWN);   // ENTRADA BOTON K3 COMO PULLDOWN
  // btnK4.attach(BTN_K4, INPUT_PULLDOWN);   // ENTRADA BOTON K4 COMO PULLDOWN

  //***** ESTABLECER EL TIEMPO DE REBOTE DE CADA BOTÓN
  btnOk.interval(20);
  btnUp.interval(20);
  btnDown.interval(20);
  btnLeft.interval(20);
  btnRight.interval(20);
  btnK1.interval(50);
  btnK2.interval(50);
  // btnK3.interval(20);
  // btnK4.interval(20);

  //***** CONFIGURACIÓN DE LOS RELÉS DE SALIDA Y ESTABLECERLOS EN BAJO
  pinMode(K1, OUTPUT);
  pinMode(K2, OUTPUT);
  pinMode(K3, OUTPUT);
  pinMode(K4, OUTPUT);
  pinMode(K1ON, OUTPUT);
  pinMode(K1OFF, OUTPUT);
  pinMode(K2ON, OUTPUT);
  pinMode(K2OFF, OUTPUT);
  digitalWrite(K1, HIGH);
  digitalWrite(K1ON, LOW);
  digitalWrite(K1OFF, HIGH);
  digitalWrite(K2, HIGH);
  digitalWrite(K2ON, LOW);
  digitalWrite(K2OFF, HIGH);
  // digitalWrite(K3, LOW);
  // digitalWrite(K4, LOW);

  config.setAllRelayState("INACTIVE");

  // Deshabilitar el watchdog de software del bucle
  disableLoopWDT();
  delay(1000);
  if (!rtc.begin())
  {
    Serial.println("Módulo RTC no detectado");
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  SPIFFS.begin();      // Inicializar el sistema de archivos SPIFFS
  config.loadConfig(); // Cargar la configuración al inicio
  DateTime now = rtc.now();
  String date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
  String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  String DateTimeLog = date + " " + time;
  config.setLastReboot(DateTimeLog);
  //***** CONFIGURACIÓN DE LA PANTALLA LCD
  bool lcdActive = displayLCD.initDisplay();
  if (!lcdActive)
  {
    Serial.println("Pantalla LCD no detectada");
  }
  else
  {
    Serial.println("Pantalla LCD iniciada");
  }

  //***** INICIALIZAR LA CONEXIÓN WIFI Y AP
  bool wifiActive = config.getWifiActive(); // Verificar si el WiFi está activado
  bool apActive = config.getApActive();     // Verificar si el AP está activado

  initConnection(wifiActive, apActive); // Inicializar la conexión WiFi y AP

  //***** CONFIGURAR EL CANAL WEBSOCKET PARA LA CONFIGURACIÓN
  ws.onEvent(onEvent);
  server.addHandler(&ws);

  //***** CONFIGURAR EL SERVIDOR HTTP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });
  server.onNotFound([](AsyncWebServerRequest *request)
                    { request->send(SPIFFS, "/index.html", "text/html"); });
  server.serveStatic("/", SPIFFS, "/");

  //***** INICIAR EL SERVIDOR HTTP
  server.begin();
  Serial.println("Servidor HTTP iniciado");

  //***** CONFIGURAR EL RTC

  setMenuPage();
  getConfigData();

  // Configurar el ticker para la escucha de los botones cada 10 ms
  tickerButton.attach_ms(10, handleButtons);

  // Otros tickers para tareas periódicas
  ticker500ms.attach(0.5, task500ms);         // Se ejecuta cada 500 ms
  ticker1s.attach(1, task1s);                 // Se ejecuta cada 1 segundo
  ticker1min.attach(60, task1min);            // Se ejecuta cada 1 minuto
  ticker5min.attach(300, task5min);           // Se ejecuta cada 5 minutos
  tickerSupervisor.attach(1, taskSupervisor); // Supervisor cada 1 segund
  logSave.saveLog(DateTimeLog, "Sistema iniciado");
}

void getConfigData()
{
  remainingTime[0] = config.getRelayTimerSelected("K1");
  remainingTime[1] = config.getRelayTimerSelected("K2");

  config.setRelayRemainingTime("K1", remainingTime[0]);
  config.setRelayRemainingTime("K2", remainingTime[1]);

  LCD_UpdateTimer("K1");
  LCD_UpdateTimer("K2");
}

//***** FUNCIONES DE LA PANTALLA LCD
void setMenuPage() // ESTABLECER LA PÁGINA DEL MENÚ EN LA PANTALLA
{
  if (menuPage == 1)
  {
    String K1Name = config.getRelayName("K1");
    displayLCD.page(K1Name, remainingTime[0]);
  }
  else if (menuPage == 2)
  {
    String K2Name = config.getRelayName("K2");
    displayLCD.page(K2Name, remainingTime[1]);
  }
}

void LCD_UpdateTimer(String relay) // ACTUALIZAR EL TEMPORIZADOR EN LA PANTALLA
{
  if (relay == "K1" && menuPage == 1)
  {
    displayLCD.countPage(remainingTime[0]);
  }
  else if (relay == "K2" && menuPage == 2)
  {
    displayLCD.countPage(remainingTime[1]);
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
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len, uint8_t num)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, (char *)data);

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    String action = doc["action"];
    Serial.print("Acción WebSocket recibida: ");
    String relayName = doc["relay"];
    Serial.println(action);

    if (action == "GETCONFIG")
    {
      updateClientConfig();
    }
    else if (action == "GETNETWORKS")
    {
      handleGetNetworks(num);
    }
    else if (action == "SETWIFICONFIG")
    {
      handleSetWifiConfig(num, doc);
    }
    else if (action == "SETAPCONFIG")
    {
      handleSetApConfig(num, doc);
    }
    else if (action == "RESET")
    {
      reset();
    }
    else if (action == "FACTORYRESET")
    {
      factoryReset();
    }
    else if (action == "GETLOGS")
    {
      getLogs();
    }
    else if (action == "SETRELAYACTIVE" || action == "SETRELAYNAME")
    {
      handleSetRelayConfig(doc);
    }
    else if (action == "RELAYHANDLER")
    {
      String relay = doc["relay"];
      int relayNumber = relay == "K1" ? K1 : K2;

      String command = doc["command"];
      handleAction(relayNumber, relay, command, doc, relay == "K1" ? K1State : K2State);
    }
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  if (type == WS_EVT_CONNECT)
  {
    Serial.print("Cliente ");
    Serial.print(client->id());
    Serial.println(" conectado");
    webSocketStatus = true;
  }
  else if (type == WS_EVT_DISCONNECT)
  {
    Serial.print("Cliente ");
    Serial.print(client->id());
    Serial.println(" desconectado");
    webSocketStatus = false;
  }
  else if (type == WS_EVT_DATA)
  {
    handleWebSocketMessage(arg, data, len, client->id());
  }
}

void updateClientConfig() // ACTUALIZAR LA CONFIGURACIÓN DEL CLIENTE
{
  if (webSocketStatus)
  {
    StaticJsonDocument<1024> configDoc = config.getConfigDoc(); // Obtener el documento JSON de configuración
    StaticJsonDocument<1280> jsonDocument;                      // Documento combinado, debe ser lo suficientemente grande para contener ambos JSON

    // Añadir la acción y los datos de configuración al nuevo documento JSON
    jsonDocument["action"] = "GETCONFIG";
    jsonDocument["Data"] = configDoc.as<JsonVariant>(); // Inserta el JsonDocument dentro del nuevo JSON

    String configJson;
    serializeJson(jsonDocument, configJson); // Serializar el JSON combinado a un String

    ws.textAll(configJson); // Enviar a todos los clientes conectados
  }
}

void getLogs() // ACTUALIZAR LA CONFIGURACIÓN DEL CLIENTE
{
  if (webSocketStatus)
  {
    StaticJsonDocument<2048> logDoc = logSave.getLogDoc(); // Obtener el documento JSON de los logs
    StaticJsonDocument<1280> jsonDocument;                 // Documento combinado, debe ser lo suficientemente grande para contener ambos JSON

    // Añadir la acción y los datos de los logs al nuevo documento JSON
    jsonDocument["action"] = "GETLOGS";
    jsonDocument["Data"] = logDoc.as<JsonVariant>(); // Inserta el JsonDocument dentro del nuevo JSON

    String logsJson;
    serializeJson(jsonDocument, logsJson); // Serializar el JSON combinado a un String

    ws.textAll(logsJson); // Enviar a todos los clientes conectados
  }
}

void sendRemainingTime() // ENVIAR EL TIEMPO RESTANTE
{
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["action"] = "UPDATE_TIMER";
  jsonDocument["timeK1"] = remainingTime[0];
  jsonDocument["timeK2"] = remainingTime[1];

  char jsonString[100];
  serializeJson(jsonDocument, jsonString);
  ws.textAll(jsonString);
}

void sendTimerStatus(String relay, bool timerState) // ENVIAR EL ESTADO DEL TEMPORIZADOR
{
  if (webSocketStatus)
  {
    StaticJsonDocument<100> jsonDocument;
    jsonDocument["relay"] = relay;
    jsonDocument["action"] = "UPDATE_TIMER_STATUS";
    jsonDocument["status"] = timerState;
    char jsonString[100];
    serializeJson(jsonDocument, jsonString);
    ws.textAll(jsonString);
  }
}

void sendRelayStatus(String relay, String status) // ENVIAR EL ESTADO DEL RELÉ
{
  if (webSocketStatus)
  {
    StaticJsonDocument<100> jsonDocument;
    jsonDocument["action"] = "UPDATE_RELAY_STATUS";
    jsonDocument["relay"] = relay;
    jsonDocument["command"] = status;
    char jsonString[100];
    serializeJson(jsonDocument, jsonString);
    ws.textAll(jsonString);
  }
}

bool sendMessage(String command, String message, String ip) // ENVIAR MENSAJE AL CLIENTE
{
  if (webSocketStatus)
  {
    String action = "MESSAGE";
    StaticJsonDocument<100> jsonDocument;
    jsonDocument["action"] = action;
    jsonDocument["command"] = command;
    jsonDocument["message"] = message;
    jsonDocument["ip"] = ip;
    String jsonString;
    serializeJson(jsonDocument, jsonString);
    ws.textAll(jsonString);
  }
  return true;
}

void handleGetNetworks(uint8_t num)
{
  wifiConfig.searchNetworks([num](const String &networksJson)
                            {
        // Enviar el resultado a través de WebSocket
        ws.text(num, networksJson);
        Serial.print("Redes WiFi encontradas: ");
        Serial.println(networksJson); });
}

void handleSetWifiConfig(uint8_t num, const JsonDocument &data)
{
  bool wifiActive = data["wifiActive"];
  IPAddress ip = wifiConfig.getIPAddress();
  String ipString = ip.toString();

  if (!wifiActive)
  {
    bool okSendMessage = sendMessage("SUCCESS", "Wifi desactivado", ipString);
    if (okSendMessage)
    {
      delay(3000);
      // Desactivar WiFi
      wifiConfig.disconnectWifi();
      config.setWifiActive(false);
      config.setWifiStatus(false);
      config.setWifiSSID("");
      config.setWifiPassword("");
      config.setWifiIP("");
      config.setWifiSubnet("");
      config.setWifiGateway("");
      displayLCD.clearWifi();
    }
  }
  else
  {
    // Configurar WiFi
    String ssid = data["ssid"];
    String password = data["password"];
    String ipConfig = data["ip"];
    String subnetConfig = data["subnet"];
    String gatewayConfig = data["gateway"];

    IPAddress ip;
    IPAddress subnet;
    IPAddress gateway;

    if (!ip.fromString(ipConfig) || !subnet.fromString(subnetConfig) || !gateway.fromString(gatewayConfig))
    {
      Serial.println("Error: Invalid network configuration");
      ws.text(num, "false");
      logSave.saveLog(getDateTime(), "Error en la configuración de red");
      return;
    }
    // wifiConfig.disconnectWifi();
    bool saveWifiConfigData = saveWifiConfig(ssid.c_str(), password.c_str(), ip, gateway, subnet);
    if (saveWifiConfigData)
    {
      logSave.saveLog(getDateTime(), "Configuración de red guardada");
      bool okSendMessage = sendMessage("SUCCESS_WIFI", "Configuración de red guardada, reiniciando el dispositivo", ipConfig);
      if (okSendMessage)
      {
        delay(3000);
        ESP.restart();
      }
    }
    else
    {
      sendMessage("ERROR", "Error al guardar la configuración de red", ipString);
      logSave.saveLog(getDateTime(), "Error al guardar la configuración de red");
    }
  }
}
bool saveWifiConfig(const String &ssid, const String &password, const IPAddress &ip, const IPAddress &gateway, const IPAddress &subnet)
{
  config.setWifiActive(true);
  config.setWifiStatus(true);
  config.setWifiSSID(ssid);
  config.setWifiPassword(password);
  config.setWifiIP(ip.toString());
  config.setWifiSubnet(subnet.toString());
  config.setWifiGateway(gateway.toString());
  return true;
}

void handleSetApConfig(uint8_t num, const JsonDocument &data)
{
  String active = data["active"];
  if (active == "true")
  {
    // Activar AP
    String ssid = config.getApSSID();
    String password = config.getApPassword();
    wifiConfig.initAP(ssid.c_str(), password.c_str());
    config.setApActive(true);
    config.setApStatus(true);
    IPAddress ip = wifiConfig.getAPIP();
    displayLCD.printAp();
    bool okSendMessage = sendMessage("SUCCESS_AP", "AP activado con el nombre " + ssid, ip.toString());
    if (okSendMessage)
    {
      Serial.println("AP activado");
    }
  }
  else if (active == "false")
  {
    // Desactivar AP
    config.setApActive(false);
    config.setApStatus(false);
    wifiConfig.stopAP();
    displayLCD.clearAp();
    bool okSendMessage = sendMessage("SUCCESS_AP", "AP desactivado", "");
    if (okSendMessage)
    {
      Serial.println("AP desactivado");
    }
  }
}

void handleSetRelayConfig(const JsonDocument &data)
{
  String relay = data["relay"];
  String action = data["action"];
  if (action == "SETRELAYACTIVE")
  {
    bool relayActive = data["KActive"];
    config.setRelayActive(relay, relayActive);
    String state = relayActive ? "activado" : "desactivado";
    bool okSendMessage = sendMessage("SUCCESS_RELAY", "Relé " + relay + " " + state, "");
    if (okSendMessage)
    {
      Serial.print("Relé ");
      Serial.print(relay);
      Serial.println(state);
    }
  }
  else if (action == "SETRELAYNAME")
  {
    String relayName = data["relayName"];
    config.setRelayName(relay, relayName);
    displayLCD.printName(relayName);
    bool okSendMessage = sendMessage("SUCCESS_RELAY", "Nombre del relé " + relay + " actualizado", "");
    if (okSendMessage)
    {
      Serial.print("Nombre del relé ");
      Serial.print(relay);
      Serial.println(" actualizado");
    }
  }
}

void handleAction(const int relayNumber, const String &relay, const String &command, const JsonDocument &data, RelayState &state)
{
  Serial.print("Relé: ");
  Serial.println(relay);

  Serial.print("Comando: ");
  Serial.println(command);

  int remainingNumber = relay == "K1" ? 0 : 1;
  if (command == "SET_TIMER")
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
  else if (command == "ON")
  {
    relayOn(relayNumber, relay, state); // Corregido
  }
  else if (command == "ACTIVE")
  {
    relayStart(relayNumber, relay, state); // Corregido
  }
  else if (command == "OFF")
  {
    relayStop(relayNumber, relay, state); // Corregido
    setState(relay, INACTIVE);
  }
  else if (command == "INACTIVE")
  {

    relayStop(relayNumber, relay, state); // Corregido
    remainingTime[remainingNumber] = config.getRelayTimerSelected(relay);
    config.setRelayRemainingTime(relay, remainingTime[remainingNumber]);
    LCD_UpdateTimer(relay);
    sendRemainingTime();
    setState(relay, INACTIVE);
  }
  else if (command == "PAUSE")
  {

    relayPause(relayNumber, relay, state); // Corregido
  }
  else if (command == "CONTINUE")
  {
    relayContinue(relayNumber, relay, state); // Corregido
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
}

void relayOn(int relayPin, String relay, RelayState &state) // ENCENDER EL RELÉ
{
  int ledOn = relay == "K1" ? K1ON : K2ON;
  int ledOff = relay == "K1" ? K1OFF : K2OFF;
  digitalWrite(ledOff, LOW);
  digitalWrite(ledOn, HIGH);
  digitalWrite(relayPin, LOW);
  state = ON;
  config.setRelayState(relay, "ON");
  config.setRelayIsOn(relay, true);
  sendRelayStatus(relay, "ON");
}
void relayStart(int relayPin, String relay, RelayState &state) // ACTIVAR EL RELÉ
{
  int ledOn = relay == "K1" ? K1ON : K2ON;
  int ledOff = relay == "K1" ? K1OFF : K2OFF;
  digitalWrite(ledOff, LOW);
  digitalWrite(ledOn, HIGH);
  digitalWrite(relayPin, LOW);
  state = ACTIVE;
  config.setRelayState(relay, "ACTIVE");
  sendRelayStatus(relay, "ACTIVE");
}
void relayPause(int relayPin, String relay, RelayState &state) // PAUSAR EL RELÉ
{
  int ledOn = relay == "K1" ? K1ON : K2ON;
  int ledOff = relay == "K1" ? K1OFF : K2OFF;
  digitalWrite(ledOn, LOW);
  digitalWrite(ledOff, HIGH);
  digitalWrite(relayPin, HIGH);
  state = PAUSE;
  config.setRelayState(relay, "PAUSE");
  sendRelayStatus(relay, "PAUSE");
}
void relayContinue(int relayPin, String relay, RelayState &state) // CONTINUAR EL RELÉ DESPUÉS DE PAUSARLO
{
  int ledOn = relay == "K1" ? K1ON : K2ON;
  int ledOff = relay == "K1" ? K1OFF : K2OFF;
  digitalWrite(ledOff, LOW);
  digitalWrite(ledOn, HIGH);
  digitalWrite(relayPin, LOW);
  state = ACTIVE;
  config.setRelayState(relay, "ACTIVE");
  sendRelayStatus(relay, "ACTIVE");
}
void relayStop(int relayPin, String relay, RelayState &state) // APAGAR EL RELÉ
{
  int ledOn = relay == "K1" ? K1ON : K2ON;
  int ledOff = relay == "K1" ? K1OFF : K2OFF;
  digitalWrite(ledOn, LOW);
  digitalWrite(ledOff, HIGH);
  digitalWrite(relayPin, HIGH);
  state = INACTIVE;
  config.setRelayState(relay, "INACTIVE");
  config.setRelayIsOn(relay, false);
  sendRelayStatus(relay, "INACTIVE");
}
void relayInactive(int relayPin, String relay, RelayState &state) // APAGAR EL RELÉ
{
  int ledOn = relay == "K1" ? K1ON : K2ON;
  int ledOff = relay == "K1" ? K1OFF : K2OFF;
  digitalWrite(ledOn, LOW);
  digitalWrite(ledOff, HIGH);
  digitalWrite(relayPin, HIGH);
  state = OFF;
  config.setRelayState(relay, "INACTIVE");
  config.setRelayIsOn(relay, false);
  sendRelayStatus(relay, "INACTIVE");
}
void setManualTimer() // MOSTRAR PARPADEO
{
  if (timer[0] == 1 && menuPage == 1) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K1
  {
    if (stateTimer[0] == false)
    {
      displayLCD.cleanCountPage();
      stateTimer[0] = true;
    }
    else
    {
      displayLCD.countPage(remainingTime[0]);
      stateTimer[0] = false;
    }
  }
  else if (timer[1] == 1 && menuPage == 2) // MOSTRAR PARPADEO AL SELECCIONAR EL TIEMPO EN K2
  {
    if (stateTimer[1] == false)
    {
      displayLCD.cleanCountPage();
      stateTimer[1] = true;
    }
    else
    {
      displayLCD.countPage(remainingTime[1]);
      stateTimer[1] = false;
    }
  }
}

void finallyRelayTimer(int relay, String relayNumber, RelayState &state, unsigned long &remainingTime) // FINALIZAR EL TEMPORIZADOR DEL RELÉ
{
  relayInactive(relay, relayNumber, state);
  remainingTime = config.getRelayTimerSelected(relayNumber);
  config.setRelayRemainingTime(relayNumber, remainingTime);
  config.setRelayState(relayNumber, "INACTIVE");
  sendRemainingTime();
  sendRelayStatus(relayNumber, "INACTIVE");
  LCD_UpdateTimer(relayNumber);
}

void updateTimeRelay() // ACTUALIZAR EL TIEMPO RESTANTE DEL RIEGO
{

  if (K1State == ACTIVE || K2State == ACTIVE)
  {
    sendRemainingTime();
  }

  if (K1State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K1
  {
    remainingTime[0] -= 1000;
    config.setRelayRemainingTime("K1", remainingTime[0]);
    LCD_UpdateTimer("K1");
    if (remainingTime[0] == 0)
    {
      finallyRelayTimer(K1, "K1", K1State, remainingTime[0]);
    }
  }
  if (K2State == ACTIVE) // ACTUALIZAR EL TIEMPO RESTANTE EN K2
  {
    remainingTime[1] -= 1000;
    config.setRelayRemainingTime("K2", remainingTime[1]);
    LCD_UpdateTimer("K2");
    if (remainingTime[1] == 0)
    {
      finallyRelayTimer(K2, "K2", K2State, remainingTime[1]);
    }
  }
}

//***** FUNCIONES DE LOOP
void handleButtons() // MANEJAR LOS BOTONES
{
  btnOk.update();
  btnUp.update();
  btnDown.update();
  btnLeft.update();
  btnRight.update();
  btnK1.update();
  btnK2.update();

  if (btnOk.fell()) // CONFIRMACIÓN DE LA INSERCION DEL TIEMPO DE RIEGO MANUAL
  {
    if (displayState == false)
    {
      displayLCD.onDisplay();
    }
    if (timer[0] == 1 && menuPage == 1)
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
    else if (timer[1] == 1 && menuPage == 2)
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
  }

  if (btnUp.fell()) // BOTON UP AUMENTA EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    if (displayState == false)
    {
      displayLCD.onDisplay();
    }
    if (timer[0] == 1)
    {
      remainingTime[0] += 60000;
    }
    else if (timer[1] == 1)
    {
      remainingTime[1] += 60000;
    }
  }

  if (btnDown.fell()) // BOTON DOWN DISMINUYE EL TIEMPO DE RIEGO EN 1 MINUTO
  {
    if (displayState == false)
    {
      displayLCD.onDisplay();
    }
    if (timer[0] == 1)
    {
      remainingTime[0] -= 60000;
    }
    else if (timer[1] == 1)
    {
      remainingTime[1] -= 60000;
    }
  }

  if (btnLeft.fell()) // BOTON LEFT CAMBIA A LA PÁGINA ANTERIOR
  {
    if (displayState == false)
    {
      displayLCD.onDisplay();
    }
    if (menuPage > 1)
    {
      menuPage -= 1;
      setMenuPage();
    }
  }

  if (btnRight.fell()) // BOTON DERECHA CAMBIA A LA PÁGINA SIGUIENTE
  {
    if (displayState == false)
    {
      displayLCD.onDisplay();
    }
    if (menuPage < 2)
    {
      menuPage += 1;
      setMenuPage();
    }
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
          relayStart(K1, "K1", K1State);
          sendRelayStatus("K1", "ACTIVE");
        }
        else
        {
          Serial.println("El tiempo de riego no está confirmado");
        }
        break;
      case ACTIVE:
        relayPause(K1, "K1", K1State);
        sendRelayStatus("K1", "PAUSE");
        break;
      case PAUSE:
        relayContinue(K1, "K1", K1State);
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
    relayInactive(K1, "K1", K1State);
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
          relayStart(K2, "K2", K2State);
          sendRelayStatus("K2", "ACTIVE");
        }
        else
        {
          Serial.println("El tiempo de riego no está confirmado");
        }
        break;
      case ACTIVE:
        relayPause(K2, "K2", K2State);
        sendRelayStatus("K2", "PAUSE");
        break;
      case PAUSE:
        relayContinue(K2, "K2", K2State);
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
    relayInactive(K2, "K2", K2State);
    LCD_UpdateTimer("K2");
    sendRelayStatus("K2", "INACTIVE");
    sendRemainingTime();
  }
}

void task500ms()
{
  setManualTimer();
  ticker500msSupervisor = millis();
}

void task1s()
{
  printLocalTime();
  getMemory();
  updateTimeRelay();
  ticker1sSupervisor = millis();
}

void task1min()
{
  verifyWifiConnection();
  ticker1minSupervisor = millis();
}

void task5min()
{
  LCD_OFF();
  displayState = false;
  ticker5minSupervisor = millis();
}

void taskSupervisor()
{
  unsigned long currentMillis = millis();

  if (currentMillis - ticker500msSupervisor >= 2000)
  {
    ticker500ms.detach();               // Detener el ticker de 500 ms
    ticker500ms.attach(0.5, task500ms); // Reiniciar el ticker de 500 ms
    logSave.saveLog(getDateTime(), "Temporizador de 500 ms bloqueado. Reiniciando...");
    ticker500msSupervisor = millis();
  }

  if (currentMillis - ticker1minSupervisor >= 120000)
  {
    ticker1min.detach();             // Detener el ticker de 1 minuto
    ticker1min.attach(60, task1min); // Reiniciar el ticker de 1 minuto
    logSave.saveLog(getDateTime(), "Temporizador de 1 minuto bloqueado. Reiniciando...");
    ticker1minSupervisor = millis();
  }

  if (currentMillis - ticker1sSupervisor >= 2000)
  {
    ticker1s.detach();          // Detener el ticker de 1 segundo
    ticker1s.attach(1, task1s); // Reiniciar el ticker de 1 segundo
    logSave.saveLog(getDateTime(), "Temporizador de 1 segundo bloqueado. Reiniciando...");
    ticker1sSupervisor = millis();
  }
}

//***** FUNCIONES DEL SISTEMA
void reset() // REINICIAR EL DISPOSITIVO
{
  bool okSendMessage = sendMessage("RESET", "Reiniciando el dispositivo", "");
  if (okSendMessage)
  {
    logSave.saveLog(getDateTime(), "Dispositivo reiniciado");
    delay(3000);
    ESP.restart();
  }
}

void factoryReset() // RESTABLECER LA CONFIGURACIÓN DE FÁBRICA
{
  Serial.println("Restableciendo la configuración de fábrica...");
  logSave.saveLog(getDateTime(), "Restableciendo la configuración de fábrica");
  bool factoryResetState = config.factoryReset();
  if (factoryResetState)
  {
    bool okSendMessage = sendMessage("FACTORYRESET", "Configuración de fábrica restablecida", "");
    if (okSendMessage)
    {
      logSave.saveLog(getDateTime(), "Configuración de fábrica restablecida");
      delay(3000);
      ESP.restart();
    }
  }
  else
  {
    logSave.saveLog(getDateTime(), "Error al restablecer la configuración de fábrica");
    bool okSendMessage = sendMessage("ERROR", "Error al restablecer la configuración de fábrica", "");
    Serial.println("Error al restablecer la configuración de fábrica");
  }
}

bool wifiConnection() // INICIAR LA CONEXIÓN WIFI
{
  String wifiSSID = config.getWifiSSID();
  String wifiPassword = config.getWifiPassword();
  IPAddress ip = config.getWifiIP();
  IPAddress subnet = config.getWifiSubnet();
  IPAddress gateway = config.getWifiGateway();
  bool wifiState = wifiConfig.initWifi(wifiSSID.c_str(), wifiPassword.c_str(), ip, subnet, gateway);
  if (wifiState)
  {
    displayLCD.printWifi();
    logSave.saveLog(getDateTime(), "Conexión WiFi establecida");
    return true;
  }
  else
  {
    displayLCD.clearWifi();
    logSave.saveLog(getDateTime(), "Error al iniciar la conexión WiFi, iniciando AP por defecto");
    return false;
  }
}

void verifyWifiConnection() // VERIFICAR LA CONEXIÓN WIFI
{

  bool wifiStatus = config.getWifiStatus();
  if (wifiStatus)
  {
    String wifiSSID = config.getWifiSSID();
    if (wifiSSID != "")
    {
      bool verifyWifi = wifiConfig.verifyWifiConnection();
      if (verifyWifi)
      {
        displayLCD.printWifi();
        Serial.println("Conexión WiFi correcta");
      }
      else
      {
        if (wifiReconnectCount > 5)
        {
          logSave.saveLog(getDateTime(), "Reintentos de conexión WiFi excedidos");
          Serial.println("Reintentos de conexión WiFi excedidos");
          config.setWifiStatus(false);
        }
        else
        {
          Serial.println("Reintentando la conexión WiFi...");
          wifiReconnect();
        }
      }
    }
  }
}

void wifiReconnect() // RECONEXIÓN WIFI
{

  Serial.println("Conexión WiFi fallida, reintentando...");
  bool wifiConnectionState = wifiConnection();
  if (wifiConnectionState)
  {
    displayLCD.printWifi();
    Serial.println("Conexión WiFi establecida");
    wifiReconnectCount = 0;
  }
  else
  {
    displayLCD.printAp();

    Serial.println("Error al conectar a la red WiFi");
    int remainingReconnect = 6 - wifiReconnectCount;
    Serial.print(remainingReconnect);
    Serial.println(" intentos restantes.");
    Serial.println("Volviendo en 5 minutos...");
    wifiReconnectCount++;
  }
}

bool apConnection() // INICIAR EL PUNTO DE ACCESO
{
  String apSSID = config.getApSSID();
  String apPassword = config.getApPassword();
  bool apState = wifiConfig.initAP(apSSID.c_str(), apPassword.c_str());
  if (apState)
  {
    displayLCD.printAp();
    logSave.saveLog(getDateTime(), "Conexión AP establecida");
    return true;
  }
  else
  {
    displayLCD.clearAp();
    logSave.saveLog(getDateTime(), "Error al iniciar el punto de acceso");
    return false;
  }
}

void initConnection(bool wifiActive, bool apActive) // INICIAR PRIMERA CONEXIÓN
{
  wifiReconnectCount = 1;
  if (wifiActive && apActive) // Inicializar WiFi y AP
  {
    displayLCD.printText("Conectando a WiFi...", 0, 1);
    displayLCD.printText("Conectando AP...", 0, 2);
    bool wifiState = wifiConnection();
    bool apState = apConnection();
    displayLCD.clearDisplay();
    if (wifiState && apState)
    {
      displayLCD.printWifi();
      config.setMacAddress(wifiConfig.getMACAddress()); // GUARDAR LA DIRECCIÓN MAC
    }
    else
    {
      bool apState = apConnection();
      if (apState)
      {
        displayLCD.printAp();
      }
    }
  }
  else if (wifiActive && !apActive) // Inicializar solo WiFi
  {
    displayLCD.printText("Conectando a WiFi...", 0, 1);
    bool wifiState = wifiConnection();
    if (wifiState)
    {
      displayLCD.printWifi();
      config.setMacAddress(wifiConfig.getMACAddress());
    }
    else
    {
      displayLCD.clearLine(1);
      displayLCD.clearLine(2);
      displayLCD.printText("Error, conectando AP", 0, 1);
      bool apState = apConnection();
      if (apState)
      {
        displayLCD.printAp();
      }
    }
  }

  else if (!wifiActive && apActive) // Inicializar solo AP
  {

    displayLCD.printText("Conectando AP", 0, 1);
    bool apState = apConnection();
    if (apState)
    {
      displayLCD.printAp();
    }
    else
    {
      Serial.println("Error al iniciar el punto de acceso, reintentando...");
      delay(1000);
      // Intentar de nuevo
      apState = apConnection();
      if (apState)
      {
        displayLCD.printAp();
      }
    }
  }
  else // Caso cuando ambos wifiActive y apActive son falsos. Este caso no debería ocurrir, pero manejamos esto para asegurarnos de que siempre haya una conexión
  {
    logSave.saveLog(getDateTime(), "Ambos WiFi y AP están desactivados");
    bool apState = apConnection();
    if (apState)
    {
      displayLCD.clearDisplay();
      displayLCD.printAp();
    }
    else
    {
      logSave.saveLog(getDateTime(), "Error al iniciar el punto de acceso.reintentando...");
      delay(1000);
      // Intentar de nuevo
      apState = apConnection();
      if (apState)
      {
        displayLCD.clearDisplay();
        displayLCD.printAp();
      }
    }
  }
}

String getDateTime() // OBTENER LA FECHA Y HORA
{
  DateTime now = rtc.now();
  String date = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year()); // Obtener la fecha (dd/mm/aaaa)
  String time = String(now.hour()) + ":" + String(now.minute()) + ":" + String(now.second());
  String dateTime = date + " " + time;
  return dateTime;
}

//***** FUNCIONES DE OBTENCIÓN DE DATOS

void printLocalTime() // IMPRIMIR LA HORA LOCAL
{
  DateTime now = rtc.now();

  int hour = now.hour();
  int minute = now.minute();
  int second = now.second();
  displayLCD.printTime(hour, minute, second);
  if (hour == 00 && minute == 00 && second == 00) // REINICIAR EL DISPOSITIVO A LA MEDIANOCHE
  {
    Serial.print(getDateTime());
    Serial.println(" - Reinicio de mantenimiento");
    reset();
  }
}

void getMemory() // OBTENER LA MEMORIA LIBRE
{

  int espTotalHeap = ESP.getHeapSize();
  int usedHeap = ESP.getMaxAllocHeap();                 // Obtener la memoria utilizada
  int useHeapPercent = (usedHeap * 100) / espTotalHeap; // Calcular el porcentaje de memoria utilizada
  // displayLCD.printMemory(useHeapPercent);
  if (useHeapPercent >= 95)
  {
    logSave.saveLog(getDateTime(), "Memoria RAM utilizada al " + String(useHeapPercent) + "%");
  }
}

//***** LOOP NO SE UTILIZA
void loop()
{
  ws.cleanupClients(); // Limpiar los clientes desconectados
}