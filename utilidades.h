#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <Arduino.h>
#include "estructuras.h"

// Conversión hexadecimal/ASCII
char hex2ascii(uint8_t hex);
uint8_t ascii2hex(char ascii);

// Conversiones de texto
void uint16ToHexStr(uint16_t value, char* buffer);
uint16_t hexStrToUint16(const char* hexStr);

// Manipulación de status
void setStatusBit(uint16_t bit);
void clearStatusBit(uint16_t bit);
bool isStatusBitSet(uint16_t bit);
void updateStatusHexString();

// Funciones de debug
void logDebug(const char* message);
void logError(const char* message);
void logCommand(const char* prefix, const char* cmd);

// Funciones para tiempo y espera
void delayMs(unsigned long ms);
unsigned long getMillis();
bool hasTimePassed(unsigned long startTime, unsigned long duration);

// Control de TX/RX para RS485
void setTxMode();
void setRxMode();

// Gestión de strings
void safeStrCopy(char* dest, const char* src, size_t destSize);
bool startsWith(const char* str, const char* prefix);
void byteArrayToHexString(const uint8_t* array, size_t len, char* result);
void hexStringToByteArray(const char* hexStr, uint8_t* array, size_t maxLen);

#endif