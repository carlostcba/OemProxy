#ifndef VARIABLES_H
#define VARIABLES_H

#include "estructuras.h"

// Constantes del protocolo
#define STX 0x02        // Start of Text
#define ETX 0x03        // End of Text
#define ACK 0x06        // Acknowledge
#define NAK 0x15        // Negative Acknowledge
#define SIB 0x1B        // Status Information Block (usado como delimitador de fin en algunas respuestas)

// Constantes para el status
#define STATUS_DDMM1    0x0001  // Detector de masa metálica 1 accionado
#define STATUS_DDMM2    0x0002  // Detector de masa metálica 2 accionado
#define STATUS_RELAY1   0x0040  // Relay 1 activado
#define STATUS_RELAY2   0x0080  // Relay 2 activado
#define STATUS_TARJ     0x0100  // Lectura de Tarjeta
#define STATUS_FRAUDE   0x0200  // Indicación de fraude
#define STATUS_PULS     0x0400  // Pulsador de papel accionado
#define STATUS_SCANNER  0x0800  // Scanner activo
#define STATUS_SALIDA   0x8000  // Indicador de salida (vs entrada)

// Direcciones EEPROM
#define ADDR_DEVICE_ID      0   // Dirección de almacenamiento del ID (1 byte)
#define ADDR_NAME0          10  // Dirección del nombre empresa (16 bytes)
#define ADDR_TCP_MODE       30  // Dirección del modo TCP/IP485 (1 byte)
#define ADDR_WORK_MODE      31  // Dirección del modo de trabajo (1 byte)
#define ADDR_DISPLAY_MODE   32  // Dirección del modo display (1 byte)
#define ADDR_QR_MODE        33  // Dirección del modo QR (1 byte)
#define ADDR_CLOCK_MODE     34  // Dirección del modo reloj (1 byte)
#define ADDR_SENS_MODE      35  // Dirección del modo sensor (1 byte)
#define ADDR_SN0            40  // Dirección del Serial Number 0 (1 byte)
#define ADDR_SN1            41  // Dirección del Serial Number 1 (1 byte)
#define ADDR_SN2            42  // Dirección del Serial Number 2 (1 byte)
#define ADDR_SN3            43  // Dirección del Serial Number 3 (1 byte)
#define ADDR_SN4            44  // Dirección del Serial Number 4 (1 byte)
#define ADDR_RELAY1_TIME    50  // Dirección tiempo relay 1 (1 byte)
#define ADDR_RELAY2_TIME    51  // Dirección tiempo relay 2 (1 byte)
#define ADDR_DDMM1_PRESENT  60  // Tiempo presencia DDMM1 (1 byte)
#define ADDR_DDMM1_ABSENT   61  // Tiempo ausencia DDMM1 (1 byte)
#define ADDR_DDMM2_PRESENT  62  // Tiempo presencia DDMM2 (1 byte)
#define ADDR_DDMM2_ABSENT   63  // Tiempo ausencia DDMM2 (1 byte)
#define ADDR_TICKET_LINEA1  70  // Linea 1 de ticket (16 bytes)
#define ADDR_TICKET_LINEA2  86  // Linea 2 de ticket (16 bytes)
#define ADDR_TICKET_LINEA3  102 // Linea 3 de ticket (16 bytes)
#define ADDR_TICKET_LINEA4  118 // Linea 4 de ticket (16 bytes)
#define ADDR_UNIDAD_MILES   134 // Unidad de mil de tickets (1 byte)
#define ADDR_TICKET_NUMBER  135 // Número de ticket (3 bytes)

// Variables externas
extern DeviceConfig config;        // Configuración del dispositivo
extern StatusInfo statusInfo;      // Información de status
extern CommandBuffer cmdBuffer;    // Buffer de comandos
extern RelayInfo relays[5];        // Información de los 5 relés

// Pines (modificar según tu hardware)
extern int DE_RE_PIN;              // Pin DE/RE para RS485
extern int RELAY_PINS[5];          // Pines para los relés

// Configuración serial
extern int RS485_BAUDRATE;         // Velocidad de comunicación RS485

#endif