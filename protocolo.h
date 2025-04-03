#ifndef PROTOCOLO_H
#define PROTOCOLO_H

#include "estructuras.h"
#include "variables.h"

// Funciones de parsing de comandos
bool parseCommand(const char* cmd, char* functionCode, char* subCode, char* data, int* dataLen);
bool validateCommand(const char* cmd, int len);
bool isCheckSumValid(const char* cmd, int len);

// Funciones para procesar comandos por tipo
CommandResponse processCommand(const char* cmd);
CommandResponse processA_Command(char subCode, const char* data, int dataLen);
CommandResponse processB_Command(char subCode, const char* data, int dataLen);
CommandResponse processC_Command(char subCode, const char* data, int dataLen);
CommandResponse processD_Command(char subCode, const char* data, int dataLen);
CommandResponse processE_Command(char subCode, const char* data, int dataLen);
CommandResponse processG_Command(char subCode, const char* data, int dataLen);
CommandResponse processH_Command(char subCode, const char* data, int dataLen);
CommandResponse processJ_Command(char subCode, const char* data, int dataLen);
CommandResponse processK_Command(char subCode, const char* data, int dataLen);
CommandResponse processM_Command(char subCode, const char* data, int dataLen);
CommandResponse processO_Command(char subCode, const char* data, int dataLen);
CommandResponse processP_Command(char subCode, const char* data, int dataLen);
CommandResponse processR_Command(char subCode, const char* data, int dataLen);
CommandResponse processS_Command(char subCode, const char* data, int dataLen);
CommandResponse processT_Command(char subCode, const char* data, int dataLen);
CommandResponse processV_Command(char subCode, const char* data, int dataLen);
CommandResponse processX_Command(char subCode, const char* data, int dataLen);
CommandResponse processZ_Command(char subCode, const char* data, int dataLen);

// Funciones para enviar comandos
bool sendCommand(const char* functionCode, const char* subCode, const char* data);
bool sendRawCommand(const char* cmd);

// Funciones de respuesta estándar
bool sendACK();
bool sendNAK();
bool sendStatus();
bool sendData(const char* functionCode, const char* subCode, const char* data);

// Funciones para recepción de datos
bool processIncomingByte(uint8_t byte);
void clearCommandBuffer();
bool isCommandComplete();
const char* getCommand();

// Funciones de relay
bool activateRelay(int relayNum);
bool deactivateRelay(int relayNum);
bool setRelayTimer(int relayNum, uint8_t time);
uint8_t getRelayTimer(int relayNum);
void updateRelays();

#endif