# Witcher3-MP - Enhanced Multiplayer Mod

## 🎮 Descripción
Mod de multijugador mejorado para The Witcher 3: Wild Hunt con sistema de gestión de versiones, configuración avanzada y logging estructurado.

## ✨ Nuevas Características

### 🔧 Sistema de Gestión de Versiones
- **Detección automática** de la versión del juego
- **Verificación de compatibilidad** con versiones soportadas
- **Detección de actualizaciones** del juego en tiempo real
- **Soporte para múltiples versiones** del juego

### ⚙️ Sistema de Configuración Avanzado
- **Configuración JSON** fácil de editar
- **Validación automática** de configuración
- **Configuración por defecto** inteligente
- **Sistema de overrides** para personalización

### 📝 Sistema de Logging Mejorado
- **Logging estructurado** con niveles (DEBUG, INFO, WARNING, ERROR, CRITICAL)
- **Logging a archivo** y consola
- **Timestamps precisos** con milisegundos
- **Rotación automática** de logs

### 🛡️ Mejoras de Seguridad
- **Validación de entrada** mejorada
- **Manejo de errores** robusto
- **Verificación de integridad** del juego
- **Protección contra actualizaciones** incompatibles

## 🚀 Instalación

### Requisitos
- The Witcher 3: Wild Hunt (versión 1.28 o superior)
- Visual Studio 2019 o superior
- Windows 10/11

### Pasos de Instalación
1. **Clonar el repositorio**
   ```bash
   git clone https://github.com/Hyjarion/Witcher3-MP.git
   cd Witcher3-MP
   ```

2. **Compilar el proyecto**
   - Abrir `Witcher3-MP.sln` en Visual Studio
   - Seleccionar configuración Release x64
   - Compilar la solución

3. **Configurar el servidor**
   - Editar `config.json` según tus necesidades
   - Ajustar el puerto y otras configuraciones

4. **Ejecutar el servidor**
   ```bash
   Witcher3-MP.exe
   ```

## 📋 Configuración

### Archivo config.json
```json
{
  "network": {
    "port": 7777,
    "host": "localhost",
    "maxConnections": 32,
    "timeoutSeconds": 30
  },
  "version": {
    "autoDetectVersion": true,
    "supportedVersions": ["1.32", "1.31", "1.30"]
  },
  "logging": {
    "logLevel": "INFO",
    "enableFileLogging": true,
    "logFilePath": "witcher3mp.log"
  },
  "game": {
    "maxPlayers": 16,
    "tickRate": 60.0,
    "enablePvP": true
  }
}
```

### Variables de Entorno
- `WITCHER3MP_PORT`: Puerto del servidor
- `WITCHER3MP_LOG_LEVEL`: Nivel de logging
- `WITCHER3MP_DEBUG`: Habilitar modo debug

## 🔍 Solución de Problemas

### Problemas de Compatibilidad
1. **Verificar versión del juego**
   ```bash
   # El servidor mostrará la versión detectada al iniciar
   ```

2. **Actualizar versiones soportadas**
   - Editar `version.json`
   - Añadir nueva versión a `supportedVersions`

3. **Forzar detección de versión**
   - Establecer `autoDetectVersion: false` en config.json
   - Especificar `gameVersion` manualmente

### Problemas de Red
1. **Verificar puerto**
   - Asegurar que el puerto no esté en uso
   - Verificar firewall y antivirus

2. **Logs de conexión**
   - Revisar `witcher3mp.log` para errores
   - Verificar configuración de red

### Problemas de Rendimiento
1. **Ajustar configuración**
   - Reducir `maxPlayers` si hay lag
   - Ajustar `tickRate` según hardware
   - Habilitar `enableCompression`

2. **Monitorear logs**
   - Revisar logs de rendimiento
   - Identificar cuellos de botella

## 📊 Monitoreo

### Logs
- **witcher3mp.log**: Log principal del servidor
- **Niveles de log**: DEBUG, INFO, WARNING, ERROR, CRITICAL
- **Rotación**: Los logs se rotan automáticamente

### Métricas
- **Conexiones activas**: Monitoreo en tiempo real
- **Rendimiento**: Tick rate y latencia
- **Errores**: Conteo y tipos de errores

## 🔄 Actualizaciones

### Actualización del Mod
1. **Backup de configuración**
   ```bash
   cp config.json config.json.backup
   cp version.json version.json.backup
   ```

2. **Actualizar archivos**
   - Reemplazar ejecutables
   - Mantener archivos de configuración

3. **Verificar compatibilidad**
   - El servidor verificará automáticamente
   - Revisar logs para advertencias

### Actualización del Juego
1. **Verificar compatibilidad**
   - El servidor detectará la actualización
   - Verificar si la versión es soportada

2. **Actualizar configuración**
   - Añadir nueva versión a `supportedVersions`
   - Probar funcionamiento

## 🤝 Contribución

### Reportar Bugs
1. **Incluir información**
   - Versión del juego
   - Versión del mod
   - Logs relevantes
   - Pasos para reproducir

2. **Crear issue**
   - Usar template de bug report
   - Adjuntar archivos de log

### Sugerir Mejoras
1. **Crear feature request**
   - Describir funcionalidad
   - Justificar necesidad
   - Proponer implementación

## 📄 Licencia

Este proyecto está bajo la Licencia GNU Affero General Public License v3.0.

## 🙏 Agradecimientos

- **Hyjarion**: Desarrollador original
- **Comunidad de The Witcher 3**: Feedback y testing
- **Contribuidores**: Mejoras y correcciones

## 📞 Soporte

- **GitHub Issues**: Para bugs y feature requests
- **Discord**: Para soporte en tiempo real
- **Documentación**: Wiki del proyecto

---

**Versión del Mod**: 0.0.1  
**Última Actualización**: 2024-01-01  
**Versiones Soportadas**: 1.28 - 1.32
