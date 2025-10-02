# Witcher3-MP

> **Mod Multiplayer para The Witcher 3: Wild Hunt Next-Gen**

Un sistema de multiplayer cooperativo completo que permite a hasta 4 jugadores explorar, combatir y vivir aventuras juntos en el mundo de The Witcher 3.

## 🚀 Inicio Rápido

### Requisitos
- **The Witcher 3: Wild Hunt** (versión Next-Gen 4.0+)
- **Visual Studio 2022** (Community, Professional o Enterprise)
- **Windows 10/11** (64-bit)

### Instalación en 3 Pasos

```bash
# 1. Compilar el proyecto
witcher3_mp.bat build

# 2. Instalar el mod
witcher3_mp.bat install

# 3. Ejecutar el mod
witcher3_mp.bat start
```

¡Y listo! El servidor se iniciará automáticamente y The Witcher 3 se abrirá con el mod cargado.

## 🎮 Cómo Jugar en Multiplayer

### **Jugador 1 (Host/Servidor):**
```bash
# 1. Compilar e instalar
witcher3_mp.bat build
witcher3_mp.bat install

# 2. Configurar servidor (opcional)
witcher3_mp.bat config

# 3. Iniciar servidor + juego
witcher3_mp.bat start
```

### **Jugadores 2-4 (Clientes):**
```bash
# 1. Compilar e instalar
witcher3_mp.bat build
witcher3_mp.bat install

# 2. Conectar al servidor
witcher3_mp.bat connect
# Ingresar IP del host (ej: 192.168.1.100)

# 3. Iniciar el juego
# Ejecutar el script generado: connect_to_server.bat
```

### **Tipos de Conexión:**
- **🏠 Local**: Mismo PC (hasta 4 jugadores)
- **🌐 LAN**: Red local (hasta 4 jugadores)
- **🌍 Internet**: Diferentes ubicaciones (hasta 4 jugadores)

## ⚙️ Configuración de Red

### **Configuración Automática:**
```bash
# Configurar servidor
witcher3_mp.bat config

# Conectar a servidor
witcher3_mp.bat connect
```

### **Configuración Manual:**
1. **Editar** `mod/config/mp_config.json`
2. **Cambiar** `server_ip` y `server_port`
3. **Reiniciar** el servidor

### **Puertos y Firewall:**
- **Puerto por defecto**: 60000
- **Protocolo**: TCP
- **Firewall**: Permitir conexiones entrantes

## 📖 Documentación

- **[Guía de Instalación](docs/installation.md)** - Instalación detallada paso a paso
- **[Guía de Desarrollo](docs/development.md)** - Documentación técnica para desarrolladores
- **[Documentación de Red](docs/network.md)** - Especificaciones del protocolo de red
- **[Changelog](docs/changelog.md)** - Historial de cambios y versiones

## ✨ Características

### 🎮 Modo Cooperativo
- **Hasta 4 jugadores** en la misma sesión
- **Exploración compartida** del mundo de The Witcher 3
- **Combate cooperativo** con sincronización en tiempo real
- **Quests compartidas** y progresión sincronizada
- **Inventario sincronizado** entre jugadores

### 🌐 Sistema de Red
- **Servidor TCP** robusto y optimizado
- **Compresión LZ4** para reducir latencia
- **Batching inteligente** de mensajes
- **Predicción de movimiento** para suavidad
- **Sistema de logging** avanzado

### ⚙️ Configuración Flexible
- **Puerto configurable** (default: 60000)
- **Máximo de conexiones** ajustable
- **Modo de juego** personalizable
- **Optimizaciones de red** configurables
- **Sistema de logging** con niveles

## 🛠️ Desarrollo

### Compilación
```bash
# Compilación automática
run_mod.bat build

# Compilación manual
cd scripts
build_ultra_simple.bat
```

### Estructura del Proyecto
```
Witcher3-MP/
├── include/          # Headers del proyecto
├── src/             # Código fuente C++
├── mod/             # Archivos del mod para TW3
├── scripts/         # Scripts de compilación
├── docs/            # Documentación
└── build/           # Archivos compilados
```

### Scripts Disponibles
- `witcher3_mp.bat build` - Compila el proyecto
- `witcher3_mp.bat install` - Instala el mod en TW3
- `witcher3_mp.bat start` - Ejecuta el mod (servidor + juego)
- `witcher3_mp.bat config` - Configura servidor (IP, puerto, etc.)
- `witcher3_mp.bat connect` - Conecta a servidor existente
- `witcher3_mp.bat clean` - Limpia archivos temporales
- `witcher3_mp.bat help` - Muestra ayuda

## 🎯 Estado Actual

### ✅ Funcionando
- Servidor TCP funcional
- Cliente-servidor básico
- Mod se carga en The Witcher 3
- Sistema de logging
- Compresión de red
- Scripts de automatización

### 🚧 En Desarrollo
- Sistema de combate avanzado
- Chat en tiempo real
- Sincronización de inventario mejorada
- Sistema de quests compartidas

### 🔮 Planificado
- Soporte para más de 4 jugadores
- Modo PvP
- Sistema de mods compartidos
- Integración con REDkit

## 🔧 Solución de Problemas

### **Problemas de Conexión:**
- **"No se puede conectar"**: Verificar IP y puerto
- **"Servidor no encontrado"**: Verificar que el servidor esté ejecutándose
- **"Conexión rechazada"**: Verificar firewall y configuración de red

### **Problemas de Instalación:**
- **"Visual Studio not found"**: Ejecutar desde Developer Command Prompt
- **"The Witcher 3 not found"**: Verificar ruta de instalación
- **"Mod not loading"**: Verificar que el mod esté instalado correctamente

### **Comandos de Diagnóstico:**
```bash
# Verificar configuración
witcher3_mp.bat config

# Limpiar e reinstalar
witcher3_mp.bat clean
witcher3_mp.bat build
witcher3_mp.bat install
```

## 🤝 Contribuir

### Cómo Contribuir
1. **Fork** del repositorio
2. **Crear branch** para nueva funcionalidad
3. **Desarrollar** y probar
4. **Pull Request** con descripción detallada

### Estándares
- **C++20** estándar
- **Documentación** en español
- **Testing** requerido para nuevas funcionalidades
- **Logging** consistente

## 🐛 Reportar Problemas

Si encuentras algún problema:
1. Revisar [documentación](docs/)
2. Verificar [changelog](docs/changelog.md)
3. Crear issue con detalles del problema
4. Incluir logs y configuración

## 📄 Licencia

Este proyecto está bajo la licencia MIT. Ver [LICENSE](LICENSE) para más detalles.

## 🙏 Agradecimientos

- **CD Projekt RED** por The Witcher 3
- **Comunidad de modders** de The Witcher 3
- **Contribuidores** del proyecto

## 📞 Soporte

- **Documentación**: [docs/](docs/)
- **Issues**: [GitHub Issues](https://github.com/tu-usuario/witcher3-mp/issues)
- **Discusiones**: [GitHub Discussions](https://github.com/tu-usuario/witcher3-mp/discussions)

---

**¡Disfruta explorando el mundo de The Witcher 3 con tus amigos!** 🗡️⚔️

---

*Última actualización: 19 de Diciembre, 2024*