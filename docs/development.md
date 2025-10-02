# Guía de Desarrollo - Witcher3-MP

## Arquitectura del Proyecto

### Estructura de Directorios
```
Witcher3-MP/
├── include/                 # Headers del proyecto
│   ├── networking/         # Sistema de red
│   ├── optimization/       # Optimizaciones de red
│   ├── integration/        # Integración con TW3
│   └── utils/             # Utilidades
├── src/                    # Código fuente
│   ├── networking/        # Implementación de red
│   ├── optimization/      # Implementación de optimizaciones
│   ├── integration/       # Implementación de integración
│   └── utils/            # Implementación de utilidades
├── mod/                   # Archivos del mod para TW3
│   ├── config/           # Configuración del mod
│   └── scripts/game/     # Scripts de WitcherScript
├── scripts/              # Scripts de compilación
├── docs/                # Documentación
└── build/               # Archivos compilados
```

## Compilación

### Requisitos de Desarrollo
- **Visual Studio 2022** (Community o superior)
- **Windows SDK 10.0** o superior
- **C++20** habilitado
- **The Witcher 3: Wild Hunt** (para testing)

### Compilación Rápida
```cmd
# Script automático
run_mod.bat build

# Script manual
cd scripts
build_ultra_simple.bat
```

### Compilación Manual
```cmd
# 1. Abrir Visual Studio Developer Command Prompt
# 2. Navegar al proyecto
cd "C:\Users\USUARIO\OneDrive\Escritorio\Mod-TW3"

# 3. Compilar
cl /EHsc /W3 /std:c++20 /I"include" src\core\MainUltraSimple.cpp /Fe:"build\Release\Witcher3-MP.exe" /link

# 4. Verificar
dir build\Release\Witcher3-MP.exe
```

### Compilación con CMake (Alternativa)
```cmd
# Configurar
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Compilar
cmake --build build --config Release

# Ejecutar
build\Release\Witcher3-MP.exe
```

## Instalación del Mod

### Instalación Automática
```cmd
# Script automático
run_mod.bat install

# Script manual
cd scripts
install_mod.bat
```

### Instalación Manual
```cmd
# 1. Crear directorio del mod
mkdir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp"

# 2. Copiar archivos del mod
xcopy "mod\*" "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\" /E /Y

# 3. Copiar servidor
copy "build\Release\Witcher3-MP.exe" "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\"

# 4. Crear scripts de lanzamiento
# (Ver scripts/create_startup_script.bat)
```

## Ejecución y Testing

### Ejecutar Servidor
```cmd
# Opción 1: Script automático
run_mod.bat start

# Opción 2: Manual
cd "E:\SteamLibrary\steamapps\common\The Witcher 3"
start_server.bat
```

### Ejecutar Cliente (Juego)
```cmd
# Opción 1: Script automático
run_mod.bat start

# Opción 2: Manual
cd "E:\SteamLibrary\steamapps\common\The Witcher 3"
launch_game.bat
```

### Verificar Funcionamiento
1. **Servidor**: Debe mostrar "Server started on port 60000"
2. **Cliente**: The Witcher 3 debe abrirse con el mod cargado
3. **Logs**: Revisar consola del servidor para mensajes de conexión

## Desarrollo de Funcionalidades

### Agregar Nuevos MessageTypes
1. Editar `include/networking/MessageTypes.h`
2. Agregar nuevo tipo al enum
3. Implementar procesamiento en `net_client.cpp` y `net_server.cpp`
4. Actualizar `NetworkOptimizer.cpp` si es necesario

### Agregar Nuevos Scripts de WitcherScript
1. Crear archivo `.ws` en `mod/scripts/game/witcher3_mp/`
2. Implementar funcionalidad en WitcherScript
3. Integrar con `MP_ModInitializer.ws`
4. Probar en el juego

### Modificar Configuración
1. Editar `mod/config/mp_config.json`
2. Actualizar `ConfigManager.cpp` si es necesario
3. Reiniciar servidor para aplicar cambios

## Debugging

### Logs del Servidor
- **Consola**: Mensajes en tiempo real
- **Archivo**: `logs/network.log` (si está habilitado)
- **Nivel**: Configurable en `mp_config.json`

### Logs del Cliente (Juego)
- **Consola**: F1 o F2 en el juego
- **Archivo**: `logs/mp_session.log` (si está habilitado)

### Herramientas de Debug
```cmd
# Verificar procesos
tasklist | findstr "Witcher3"

# Verificar puertos
netstat -an | findstr "60000"

# Verificar archivos del mod
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp"
```

## Testing

### Tests Unitarios
```cmd
# Compilar tests
cd scripts
build_test.bat

# Ejecutar tests
cd build\Release
test_network_connection.exe
```

### Tests de Integración
1. **Servidor**: Verificar que acepta conexiones
2. **Cliente**: Verificar que se conecta al servidor
3. **Mod**: Verificar que se carga en el juego
4. **Red**: Verificar comunicación cliente-servidor

### Tests de Rendimiento
- **Latencia**: Medir tiempo de respuesta
- **Compresión**: Verificar eficiencia de compresión
- **Memoria**: Monitorear uso de memoria
- **CPU**: Monitorear uso de CPU

## Contribución

### Flujo de Trabajo
1. **Fork** del repositorio
2. **Branch** para nueva funcionalidad
3. **Desarrollo** y testing
4. **Pull Request** con descripción detallada

### Estándares de Código
- **C++20** estándar
- **Naming**: camelCase para variables, PascalCase para clases
- **Comentarios**: Documentar funciones públicas
- **Logging**: Usar sistema de logging consistente

### Testing Requerido
- [ ] Compilación exitosa
- [ ] Tests unitarios pasan
- [ ] Mod se carga en el juego
- [ ] Servidor acepta conexiones
- [ ] Cliente se conecta al servidor

## Troubleshooting

### Problemas Comunes

#### Error de Compilación
```cmd
# Verificar Visual Studio
where cl

# Verificar includes
dir include\networking\*.h

# Limpiar y recompilar
run_mod.bat clean
run_mod.bat build
```

#### Error de Instalación
```cmd
# Verificar ruta de TW3
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\x64\witcher3.exe"

# Verificar permisos
# Ejecutar como administrador
```

#### Error de Ejecución
```cmd
# Verificar dependencias
dumpbin /dependents build\Release\Witcher3-MP.exe

# Verificar logs
type logs\network.log
```

### Logs de Debug
```cmd
# Habilitar debug en mp_config.json
"debug": {
    "enable_debug_mode": true,
    "enable_performance_monitoring": true,
    "enable_network_debug": true
}
```

## Recursos Adicionales

### Documentación Externa
- [The Witcher 3 Modding Guide](https://wiki.nexusmods.com/index.php/The_Witcher_3_Modding_Guide)
- [WitcherScript Documentation](https://wiki.nexusmods.com/index.php/WitcherScript)
- [Visual Studio C++ Documentation](https://docs.microsoft.com/en-us/cpp/)

### Herramientas Útiles
- **Visual Studio Code** (para edición)
- **Git** (para control de versiones)
- **CMake** (para compilación alternativa)
- **Wireshark** (para debugging de red)

---

**Nota**: Esta guía está diseñada para desarrolladores con experiencia en C++ y modding de The Witcher 3.
