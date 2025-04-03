// Establecer bit de status
void setStatusBit(uint16_t bit) {
  status |= bit;
  updateStatusHexString();
}

// Limpiar bit de status
void clearStatusBit(uint16_t bit) {
  status &= ~bit;
  updateStatusHexString();
}

// Verificar si un bit de status está establecido
bool isStatusBitSet(uint16_t bit) {
  return (status & bit) != 0;
}

// Convertir ASCII a hexadecimal
uint8_t ascii2hex(char ascii) {
  if (ascii >= '0' && ascii <= '9') return ascii - '0';
  if (ascii >= 'A' && ascii <= 'F') return ascii - 'A' + 10;
  if (ascii >= 'a' && ascii <= 'f') return ascii - 'a' + 10;
  return 0;
}

// Convertir hexadecimal a ASCII
char hex2ascii(uint8_t hex) {
  if (hex < 10) return '0' + hex;
  return 'A' + (hex - 10);
}

// Leer configuración de EEPROM
void readConfiguration() {
  // Leer ID del dispositivo
  deviceId = EEPROM.read(ADDR_DEVICE_ID);
  if (deviceId > 99) deviceId = 0; // Valor por defecto si inválido
  sprintf(deviceIdStr, "%02X", deviceId);
  
  // Leer nombre de empresa
  for (int i = 0; i < 16; i++) {
    char c = EEPROM.read(ADDR_COMPANY_NAME + i);
    if (c == 0 || c == 0xFF) break;
    companyName[i] = c;
  }
  
  // Asegurar que el nombre termine con null
  companyName[16] = '\0';
  
  // Leer modos de funcionamiento
  modoTcpip485 = EEPROM.read(ADDR_TCP_MODE);
  modoWork = EEPROM.read(ADDR_WORK_MODE);
  modoDisplay = EEPROM.read(ADDR_DISPLAY_MODE);
  modoQR_8_12 = EEPROM.read(ADDR_QR_MODE);
  modoClock = EEPROM.read(ADDR_CLOCK_MODE);
  modoSensAltura = EEPROM.read(ADDR_SENSOR_MODE);
  
  // Leer tiempos de relés
  relays[0].time = EEPROM.read(ADDR_RELAY1_TIME);
  if (relays[0].time < 1 || relays[0].time > 99) relays[0].time = 5;
  
  relays[1].time = EEPROM.read(ADDR_RELAY2_TIME);
  if (relays[1].time < 1 || relays[1].time > 99) relays[1].time = 5;
  
  // Deducir si es puerta de entrada
  esPuertaEntrada = (modoWork != 4);
}

// Actualizar estados de relés
void updateRelays() {
  for (int i = 0; i < MAX_RELAYS; i++) {
    // Si el relé está en un estado temporal (>1)
    if (relays[i].state > 1) {
      // Estado 2: Desactivación inmediata
      if (relays[i].state == 2) {
        digitalWrite(relays[i].pin, HIGH); // Desactivar (lógica invertida)
        relays[i].state = 0;
        if (i == 0) clearStatusBit(STATUS_RELAY1);
        else if (i == 1) clearStatusBit(STATUS_RELAY2);
      }
      // Estado 3: Pulso por tiempo definido
      else if (relays[i].state == 3) {
        digitalWrite(relays[i].pin, LOW); // Activar (lógica invertida)
        if (i == 0) setStatusBit(STATUS_RELAY1);
        else if (i == 1) setStatusBit(STATUS_RELAY2);
        
        relays[i].tmr_100ms = relays[i].time * 10; // Convertir a incrementos de 100ms
        relays[i].state = 4; // Pasar al siguiente estado
      }
      // Estado 4: Esperando fin de pulso
      else if (relays[i].state == 4) {
        if (relays[i].tmr_100ms > 0) {
          relays[i].tmr_100ms--;
        } else {
          digitalWrite(relays[i].pin, HIGH); // Desactivar (lógica invertida)
          relays[i].state = 0;
          if (i == 0) clearStatusBit(STATUS_RELAY1);
          else if (i == 1) clearStatusBit(STATUS_RELAY2);
        }
      }
      // Estado 5: Activación por tiempo definido
      else if (relays[i].state == 5) {
        digitalWrite(relays[i].pin, LOW); // Activar (lógica invertida)
        if (i == 0) setStatusBit(STATUS_RELAY1);
        else if (i == 1) setStatusBit(STATUS_RELAY2);
        
        relays[i].tmr_100ms = relays[i].time * 10; // Convertir a incrementos de 100ms
        relays[i].state = 6; // Pasar al siguiente estado
      }
      // Estado 6: Esperando fin de activación
      else if (relays[i].state == 6) {
        if (relays[i].tmr_100ms > 0) {
          relays[i].tmr_100ms--;
        } else {
          digitalWrite(relays[i].pin, HIGH); // Desactivar (lógica invertida)
          relays[i].state = 0;
          if (i == 0) clearStatusBit(STATUS_RELAY1);
          else if (i == 1) clearStatusBit(STATUS_RELAY2);
        }
      }
      // Estado 7: Activación permanente
      else if (relays[i].state == 7) {
        digitalWrite(relays[i].pin, LOW); // Activar (lógica invertida)
        if (i == 0) setStatusBit(STATUS_RELAY1);
        else if (i == 1) setStatusBit(STATUS_RELAY2);
        // Este estado permanece hasta que se cambie manualmente
      }
      // Otros estados especiales
      else if (relays[i].state == 10 || relays[i].state == 20 || relays[i].state == 30 || relays[i].state == 45) {
        digitalWrite(relays[i].pin, LOW); // Activar (lógica invertida)
        if (i == 0) setStatusBit(STATUS_RELAY1);
        else if (i == 1) setStatusBit(STATUS_RELAY2);
        
        relays[i].tmr_100ms = relays[i].state * 10; // Usar el propio estado como tiempo
        relays[i].state = 6; // Pasar al estado de espera
      }
    }
  }
}

// Configuración del servidor web
void setupWebServer() {
  // Configurar rutas para páginas web
  server.on("/", HTTP_GET, handleRoot);
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/relay", HTTP_POST, handleRelay);
  server.on("/command", HTTP_POST, handleCommand);
  server.on("/config", HTTP_GET, handleConfig);
  server.onNotFound(handleNotFound);
}

// Página principal
void handleRoot() {
  String html = "<html><head><title>OemAccess</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;margin:0;padding:20px}";
  html += ".container{max-width:800px;margin:0 auto}";
  html += "h1{color:#333}h2{color:#666}";
  html += ".card{background:#f5f5f5;border-radius:5px;padding:15px;margin-bottom:15px}";
  html += "button{background:#4CAF50;color:white;border:none;padding:10px 15px;margin:5px;border-radius:4px;cursor:pointer}";
  html += "button:hover{background:#45a049}";
  html += ".off{background:#f44336}.off:hover{background:#d32f2f}";
  html += "table{width:100%;border-collapse:collapse;margin-bottom:20px}";
  html += "th,td{text-align:left;padding:8px;border-bottom:1px solid #ddd}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>OemAccess Sistema de Control</h1>";
  
  // Información de dispositivo
  html += "<div class='card'>";
  html += "<h2>Información del Dispositivo</h2>";
  html += "<table>";
  html += "<tr><td>ID:</td><td>" + String(deviceIdStr) + "</td></tr>";
  html += "<tr><td>Nombre:</td><td>" + String(companyName) + "</td></tr>";
  html += "<tr><td>Status:</td><td>" + String(statusHex) + "</td></tr>";
  html += "<tr><td>Modo trabajo:</td><td>" + String(modoWork) + "</td></tr>";
  html += "<tr><td>Puerta:</td><td>" + String(esPuertaEntrada ? "Entrada" : "Salida") + "</td></tr>";
  html += "</table>";
  html += "</div>";
  
  // Control de relés
  html += "<div class='card'>";
  html += "<h2>Control de Relés</h2>";
  html += "<div>";
  html += "<button onclick=\"fetch('/relay?relay=1&state=1',{method:'POST'})\">Activar Relé 1</button>";
  html += "<button class='off' onclick=\"fetch('/relay?relay=1&state=0',{method:'POST'})\">Desactivar Relé 1</button>";
  html += "</div><div>";
  html += "<button onclick=\"fetch('/relay?relay=2&state=1',{method:'POST'})\">Activar Relé 2</button>";
  html += "<button class='off' onclick=\"fetch('/relay?relay=2&state=0',{method:'POST'})\">Desactivar Relé 2</button>";
  html += "</div>";
  html += "</div>";
  
  // Comandos rápidos
  html += "<div class='card'>";
  html += "<h2>Comandos Rápidos</h2>";
  html += "<div>";
  html += "<button onclick=\"fetch('/command?cmd=S0',{method:'POST'})\">Status (S0)</button>";
  html += "<button onclick=\"fetch('/command?cmd=C1',{method:'POST'})\">Habilitar Scanner (C1)</button>";
  html += "<button onclick=\"fetch('/command?cmd=C5',{method:'POST'})\">Activar Molinete (C5)</button>";
  html += "<button onclick=\"fetch('/command?cmd=X0',{method:'POST'}).then(()=>alert('Reiniciando dispositivo...'))\">Reiniciar (X0)</button>";
  html += "</div>";
  html += "</div>";
  
  html += "</div>"; // Fin container
  html += "</body></html>";
  
  server.send(200, "text/html", html);
}

// Estado del dispositivo en formato JSON
void handleStatus() {
  String statusJson = "{";
  statusJson += "\"id\":\"" + String(deviceIdStr) + "\",";
  statusJson += "\"status\":\"" + String(statusHex) + "\",";
  statusJson += "\"relay1\":" + String(relays[0].state) + ",";
  statusJson += "\"relay2\":" + String(relays[1].state) + ",";
  
  // Agregar bits de status individuales
  statusJson += "\"statusBits\":{";
  statusJson += "\"ddmm1\":" + String(isStatusBitSet(STATUS_DDMM1)) + ",";
  statusJson += "\"ddmm2\":" + String(isStatusBitSet(STATUS_DDMM2)) + ",";
  statusJson += "\"relay1\":" + String(isStatusBitSet(STATUS_RELAY1)) + ",";
  statusJson += "\"relay2\":" + String(isStatusBitSet(STATUS_RELAY2)) + ",";
  statusJson += "\"tarjeta\":" + String(isStatusBitSet(STATUS_TARJ)) + ",";
  statusJson += "\"fraude\":" + String(isStatusBitSet(STATUS_FRAUDE)) + ",";
  statusJson += "\"pulso\":" + String(isStatusBitSet(STATUS_PULS)) + ",";
  statusJson += "\"scanner\":" + String(isStatusBitSet(STATUS_SCANNER)) + ",";
  statusJson += "\"salida\":" + String(isStatusBitSet(STATUS_SALIDA));
  statusJson += "},";
  
  if (strlen(rfidData) > 0) {
    statusJson += "\"rfid\":\"" + String(rfidData) + "\",";
  }
  
  statusJson += "\"config\":{";
  statusJson += "\"tcpipMode\":" + String(modoTcpip485) + ",";
  statusJson += "\"workMode\":" + String(modoWork) + ",";
  statusJson += "\"displayMode\":" + String(modoDisplay) + ",";
  statusJson += "\"qrMode\":" + String(modoQR_8_12) + ",";
  statusJson += "\"clockMode\":" + String(modoClock) + ",";
  statusJson += "\"sensorMode\":" + String(modoSensAltura);
  statusJson += "},";
  
  statusJson += "\"uptime\":" + String(millis()/1000);
  statusJson += "}";
  
  server.send(200, "application/json", statusJson);
}

// Control de relés
void handleRelay() {
  // Verificar parámetros
  if (!server.hasArg("relay") || !server.hasArg("state")) {
    server.send(400, "text/plain", "Faltan parámetros");
    return;
  }
  
  int relay = server.arg("relay").toInt();
  int state = server.arg("state").toInt();
  
  // Validar parámetros
  if (relay < 1 || relay > MAX_RELAYS || state < 0 || state > 1) {
    server.send(400, "text/plain", "Parámetros inválidos");
    return;
  }
  
  // Cambiar estado del relé
  if (state == 1) {
    digitalWrite(relays[relay-1].pin, LOW); // Activo en LOW
    relays[relay-1].state = 1;
    if (relay == 1) setStatusBit(STATUS_RELAY1);
    else if (relay == 2) setStatusBit(STATUS_RELAY2);
  } else {
    digitalWrite(relays[relay-1].pin, HIGH); // Inactivo en HIGH
    relays[relay-1].state = 0;
    if (relay == 1) clearStatusBit(STATUS_RELAY1);
    else if (relay == 2) clearStatusBit(STATUS_RELAY2);
  }
  
  server.send(200, "text/plain", "OK");
}

// Enviar comandos
void handleCommand() {
  // Verificar parámetros
  if (!server.hasArg("cmd")) {
    server.send(400, "text/plain", "Falta parámetro cmd");
    return;
  }
  
  String cmd = server.arg("cmd");
  
  // Validar comando
  if (cmd.length() < 2) {
    server.send(400, "text/plain", "Comando inválido");
    return;
  }
  
  // Procesamiento básico de comandos
  char funcCode = cmd.charAt(0);
  char subCode = cmd.charAt(1);
  String response = "Comando no soportado";
  
  if (funcCode == 'S' && subCode == '0') {
    // S0: Status
    response = "Status enviado: " + String(statusHex);
    sendStatus();
  } else if (funcCode == 'S' && subCode == '1') {
    // S1: Activar Relé 1
    digitalWrite(relays[0].pin, LOW);
    relays[0].state = 1;
    setStatusBit(STATUS_RELAY1);
    response = "Relé 1 activado";
  } else if (funcCode == 'S' && subCode == '2') {
    // S2: Activar Relé 2
    digitalWrite(relays[1].pin, LOW);
    relays[1].state = 1;
    setStatusBit(STATUS_RELAY2);
    response = "Relé 2 activado";
  } else if (funcCode == 'R' && subCode == '1') {
    // R1: Desactivar Relé 1
    digitalWrite(relays[0].pin, HIGH);
    relays[0].state = 0;
    clearStatusBit(STATUS_RELAY1);
    response = "Relé 1 desactivado";
  } else if (funcCode == 'R' && subCode == '2') {
    // R2: Desactivar Relé 2
    digitalWrite(relays[1].pin, HIGH);
    relays[1].state = 0;
    clearStatusBit(STATUS_RELAY2);
    response = "Relé 2 desactivado";
  } else if (funcCode == 'C' && subCode == '1') {
    // C1: Habilitar scanner
    scannerActivo = true;
    tarjetaLeida = false;
    clearStatusBit(STATUS_TARJ);
    clearStatusBit(STATUS_FRAUDE);
    clearStatusBit(STATUS_PULS);
    clearStatusBit(STATUS_SCANNER);
    relays[0].state = 2;
    response = "Scanner habilitado, estado reiniciado";
  } else if (funcCode == 'C' && subCode == '5') {
    // C5: Activar molinete por pulso
    relays[0].state = 3;
    response = "Molinete activado por pulso";
  } else if (funcCode == 'X' && subCode == '0') {
    // X0: Reiniciar
    response = "Reiniciando...";
    server.send(200, "text/plain", response);
    delay(500);
    ESP.restart();
    return;
  }
  
  server.send(200, "text/plain", response);
}

// Configuración
void handleConfig() {
  // Mostrar/cambiar configuración
  String html = "<html><head><title>Configuración</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial;margin:0;padding:20px}";
  html += ".container{max-width:800px;margin:0 auto}";
  html += "h1{color:#333}h2{color:#666}";
  html += ".card{background:#f5f5f5;border-radius:5px;padding:15px;margin-bottom:15px}";
  html += "label{display:block;margin:10px 0 5px}";
  html += "input,select{width:100%;padding:8px;margin-bottom:10px;border:1px solid #ddd;border-radius:4px}";
  html += "button{background:#4CAF50;color:white;border:none;padding:10px 15px;margin:5px;border-radius:4px;cursor:pointer}";
  html += "</style></head><body>";
  html += "<div class='container'>";
  html += "<h1>Configuración OemAccess</h1>";
  
  html += "<div class='card'>";
  html += "<h2>Configuración General</h2>";
  html += "<form method='post' action='/config'>";
  html += "<label for='deviceId'>ID del Dispositivo (0-99):</label>";
  html += "<input type='number' name='deviceId' id='deviceId' min='0' max='99' value='" + String(deviceId) + "'>";
  
  html += "<label for='companyName'>Nombre de Empresa:</label>";
  html += "<input type='text' name='companyName' id='companyName' maxlength='16' value='" + String(companyName) + "'>";
  
  html += "<label for='modoWork'>Modo de Trabajo:</label>";
  html += "<select name='modoWork' id='modoWork'>";
  for (int i = 0; i <= 9; i++) {
    html += "<option value='" + String(i) + "'" + (modoWork == i ? " selected" : "") + ">" + String(i) + "</option>";
  }
  html += "</select>";
  
  html += "<label for='relay1Time'>Tiempo Relé 1 (segundos):</label>";
  html += "<input type='number' name='relay1Time' id='relay1Time' min='1' max='99' value='" + String(relays[0].time) + "'>";
  
  html += "<label for='relay2Time'>Tiempo Relé 2 (segundos):</label>";
  html += "<input type='number' name='relay2Time' id='relay2Time' min='1' max='99' value='" + String(relays[1].time) + "'>";
  
  html += "<button type='submit'>Guardar Configuración</button>";
  html += "</form>";
  html += "</div>";
  
  html += "</div>"; // Fin container
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