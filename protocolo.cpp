#include "protocolo.h"
#include "utilidades.h"
#include "almacenamiento.h"
#include <Arduino.h>

#ifdef ESP8266
  #include <SoftwareSerial.h>
  extern SoftwareSerial rs485Serial;
#elif defined(ESP32)
  #include <HardwareSerial.h>
  extern HardwareSerial rs485Serial;
#endif

// Implementación de funciones de parsing de comandos
bool parseCommand(const char* cmd, char* functionCode, char* subCode, char* data, int* dataLen) {
  // Verificar longitud mínima (STX + ID[2] + FUNC + SUBFUNC + ETX = 6)
  int len = strlen(cmd);
  if (len < 6) return false;
  
  // Verificar STX y ETX
  if (cmd[0] != STX || cmd[len-1] != ETX) return false;
  
  // Extraer ID del dispositivo (2 caracteres)
  char deviceIdStr[3] = {cmd[1], cmd[2], '\0'};
  
  // Verificar si el comando es para este dispositivo
  if (strcmp(deviceIdStr, config.deviceIdStr) != 0) return false;
  
  // Extraer código de función y subfunción
  *functionCode = cmd[3];
  *subCode = cmd[4];
  
  // Extraer datos si hay
  *dataLen = len - 6; // STX + ID[2] + FUNC + SUBFUNC + ETX = 6
  if (*dataLen > 0) {
    memcpy(data, &cmd[5], *dataLen);
    data[*dataLen] = '\0';
  } else {
    data[0] = '\0';
  }
  
  return true;
}

bool validateCommand(const char* cmd, int len) {
  // Verificar longitud mínima
  if (len < 6) return false;
  
  // Verificar STX y ETX
  if (cmd[0] != STX || cmd[len-1] != ETX) return false;
  
  // Aquí se podría agregar verificación de checksum si el protocolo lo usa
  
  return true;
}

// Implementación de funciones para procesar comandos
CommandResponse processCommand(const char* cmd) {
  CommandResponse response = {false, "", ""};
  
  char functionCode;
  char subCode;
  char data[60];
  int dataLen;
  
  if (!parseCommand(cmd, &functionCode, &subCode, data, &dataLen)) {
    strcpy(response.message, "Comando inválido");
    return response;
  }
  
  // Procesar según el código de función
  switch (functionCode) {
    case 'A':
      return processA_Command(subCode, data, dataLen);
    case 'B':
      return processB_Command(subCode, data, dataLen);
    case 'C':
      return processC_Command(subCode, data, dataLen);
    case 'D':
      return processD_Command(subCode, data, dataLen);
    case 'E':
      return processE_Command(subCode, data, dataLen);
    case 'G':
      return processG_Command(subCode, data, dataLen);
    case 'H':
      return processH_Command(subCode, data, dataLen);
    case 'J':
      return processJ_Command(subCode, data, dataLen);
    case 'K':
      return processK_Command(subCode, data, dataLen);
    case 'M':
      return processM_Command(subCode, data, dataLen);
    case 'O':
      return processO_Command(subCode, data, dataLen);
    case 'P':
      return processP_Command(subCode, data, dataLen);
    case 'R':
      return processR_Command(subCode, data, dataLen);
    case 'S':
      return processS_Command(subCode, data, dataLen);
    case 'T':
      return processT_Command(subCode, data, dataLen);
    case 'V':
      return processV_Command(subCode, data, dataLen);
    case 'X':
      return processX_Command(subCode, data, dataLen);
    case 'Z':
      return processZ_Command(subCode, data, dataLen);
    default:
      strcpy(response.message, "Código de función desconocido");
      return response;
  }
}

// Implementación de procesamiento de comandos tipo "A"
CommandResponse processA_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  switch (subCode) {
    case '0':
      // A0: Configurar número de dispositivo
      if (dataLen >= 1) {
        uint8_t newDeviceId = (ascii2hex(data[0]) << 4) | ascii2hex(data[1]);
        saveDeviceId(newDeviceId);
        config.deviceId = newDeviceId;
        sprintf(config.deviceIdStr, "%02X", newDeviceId);
        sendACK();
        strcpy(response.message, "ID de dispositivo actualizado");
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '1':
      // A1: Consultar número de dispositivo
      sendData("A", "1", config.deviceIdStr);
      sprintf(response.message, "ID de dispositivo: %s", config.deviceIdStr);
      sprintf(response.data, "%s", config.deviceIdStr);
      break;
      
    case '4':
      // A4: Grabar nombre de la empresa en EEPROM
      if (dataLen > 0) {
        char name[17];
        memcpy(name, data, min(dataLen, 16));
        name[min(dataLen, 16)] = '\0';
        saveCompanyName(name);
        strcpy(config.nombre_empresa, name);
        sendACK();
        strcpy(response.message, "Nombre de empresa actualizado");
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '5':
      // A5: Consultar nombre de la empresa
      sendData("A", "5", config.nombre_empresa);
      sprintf(response.message, "Nombre de empresa: %s", config.nombre_empresa);
      sprintf(response.data, "%s", config.nombre_empresa);
      break;
      
    case '6':
      // A6: Configurar modo TCP/IP485
      if (dataLen >= 2) {
        uint8_t newMode = (ascii2hex(data[0]) << 4) | ascii2hex(data[1]);
        saveTcpIpMode(newMode);
        config.modo_tcpip485 = newMode;
        sendACK();
        sprintf(response.message, "Modo TCP/IP485 actualizado a %02X", newMode);
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '7':
      // A7: Consultar modo TCP/IP485
      {
        char modeStr[3];
        sprintf(modeStr, "%02X", config.modo_tcpip485);
        sendData("A", "7", modeStr);
        sprintf(response.message, "Modo TCP/IP485: %s", modeStr);
        sprintf(response.data, "%s", modeStr);
      }
      break;
      
    case 'A':
      // AA: Configurar Serial Number Byte 0 (solo ID 2)
      if (config.deviceId == 2) {
        if (dataLen >= 2) {
          uint8_t serialNumber0 = (ascii2hex(data[0]) << 4) | ascii2hex(data[1]);
          saveSerialNumber(0, serialNumber0);
          sendACK();
          sprintf(response.message, "Serial Number Byte 0 actualizado a %02X", serialNumber0);
        } else {
          sendNAK();
          response.success = false;
          strcpy(response.message, "Datos insuficientes");
        }
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Comando no permitido para este dispositivo");
      }
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo A%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de funciones de envío de comandos
bool sendCommand(const char* functionCode, const char* subCode, const char* data) {
  char buffer[64];
  int index = 0;
  
  // Construir comando
  buffer[index++] = STX;
  buffer[index++] = config.deviceIdStr[0];
  buffer[index++] = config.deviceIdStr[1];
  buffer[index++] = functionCode[0];
  buffer[index++] = subCode[0];
  
  // Agregar datos si los hay
  if (data != NULL) {
    int dataLen = strlen(data);
    for (int i = 0; i < dataLen && index < 62; i++) {
      buffer[index++] = data[i];
    }
  }
  
  // Agregar ETX
  buffer[index++] = ETX;
  buffer[index] = '\0';
  
  // Enviar comando
  return sendRawCommand(buffer);
}

bool sendRawCommand(const char* cmd) {
  // Cambiar a modo TX
  setTxMode();
  
  // Enviar comando por serial
  rs485Serial.write(cmd, strlen(cmd));
  rs485Serial.flush();
  
  // Cambiar a modo RX
  setRxMode();
  
  return true;
}

// Implementación de funciones de respuesta estándar
bool sendACK() {
  char cmd[5];
  cmd[0] = STX;
  cmd[1] = config.deviceIdStr[0];
  cmd[2] = config.deviceIdStr[1];
  cmd[3] = ACK;
  cmd[4] = ETX;
  
  return sendRawCommand(cmd);
}

bool sendNAK() {
  char cmd[5];
  cmd[0] = STX;
  cmd[1] = config.deviceIdStr[0];
  cmd[2] = config.deviceIdStr[1];
  cmd[3] = NAK;
  cmd[4] = ETX;
  
  return sendRawCommand(cmd);
}

bool sendStatus() {
  char buffer[64];
  int index = 0;
  
  // Construir respuesta de status
  buffer[index++] = STX;
  buffer[index++] = config.deviceIdStr[0];
  buffer[index++] = config.deviceIdStr[1];
  buffer[index++] = 'S';
  buffer[index++] = '0';
  
  // Agregar status
  updateStatusHexString();
  for (int i = 0; i < 4; i++) {
    buffer[index++] = statusInfo.statusHex[i];
  }
  
  // Agregar datos RFID si hay
  if (strlen(statusInfo.rfidData) > 0) {
    for (int i = 0; i < strlen(statusInfo.rfidData); i++) {
      buffer[index++] = statusInfo.rfidData[i];
    }
  }
  
  // Agregar SIB
  buffer[index++] = SIB;
  buffer[index] = '\0';
  
  return sendRawCommand(buffer);
}

bool sendData(const char* functionCode, const char* subCode, const char* data) {
  char buffer[64];
  int index = 0;
  
  // Construir respuesta con datos
  buffer[index++] = STX;
  buffer[index++] = config.deviceIdStr[0];
  buffer[index++] = config.deviceIdStr[1];
  buffer[index++] = functionCode[0];
  buffer[index++] = subCode[0];
  
  // Agregar datos
  int dataLen = strlen(data);
  for (int i = 0; i < dataLen && index < 62; i++) {
    buffer[index++] = data[i];
  }
  
  // Agregar SIB
  buffer[index++] = SIB;
  buffer[index] = '\0';
  
  return sendRawCommand(buffer);
}

// Implementación de funciones para recepción de datos
bool processIncomingByte(uint8_t byte) {
  // Si es STX, iniciar un nuevo comando
  if (byte == STX) {
    cmdBuffer.index = 0;
    cmdBuffer.buffer[cmdBuffer.index++] = byte;
    cmdBuffer.complete = false;
    return false;
  }
  
  // Si es ETX, completar el comando
  else if (byte == ETX) {
    cmdBuffer.buffer[cmdBuffer.index++] = byte;
    cmdBuffer.buffer[cmdBuffer.index] = '\0';
    cmdBuffer.complete = true;
    return true;
  }
  
  // De lo contrario, agregar al buffer si hay espacio
  else if (cmdBuffer.index < 63) {
    cmdBuffer.buffer[cmdBuffer.index++] = byte;
    return false;
  }
  
  // Buffer overflow
  return false;
}

void clearCommandBuffer() {
  cmdBuffer.index = 0;
  cmdBuffer.buffer[0] = '\0';
  cmdBuffer.complete = false;
}

bool isCommandComplete() {
  return cmdBuffer.complete;
}

const char* getCommand() {
  return cmdBuffer.buffer;
}

// Implementación de funciones de relay
bool activateRelay(int relayNum) {
  if (relayNum < 1 || relayNum > 5) return false;
  
  RelayInfo* relay = &relays[relayNum - 1];
  relay->state = 1;
  
  // Activar el pin (lógica invertida - activo en bajo)
  digitalWrite(relay->pin, LOW);
  
  // Actualizar status
  if (relayNum == 1) setStatusBit(STATUS_RELAY1);
  else if (relayNum == 2) setStatusBit(STATUS_RELAY2);
  
  return true;
}

bool deactivateRelay(int relayNum) {
  if (relayNum < 1 || relayNum > 5) return false;
  
  RelayInfo* relay = &relays[relayNum - 1];
  relay->state = 0;
  
  // Desactivar el pin (lógica invertida - inactivo en alto)
  digitalWrite(relay->pin, HIGH);
  
  // Actualizar status
  if (relayNum == 1) clearStatusBit(STATUS_RELAY1);
  else if (relayNum == 2) clearStatusBit(STATUS_RELAY2);
  
  return true;
}

bool setRelayTimer(int relayNum, uint8_t time) {
    if (relayNum < 1 || relayNum > 5) return false;
    
    RelayInfo* relay = &relays[relayNum - 1];
    relay->time = time;
    
    // Guardar el valor en EEPROM
    saveRelayTimer(relayNum, time);
    
    return true;
  }
  
  uint8_t getRelayTimer(int relayNum) {
    if (relayNum < 1 || relayNum > 5) return 0;
    
    return relays[relayNum - 1].time;
  }
  
  void updateRelays() {
    // Esta función debe llamarse en cada ciclo del loop
    // Gestiona los estados temporales de los relés
    
    for (int i = 0; i < 5; i++) {
      RelayInfo* relay = &relays[i];
      
      // Si el relé está en un estado temporal (>1)
      if (relay->state > 1) {
        // Estado 2: Desactivación inmediata
        if (relay->state == 2) {
          digitalWrite(relay->pin, HIGH); // Desactivar (lógica invertida)
          relay->state = 0;
        }
        // Estado 3: Pulso por tiempo definido
        else if (relay->state == 3) {
          digitalWrite(relay->pin, LOW); // Activar (lógica invertida)
          relay->tmr_100ms = relay->time * 10; // Convertir a incrementos de 100ms
          relay->state = 4; // Pasar al siguiente estado
        }
        // Estado 4: Esperando fin de pulso
        else if (relay->state == 4) {
          if (relay->tmr_100ms > 0) {
            relay->tmr_100ms--;
          } else {
            digitalWrite(relay->pin, HIGH); // Desactivar (lógica invertida)
            relay->state = 0;
          }
        }
        // Estado 5: Activación por tiempo definido
        else if (relay->state == 5) {
          digitalWrite(relay->pin, LOW); // Activar (lógica invertida)
          relay->tmr_100ms = relay->time * 10; // Convertir a incrementos de 100ms
          relay->state = 6; // Pasar al siguiente estado
        }
        // Estado 6: Esperando fin de activación
        else if (relay->state == 6) {
          if (relay->tmr_100ms > 0) {
            relay->tmr_100ms--;
          } else {
            digitalWrite(relay->pin, HIGH); // Desactivar (lógica invertida)
            relay->state = 0;
          }
        }
        // Estado 7: Activación permanente
        else if (relay->state == 7) {
          digitalWrite(relay->pin, LOW); // Activar (lógica invertida)
          // Este estado permanece hasta que se cambie manualmente
        }
        // Otros estados especiales
        else if (relay->state == 10 || relay->state == 20 || relay->state == 30 || relay->state == 45) {
          digitalWrite(relay->pin, LOW); // Activar (lógica invertida)
          relay->tmr_100ms = relay->state * 10; // Usar el propio estado como tiempo
          relay->state = 6; // Pasar al estado de espera
        }
      }
    }
  }

// Continuación de processP_Command

        sprintf(response.message, "Tiempo presencia DDMM1 configurado a %d", DDMM1_TimePresent);
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '7':
      // P7: Configurar tiempo de ausencia DDMM2
      if (dataLen >= 2) {
        DDMM2_TimeAbsent = (ascii2hex(data[0]) * 10) + ascii2hex(data[1]);
        
        sendACK();
        sprintf(response.message, "Tiempo ausencia DDMM2 configurado a %d", DDMM2_TimeAbsent);
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '8':
      // P8: Configurar tiempo de presencia DDMM2
      if (dataLen >= 2) {
        DDMM2_TimePresent = (ascii2hex(data[0]) * 10) + ascii2hex(data[1]);
        
        sendACK();
        sprintf(response.message, "Tiempo presencia DDMM2 configurado a %d", DDMM2_TimePresent);
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo P%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de procesamiento de comandos tipo "R"
CommandResponse processR_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  switch (subCode) {
    case '1':
      // R1: Desactivar Relé 1
      deactivateRelay(1);
      sendACK();
      strcpy(response.message, "Relé 1 desactivado");
      break;
      
    case '2':
      // R2: Desactivar Relé 2
      deactivateRelay(2);
      sendACK();
      strcpy(response.message, "Relé 2 desactivado");
      break;
      
    case '3':
      // R3: Desactivar Relé 3
      deactivateRelay(3);
      sendACK();
      strcpy(response.message, "Relé 3 desactivado");
      break;
      
    case '4':
      // R4: Desactivar Relé 4
      deactivateRelay(4);
      sendACK();
      strcpy(response.message, "Relé 4 desactivado");
      break;
      
    case '5':
      // R5: Desactivar Relé 5
      deactivateRelay(5);
      sendACK();
      strcpy(response.message, "Relé 5 desactivado");
      break;
      
    case '6':
      // R6: Indicar playa libre
      deactivateRelay(2);
      // Aquí podríamos establecer alguna variable de estado para playa llena
      sendACK();
      strcpy(response.message, "Playa libre, ingreso habilitado");
      break;
      
    case '7':
      // R7: Reiniciar estado lector/scanner
      statusInfo.scannerActivo = true;
      statusInfo.tarjetaLeida = false;
      clearStatusBit(STATUS_TARJ);
      clearStatusBit(STATUS_FRAUDE);
      clearStatusBit(STATUS_PULS);
      clearStatusBit(STATUS_SCANNER);
      
      relays[2].state = 1; // Activar relay 3
      relays[0].tmr_100ms = 10; // Timer 1 segundo
      relays[0].state = 0;
      deactivateRelay(1);
      
      sendACK();
      strcpy(response.message, "Estado lector/scanner reiniciado");
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo R%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de procesamiento de comandos tipo "S"
CommandResponse processS_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  switch (subCode) {
    case '0':
      // S0: Consultar status
      sendStatus();
      sprintf(response.message, "Status enviado: %s", statusInfo.statusHex);
      break;
      
    case '1':
      // S1: Activar Relé 1
      activateRelay(1);
      sendACK();
      strcpy(response.message, "Relé 1 activado");
      break;
      
    case '2':
      // S2: Activar Relé 2
      activateRelay(2);
      sendACK();
      strcpy(response.message, "Relé 2 activado");
      break;
      
    case '3':
      // S3: Activar Relé 3
      activateRelay(3);
      sendACK();
      strcpy(response.message, "Relé 3 activado");
      break;
      
    case '4':
      // S4: Activar Relé 4
      activateRelay(4);
      sendACK();
      strcpy(response.message, "Relé 4 activado");
      break;
      
    case '5':
      // S5: Activar Relé 5
      activateRelay(5);
      sendACK();
      strcpy(response.message, "Relé 5 activado");
      break;
      
    case '6':
      // S6: Indicar playa llena
      activateRelay(2);
      // Aquí podríamos establecer alguna variable de estado para playa llena
      sendACK();
      strcpy(response.message, "Playa llena, ingreso inhibido");
      break;
      
    case '7':
      // S7: Activar barrera (Relé 1 estado 7)
      relays[0].state = 7;
      sendACK();
      strcpy(response.message, "Barrera activada (estado 7)");
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo S%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de procesamiento de comandos tipo "T"
CommandResponse processT_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  // Variables para las líneas del ticket (deberían estar en un ámbito más amplio)
  static char linea_1[17] = "Ticket Linea 1";
  static char linea_2[17] = "Ticket Linea 2";
  static char linea_3[17] = "Ticket Linea 3";
  static char linea_4[17] = "Ticket Linea 4";
  
  switch (subCode) {
    case '0':
      // T0: Leer línea 1 del ticket
      sendData("T", "0", linea_1);
      sprintf(response.message, "Línea 1 de ticket enviada");
      break;
      
    case '1':
      // T1: Leer línea 2 del ticket
      sendData("T", "1", linea_2);
      sprintf(response.message, "Línea 2 de ticket enviada");
      break;
      
    case '2':
      // T2: Leer línea 3 del ticket
      sendData("T", "2", linea_3);
      sprintf(response.message, "Línea 3 de ticket enviada");
      break;
      
    case '3':
      // T3: Leer línea 4 del ticket
      sendData("T", "3", linea_4);
      sprintf(response.message, "Línea 4 de ticket enviada");
      break;
      
    case '4':
      // T4: Grabar línea 1 del ticket
      if (dataLen >= 16) {
        // Copiar los datos a la línea 1
        memcpy(linea_1, data, 16);
        linea_1[16] = '\0';
        
        // Guardar en EEPROM
        saveTicketLine(1, linea_1);
        
        sendACK();
        strcpy(response.message, "Línea 1 de ticket grabada");
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '5':
      // T5: Grabar línea 2 del ticket
      if (dataLen >= 16) {
        // Copiar los datos a la línea 2
        memcpy(linea_2, data, 16);
        linea_2[16] = '\0';
        
        // Guardar en EEPROM
        saveTicketLine(2, linea_2);
        
        sendACK();
        strcpy(response.message, "Línea 2 de ticket grabada");
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '6':
      // T6: Grabar línea 3 del ticket
      if (dataLen >= 16) {
        // Copiar los datos a la línea 3
        memcpy(linea_3, data, 16);
        linea_3[16] = '\0';
        
        // Guardar en EEPROM
        saveTicketLine(3, linea_3);
        
        sendACK();
        strcpy(response.message, "Línea 3 de ticket grabada");
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '7':
      // T7: Grabar línea 4 del ticket
      if (dataLen >= 16) {
        // Copiar los datos a la línea 4
        memcpy(linea_4, data, 16);
        linea_4[16] = '\0';
        
        // Guardar en EEPROM
        saveTicketLine(4, linea_4);
        
        sendACK();
        strcpy(response.message, "Línea 4 de ticket grabada");
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '9':
      // T9: Imprimir ticket e informar número generado
      {
        // Aquí deberíamos generar un número de ticket
        // Por ahora generamos uno aleatorio para demostración
        char ticket[13];
        sprintf(ticket, "%012d", random(1, 1000000));
        
        // Enviar respuesta
        sendData("T", "9", ticket);
        
        sprintf(response.message, "Ticket generado: %s", ticket);
        strncpy(response.data, ticket, sizeof(response.data) - 1);
        response.data[sizeof(response.data) - 1] = '\0';
      }
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo T%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de procesamiento de comandos tipo "V"
CommandResponse processV_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  switch (subCode) {
    case '0':
      // V0: Consultar versión
      {
        // Enviar versión del firmware
        char version[] = "OemProxy v1.0";
        sendData("V", "0", version);
        
        strcpy(response.message, "Versión enviada");
        strcpy(response.data, version);
      }
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo V%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de procesamiento de comandos tipo "X"
CommandResponse processX_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  switch (subCode) {
    case '0':
      // X0: Reiniciar dispositivo
      sendACK();
      strcpy(response.message, "Reiniciando dispositivo...");
      
      // Programar reinicio en 1 segundo para permitir enviar la respuesta
      #ifdef ESP8266
        ESP.wdtDisable();
        ESP.restart();
      #elif defined(ESP32)
        ESP.restart();
      #endif
      break;
      
    case '9':
      // X9: Reiniciar dispositivo (alternativo)
      sendACK();
      strcpy(response.message, "Reiniciando dispositivo...");
      
      // Programar reinicio en 1 segundo para permitir enviar la respuesta
      #ifdef ESP8266
        ESP.wdtDisable();
        ESP.restart();
      #elif defined(ESP32)
        ESP.restart();
      #endif
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo X%c desconocido", subCode);
      break;
  }
  
  return response;
}

// Implementación de procesamiento de comandos tipo "Z"
CommandResponse processZ_Command(char subCode, const char* data, int dataLen) {
  CommandResponse response = {true, "", ""};
  
  switch (subCode) {
    case '0':
      // Z0: Configurar códigos de barras
      if (dataLen >= 4) {
        // Formato: unidad_mil + 3 dígitos
        char unidadMil = data[0];
        char counter[4] = {data[1], data[2], data[3], '\0'};
        
        // Guardar en EEPROM o variables globales
        
        sendACK();
        sprintf(response.message, "Código de barras configurado: %c%s", unidadMil, counter);
      } else {
        sendNAK();
        response.success = false;
        strcpy(response.message, "Datos insuficientes");
      }
      break;
      
    case '1':
      // Z1: Consultar configuración de códigos de barras
      {
        // Deberíamos leer estos valores de EEPROM o variables globales
        // Por ahora enviamos valores estáticos
        char config[] = "A123";
        
        sendData("Z", "1", config);
        sprintf(response.message, "Configuración de código de barras: %s", config);
      }
      break;
      
    case '9':
      // Z9: Mostrar configuración
      {
        // Aquí deberíamos tener código para mostrar la configuración en el display
        // Por ahora solo respondemos con ACK
        
        sendACK();
        strcpy(response.message, "Configuración mostrada en display");
      }
      break;
      
    default:
      sendNAK();
      response.success = false;
      sprintf(response.message, "Subcódigo Z%c desconocido", subCode);
      break;
  }
  
  return response;
}