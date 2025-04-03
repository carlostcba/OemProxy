// Actualiza el archivo apis.h con estas definiciones

#ifndef APIS_H
#define APIS_H

#include "estructuras.h"
#include <Arduino.h>

// Configuración de la API
void setupApi();
void handleApiRequests();

// Endpoints de API
bool apiGetStatus(String& response);
bool apiActivateRelay(int relayNum, bool activate, String& response);
bool apiSendCommand(const String& command, String& response);
bool apiGetConfig(String& response);
bool apiSetConfig(const String& configJson, String& response);
bool apiReset(String& response);

// Conversiones para la API
void statusToJson(const StatusInfo& status, String& json);
void configToJson(const DeviceConfig& config, String& json);
bool jsonToConfig(const String& json, DeviceConfig& config);

// Endpoints HTTP
void handleApiStatus();
void handleApiRelay();
void handleApiCommand();
void handleApiGetConfig();
void handleApiSetConfig();
void handleApiReset();

// Respuestas de API
void sendApiResponse(bool success, const String& message, const String& data);
void sendApiError(const String& errorMessage);

#endif