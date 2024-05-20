#ifndef LOG_H
#define LOG_H

#include <ArduinoJson.h>
#include <SPIFFS.h>

class LogManager {
private:
  StaticJsonDocument<2048> logDoc; // Documento JSON para los registros
  const String logFilePath = "/logs.json"; // Ruta del archivo de logs

public:
  LogManager() {}

  StaticJsonDocument<2048>& getLogDoc() { // MÉTODO PARA OBTENER EL DOCUMENTO JSON
    return logDoc;
  }

  void initSPIFFS() { // MÉTODO PARA INICIAR EL SISTEMA DE ARCHIVOS SPIFFS
    if (!SPIFFS.begin(true)) {
      Serial.println("Failed to mount file system");
      return;
    }
  }

  void loadLogs() { // MÉTODO PARA CARGAR LOS REGISTROS DESDE EL ARCHIVO JSON
    File logFile = SPIFFS.open(logFilePath, "r");
    if (!logFile) {
      Serial.println("Failed to open log file");
      return;
    }

    DeserializationError error = deserializeJson(logDoc, logFile);
    if (error) {
      Serial.println("Failed to parse log file");
      return;
    }

    logFile.close();
  }

  void saveLog(String date, String error) { // MÉTODO PARA GUARDAR UN REGISTRO EN EL ARCHIVO JSON
    // Cargar los registros existentes
    loadLogs();

    // Obtener el array de logs, o crearlo si no existe
    JsonArray logs = logDoc["logs"].as<JsonArray>();
    if (logs.isNull()) {
      logs = logDoc.createNestedArray("logs");
    }

    // Añadir el nuevo registro
    JsonObject logEntry = logs.createNestedObject();
    logEntry["date"] = date;
    logEntry["error"] = error;

    // Escribir los logs de vuelta en el archivo
    File logFile = SPIFFS.open(logFilePath, "w");
    if (!logFile) {
      Serial.println("Failed to open log file for writing");
      return;
    }

    if (serializeJson(logDoc, logFile) == 0) {
      Serial.println("Failed to write to log file");
    }

    logFile.close();
    Serial.println("Log saved: " + date + " - " + error);
  }

  void printLogs() { // MÉTODO PARA IMPRIMIR TODOS LOS REGISTROS
    File logFile = SPIFFS.open(logFilePath, "r");
    if (!logFile) {
      Serial.println("Failed to open log file");
      return;
    }

    while (logFile.available()) {
      Serial.write(logFile.read());
    }
    logFile.close();
  }
};

#endif
