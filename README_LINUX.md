# Witcher3-MP para Linux

Mod de multijugador para The Witcher 3: Wild Hunt Next-Gen en distribuciones Linux.

## 🐧 Distribuciones Soportadas

- **Ubuntu** 20.04+ / **Debian** 11+
- **Arch Linux** / **Manjaro**
- **Fedora** 35+ / **RHEL** 8+
- **openSUSE** Leap 15.3+
- Otras distribuciones con soporte para Steam

## 📋 Requisitos

### Sistema
- **The Witcher 3: Wild Hunt Next-Gen** (v4.0+) instalado via Steam
- **Steam** funcionando correctamente
- **GCC** 9.0+ o **Clang** 10.0+
- **CMake** 3.16+
- **Git**

### Dependencias
- `build-essential` (gcc, g++, make)
- `cmake`
- `git`
- `pkg-config`
- `libssl-dev`
- `zlib-dev`
- `lz4-dev`

## 🚀 Instalación Rápida

### Opción 1: Script Automático (Recomendado)

```bash
# 1. Clonar el repositorio
git clone https://github.com/tu-usuario/witcher3-mp.git
cd witcher3-mp

# 2. Hacer ejecutable el script
chmod +x witcher3_mp.sh

# 3. Instalar dependencias
./witcher3_mp.sh deps

# 4. Compilar el proyecto
./witcher3_mp.sh build

# 5. Instalar el mod
./witcher3_mp.sh install

# 6. Ejecutar el servidor
./witcher3_mp.sh start
```

### Opción 2: Makefile

```bash
# 1. Instalar dependencias
make deps

# 2. Compilar e instalar
make install

# 3. Ejecutar servidor
make start
```

## 🔧 Instalación Manual de Dependencias

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake git pkg-config libssl-dev libz-dev liblz4-dev
```

### Arch Linux/Manjaro
```bash
sudo pacman -S --needed base-devel cmake git pkg-config openssl zlib lz4
```

### Fedora/RHEL/CentOS
```bash
sudo dnf install gcc gcc-c++ cmake git pkg-config openssl-devel zlib-devel lz4-devel
```

### openSUSE
```bash
sudo zypper install gcc gcc-c++ cmake git pkg-config libopenssl-devel zlib-devel liblz4-devel
```

## 🎮 Uso

### Comandos Disponibles

```bash
./witcher3_mp.sh deps        # Instalar dependencias
./witcher3_mp.sh build       # Compilar proyecto
./witcher3_mp.sh install     # Instalar mod en TW3
./witcher3_mp.sh start       # Ejecutar servidor
./witcher3_mp.sh uninstall   # Desinstalar mod
./witcher3_mp.sh help        # Mostrar ayuda
```

### Ejecutar el Servidor

```bash
# Ejecutar servidor con logs
./witcher3_mp.sh start

# O usar make
make start
```

El servidor se ejecutará en el puerto **60002** por defecto.

## 🔍 Detección Automática

El script detecta automáticamente:

- **Distribución Linux** (Ubuntu, Arch, Fedora, etc.)
- **Ubicación de The Witcher 3** en Steam
- **Dependencias faltantes**
- **Conflictos de mods**

## 📁 Estructura de Archivos

```
witcher3-mp/
├── witcher3_mp.sh          # Script principal para Linux
├── Makefile                # Makefile para Linux
├── CMakeLists.txt          # Configuración CMake multiplataforma
├── src/                    # Código fuente
├── mod/                    # Archivos del mod
│   ├── scripts/            # Scripts del juego
│   ├── config/             # Configuración
│   └── modInfo.ws          # Información del mod
└── build/                  # Archivos de compilación
    └── Release/
        └── Witcher3-MP     # Ejecutable compilado
```

## 🛠️ Solución de Problemas

### Error: "The Witcher 3 no encontrado"
```bash
# Verificar que TW3 esté instalado via Steam
ls ~/.steam/steam/steamapps/common/The\ Witcher\ 3/

# O en ubicación personalizada
ls ~/.local/share/Steam/steamapps/common/The\ Witcher\ 3/
```

### Error: "Dependencias faltantes"
```bash
# Reinstalar dependencias
./witcher3_mp.sh deps
```

### Error: "Permisos denegados"
```bash
# Hacer ejecutable el script
chmod +x witcher3_mp.sh

# O usar sudo si es necesario
sudo ./witcher3_mp.sh deps
```

### Error: "CMake no encontrado"
```bash
# Instalar CMake
sudo apt install cmake  # Ubuntu/Debian
sudo pacman -S cmake    # Arch
sudo dnf install cmake  # Fedora
```

## 🔧 Configuración Avanzada

### Cambiar Puerto del Servidor
Edita `mod/config/mp_config.json`:
```json
{
    "server": {
        "port": 60002
    }
}
```

### Habilitar Logs de Debug
Edita `mod/config/witcher3_mp_config.xml`:
```xml
<setting name="Enable Debug Logging" type="bool" default="true"/>
<setting name="Log Level" type="enum" default="DEBUG"/>
```

## 📊 Monitoreo

### Ver Logs del Servidor
```bash
# Los logs aparecen en la consola cuando ejecutas:
./witcher3_mp.sh start

# O para ver logs persistentes:
tail -f logs/witcher3_mp.log
```

### Verificar Estado del Mod
```bash
# Verificar archivos instalados
ls ~/.steam/steam/steamapps/common/The\ Witcher\ 3/mods/witcher3_mp/

# Verificar configuración XML
ls ~/.steam/steam/steamapps/common/The\ Witcher\ 3/bin/config/r4game/user_config_matrix/pc/
```

## 🤝 Contribuir

1. Fork el repositorio
2. Crea una rama para tu feature (`git checkout -b feature/nueva-funcionalidad`)
3. Commit tus cambios (`git commit -am 'Agregar nueva funcionalidad'`)
4. Push a la rama (`git push origin feature/nueva-funcionalidad`)
5. Crea un Pull Request

## 📝 Licencia

Este proyecto está bajo la licencia MIT. Ver `LICENSE` para más detalles.

## 🆘 Soporte

- **Issues**: [GitHub Issues](https://github.com/tu-usuario/witcher3-mp/issues)
- **Discord**: [Servidor de Discord](https://discord.gg/tu-servidor)
- **Wiki**: [Documentación completa](https://github.com/tu-usuario/witcher3-mp/wiki)

## 🙏 Agradecimientos

- **CD Projekt Red** por The Witcher 3
- **Comunidad de modders** de The Witcher 3
- **Desarrolladores de Linux** por el soporte multiplataforma
