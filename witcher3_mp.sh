#!/bin/bash

# Witcher3-MP - Script Unificado para Linux
# Compatible con Ubuntu, Debian, Arch Linux, etc.

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${CYAN}========================================"
    echo -e "   $1"
    echo -e "========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

print_error() {
    echo -e "${RED}❌ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

print_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Function to detect Linux distribution
detect_distro() {
    if [ -f /etc/os-release ]; then
        . /etc/os-release
        echo $ID
    elif [ -f /etc/redhat-release ]; then
        echo "rhel"
    elif [ -f /etc/debian_version ]; then
        echo "debian"
    else
        echo "unknown"
    fi
}

# Function to install dependencies
install_dependencies() {
    local distro=$(detect_distro)
    
    print_status "Instalando dependencias para $distro"
    
    case $distro in
        ubuntu|debian)
            sudo apt update
            sudo apt install -y build-essential cmake git pkg-config libssl-dev libz-dev liblz4-dev
            ;;
        arch|manjaro)
            sudo pacman -S --needed base-devel cmake git pkg-config openssl zlib lz4
            ;;
        fedora|rhel|centos)
            sudo dnf install -y gcc gcc-c++ cmake git pkg-config openssl-devel zlib-devel lz4-devel
            ;;
        *)
            print_warning "Distribución no reconocida. Instalando dependencias básicas..."
            # Try to install common build tools
            if command_exists apt; then
                sudo apt update && sudo apt install -y build-essential cmake git pkg-config libssl-dev libz-dev liblz4-dev
            elif command_exists pacman; then
                sudo pacman -S --needed base-devel cmake git pkg-config openssl zlib lz4
            elif command_exists dnf; then
                sudo dnf install -y gcc gcc-c++ cmake git pkg-config openssl-devel zlib-devel lz4-devel
            else
                print_error "No se pudo instalar las dependencias automáticamente"
                print_info "Por favor instala manualmente: gcc, g++, cmake, git, pkg-config, libssl-dev, zlib-dev, lz4-dev"
                exit 1
            fi
            ;;
    esac
    
    print_success "Dependencias instaladas correctamente"
}

# Function to detect Witcher 3 installation
detect_witcher3() {
    local possible_paths=(
        "$HOME/.steam/steam/steamapps/common/The Witcher 3"
        "$HOME/.local/share/Steam/steamapps/common/The Witcher 3"
        "/opt/steam/steamapps/common/The Witcher 3"
        "/usr/share/steam/steamapps/common/The Witcher 3"
    )
    
    for path in "${possible_paths[@]}"; do
        if [ -d "$path" ] && [ -f "$path/bin/x64/witcher3" ]; then
            echo "$path"
            return 0
        fi
    done
    
    return 1
}

# Function to build the project
build_project() {
    print_status "Compilando Witcher3-MP..."
    
    # Check if build directory exists
    if [ ! -d "build" ]; then
        mkdir -p build/Release
    fi
    
    cd build
    
    # Configure with CMake
    print_info "Configurando proyecto con CMake..."
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build the project
    print_info "Compilando proyecto..."
    make -j$(nproc)
    
    cd ..
    
    print_success "Compilación exitosa!"
    print_info "Ejecutable creado: build/Release/Witcher3-MP"
}

# Function to install the mod
install_mod() {
    print_status "Instalando Mod en The Witcher 3..."
    
    # Detect Witcher 3 installation
    local tw3_path=$(detect_witcher3)
    if [ -z "$tw3_path" ]; then
        print_error "The Witcher 3 no encontrado!"
        print_info "Por favor asegúrate de que The Witcher 3 esté instalado via Steam"
        exit 1
    fi
    
    print_success "The Witcher 3 encontrado en: $tw3_path"
    
    # Check if executable exists
    if [ ! -f "build/Release/Witcher3-MP" ]; then
        print_error "Ejecutable no encontrado!"
        print_info "Por favor ejecuta: ./witcher3_mp.sh build"
        exit 1
    fi
    
    # Create mod directories
    print_info "Creando directorios del mod..."
    mkdir -p "$tw3_path/mods/witcher3_mp"
    mkdir -p "$tw3_path/mods/witcher3_mp/config"
    mkdir -p "$tw3_path/mods/witcher3_mp/scripts/game/witcher3_mp"
    mkdir -p "$tw3_path/bin/config/r4game/user_config_matrix/pc"
    
    # Copy mod files
    print_info "Copiando archivos del mod..."
    cp mod/config/mp_config.json "$tw3_path/mods/witcher3_mp/config/"
    cp mod/scripts/game/witcher3_mp/*.ws "$tw3_path/mods/witcher3_mp/scripts/game/witcher3_mp/"
    cp mod/modInfo.ws "$tw3_path/mods/witcher3_mp/"
    cp mod/mod.settings "$tw3_path/mods/witcher3_mp/"
    cp build/Release/Witcher3-MP "$tw3_path/mods/witcher3_mp/"
    
    # Copy XML configuration files
    print_info "Copiando archivos de configuración XML..."
    cp mod/config/witcher3_mp_config.xml "$tw3_path/bin/config/r4game/user_config_matrix/pc/"
    cp mod/config/filelist.txt "$tw3_path/bin/config/r4game/user_config_matrix/pc/"
    
    # Update game filelists
    print_info "Actualizando filelists del juego..."
    local dx11_filelist="$tw3_path/bin/config/r4game/user_config_matrix/pc/dx11filelist.txt"
    local dx12_filelist="$tw3_path/bin/config/r4game/user_config_matrix/pc/dx12filelist.txt"
    
    # Add to dx11filelist.txt
    if [ -f "$dx11_filelist" ]; then
        if ! grep -q "witcher3_mp_config.xml" "$dx11_filelist"; then
            echo "witcher3_mp_config.xml;" >> "$dx11_filelist"
            print_success "Agregado a dx11filelist.txt"
        fi
    else
        echo "witcher3_mp_config.xml;" > "$dx11_filelist"
        print_success "Creado dx11filelist.txt"
    fi
    
    # Add to dx12filelist.txt
    if [ -f "$dx12_filelist" ]; then
        if ! grep -q "witcher3_mp_config.xml" "$dx12_filelist"; then
            echo "witcher3_mp_config.xml;" >> "$dx12_filelist"
            print_success "Agregado a dx12filelist.txt"
        fi
    else
        echo "witcher3_mp_config.xml;" > "$dx12_filelist"
        print_success "Creado dx12filelist.txt"
    fi
    
    print_success "Instalación exitosa!"
    print_info "Mod instalado en: $tw3_path/mods/witcher3_mp"
    print_info "Configuración XML en: $tw3_path/bin/config/r4game/user_config_matrix/pc/"
}

# Function to start the server
start_server() {
    print_status "Ejecutando Witcher3-MP..."
    
    # Check if executable exists
    if [ ! -f "build/Release/Witcher3-MP" ]; then
        print_error "Ejecutable no encontrado!"
        print_info "Por favor ejecuta: ./witcher3_mp.sh build"
        exit 1
    fi
    
    print_info "🚀 Iniciando servidor Witcher3-MP..."
    print_info "El servidor se ejecutará en esta ventana. Presiona Ctrl+C para detenerlo."
    print_info ""
    
    # Change to build directory and run server
    cd build/Release
    ./Witcher3-MP
}

# Function to uninstall the mod
uninstall_mod() {
    print_status "Desinstalando Mod de The Witcher 3..."
    
    # Detect Witcher 3 installation
    local tw3_path=$(detect_witcher3)
    if [ -z "$tw3_path" ]; then
        print_error "The Witcher 3 no encontrado!"
        exit 1
    fi
    
    print_success "The Witcher 3 encontrado en: $tw3_path"
    
    # Remove mod files
    print_info "Eliminando archivos del mod..."
    rm -rf "$tw3_path/mods/witcher3_mp"
    
    # Remove XML configuration files
    print_info "Eliminando archivos de configuración XML..."
    rm -f "$tw3_path/bin/config/r4game/user_config_matrix/pc/witcher3_mp_config.xml"
    rm -f "$tw3_path/bin/config/r4game/user_config_matrix/pc/filelist.txt"
    
    # Clean game filelists
    print_info "Limpiando filelists del juego..."
    local dx11_filelist="$tw3_path/bin/config/r4game/user_config_matrix/pc/dx11filelist.txt"
    local dx12_filelist="$tw3_path/bin/config/r4game/user_config_matrix/pc/dx12filelist.txt"
    
    if [ -f "$dx11_filelist" ]; then
        sed -i '/witcher3_mp_config.xml/d' "$dx11_filelist"
        print_success "Limpiado dx11filelist.txt"
    fi
    
    if [ -f "$dx12_filelist" ]; then
        sed -i '/witcher3_mp_config.xml/d' "$dx12_filelist"
        print_success "Limpiado dx12filelist.txt"
    fi
    
    print_success "Desinstalación exitosa!"
    print_info "Mod completamente eliminado de The Witcher 3"
}

# Function to show help
show_help() {
    echo "Uso: ./witcher3_mp.sh [comando]"
    echo ""
    echo "Comandos disponibles:"
    echo ""
    echo "  deps        - Instalar dependencias del sistema"
    echo "  build       - Compilar el proyecto"
    echo "  install     - Instalar el mod en The Witcher 3"
    echo "  start       - Ejecutar el servidor"
    echo "  uninstall   - Desinstalar el mod"
    echo "  help        - Mostrar esta ayuda"
    echo ""
    echo "Ejemplos:"
    echo "  ./witcher3_mp.sh deps     # Instalar dependencias"
    echo "  ./witcher3_mp.sh build    # Compilar proyecto"
    echo "  ./witcher3_mp.sh install  # Instalar mod"
    echo "  ./witcher3_mp.sh start    # Ejecutar servidor"
}

# Main script logic
case "${1:-help}" in
    deps)
        install_dependencies
        ;;
    build)
        build_project
        ;;
    install)
        install_mod
        ;;
    start)
        start_server
        ;;
    uninstall)
        uninstall_mod
        ;;
    help|--help|-h)
        show_help
        ;;
    *)
        print_error "Comando no reconocido: $1"
        echo ""
        show_help
        exit 1
        ;;
esac
