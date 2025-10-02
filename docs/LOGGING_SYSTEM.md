# 📝 Sistema de Logging de Witcher3-MP

## 🎯 **Descripción General**

El sistema de logging de Witcher3-MP es un sistema avanzado de registro de eventos diseñado específicamente para el mod de multiplayer de The Witcher 3. Proporciona logging en tiempo real, buffered logging para optimizar el rendimiento, y comandos de consola para control dinámico.

## 🏗️ **Arquitectura del Sistema**

### **Componentes Principales**

1. **Logger C++** (`src/utils/Logger.cpp`): Sistema de logging principal
2. **LoggerIntegration** (`src/integration/LoggerIntegration.cpp`): Integración con WitcherScript
3. **ConsoleCommands** (`src/utils/ConsoleCommands.cpp`): Sistema de comandos de consola
4. **MP_Logger.ws**: Interfaz WitcherScript para el logging
5. **MP_ConsoleSystem.ws**: Sistema de consola para WitcherScript

### **Flujo de Datos**

```
WitcherScript → MP_Logger.ws → LoggerIntegration.cpp → Logger.cpp → logs/mp_session.log
     ↓
Console Commands → ConsoleCommands.cpp → Logger.cpp
```

## 📊 **Niveles de Log**

| Nivel | Valor | Descripción |
|-------|-------|-------------|
| DEBUG | 0 | Información detallada para debugging |
| INFO | 1 | Información general del sistema |
| WARNING | 2 | Advertencias que no detienen la ejecución |
| ERROR | 3 | Errores que pueden afectar funcionalidad |
| CRITICAL | 4 | Errores críticos que requieren atención inmediata |

## 🏷️ **Categorías de Log**

| Categoría | Descripción | Uso Típico |
|-----------|-------------|------------|
| GENERAL | Eventos generales | Inicialización, configuración |
| NETWORK | Eventos de red | Conexiones, paquetes, latencia |
| COMBAT | Sistema de combate | Ataques, daño, animaciones |
| QUEST | Sistema de misiones | Inicio, progreso, finalización |
| INVENTORY | Inventario | Sincronización, items, equipamiento |
| DIALOG | Diálogos | Conversaciones, opciones |
| PLAYER | Jugadores | Conexiones, desconexiones, acciones |
| SYSTEM | Sistema | Errores, rendimiento, configuración |

## 🚀 **Características Principales**

### **1. Buffered Logging**
- **Propósito**: Optimizar el rendimiento del juego
- **Funcionamiento**: Los logs se almacenan en memoria y se escriben en lotes
- **Configuración**: Buffer size configurable (por defecto: 1000 entradas)
- **Flush**: Comando `mp_log flush` para volcar logs inmediatamente

### **2. Timestamps Precisos**
- **Formato**: `YYYY-MM-DD HH:MM:SS.mmm`
- **Precisión**: Milisegundos
- **Ejemplo**: `[2025-02-10 14:30:25.123] [INFO] [NETWORK] [NetworkSystem] Connection established`

### **3. Filtrado por Nivel y Categoría**
- **Nivel**: Solo se registran logs del nivel configurado o superior
- **Categoría**: Cada categoría puede habilitarse/deshabilitarse independientemente
- **Control dinámico**: Cambios en tiempo real sin reiniciar

### **4. Integración con WitcherScript**
- **Funciones nativas**: Llamadas directas desde WitcherScript a C++
- **Macros de conveniencia**: `LOG_PLAYER_CONNECTION`, `LOG_QUEST_EVENT`, etc.
- **Sincronización**: Thread-safe para uso concurrente

## 🎮 **Comandos de Consola**

### **Comando Principal: `mp_log`**

#### **Subcomandos Disponibles:**

| Comando | Descripción | Ejemplo |
|---------|-------------|---------|
| `flush` | Volcar logs buffered al archivo | `mp_log flush` |
| `level <nivel>` | Cambiar nivel de log | `mp_log level debug` |
| `enable <categoría>` | Habilitar categoría | `mp_log enable combat` |
| `disable <categoría>` | Deshabilitar categoría | `mp_log disable network` |
| `performance` | Toggle performance logging | `mp_log performance` |
| `status` | Mostrar estado del logging | `mp_log status` |

#### **Ejemplos de Uso:**

```bash
# Cambiar a nivel debug
mp_log level debug

# Habilitar logging de combate
mp_log enable combat

# Deshabilitar logging de red
mp_log disable network

# Volcar logs inmediatamente
mp_log flush

# Ver estado actual
mp_log status
```

### **Otros Comandos de Consola:**

| Comando | Descripción |
|---------|-------------|
| `mp_status` | Estado general del multiplayer |
| `mp_quest` | Gestión de misiones |
| `mp_inventory` | Gestión de inventario |
| `mp_network` | Estado de la red |
| `help` | Ayuda general |
| `clear` | Limpiar consola |

## 📁 **Estructura de Archivos de Log**

### **Ubicación Principal**
```
logs/
└── mp_session.log          # Archivo principal de logs
```

### **Formato de Entrada de Log**
```
[timestamp] [level] [category] [source] message
```

### **Ejemplo de Log Entry**
```
[2025-02-10 14:30:25.123] [INFO] [NETWORK] [NetworkSystem] Player player123 connected
[2025-02-10 14:30:26.456] [INFO] [QUEST] [QuestSystem] Quest [main_quest_001] started (Player: player123)
[2025-02-10 14:30:27.789] [INFO] [INVENTORY] [InventorySystem] Inventory sync - Player: player123, Item: sword_001, Action: added
[2025-02-10 14:30:28.012] [WARNING] [COMBAT] [CombatSystem] Low damage detected - Player: player123 - Damage: 5
[2025-02-10 14:30:29.345] [ERROR] [NETWORK] [NetworkSystem] Connection timeout - Player: player456
```

## ⚙️ **Configuración**

### **Archivo de Configuración: `mod/config/mp_config.json`**

```json
{
  "logging": {
    "level": "INFO",
    "enable_file_logging": true,
    "log_file": "logs/mp_session.log",
    "enable_console_logging": true,
    "enable_buffered_logging": true,
    "buffer_size": 1000,
    "max_log_size": "10MB",
    "max_log_files": 5,
    "enable_performance_logging": false,
    "categories": {
      "general": true,
      "network": true,
      "combat": true,
      "quest": true,
      "inventory": true,
      "dialog": true,
      "player": true,
      "system": true
    }
  }
}
```

### **Parámetros de Configuración**

| Parámetro | Tipo | Descripción | Valor por Defecto |
|-----------|------|-------------|-------------------|
| `level` | string | Nivel de log | "INFO" |
| `enable_file_logging` | bool | Habilitar logging a archivo | true |
| `log_file` | string | Ruta del archivo de log | "logs/mp_session.log" |
| `enable_console_logging` | bool | Habilitar logging a consola | true |
| `enable_buffered_logging` | bool | Habilitar buffered logging | true |
| `buffer_size` | int | Tamaño del buffer | 1000 |
| `max_log_size` | string | Tamaño máximo del archivo | "10MB" |
| `max_log_files` | int | Número máximo de archivos | 5 |
| `enable_performance_logging` | bool | Habilitar logging de rendimiento | false |

## 🔧 **Uso en Código**

### **C++ (Sistema Principal)**

```cpp
#include "utils/Logger.h"

// Logging básico
LOG_INFO("Sistema inicializado");
LOG_ERROR("Error de conexión");

// Logging con categorías
LOG_INFO_CAT(LogCategory::NETWORK, "Conexión establecida");
LOG_WARNING_CAT(LogCategory::COMBAT, "Daño bajo detectado");

// Logging especializado
LOG_PLAYER_CONNECTION("player123", "connected");
LOG_QUEST_EVENT("quest_001", "started", "player123");
LOG_INVENTORY_SYNC("player123", "sword_001", "added");
```

### **WitcherScript (Mod)**

```javascript
// Obtener instancia del logger
var logger = MP_Logger.GetInstance();

// Logging básico
logger.LogInfo("Mod inicializado");
logger.LogError("Error en el sistema");

// Logging con categorías
logger.LogInfo(MP_Logger.NETWORK_CATEGORY, "Conexión establecida");
logger.LogWarning(MP_Logger.COMBAT_CATEGORY, "Daño bajo detectado");

// Logging especializado
logger.LogPlayerConnection("player123", "connected");
logger.LogQuestEvent("quest_001", "started", "player123");
logger.LogInventorySync("player123", "sword_001", "added");
```

## 🧪 **Testing**

### **Ejecutar Tests del Sistema de Logging**

```bash
# Compilar tests
scripts\build.bat

# Ejecutar demo
scripts\demo_logging.bat

# Ejecutar tests unitarios
scripts\run_tests.bat
```

### **Tests Disponibles**

1. **test_logging_system.cpp**: Tests completos del sistema
2. **demo_logging.bat**: Demostración interactiva
3. **Tests de integración**: Verificación de funcionalidad completa

## 📈 **Monitoreo de Rendimiento**

### **Performance Logging**

```cpp
// Iniciar monitoreo
Logger::GetInstance().StartPerformanceLogging();

// Logear operaciones
Logger::GetInstance().LogPerformance("network_sync", 15.5);
Logger::GetInstance().LogPerformance("inventory_update", 8.2);

// Detener monitoreo
Logger::GetInstance().StopPerformanceLogging();
```

### **Métricas Disponibles**

- **Tiempo de operaciones**: Duración de funciones críticas
- **Frecuencia de logs**: Número de logs por segundo
- **Uso de memoria**: Tamaño del buffer de logs
- **I/O de disco**: Escritura de archivos de log

## 🐛 **Solución de Problemas**

### **Problemas Comunes**

#### **1. Logs no aparecen**
- **Causa**: Nivel de log muy alto
- **Solución**: `mp_log level debug`

#### **2. Archivo de log no se crea**
- **Causa**: Permisos de escritura o directorio no existe
- **Solución**: Verificar permisos y crear directorio `logs/`

#### **3. Performance degradada**
- **Causa**: Buffered logging deshabilitado
- **Solución**: `mp_log status` y verificar configuración

#### **4. Logs duplicados**
- **Causa**: Múltiples instancias del logger
- **Solución**: Verificar inicialización única

### **Comandos de Diagnóstico**

```bash
# Ver estado del logging
mp_log status

# Verificar configuración
mp_log level debug
mp_log enable system

# Forzar flush de logs
mp_log flush

# Ver estado general
mp_status
```

## 📚 **API Reference**

### **Funciones C++ Principales**

```cpp
// Configuración
void SetFileLogging(bool enable, const std::string& filename);
void SetConsoleLogging(bool enable);
void SetLogLevel(LogLevel level);
void SetBufferedLogging(bool enable, size_t bufferSize);

// Logging básico
void Log(LogLevel level, LogCategory category, const std::string& message, const std::string& source);
void Debug(LogCategory category, const std::string& message, const std::string& source);
void Info(LogCategory category, const std::string& message, const std::string& source);
void Warning(LogCategory category, const std::string& message, const std::string& source);
void Error(LogCategory category, const std::string& message, const std::string& source);
void Critical(LogCategory category, const std::string& message, const std::string& source);

// Logging especializado
void LogPlayerConnection(const std::string& playerId, const std::string& action);
void LogQuestEvent(const std::string& questId, const std::string& event, const std::string& playerId);
void LogInventorySync(const std::string& playerId, const std::string& itemId, const std::string& action);
void LogDialogEvent(const std::string& dialogId, const std::string& event, const std::string& playerId);
void LogNetworkEvent(const std::string& event, const std::string& details);
void LogCombatEvent(const std::string& event, const std::string& playerId, const std::string& details);

// Control
void FlushLogs();
void EnableCategory(LogCategory category, bool enable);
void StartPerformanceLogging();
void StopPerformanceLogging();
```

### **Funciones WitcherScript**

```javascript
// Logging básico
void LogDebug(category: int, message: String, source: String);
void LogInfo(category: int, message: String, source: String);
void LogWarning(category: int, message: String, source: String);
void LogError(category: int, message: String, source: String);
void LogCritical(category: int, message: String, source: String);

// Logging especializado
void LogPlayerConnection(playerId: String, action: String);
void LogQuestEvent(questId: String, event: String, playerId: String);
void LogInventorySync(playerId: String, itemId: String, action: String);
void LogDialogEvent(dialogId: String, event: String, playerId: String);
void LogNetworkEvent(event: String, details: String);
void LogCombatEvent(event: String, playerId: String, details: String);

// Control
void FlushLogs();
void SetLogLevel(level: String);
void EnableCategory(category: String, enable: bool);
void TogglePerformanceLogging();
```

## 🎯 **Mejores Prácticas**

### **1. Uso de Categorías**
- **Usa categorías específicas** para diferentes sistemas
- **Evita GENERAL** para eventos específicos
- **Mantén consistencia** en el uso de categorías

### **2. Niveles de Log**
- **DEBUG**: Solo para desarrollo
- **INFO**: Eventos importantes del usuario
- **WARNING**: Problemas que no detienen la ejecución
- **ERROR**: Errores que afectan funcionalidad
- **CRITICAL**: Errores que requieren atención inmediata

### **3. Performance**
- **Usa buffered logging** en producción
- **Evita logging excesivo** en bucles críticos
- **Usa performance logging** solo cuando sea necesario

### **4. Mensajes de Log**
- **Sé descriptivo** pero conciso
- **Incluye contexto** relevante
- **Usa formato consistente** para parsing

## 🚀 **Conclusión**

El sistema de logging de Witcher3-MP proporciona:

- **✅ Logging completo** para todos los sistemas del mod
- **✅ Optimización de rendimiento** con buffered logging
- **✅ Control dinámico** via comandos de consola
- **✅ Integración perfecta** entre C++ y WitcherScript
- **✅ Timestamps precisos** para debugging
- **✅ Filtrado flexible** por nivel y categoría
- **✅ Monitoreo de rendimiento** integrado

**¡El sistema está listo para usar en producción!** 🎮
