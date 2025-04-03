#ifndef WEB_H
#define WEB_H

#include <Arduino.h>

// Configuración del servidor web
void setupWebServer();
void handleClient();

// Páginas y assets web
void handleRoot();
void handleNotFound();
void serveConfigPage();
void serveStylesheet();
void serveScript();

// WebSocket (para actualizaciones en tiempo real)
void setupWebSocket();
void handleWebSocketEvent(uint8_t num, uint8_t type, uint8_t* payload, size_t length);
void broadcastStatus();

// Autenticación
bool isAuthenticated();
void handleAuthentication();
void handleLogout();

// Utilidades Web
String getContentType(const String& path);
String urlDecode(const String& text);
void sendJsonResponse(int code, const String& json);
void sendTextResponse(int code, const String& text, const String& contentType);
void redirectTo(const String& url);

#endif