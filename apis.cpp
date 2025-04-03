#include "apis.h"
#include "protocolo.h"
#include "utilidades.h"
#include "estructuras.h"
#include <ArduinoJson.h>

#ifdef ESP8266
  #include <ESP8266WebServer.h>
  extern ESP8266WebServer server;
#elif defined(ESP32)
  #include <WebServer.h>
  extern WebServer server;
#endif

// Inicialización de la API
void setupApi() {
  // Configurar rutas para la API REST
  server.on("/api/status", HTTP_GET, handleApiStatus);
  server.on("/api/relay", HTTP_POST, handleApiRelay);
  server.on("/api/command", HTTP_POST, handleApiCommand);
  server.on("/api/config", HTTP_GET, handleApiGetConfig);
  server.on("/api/config", HTTP_POST, handleApiSetConfig);
  server.on("/api/reset", HTTP_POST, handleApiReset);
}

// Implementación de endpoints de la API

// GET /api/status - Obtener el estado actual
bool apiGetStatus(String& response) {
  // Crear objeto JSON para la respuesta
  StaticJsonDocument<256> doc;
  
  // Añadir información de status
  doc["success"] = true;
  doc["deviceId"] = config.deviceId;
  doc["status"] = statusInfo.status;
  doc["statusHex"] = statusInfo.statusHex;
  
  // Agregar estado de bits individuales para facilitar uso
  JsonObject bits = doc.createNestedObject("bits");
  bits["ddmm1"] = isStatusBitSet(STATUS_DDMM1);
  bits["ddmm2"] = isStatusBitSet(STATUS_DDMM2);
  bits["relay1"] = isStatusBitSet(STATUS_RELAY1);
  bits["relay2"] = isStatusBitSet(STATUS_RELAY2);
  bits["tarjeta"] = isStatusBitSet(STATUS_TARJ);
  bits["fraude"] = isStatusBitSet(STATUS_FRAUDE);
  bits["pulso"] = isStatusBitSet(STATUS_PULS);
  bits["scanner"] = isStatusBitSet(STATUS_SCANNER);
  bits["salida"] = isStatusBitSet(STATUS_SALIDA);
  
  // Añadir datos RFID si hay
  if (strlen(statusInfo.rfidData) > 0) {
    doc["rfidData"] = statusInfo.rfidData;
  }
  
  // Serializar a JSON
  serializeJson(doc, response);
  
  return true;
}

// POST /api/relay - Activar/Desactivar un relé
bool apiActivateRelay(int relayNum, bool activate, String& response) {
  StaticJsonDocument<128> doc;
  bool success = false;
  
  if (relayNum >= 1 && relayNum <= 5) {
    if (activate) {
      success = activateRelay(relayNum);
    } else {
      success = deactivateRelay(relayNum);
    }
    
    doc["success"] = success;
    doc["relay"] = relayNum;
    doc["state"] = activate ? "activated" : "deactivated";
    doc["message"] = success ? 
                     (activate ? "Relé activado correctamente" : "Relé desactivado correctamente") : 
                     "Error al cambiar estado del relé";
  } else {
    doc["success"] = false;
    doc["message"] = "Número de relé inválido. Debe estar entre 1 y 5.";
  }
  
  serializeJson(doc, response);
  return success;
}

// POST /api/command - Enviar un comando al dispositivo OemAccess
bool apiSendCommand(const String& commandStr, String& response) {
  StaticJsonDocument<256> doc;
  
  // Procesar el comando
  const char* command = commandStr.c_str();
  CommandResponse cmdResponse = processCommand(command);
  
  // Preparar respuesta JSON
  doc["success"] = cmdResponse.success;
  doc["command"] = commandStr;
  doc["message"] = cmdResponse.message;
  
  if (strlen(cmdResponse.data) > 0) {
    doc["data"] = cmdResponse.data;
  }
  
  serializeJson(doc, response);
  return cmdResponse.success;
}

// GET /api/config - Obtener configuración actual
bool apiGetConfig(String& response) {
  StaticJsonDocument<384> doc;
  
  doc["deviceId"] = config.deviceId;
  doc["deviceIdStr"] = config.deviceIdStr;
  doc["companyName"] = config.nombre_empresa;
  doc["tcpipMode"] = config.modo_tcpip485;
  doc["workMode"] = config.modo_work;
  doc["displayMode"] = config.modo_display;
  doc["qrMode"] = config.modo_QR_8_12;
  doc["clockMode"] = config.modo_clock;
  doc["sensorMode"] = config.modo_sens_altura;
  doc["isEntrance"] = config.esPuertaEntrada;
  
  // Añadir información de relés
  JsonArray relaysArray = doc.createNestedArray("relays");
  for (int i = 0; i < 5; i++) {
    JsonObject relay = relaysArray.createNestedObject();
    relay["number"] = i + 1;
    relay["pin"] = relays[i].pin;
    relay["state"] = relays[i].state;
    relay["time"] = relays[i].time;
  }
  
  serializeJson(doc, response);
  return true;
}

// POST /api/config - Actualizar configuración
bool apiSetConfig(const String& configJson, String& response) {
  StaticJsonDocument<384> doc;
  DeserializationError error = deserializeJson(doc, configJson);
  
  if (error) {
    String errMsg = "Error al procesar JSON: ";
    errMsg += error.c_str();
    
    StaticJsonDocument<128> errorDoc;
    errorDoc["success"] = false;
    errorDoc["message"] = errMsg;
    serializeJson(errorDoc, response);
    return false;
  }
  
  bool needsRestart = false;
  
  // Actualizar configuración
  if (doc.containsKey("deviceId")) {
    uint8_t newId = doc["deviceId"];
    if (newId != config.deviceId) {
      saveDeviceId(newId);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("companyName")) {
    const char* name = doc["companyName"];
    saveCompanyName(name);
    strcpy(config.nombre_empresa, name);
  }
  
  if (doc.containsKey("tcpipMode")) {
    uint8_t mode = doc["tcpipMode"];
    if (mode != config.modo_tcpip485) {
      saveTcpIpMode(mode);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("workMode")) {
    uint8_t mode = doc["workMode"];
    if (mode != config.modo_work) {
      saveWorkMode(mode);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("displayMode")) {
    uint8_t mode = doc["displayMode"];
    if (mode != config.modo_display) {
      saveDisplayMode(mode);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("qrMode")) {
    uint8_t mode = doc["qrMode"];
    if (mode != config.modo_QR_8_12) {
      saveQRMode(mode);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("clockMode")) {
    uint8_t mode = doc["clockMode"];
    if (mode != config.modo_clock) {
      saveClockMode(mode);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("sensorMode")) {
    uint8_t mode = doc["sensorMode"];
    if (mode != config.modo_sens_altura) {
      saveSensorMode(mode);
      needsRestart = true;
    }
  }
  
  if (doc.containsKey("isEntrance")) {
    bool isEntrance = doc["isEntrance"];
    config.esPuertaEntrada = isEntrance;
    // No es necesario guardar, ya que se deduce del modo de trabajo
  }
  
  // Relés
  if (doc.containsKey("relays")) {
    JsonArray relaysArray = doc["relays"];
    for (JsonObject relay : relaysArray) {
      if (relay.containsKey("number") && relay.containsKey("time")) {
        int number = relay["number"];
        uint8_t time = relay["time"];
        if (number >= 1 && number <= 5) {
          setRelayTimer(number, time);
        }
      }
    }
  }
  
  // Preparar respuesta
  StaticJsonDocument<128> respDoc;
  respDoc["success"] = true;
  respDoc["message"] = "Configuración actualizada correctamente";
  respDoc["needsRestart"] = needsRestart;
  
  serializeJson(respDoc, response);
  return true;
}

// POST /api/reset - Reiniciar el dispositivo
bool apiReset(String& response) {
  StaticJsonDocument<128> doc;
  doc["success"] = true;
  doc["message"] = "Reiniciando dispositivo...";
  
  serializeJson(doc, response);
  
  // Programar reinicio en 1 segundo para permitir enviar la respuesta
  #ifdef ESP8266
    ESP.wdtDisable();
    ESP.restart();
  #elif defined(ESP32)
    ESP.restart();
  #endif
  
  return true;
}

// Manejadores para endpoints HTTP
void handleApiStatus() {
  String response;
  apiGetStatus(response);
  server.send(200, "application/json", response);
}

void handleApiRelay() {
  if (!server.hasArg("relay") || !server.hasArg("action")) {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Parámetros faltantes\"}");
    return;
  }
  
  int relayNum = server.arg("relay").toInt();
  String action = server.arg("action");
  bool activate = (action == "activate");
  
  String response;
  apiActivateRelay(relayNum, activate, response);
  server.send(200, "application/json", response);
}

void handleApiCommand() {
  if (!server.hasArg("command")) {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Parámetro 'command' requerido\"}");
    return;
  }
  
  String command = server.arg("command");
  String response;
  apiSendCommand(command, response);
  server.send(200, "application/json", response);
}

void handleApiGetConfig() {
  String response;
  apiGetConfig(response);
  server.send(200, "application/json", response);
}

void handleApiSetConfig() {
  if (!server.hasArg("plain")) {
    server.send(400, "application/json", "{\"success\":false,\"message\":\"Configuración JSON requerida\"}");
    return;
  }
  
  String configJson = server.arg("plain");
  String response;
  apiSetConfig(configJson, response);
  server.send(200, "application/json", response);
}

void handleApiReset() {
  String response;
  apiReset(response);
  server.send(200, "application/json", response);
}