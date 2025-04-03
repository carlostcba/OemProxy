#include "utilidades.h"
#include "variables.h"

// Conversión hexadecimal/ASCII
char hex2ascii(uint8_t hex) {
  if (hex < 10) return '0' + hex;
  return 'A' + (hex - 10);
}

uint8_t ascii2hex(char ascii) {
  if (ascii >= '0' && ascii <= '9') return ascii - '0';
  if (ascii >= 'A' && ascii <= 'F') return ascii - 'A' + 10;
  if (ascii >= 'a' && ascii <= 'f') return ascii - 'a' + 10;
  return 0;
}

// Conversiones de texto
void uint16ToHexStr(uint16_t value, char* buffer) {
  buffer[0] = hex2ascii((value >> 12) & 0x0F);
  buffer[1] = hex2ascii((value >> 8) & 0x0F);
  buffer[2] = hex2ascii((value >> 4) & 0x0F);
  buffer[3] = hex2ascii(value & 0x0F);
  buffer[4] = '\0';
}

uint16_t hexStrToUint16(const char* hexStr) {
  uint16_t result = 0;
  
  for (int i = 0; hexStr[i] != '\0' && i < 4; i++) {
    result = (result << 4) | ascii2hex(hexStr[i]);
  }
  
  return result;
}

// Manipulación de status
void setStatusBit(uint16_t bit) {
  statusInfo.status |= bit;
  updateStatusHexString();
}

void clearStatusBit(uint16_t bit) {
  statusInfo.status &= ~bit;
  updateStatusHexString();
}

bool isStatusBitSet(uint16_t bit) {
  return (statusInfo.status & bit) != 0;
}

void updateStatusHexString() {
  uint16ToHexStr(statusInfo.status, statusInfo.statusHex);
}

// Funciones de debug
void logDebug(const char* message) {
  Serial.print("DEBUG: ");
  Serial.println(message);
}

void logError(const char* message) {
  Serial.print("ERROR: ");
  Serial.println(message);
}

void logCommand(const char* prefix, const char* cmd) {
  Serial.print(prefix);
  Serial.print(": ");
  
  // Imprimir el comando con interpretación de caracteres de control
  for (size_t i = 0; i < strlen(cmd); i++) {
    uint8_t c = (uint8_t)cmd[i];
    if (c < 32 || c > 127) {
      // Caracteres de control o no imprimibles
      Serial.print("\\x");
      Serial.print(hex2ascii(c >> 4));
      Serial.print(hex2ascii(c & 0x0F));
    } else {
      // Caracteres imprimibles
      Serial.write(c);
    }
  }
  
  Serial.println();
}

// Funciones para tiempo y espera
void delayMs(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    yield(); // Ceder CPU para evitar WDT reset en ESP8266/ESP32
  }
}

unsigned long getMillis() {
  return millis();
}

bool hasTimePassed(unsigned long startTime, unsigned long duration) {
  return (millis() - startTime) >= duration;
}

// Control de TX/RX para RS485
void setTxMode() {
  digitalWrite(DE_RE_PIN, HIGH); // Habilitar transmisión
  delayMs(1); // Pequeña pausa para estabilizar
}

void setRxMode() {
  // Esperar a que se envíen todos los datos en el buffer
  delay(1);
  digitalWrite(DE_RE_PIN, LOW); // Habilitar recepción
}

// Gestión de strings
void safeStrCopy(char* dest, const char* src, size_t destSize) {
  size_t i;
  for (i = 0; i < destSize - 1 && src[i] != 0; i++) {
    dest[i] = src[i];
  }
  dest[i] = 0; // Asegurar terminación null
}

bool startsWith(const char* str, const char* prefix) {
  size_t prefixLen = strlen(prefix);
  return strncmp(str, prefix, prefixLen) == 0;
}

void byteArrayToHexString(const uint8_t* array, size_t len, char* result) {
  for (size_t i = 0; i < len; i++) {
    result[i*2] = hex2ascii(array[i] >> 4);
    result[i*2 + 1] = hex2ascii(array[i] & 0x0F);
  }
  result[len*2] = '\0';
}

void hexStringToByteArray(const char* hexStr, uint8_t* array, size_t maxLen) {
  size_t hexLen = strlen(hexStr);
  size_t byteLen = hexLen / 2;
  
  if (byteLen > maxLen) byteLen = maxLen;
  
  for (size_t i = 0; i < byteLen; i++) {
    array[i] = (ascii2hex(hexStr[i*2]) << 4) | ascii2hex(hexStr[i*2 + 1]);
  }
}