#include "variables.h"

// Definición de variables globales (estas irán en un archivo .cpp)
DeviceConfig config;
StatusInfo statusInfo;
CommandBuffer cmdBuffer;
RelayInfo relays[5];

// Pines (modificar según tu hardware)
#ifdef ESP8266
int DE_RE_PIN = D4;      // Pin DE/RE para RS485
int RELAY_PINS[5] = {    // Pines para los relés
  D5, D6, D7, D8, D0
};
#elif defined(ESP32)
int DE_RE_PIN = 4;       // Pin DE/RE para RS485
int RELAY_PINS[5] = {    // Pines para los relés
  5, 18, 19, 21, 22
};
#endif

// Velocidad de comunicación RS485
int RS485_BAUDRATE = 9600;