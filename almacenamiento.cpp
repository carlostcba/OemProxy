#include "almacenamiento.h"
#include "variables.h"
#include <EEPROM.h>

// Inicialización del almacenamiento
void initStorage() {
  // No es necesario hacer nada aquí para ESP8266/ESP32
  // EEPROM.begin() se llama en setup()
}

// Leer configuración de la EEPROM
void readConfigFromEEPROM() {
  // Cargar ID del dispositivo
  config.deviceId = loadDeviceId();
  sprintf(config.deviceIdStr, "%02X", config.deviceId);
  
  // Cargar nombre de empresa
  loadCompanyName(config.nombre_empresa, sizeof(config.nombre_empresa));
  
  // Cargar modos de funcionamiento
  config.modo_tcpip485 = EEPROM.read(ADDR_TCP_MODE);
  config.modo_work = EEPROM.read(ADDR_WORK_MODE);
  config.modo_display = EEPROM.read(ADDR_DISPLAY_MODE);
  config.modo_QR_8_12 = EEPROM.read(ADDR_QR_MODE);
  config.modo_clock = EEPROM.read(ADDR_CLOCK_MODE);
  config.modo_sens_altura = EEPROM.read(ADDR_SENS_MODE);
  
  // Configuración por defecto para valores inválidos
  if (config.deviceId > 99) config.deviceId = 0;
  if (config.modo_tcpip485 > 9) config.modo_tcpip485 = 0;
  if (config.modo_work > 9) config.modo_work = 0;
  if (config.modo_display > 9) config.modo_display = 0;
  if (config.modo_QR_8_12 > 9) config.modo_QR_8_12 = 0;
  if (config.modo_clock > 9) config.modo_clock = 0;
  if (config.modo_sens_altura > 9) config.modo_sens_altura = 0;
  
  // Deducir si es puerta de entrada
  config.esPuertaEntrada = (config.modo_work != 4);
}

// Restablecer configuración a valores predeterminados
void resetConfigToDefaults() {
  config.deviceId = 0;
  strcpy(config.deviceIdStr, "00");
  strcpy(config.nombre_empresa, "OemAccess");
  config.modo_tcpip485 = 0;
  config.modo_work = 0;
  config.modo_display = 0;
  config.modo_QR_8_12 = 0;
  config.modo_clock = 0;
  config.modo_sens_altura = 0;
  config.esPuertaEntrada = true;
  
  // Guardar en EEPROM
  saveDeviceId(config.deviceId);
  saveCompanyName(config.nombre_empresa);
  saveTcpIpMode(config.modo_tcpip485);
  saveWorkMode(config.modo_work);
  saveDisplayMode(config.modo_display);
  saveQRMode(config.modo_QR_8_12);
  saveClockMode(config.modo_clock);
  saveSensorMode(config.modo_sens_altura);
}

// Funciones específicas
uint8_t loadRelayTimer(int relayNum) {
  if (relayNum < 1 || relayNum > 5) return 5; // Valor predeterminado
  
  uint8_t time = EEPROM.read(ADDR_RELAY1_TIME + (relayNum - 1));
  return (time > 0 && time < 100) ? time : 5; // Valor predeterminado si fuera de rango
}

uint8_t loadDeviceId() {
  uint8_t id = EEPROM.read(ADDR_DEVICE_ID);
  return (id <= 99) ? id : 0; // Valor predeterminado si fuera de rango
}

void saveDeviceId(uint8_t id) {
  EEPROM.write(ADDR_DEVICE_ID, id);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void loadCompanyName(char* name, size_t maxLen) {
  uint8_t i;
  for (i = 0; i < maxLen - 1; i++) {
    char c = EEPROM.read(ADDR_NAME0 + i);
    name[i] = c;
    if (c == 0) break;
  }
  name[i] = 0; // Asegurar terminación null
  
  // Valor predeterminado si está vacío
  if (name[0] == 0 || name[0] == 0xFF) {
    strcpy(name, "OemAccess");
  }
}

void saveCompanyName(const char* name) {
  uint8_t i;
  for (i = 0; i < 16 && name[i] != 0; i++) {
    EEPROM.write(ADDR_NAME0 + i, name[i]);
  }
  EEPROM.write(ADDR_NAME0 + i, 0); // Terminar con null
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveTcpIpMode(uint8_t mode) {
  EEPROM.write(ADDR_TCP_MODE, mode);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveWorkMode(uint8_t mode) {
  EEPROM.write(ADDR_WORK_MODE, mode);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveDisplayMode(uint8_t mode) {
  EEPROM.write(ADDR_DISPLAY_MODE, mode);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveQRMode(uint8_t mode) {
  EEPROM.write(ADDR_QR_MODE, mode);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveClockMode(uint8_t mode) {
  EEPROM.write(ADDR_CLOCK_MODE, mode);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveSensorMode(uint8_t mode) {
  EEPROM.write(ADDR_SENS_MODE, mode);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

// Cargar modos desde EEPROM
uint8_t loadTcpIpMode() {
  uint8_t mode = EEPROM.read(ADDR_TCP_MODE);
  return (mode <= 9) ? mode : 0; // Valor predeterminado si fuera de rango
}

uint8_t loadWorkMode() {
  uint8_t mode = EEPROM.read(ADDR_WORK_MODE);
  return (mode <= 9) ? mode : 0; // Valor predeterminado si fuera de rango
}

uint8_t loadDisplayMode() {
  uint8_t mode = EEPROM.read(ADDR_DISPLAY_MODE);
  return (mode <= 9) ? mode : 0; // Valor predeterminado si fuera de rango
}

uint8_t loadQRMode() {
  uint8_t mode = EEPROM.read(ADDR_QR_MODE);
  return (mode <= 9) ? mode : 0; // Valor predeterminado si fuera de rango
}

uint8_t loadClockMode() {
  uint8_t mode = EEPROM.read(ADDR_CLOCK_MODE);
  return (mode <= 9) ? mode : 0; // Valor predeterminado si fuera de rango
}

uint8_t loadSensorMode() {
  uint8_t mode = EEPROM.read(ADDR_SENS_MODE);
  return (mode <= 9) ? mode : 0; // Valor predeterminado si fuera de rango
}

void saveRelayTimer(int relayNum, uint8_t time) {
  if (relayNum < 1 || relayNum > 5) return; // Validación
  
  EEPROM.write(ADDR_RELAY1_TIME + (relayNum - 1), time);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void saveSerialNumber(int index, uint8_t value) {
  if (index < 0 || index > 4) return; // Validación
  
  EEPROM.write(ADDR_SN0 + index, value);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

uint8_t loadSerialNumber(int index) {
  if (index < 0 || index > 4) return 0; // Validación
  
  return EEPROM.read(ADDR_SN0 + index);
}

// Funciones para tickets
void saveTicketLine(int lineNum, const char* text) {
  if (lineNum < 1 || lineNum > 4) return; // Validación
  
  int addr = ADDR_TICKET_LINEA1 + (lineNum - 1) * 16;
  for (int i = 0; i < 16 && text[i] != 0; i++) {
    EEPROM.write(addr + i, text[i]);
  }
  
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void loadTicketLine(int lineNum, char* text, size_t maxLen) {
  if (lineNum < 1 || lineNum > 4) {
    text[0] = 0;
    return;
  }
  
  int addr = ADDR_TICKET_LINEA1 + (lineNum - 1) * 16;
  for (int i = 0; i < min(maxLen - 1, (size_t)16); i++) {
    char c = EEPROM.read(addr + i);
    text[i] = c;
    if (c == 0) break;
  }
  
  text[min(maxLen - 1, (size_t)16)] = 0; // Asegurar terminación null
}

void saveTicketCounter(uint32_t counter) {
  // La unidad de mil se guarda por separado
  uint8_t unidadMil = counter / 1000;
  uint16_t resto = counter % 1000;
  
  EEPROM.write(ADDR_UNIDAD_MILES, unidadMil);
  
  char buffer[4];
  sprintf(buffer, "%03d", resto);
  
  for (int i = 0; i < 3; i++) {
    EEPROM.write(ADDR_TICKET_NUMBER + i, buffer[i]);
  }
  
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

uint32_t loadTicketCounter() {
  uint8_t unidadMil = EEPROM.read(ADDR_UNIDAD_MILES);
  
  char buffer[4];
  for (int i = 0; i < 3; i++) {
    buffer[i] = EEPROM.read(ADDR_TICKET_NUMBER + i);
  }
  buffer[3] = 0;
  
  uint16_t resto = atoi(buffer);
  
  return unidadMil * 1000 + resto;
}

// Funciones EEPROM básicas
uint8_t readEEPROM(int address) {
  return EEPROM.read(address);
}

void writeEEPROM(int address, uint8_t value) {
  EEPROM.write(address, value);
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void writeEEPROMBlock(int address, const uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    EEPROM.write(address + i, data[i]);
  }
  
  #if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
  #endif
}

void readEEPROMBlock(int address, uint8_t* data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    data[i] = EEPROM.read(address + i);
  }
}

// Escritura de configuración completa
void writeConfigToEEPROM() {
  saveDeviceId(config.deviceId);
  saveCompanyName(config.nombre_empresa);
  saveTcpIpMode(config.modo_tcpip485);
  saveWorkMode(config.modo_work);
  saveDisplayMode(config.modo_display);
  saveQRMode(config.modo_QR_8_12);
  saveClockMode(config.modo_clock);
  saveSensorMode(config.modo_sens_altura);
}
