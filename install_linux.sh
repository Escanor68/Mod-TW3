#!/bin/bash

# Witcher3-MP - Instalador Automático para Linux
# Este script instala automáticamente el mod en sistemas Linux

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
CYAN='\033[0;36m'
NC='\033[0m'

print_banner() {
    echo -e "${CYAN}"
    echo "  _    _ _ _                _   _ 3 - MP"
    echo " | |  | | | |              | | | |"
    echo " | |  | | | |__  _   _ _ __| |_| |__  _ __ ___  _ __"
    echo " | |/\| | | '_ \| | | | '__| __| '_ \| '_ \` _ \| '_ \"
    echo " \  /\  / | | | | |_| | |  | |_| | | | | | | | | |_) |"
    echo "  \/  \/|_|_| |_|\__,_|_|   \__|_| |_|_| |_| |_| .__/"
    echo "                                               | |"
    echo "                                               |_|"
    echo -e "${NC}"
    echo -e "${BLUE}Instalador Automático para Linux${NC}"
    echo -e "${BLUE}=================================${NC}"
    echo ""
}

print_status() {
    echo -e "${CYAN}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if running as root
check_root() {
    if [ "$EUID" -eq 0 ]; then
        print_error "No ejecutes este script como root (sudo)"
        print_info "El script pedirá permisos cuando sea necesario"
        exit 1
    fi
}

# Check if running on Linux
check_linux() {
    if [ "$(uname)" != "Linux" ]; then
        print_error "Este script es solo para sistemas Linux"
        exit 1
    fi
}

# Detect distribution
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

# Install dependencies
install_dependencies() {
    local distro=$(detect_distro)
    
    print_status "Detectada distribución: $distro"
    print_status "Instalando dependencias..."
    
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
        opensuse*|sles)
            sudo zypper install -y gcc gcc-c++ cmake git pkg-config libopenssl-devel zlib-devel liblz4-devel
            ;;
        *)
            print_warning "Distribución no reconocida: $distro"
            print_info "Intentando instalar dependencias básicas..."
            if command -v apt >/dev/null 2>&1; then
                sudo apt update && sudo apt install -y build-essential cmake git pkg-config libssl-dev libz-dev liblz4-dev
            elif command -v pacman >/dev/null 2>&1; then
                sudo pacman -S --needed base-devel cmake git pkg-config openssl zlib lz4
            elif command -v dnf >/dev/null 2>&1; then
                sudo dnf install -y gcc gcc-c++ cmake git pkg-config openssl-devel zlib-devel lz4-devel
            elif command -v zypper >/dev/null 2>&1; then
                sudo zypper install -y gcc gcc-c++ cmake git pkg-config libopenssl-devel zlib-devel liblz4-devel
            else
                print_error "No se pudo instalar las dependencias automáticamente"
                print_info "Por favor instala manualmente: gcc, g++, cmake, git, pkg-config, libssl-dev, zlib-dev, lz4-dev"
                exit 1
            fi
            ;;
    esac
    
    print_success "Dependencias instaladas correctamente"
}

# Check if Steam is installed
check_steam() {
    print_status "Verificando instalación de Steam..."
    
    if ! command -v steam >/dev/null 2>&1; then
        print_error "Steam no está instalado o no está en el PATH"
        print_info "Por favor instala Steam desde: https://store.steampowered.com/about/"
        exit 1
    fi
    
    print_success "Steam encontrado"
}

# Check if The Witcher 3 is installed
check_witcher3() {
    print_status "Verificando instalación de The Witcher 3..."
    
    local possible_paths=(
        "$HOME/.steam/steam/steamapps/common/The Witcher 3"
        "$HOME/.local/share/Steam/steamapps/common/The Witcher 3"
        "/opt/steam/steamapps/common/The Witcher 3"
        "/usr/share/steam/steamapps/common/The Witcher 3"
    )
    
    for path in "${possible_paths[@]}"; do
        if [ -d "$path" ] && [ -f "$path/bin/x64/witcher3" ]; then
            print_success "The Witcher 3 encontrado en: $path"
            return 0
        fi
    done
    
    print_error "The Witcher 3 no encontrado!"
    print_info "Por favor instala The Witcher 3: Wild Hunt Next-Gen via Steam"
    print_info "Asegúrate de que esté completamente instalado y actualizado"
    exit 1
}

# Build the project
build_project() {
    print_status "Compilando Witcher3-MP..."
    
    # Make script executable
    chmod +x witcher3_mp.sh
    
    # Build
    ./witcher3_mp.sh build
    
    print_success "Proyecto compilado correctamente"
}

# Install the mod
install_mod() {
    print_status "Instalando mod en The Witcher 3..."
    
    ./witcher3_mp.sh install
    
    print_success "Mod instalado correctamente"
}

# Show completion message
show_completion() {
    echo ""
    print_success "¡Instalación completada exitosamente!"
    echo ""
    echo -e "${BLUE}Próximos pasos:${NC}"
    echo "1. Inicia The Witcher 3"
    echo "2. Verifica que el mod aparezca en la lista de mods"
    echo "3. Ejecuta el servidor con: ${CYAN}./witcher3_mp.sh start${NC}"
    echo "4. Configura la conexión en el juego"
    echo ""
    echo -e "${BLUE}Comandos útiles:${NC}"
    echo "• ${CYAN}./witcher3_mp.sh start${NC}     - Ejecutar servidor"
    echo "• ${CYAN}./witcher3_mp.sh uninstall${NC} - Desinstalar mod"
    echo "• ${CYAN}./witcher3_mp.sh help${NC}      - Ver ayuda"
    echo ""
    echo -e "${GREEN}¡Disfruta del multijugador en The Witcher 3!${NC}"
}

# Main installation process
main() {
    print_banner
    
    # Pre-flight checks
    check_root
    check_linux
    check_steam
    check_witcher3
    
    # Installation steps
    install_dependencies
    build_project
    install_mod
    
    # Completion
    show_completion
}

# Run main function
main "$@"
