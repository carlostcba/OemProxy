server.send(200, "application/json", statusJson);
}

void handleRelay() {
  // Verificar parámetros
  if (!server.hasArg("relay") || !server.hasArg("state")) {
    server.send(400, "text/plain", "Faltan parámetros");
    return;
  }
  
  int relay = server.arg("relay").toInt();
  int state = server.arg("state").toInt();
  
  // Validar parámetros
  if (relay < 1 || relay > 2 || state < 0 || state > 1) {
    server.send(400, "text/plain", "Parámetros inválidos");
    return;
  }
  
  // Cambiar estado del relé
  if (relay == 1) {
    digitalWrite(PIN_RELAY1, state == 0 ? HIGH : LOW);
    relay1State = state;
  } else if (relay == 2) {
    digitalWrite(PIN_RELAY2, state == 0 ? HIGH : LOW);
    relay2State = state;
  }
  
  server.send(200, "text/plain", "OK");
}

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
    String statusHex = String(status, HEX);
    while (statusHex.length() < 4) statusHex = "0" + statusHex;
    response = "Status: " + statusHex;
  } else if (funcCode == 'S' && subCode == '1') {
    // S1: Activar Relé 1
    digitalWrite(PIN_RELAY1, LOW);
    relay1State = 1;
    response = "Relé 1 activado";
  } else if (funcCode == 'S' && subCode == '2') {
    // S2: Activar Relé 2
    digitalWrite(PIN_RELAY2, LOW);
    relay2State = 1;
    response = "Relé 2 activado";
  } else if (funcCode == 'R' && subCode == '1') {
    // R1: Desactivar Relé 1
    digitalWrite(PIN_RELAY1, HIGH);
    relay1State = 0;
    response = "Relé 1 desactivado";
  } else if (funcCode == 'R' && subCode == '2') {
    // R2: Desactivar Relé 2
    digitalWrite(PIN_RELAY2, HIGH);
    relay2State = 0;
    response = "Relé 2 desactivado";
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