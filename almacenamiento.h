#ifndef ALMACENAMIENTO_H
#define ALMACENAMIENTO_H

#include "estructuras.h"
#include <Arduino.h>

// Inicialización del almacenamiento
void initStorage();

// Funciones EEPROM
void writeConfigToEEPROM();
void readConfigFromEEPROM();
void resetConfigToDefaults();

// Funciones específicas
uint8_t readEEPROM(int address);
void writeEEPROM(int address, uint8_t value);
void writeEEPROMBlock(int address, const uint8_t* data, size_t len);
void readEEPROMBlock(int address, uint8_t* data, size_t len);

// Funciones para la configuración
void saveDeviceId(uint8_t id);
uint8_t loadDeviceId();
void saveCompanyName(const char* name);
void loadCompanyName(char* name, size_t maxLen);
void saveTcpIpMode(uint8_t mode);
uint8_t loadTcpIpMode();
void saveWorkMode(uint8_t mode);
uint8_t loadWorkMode();
void saveDisplayMode(uint8_t mode);
uint8_t loadDisplayMode();
void saveQRMode(uint8_t mode);
uint8_t loadQRMode();
void saveClockMode(uint8_t mode);
uint8_t loadClockMode();
void saveSensorMode(uint8_t mode);
uint8_t loadSensorMode();
void saveRelayTimer(int relayNum, uint8_t time);
uint8_t loadRelayTimer(int relayNum);
void saveSerialNumber(int index, uint8_t value);
uint8_t loadSerialNumber(int index);

// Funciones para tickets
void saveTicketLine(int lineNum, const char* text);
void loadTicketLine(int lineNum, char* text, size_t maxLen);
void saveTicketCounter(uint32_t counter);
uint32_t loadTicketCounter();

#endif