# Configuración XML para The Witcher 3 - Post Patch

## Problema Resuelto

Después del parche reciente de The Witcher 3, los mods necesitan archivos XML de configuración específicos para aparecer en el menú de mods del juego. Este documento explica cómo se ha implementado esta solución en el mod Witcher3-MP.

## Archivos Creados

### 1. `witcher3_mp_config.xml`
Archivo de configuración principal que define:
- Configuración del servidor (IP, puerto, conexiones máximas)
- Configuración del cliente (auto-conexión, IP del servidor)
- Características del juego (misiones cooperativas, inventario compartido)
- Optimizaciones de red (latencia baja, compresión de datos)
- Configuración de debug

### 2. `filelist.txt`
Archivo que lista los archivos XML del mod para que el juego los reconozca.

## Ubicación de Archivos

Los archivos XML deben estar ubicados en:
```
[The Witcher 3]\bin\config\r4game\user_config_matrix\pc\
```

## Archivos filelist.txt del Juego

El mod actualiza automáticamente los siguientes archivos:
- `dx11filelist.txt` (para DirectX 11)
- `dx12filelist.txt` (para DirectX 12)

## Instalación

1. Ejecuta `install_mod.bat` para instalar el mod con soporte XML
2. El script automáticamente:
   - Copia los archivos XML a la ubicación correcta
   - Actualiza los filelist.txt del juego
   - Instala todos los archivos del mod

## Desinstalación

1. Ejecuta `uninstall_mod.bat` para desinstalar completamente el mod
2. El script automáticamente:
   - Elimina los archivos del mod
   - Elimina los archivos XML
   - Limpia las entradas de los filelist.txt

## Verificación

Después de la instalación, deberías ver:
1. El mod "Witcher3-MP Multiplayer Server" en el menú de mods del juego
2. Un menú de configuración con pestañas para diferentes categorías
3. Opciones para configurar servidor, cliente, características y optimizaciones

## Solución de Problemas

### El mod no aparece en el menú
1. Verifica que los archivos XML estén en la ubicación correcta
2. Verifica que el archivo esté listado en dx11filelist.txt o dx12filelist.txt
3. Reinicia el juego

### Error de configuración
1. Verifica que el archivo XML esté bien formateado
2. Revisa los logs del juego para errores específicos
3. Reinstala el mod usando `uninstall_mod.bat` y luego `install_mod.bat`

## Estructura del XML

El archivo XML sigue el formato estándar de The Witcher 3:
- `<config>`: Elemento raíz
- `<settings>`: Configuraciones del mod
- `<category>`: Categorías de configuración
- `<setting>`: Configuraciones individuales
- `<ui>`: Definición de la interfaz de usuario
- `<dependencies>`: Dependencias del mod
- `<conflicts>`: Mods conflictivos

## Compatibilidad

- The Witcher 3: Wild Hunt Next-Gen v4.0+
- DirectX 11 y DirectX 12
- Windows 10/11 (64-bit)
