# Comandos RS485 - Sistema OemProxy

## Estructura del Protocolo

### Formato de Comando
```
STX + ID[2] + FUNCION + SUBFUNCION + [DATOS] + ETX
```

- **STX**: 0x02 (Start of Text)
- **ID**: 2 caracteres hexadecimales del dispositivo (ej: "00", "01", "02"...)
- **FUNCION**: Código de función (A-Z)
- **SUBFUNCION**: Subcódigo específico (0-9, A-F)
- **DATOS**: Datos opcionales según el comando
- **ETX**: 0x03 (End of Text)

### Respuestas del Sistema
- **ACK**: 0x06 - Comando ejecutado correctamente
- **NAK**: 0x15 - Error en la ejecución del comando
- **SIB**: 0x1B - Fin de bloque de información de status

---

## Comandos Tipo "A" - Configuración del Dispositivo

### A0 - Configurar ID del Dispositivo
```
Comando: STX + ID + A + 0 + [NUEVO_ID_HEX] + ETX
Ejemplo: 0x02 + "00" + "A" + "0" + "05" + 0x03
```
Configura el ID del dispositivo (0-255 en hexadecimal).

### A1 - Consultar ID del Dispositivo
```
Comando: STX + ID + A + 1 + ETX
Respuesta: STX + ID + A + 1 + [ID_ACTUAL] + SIB
```

### A4 - Grabar Nombre de Empresa
```
Comando: STX + ID + A + 4 + [NOMBRE_16_CHARS] + ETX
Ejemplo: STX + "00" + "A" + "4" + "MI EMPRESA      " + ETX
```
Guarda el nombre de la empresa (máximo 16 caracteres).

### A5 - Consultar Nombre de Empresa
```
Comando: STX + ID + A + 5 + ETX
Respuesta: STX + ID + A + 5 + [NOMBRE_EMPRESA] + SIB
```

### A6 - Configurar Modo TCP/IP485
```
Comando: STX + ID + A + 6 + [MODO_HEX] + ETX
```
Configura el modo de comunicación TCP/IP o RS485.

### A7 - Consultar Modo TCP/IP485
```
Comando: STX + ID + A + 7 + ETX
Respuesta: STX + ID + A + 7 + [MODO_ACTUAL] + SIB
```

### AA - Configurar Serial Number Byte 0
```
Comando: STX + "02" + A + A + [BYTE_HEX] + ETX
```
**Nota**: Solo disponible para dispositivo ID=2.

---

## Comandos Tipo "S" - Control de Relés (Activar)

### S0 - Consultar Status
```
Comando: STX + ID + S + 0 + ETX
Respuesta: STX + ID + S + 0 + [STATUS_4_HEX] + [DATOS_RFID] + SIB
```

### S1-S5 - Activar Relés
```
S1: Activar Relé 1
S2: Activar Relé 2
S3: Activar Relé 3
S4: Activar Relé 4
S5: Activar Relé 5

Ejemplo: STX + "00" + "S" + "1" + ETX (Activa Relé 1)
```

### S6 - Indicar Playa Llena
```
Comando: STX + ID + S + 6 + ETX
```
Activa el Relé 2 para indicar playa llena e inhibir ingreso.

### S7 - Activar Barrera (Estado Permanente)
```
Comando: STX + ID + S + 7 + ETX
```
Activa el Relé 1 en estado permanente (estado 7).

---

## Comandos Tipo "R" - Control de Relés (Desactivar)

### R1-R5 - Desactivar Relés
```
R1: Desactivar Relé 1
R2: Desactivar Relé 2
R3: Desactivar Relé 3
R4: Desactivar Relé 4
R5: Desactivar Relé 5

Ejemplo: STX + "00" + "R" + "1" + ETX (Desactiva Relé 1)
```

### R6 - Indicar Playa Libre
```
Comando: STX + ID + R + 6 + ETX
```
Desactiva el Relé 2 para habilitar ingreso.

### R7 - Reiniciar Estado Lector/Scanner
```
Comando: STX + ID + R + 7 + ETX
```
Reinicia el estado del lector y scanner, limpia flags de error.

---

## Comandos Tipo "T" - Gestión de Tickets

### T0-T3 - Leer Líneas de Ticket
```
T0: Leer línea 1 del ticket
T1: Leer línea 2 del ticket
T2: Leer línea 3 del ticket
T3: Leer línea 4 del ticket

Respuesta: STX + ID + T + [NUM] + [TEXTO_LINEA] + SIB
```

### T4-T7 - Grabar Líneas de Ticket
```
T4: Grabar línea 1 (requiere 16 caracteres)
T5: Grabar línea 2 (requiere 16 caracteres)
T6: Grabar línea 3 (requiere 16 caracteres)  
T7: Grabar línea 4 (requiere 16 caracteres)

Ejemplo: STX + "00" + "T" + "4" + "LINEA 1 TICKET  " + ETX
```

### T9 - Imprimir Ticket
```
Comando: STX + ID + T + 9 + ETX
Respuesta: STX + ID + T + 9 + [NUMERO_TICKET_12_DIGITOS] + SIB
```

---

## Comandos Tipo "V" - Información del Sistema

### V0 - Consultar Versión
```
Comando: STX + ID + V + 0 + ETX
Respuesta: STX + ID + V + 0 + "OemProxy v1.0" + SIB
```

---

## Comandos Tipo "X" - Control del Sistema

### X0 / X9 - Reiniciar Dispositivo
```
Comando: STX + ID + X + 0 + ETX
Comando: STX + ID + X + 9 + ETX
```
Ambos comandos reinician el dispositivo ESP8266/ESP32.

---

## Comandos Tipo "Z" - Códigos de Barras

### Z0 - Configurar Códigos de Barras
```
Comando: STX + ID + Z + 0 + [UNIDAD_MIL][3_DIGITOS] + ETX
Ejemplo: STX + "00" + "Z" + "0" + "A123" + ETX
```

### Z1 - Consultar Configuración Códigos de Barras
```
Comando: STX + ID + Z + 1 + ETX
Respuesta: STX + ID + Z + 1 + [CONFIGURACION] + SIB
```

### Z9 - Mostrar Configuración en Display
```
Comando: STX + ID + Z + 9 + ETX
```

---

## Status Bits (Respuesta S0)

El comando S0 devuelve un status de 16 bits en formato hexadecimal:

| Bit | Valor | Descripción |
|-----|-------|-------------|
| 0 | 0x0001 | Detector de masa metálica 1 accionado |
| 1 | 0x0002 | Detector de masa metálica 2 accionado |
| 6 | 0x0040 | Relé 1 activado |
| 7 | 0x0080 | Relé 2 activado |
| 8 | 0x0100 | Lectura de tarjeta |
| 9 | 0x0200 | Indicación de fraude |
| 10 | 0x0400 | Pulsador de papel accionado |
| 11 | 0x0800 | Scanner activo |
| 15 | 0x8000 | Indicador de salida (vs entrada) |

---

## Ejemplo de Uso Completo

```bash
# Configurar dispositivo ID 00 con nombre "MI EMPRESA"
STX + "00" + "A" + "4" + "MI EMPRESA      " + ETX

# Activar relé 1 para abrir barrera
STX + "00" + "S" + "1" + ETX

# Consultar status
STX + "00" + "S" + "0" + ETX
# Respuesta: STX + "00" + "S" + "0" + "0040" + SIB (Relé 1 activo)

# Desactivar relé 1
STX + "00" + "R" + "1" + ETX

# Reiniciar dispositivo
STX + "00" + "X" + "0" + ETX
```

---

## Notas Importantes

1. **Direccionamiento**: Cada dispositivo tiene un ID único (00-99 en hex)
2. **Validación**: Solo se procesan comandos dirigidos al ID correcto del dispositivo
3. **EEPROM**: La configuración se guarda automáticamente en memoria no volátil
4. **Relés**: Lógica invertida - activo en LOW, inactivo en HIGH
5. **Timeouts**: Los relés pueden configurarse con temporizadores automáticos
6. **Estados Especiales**: Los relés soportan múltiples estados (permanente, pulsado, temporizado)
