# Guía de Instalación - Witcher3-MP

> **Mod Multiplayer para The Witcher 3: Wild Hunt Next-Gen**  
> Compatible con Windows y Linux

## 📋 Requisitos del Sistema

### Windows
- **The Witcher 3: Wild Hunt** (versión Next-Gen 4.0+)
- **Visual Studio 2022** (Community, Professional o Enterprise)
- **Windows 10/11** (64-bit)
- **Steam, GOG o Epic Games** (para The Witcher 3)

### Linux
- **The Witcher 3: Wild Hunt** (versión Next-Gen 4.0+)
- **Ubuntu 20.04+**, **Debian 11+**, **Arch Linux**, **Fedora 35+**, etc.
- **GCC 9.0+** o **Clang 10.0+**
- **CMake 3.16+**
- **Steam** funcionando correctamente

### Hardware Mínimo
- **CPU**: Intel Core i5-7400 / AMD Ryzen 5 1600
- **RAM**: 8 GB
- **GPU**: NVIDIA GTX 1060 6GB / AMD RX 580 8GB
- **Espacio**: 2 GB libres

## 🚀 Instalación Rápida

### Windows

#### Opción 1: Script Unificado (Recomendado)
```bash
# 1. Compilar el proyecto
witcher3_mp.bat build

# 2. Instalar el mod
witcher3_mp.bat install

# 3. Ejecutar el servidor
witcher3_mp.bat start
```

#### Opción 2: Instalación Simple
```bash
# Instalación automática con detección de Steam
install_simple.bat
```

### Linux

#### Opción 1: Instalación Automática (Recomendado)
```bash
# 1. Hacer ejecutable el instalador
chmod +x install_linux.sh

# 2. Ejecutar instalación automática
./install_linux.sh
```

#### Opción 2: Instalación Manual
```bash
# 1. Instalar dependencias
./witcher3_mp.sh deps

# 2. Compilar e instalar
./witcher3_mp.sh build
./witcher3_mp.sh install

# 3. Ejecutar servidor
./witcher3_mp.sh start
```

#### Opción 3: Usando Makefile
```bash
# 1. Instalar dependencias
make deps

# 2. Compilar e instalar
make install

# 3. Ejecutar servidor
make start
```

## 🔧 Instalación Manual Detallada

### Windows

#### Paso 1: Preparar el Entorno
1. Abrir **Visual Studio Developer Command Prompt**
2. Navegar al directorio del proyecto:
   ```cmd
   cd "C:\Users\USUARIO\OneDrive\Escritorio\Mod-TW3"
   ```

#### Paso 2: Compilar el Servidor
```cmd
# Opción 1: Script unificado
witcher3_mp.bat build

# Opción 2: Script simple
install_simple.bat

# Opción 3: Manual
cd scripts
build_ultra_simple.bat
```

#### Paso 3: Instalar en The Witcher 3
```cmd
# Opción 1: Script unificado
witcher3_mp.bat install

# Opción 2: Script simple
install_simple.bat

# Opción 3: Manual
cd scripts
install_mod.bat
```

### Linux

#### Paso 1: Instalar Dependencias
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential cmake git pkg-config libssl-dev libz-dev liblz4-dev

# Arch Linux/Manjaro
sudo pacman -S --needed base-devel cmake git pkg-config openssl zlib lz4

# Fedora/RHEL/CentOS
sudo dnf install gcc gcc-c++ cmake git pkg-config openssl-devel zlib-devel lz4-devel

# openSUSE
sudo zypper install gcc gcc-c++ cmake git pkg-config libopenssl-devel zlib-devel liblz4-devel
```

#### Paso 2: Compilar el Proyecto
```bash
# Crear directorio de compilación
mkdir -p build/Release
cd build

# Configurar con CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Compilar
make -j$(nproc)

# Volver al directorio raíz
cd ..
```

#### Paso 3: Instalar el Mod
```bash
# Detectar ubicación de The Witcher 3
TW3_PATH="$HOME/.steam/steam/steamapps/common/The Witcher 3"

# Crear directorios del mod
mkdir -p "$TW3_PATH/mods/witcher3_mp"
mkdir -p "$TW3_PATH/mods/witcher3_mp/config"
mkdir -p "$TW3_PATH/mods/witcher3_mp/scripts/game/witcher3_mp"
mkdir -p "$TW3_PATH/bin/config/r4game/user_config_matrix/pc"

# Copiar archivos del mod
cp mod/config/mp_config.json "$TW3_PATH/mods/witcher3_mp/config/"
cp mod/scripts/game/witcher3_mp/*.ws "$TW3_PATH/mods/witcher3_mp/scripts/game/witcher3_mp/"
cp mod/modInfo.ws "$TW3_PATH/mods/witcher3_mp/"
cp mod/mod.settings "$TW3_PATH/mods/witcher3_mp/"
cp build/Release/Witcher3-MP "$TW3_PATH/mods/witcher3_mp/"

# Copiar configuración XML
cp mod/config/witcher3_mp_config.xml "$TW3_PATH/bin/config/r4game/user_config_matrix/pc/"
cp mod/config/filelist.txt "$TW3_PATH/bin/config/r4game/user_config_matrix/pc/"

# Actualizar filelists del juego
echo "witcher3_mp_config.xml;" >> "$TW3_PATH/bin/config/r4game/user_config_matrix/pc/dx11filelist.txt"
echo "witcher3_mp_config.xml;" >> "$TW3_PATH/bin/config/r4game/user_config_matrix/pc/dx12filelist.txt"
```

## 🔍 Verificación de la Instalación

### 1. Verificar Archivos del Mod

#### Windows
```
E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp\
├── Witcher3-MP.exe          # Servidor
├── modInfo.ws               # Información del mod
├── mod.settings             # Configuración adicional
├── config\
│   └── mp_config.json       # Configuración del servidor
└── scripts\game\witcher3_mp\
    ├── MP_ModInitializer.ws
    ├── MP_GameHooks.ws
    ├── MP_ServerManager.ws
    ├── MP_REDkitIntegration.ws
    └── MP_WeaponsDatabase.ws
```

#### Linux
```
~/.steam/steam/steamapps/common/The Witcher 3/mods/witcher3_mp/
├── Witcher3-MP              # Servidor
├── modInfo.ws               # Información del mod
├── mod.settings             # Configuración adicional
├── config/
│   └── mp_config.json       # Configuración del servidor
└── scripts/game/witcher3_mp/
    ├── MP_ModInitializer.ws
    ├── MP_GameHooks.ws
    ├── MP_ServerManager.ws
    ├── MP_REDkitIntegration.ws
    └── MP_WeaponsDatabase.ws
```

### 2. Verificar Configuración XML
```
[TW3_PATH]/bin/config/r4game/user_config_matrix/pc/
├── witcher3_mp_config.xml   # Configuración del menú del juego
├── dx11filelist.txt         # Lista de archivos DX11
└── dx12filelist.txt         # Lista de archivos DX12
```

### 3. Probar la Instalación

#### Windows
```bash
# 1. Ejecutar servidor
witcher3_mp.bat start

# 2. Verificar que aparezca: "Server started on port 60002"
# 3. Abrir The Witcher 3
# 4. Verificar que aparezca el menú "Witcher3-MP Settings"
```

#### Linux
```bash
# 1. Ejecutar servidor
./witcher3_mp.sh start

# 2. Verificar que aparezca: "Server started on port 60002"
# 3. Abrir The Witcher 3
# 4. Verificar que aparezca el menú "Witcher3-MP Settings"
```

## 🛠️ Solución de Problemas

### Error: "Visual Studio compiler not found" (Windows)
**Solución**: 
- Ejecutar desde Visual Studio Developer Command Prompt
- O instalar Visual Studio Build Tools

### Error: "The Witcher 3 installation not found"
**Solución**: 
- Verificar que The Witcher 3 esté instalado via Steam
- El script detecta automáticamente ubicaciones comunes
- Verificar rutas en `install_simple.bat` o `witcher3_mp.sh`

### Error: "Server executable not found"
**Solución**: 
- Verificar que `Witcher3-MP.exe` esté en `build/Release/`
- Recompilar el proyecto: `witcher3_mp.bat build` o `./witcher3_mp.sh build`

### Error: "Mod not loading in game"
**Solución**:
1. Verificar que `modInfo.ws` esté en la carpeta correcta
2. Verificar que The Witcher 3 esté en modo Next-Gen
3. Verificar logs del juego (F1 o F2)
4. Verificar que el mod aparezca en la lista de mods del juego

### Error: "Dependencies not found" (Linux)
**Solución**:
```bash
# Reinstalar dependencias
./witcher3_mp.sh deps

# O manualmente según tu distribución
sudo apt install build-essential cmake git pkg-config libssl-dev libz-dev liblz4-dev
```

### Error: "Permission denied" (Linux)
**Solución**:
```bash
# Hacer ejecutable los scripts
chmod +x witcher3_mp.sh
chmod +x install_linux.sh

# O usar sudo si es necesario
sudo ./witcher3_mp.sh deps
```

## 🗑️ Desinstalación

### Windows
```cmd
# Opción 1: Script unificado
witcher3_mp.bat uninstall

# Opción 2: Script simple
uninstall_mod.bat

# Opción 3: Manual
rmdir /s "E:\SteamLibrary\steamapps\common\The Witcher 3\mods\witcher3_mp"
del "E:\SteamLibrary\steamapps\common\The Witcher 3\bin\config\r4game\user_config_matrix\pc\witcher3_mp_config.xml"
```

### Linux
```bash
# Opción 1: Script unificado
./witcher3_mp.sh uninstall

# Opción 2: Manual
rm -rf ~/.steam/steam/steamapps/common/The\ Witcher\ 3/mods/witcher3_mp
rm -f ~/.steam/steam/steamapps/common/The\ Witcher\ 3/bin/config/r4game/user_config_matrix/pc/witcher3_mp_config.xml
```

### Limpiar Proyecto
```bash
# Windows
witcher3_mp.bat clean

# Linux
make clean

# O manualmente
rm -rf build/
```

## 📊 Comandos Disponibles

### Windows
```bash
witcher3_mp.bat build      # Compilar proyecto
witcher3_mp.bat install    # Instalar mod
witcher3_mp.bat start      # Ejecutar servidor
witcher3_mp.bat uninstall  # Desinstalar mod
witcher3_mp.bat clean      # Limpiar archivos
witcher3_mp.bat help       # Mostrar ayuda
```

### Linux
```bash
./witcher3_mp.sh deps      # Instalar dependencias
./witcher3_mp.sh build     # Compilar proyecto
./witcher3_mp.sh install   # Instalar mod
./witcher3_mp.sh start     # Ejecutar servidor
./witcher3_mp.sh uninstall # Desinstalar mod
./witcher3_mp.sh help      # Mostrar ayuda

# O usando make
make deps
make build
make install
make start
make uninstall
make clean
```

## 🔧 Configuración Avanzada

### Cambiar Puerto del Servidor
Editar `mod/config/mp_config.json`:
```json
{
    "server": {
        "port": 60002
    }
}
```

### Habilitar Logs de Debug
Editar `mod/config/witcher3_mp_config.xml`:
```xml
<setting name="Enable Debug Logging" type="bool" default="true"/>
<setting name="Log Level" type="enum" default="DEBUG"/>
<setting name="Enable Network Debug" type="bool" default="true"/>
```

### Configurar IP del Servidor
Editar `mod/config/witcher3_mp_config.xml`:
```xml
<setting name="Server IP" type="string" default="127.0.0.1"/>
<setting name="Client Server IP" type="string" default="127.0.0.1"/>
```

## 📚 Documentación Adicional

- **[README.md](../README.md)** - Información general del proyecto
- **[README_LINUX.md](../README_LINUX.md)** - Documentación específica para Linux
- **[docs/development.md](development.md)** - Detalles técnicos de desarrollo
- **[docs/xml_configuration.md](xml_configuration.md)** - Configuración XML del mod

## 🆘 Soporte

Si encuentras problemas:
1. Revisar logs en `logs/` (si existen)
2. Verificar configuración en `mp_config.json`
3. Consultar `docs/development.md` para detalles técnicos
4. Reportar issues en el repositorio
5. Unirse al servidor de Discord para soporte comunitario

---

**Nota**: Esta guía asume una instalación estándar de The Witcher 3 en Steam. Ajusta las rutas según tu instalación específica.