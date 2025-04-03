#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

#include <Arduino.h>

// Estructura para la configuración del dispositivo
typedef struct {
    uint8_t deviceId;            // ID del dispositivo (0-255)
    char deviceIdStr[3];         // ID en formato string (ej. "00")
    char nombre_empresa[17];     // Nombre de la empresa (16 caracteres + null)
    uint8_t modo_tcpip485;       // Modo de comunicación TCP/IP485
    uint8_t modo_work;           // Modo de trabajo
    uint8_t modo_display;        // Modo de pantalla
    uint8_t modo_QR_8_12;        // Modo de lectura QR
    uint8_t modo_clock;          // Modo reloj
    uint8_t modo_sens_altura;    // Modo sensor de altura
    bool esPuertaEntrada;        // True si es puerta de entrada, false si es salida
} DeviceConfig;

// Estructura para almacenar información del status
typedef struct {
    uint16_t status;             // Status actual (16 bits)
    char statusHex[5];           // Status en formato hexadecimal (4 char + null)
    char rfidData[16];           // Datos RFID o código leído
    bool tarjetaLeida;           // Flag de lectura de tarjeta
    bool pulsoDetectado;         // Flag de detección de pulso
    bool scannerActivo;          // Flag de scanner activo
} StatusInfo;

// Estructura para respuestas de comandos
typedef struct {
    bool success;                // Si la operación fue exitosa
    char message[64];            // Mensaje descriptivo
    char data[128];              // Datos de respuesta (si los hay)
} CommandResponse;

// Estructura para buffer de comandos
typedef struct {
    char buffer[64];             // Buffer para comandos
    uint8_t index;               // Índice actual en el buffer
    bool complete;               // Si el comando está completo
} CommandBuffer;

// Estructura para la gestión de relés
typedef struct {
    uint8_t pin;                 // Pin GPIO
    uint8_t state;               // Estado (0=OFF, 1=ON, otros valores especiales)
    uint8_t time;                // Tiempo asociado (usado en algunos comandos)
    uint16_t tmr_100ms;          // Timer interno (contador de 100ms)
} RelayInfo;

#endif