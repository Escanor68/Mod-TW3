# Guía de Instalación - Witcher3-MP

## Requisitos del Sistema

### Software Requerido
- **The Witcher 3: Wild Hunt** (versión Next-Gen 4.0+)
- **Visual Studio 2022** (Community, Professional o Enterprise)
- **Windows 10/11** (64-bit)
- **Steam, GOG o Epic Games** (para The Witcher 3)

### Hardware Mínimo
- **CPU**: Intel Core i5-7400 / AMD Ryzen 5 1600
- **RAM**: 8 GB
- **GPU**: NVIDIA GTX 1060 6GB / AMD RX 580 8GB
- **Espacio**: 2 GB libres

## Instalación Rápida

### 1. Compilar el Proyecto
```bash
# Opción 1: Script automático
witcher3_mp.bat build

# Opción 2: Manual
cd scripts
build_ultra_simple.bat
```

### 2. Instalar el Mod
```bash
# Opción 1: Script automático
run_mod.bat install

# Opción 2: Manual
cd scripts
install_mod.bat
```

### 3. Ejecutar el Mod
```bash
# Opción 1: Script automático
run_mod.bat start

# Opción 2: Manual
cd "E:\SteamLibrary\steamapps\common\The Witcher 3"
start_server.bat
launch_game.bat
```

## Instalación Manual Detallada

### Paso 1: Preparar el Entorno
1. Abrir **Visual Studio Developer Command Prompt**
2. Navegar al directorio del proyecto:
   ```cmd
   cd "C:\Users\USUARIO\OneDrive\Escritorio\Mod-TW3"
   ```

### Paso 2: Compilar el Servidor
```cmd
# Compilar servidor
scripts\build_ultra_simple.bat

# Verificar compilación
dir build\Release\Witcher3-MP.exe
```

### Paso 3: Instalar en The Witcher 3
```cmd
# Ejecutar instalador
scripts\install_mod.bat

# Verificar instalación
dir "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp"
```

### Paso 4: Configurar el Mod
1. Editar `mods\witcher3_mp\config\mp_config.json`
2. Ajustar configuración según necesidades:
   - Puerto del servidor (default: 60000)
   - Máximo de conexiones (default: 4)
   - Modo de juego (coop_story)

## Verificación de la Instalación

### 1. Verificar Archivos del Mod
```
E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\
├── Witcher3-MP.exe          # Servidor
├── modInfo.ws               # Información del mod
├── config\
│   └── mp_config.json       # Configuración
└── scripts\game\witcher3_mp\
    ├── MP_ModInitializer.ws
    ├── MP_ServerManager.ws
    ├── MP_REDkitIntegration.ws
    └── MP_WeaponsDatabase.ws
```

### 2. Verificar Scripts de Lanzamiento
```
E:\SteamLibrary\steamapps\common\The Witcher 3\
├── start_server.bat         # Iniciar servidor
├── launch_game.bat          # Lanzar juego
└── start_nextgen.bat        # Lanzar todo junto
```

### 3. Probar la Instalación
1. Ejecutar `start_server.bat`
2. Verificar que aparezca: "Server started on port 60000"
3. Ejecutar `launch_game.bat`
4. Verificar que The Witcher 3 se abra con el mod cargado

## Solución de Problemas

### Error: "Visual Studio compiler not found"
**Solución**: Ejecutar desde Visual Studio Developer Command Prompt

### Error: "The Witcher 3 installation not found"
**Solución**: 
1. Verificar ruta de instalación en `scripts\install_mod.bat`
2. Ajustar variable `TW3_PATH` si es necesario

### Error: "Server executable not found"
**Solución**: 
1. Verificar que `Witcher3-MP.exe` esté en `build\Release\`
2. Recompilar el proyecto

### Error: "Mod not loading in game"
**Solución**:
1. Verificar que `modInfo.ws` esté en la carpeta correcta
2. Verificar que The Witcher 3 esté en modo Next-Gen
3. Verificar logs del juego (F1 o F2)

## Desinstalación

### Eliminar el Mod
```cmd
# Eliminar carpeta del mod
rmdir /s "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp"

# Eliminar scripts
del "E:\SteamLibrary\steamapps\common\The Witcher 3\start_server.bat"
del "E:\SteamLibrary\steamapps\common\The Witcher 3\launch_game.bat"
del "E:\SteamLibrary\steamapps\common\The Witcher 3\start_nextgen.bat"
```

### Limpiar Proyecto
```cmd
# Limpiar compilación
run_mod.bat clean

# O manualmente
rmdir /s build
```

## Soporte

Si encuentras problemas:
1. Revisar logs en `logs\` (si existen)
2. Verificar configuración en `mp_config.json`
3. Consultar `docs\development.md` para detalles técnicos
4. Reportar issues en el repositorio

---

**Nota**: Esta guía asume una instalación estándar de The Witcher 3 en Steam. Ajusta las rutas según tu instalación.
