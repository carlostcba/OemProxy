#include "web.h"
#include "variables.h"

#ifdef ESP8266
  #include <ESP8266WebServer.h>
  extern ESP8266WebServer server;
#elif defined(ESP32)
  #include <WebServer.h>
  extern WebServer server;
#endif

// Configuración del servidor web
void setupWebServer() {
  // Configurar rutas para páginas web
  server.on("/", HTTP_GET, handleRoot);
  server.onNotFound(handleNotFound);
}

// Página principal
void handleRoot() {
  String html = "<html><head><title>OemAccess API</title></head><body>";
  html += "<h1>OemAccess API</h1>";
  html += "<p>Dispositivo ID: " + String(config.deviceId) + "</p>";
  html += "<p>Nombre: " + String(config.nombre_empresa) + "</p>";
  html += "<h2>Endpoints disponibles:</h2>";
  html += "<ul>";
  html += "<li>GET /api/status - Obtener estado actual</li>";
  html += "<li>POST /api/relay?relay=1&action=activate - Activar relé 1</li>";
  html += "<li>POST /api/relay?relay=1&action=deactivate - Desactivar relé 1</li>";
  html += "<li>POST /api/command?command=S1 - Enviar comando S1 (activa relé 1)</li>";
  html += "<li>POST /api/command?command=R1 - Enviar comando R1 (desactiva relé 1)</li>";
  html += "<li>GET /api/config - Obtener configuración</li>";
  html += "<li>POST /api/reset - Reiniciar dispositivo</li>";
  html += "</ul>";
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Página no encontrada
void handleNotFound() {
  String message = "Página no encontrada\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMétodo: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArgumentos: ";
  message += server.args();
  message += "\n";
  
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  
  server.send(404, "text/plain", message);
}

