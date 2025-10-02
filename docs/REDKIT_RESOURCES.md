# Recursos REDkit y Herramientas de Modding

## 🎯 **RECURSOS OFICIALES REDKIT**

### **Documentación Oficial:**
- **CD PROJEKT RED - REDkit Wiki**: https://cdprojektred.atlassian.net/wiki/spaces/W3REDkit/overview
  - ✅ **Guía completa de usuario** con secciones específicas:
    - General information, Starting the Editor, Working with the Editor
    - How-to tutorials, Modding Guide, Tools
    - **WitcherScript** (¡clave para comunicación C++ ↔ .ws!)
    - Video tutorials, Changelog, Community
  - ✅ **WitcherScript** es la clave para comunicación con C++

- **The Witcher 3 REDkit Portal**: https://www.thewitcher.com/ar/en/redkit/modding
  - ✅ **Herramientas de desarrollador** - acceso a las mismas herramientas que CD PROJEKT RED
  - ✅ **Tutoriales oficiales** en YouTube del canal oficial
  - ✅ **Comunidad activa** en CDPR forums, Steam Community Hub, Discord
  - ✅ **FAQ detallado** sobre capacidades y requisitos del sistema

- **Tutoriales Oficiales**: https://forums.cdprojektred.com
  - ✅ **Serie completa de tutoriales** oficiales
  - ✅ **Comunidad activa** de modders
  - ✅ **Soporte técnico** directo de desarrolladores

### **Soporte Técnico:**
- **Technical Support CD PROJEKT RED**: https://support.cdprojektred.com/en/witcher-3/pc/the-witcher-3-redkit
  - ✅ **Soporte específico para REDkit** - sección dedicada
  - ✅ **Contacto directo** con desarrolladores
  - ✅ **Recomendado** para solicitar headers de REDkit y APIs avanzadas

### **Recursos Adicionales:**
- **REDkit Bug Tracker**: https://github.com/CDPR-Modding-Documentation/REDkit-bugtracker
  - ✅ **Tracker de bugs** oficial de la comunidad
  - ✅ **Issues conocidos** y soluciones
  - ✅ **Contribuciones** de la comunidad
  - ✅ **Wiki comunitaria** con tutoriales adicionales

- **Tutoriales de Audio**: https://www.audiokinetic.com/en/blog/the-witcher-3-redkit-tutorial/
  - ✅ **Integración con Wwise** para audio
  - ✅ **Aspectos técnicos** del entorno de desarrollo
  - ✅ **Configuración avanzada** de audio

---

## 🔧 **HERRAMIENTAS COMUNITARIAS**

### **Extracción de Bundles:**
- **Witcher 3 BUNDLE Extract**: https://github.com/1vanK/Witcher-3-BUNDLE-Extract
  - ✅ **Herramienta específica** para extraer archivos .bundle
  - ✅ **Mantenida activamente** por la comunidad
  - ✅ **Documentación clara** de uso
  - ✅ **Compatible** con versiones recientes del juego

- **QuickBMS Scripts**: https://www.zenhax.com/viewtopic.php?t=1522
  - ✅ **Scripts específicos** para Witcher 3
  - ✅ **Extracción avanzada** de contenido
  - ✅ **Soporte comunitario** activo
  - ⚠️ Algunos archivos pueden no extraerse completamente

### **Tutoriales de Modding:**
- **NexusMods - The Witcher 3 Modding**: https://www.nexusmods.com/witcher3/mods/14
  - ✅ **Tutorial completo** de modding para TW3
  - ✅ **Guías paso a paso** para modificación de contenidos
  - ✅ **Comunidad activa** de modders
  - ✅ **Recursos compartidos** por la comunidad

- **Conversor de Modelos**: https://jlouisb.users.sourceforge.net/tuto_get_the_witcher_3_models.html
  - ✅ **Tutorial específico** para extraer modelos .w2mesh
  - ✅ **Explica relación** .w2mesh + .buffer
  - ✅ **Herramientas prácticas** para conversión
  - ✅ **Confirmación** de estructura de bundles

- **Creación de Bundles**: https://forums.nexusmods.com/topic/4877680-how-do-i-create-a-blobbundle/
  - ✅ **Proceso de empaquetado** de mods
  - ✅ **Herramientas necesarias** para crear .bundle
  - ✅ **Comunidad activa** discutiendo técnicas avanzadas
  - ✅ **Soluciones** a problemas comunes

---

## 📁 **ESTRUCTURA DE ARCHIVOS TW3**

### **Tipos de Archivos Identificados:**
- **`.bundle`** - Archivos comprimidos principales
- **`.w2mesh`** - Modelos 3D
- **`.buffer`** - Datos binarios de modelos
- **`.ws`** - Scripts de REDkit
- **`.xml`** - Datos de configuración (estadísticas, loot, etc.)
- **`.w2ent`** - Entidades del juego
- **`.w2rig`** - Rigging de personajes
- **`.w2animev`** - Animaciones
- **`.w2mi`** - Items del juego
- **`.w2p`** - Partículas
- **`.w2scene`** - Escenas
- **`.w2beh`** - Comportamientos
- **`.w2behtree`** - Árboles de comportamiento
- **`.w2quest`** - Misiones
- **`.w2phase`** - Fases de misiones
- **`.w2job`** - Trabajos/actividades
- **`.w2l`** - Localización
- **`.w2w`** - Mundo
- **`.w2x`** - Extras
- **`.w2fx`** - Efectos
- **`.w2mg`** - Materiales
- **`.w2em`** - Emisores
- **`.w2et`** - Entidades temporales
- **`.w2pe`** - Partículas de efectos
- **`.w2sm`** - Sonidos

---

## 🚨 **INFORMACIÓN CRÍTICA FALTANTE**

### **Bindings C++:**
- ❌ Headers de REDkit para C++
- ❌ APIs nativas para comunicación C++ ↔ .ws
- ❌ Ejemplos de wrappers o bridges
- ❌ Documentación de sincronización

### **Estructura Interna:**
- ❌ Formato de cabecera de .bundle
- ❌ Índices y offsets de contenido
- ❌ Tipos de compresión interna
- ❌ Tablas de contenido y hashes
- ❌ Timestamps y metadatos

### **APIs Next-Gen:**
- ❌ Nuevas funciones específicas de Next-Gen
- ❌ Diferencias con versión estándar
- ❌ Datos adicionales disponibles
- ❌ Cambios en rutas de assets

---

## 🎯 **PLAN DE INVESTIGACIÓN**

### **Paso 1: Explorar REDkit Instalado**
1. Instalar REDkit desde Steam/portal oficial
2. Buscar carpetas `include/`, `lib/`, `bin/`
3. Identificar headers `.h` o archivos `.dll/.so`
4. Documentar estructura de instalación

### **Paso 2: Analizar Herramientas Comunitarias**
1. Probar Witcher 3 BUNDLE Unpacker con Next-Gen
2. Usar QuickBMS scripts para extraer bundles
3. Documentar estructura de archivos extraídos
4. Identificar rutas de assets específicas

### **Paso 3: Contactar Soporte Oficial**
1. Enviar consulta a CD PROJEKT RED Technical Support
2. Preguntar por SDKs internos o documentación avanzada
3. Solicitar headers de REDkit para modders avanzados
4. Consultar sobre APIs específicas de TW3 Next-Gen

### **Paso 4: Investigación Avanzada**
1. Usar herramientas de ingeniería inversa (con cuidado del EULA)
2. Explorar repositorios de modders avanzados
3. Buscar en foros especializados (NexusMods, Reddit, Discord)
4. Documentar descubrimientos internamente

---

## 📞 **CONTACTOS RECOMENDADOS**

### **Oficiales:**
- **CD PROJEKT RED Technical Support**: https://support.cdprojektred.com
- **REDkit Forums**: https://forums.cdprojektred.com
- **The Witcher 3 REDkit Portal**: https://thewitcher.com

### **Comunitarios:**
- **NexusMods Forums**: https://nexusmods.com
- **Reddit r/witchermods**: https://reddit.com/r/witchermods
- **GitHub Repositories**: Buscar "Witcher 3 modding" o "REDkit"
- **Discord CD Projekt RED**: Canales de modding oficial

---

## ⚠️ **CONSIDERACIONES LEGALES**

- **Respetar EULA** de The Witcher 3 y REDkit
- **No distribuir** contenido propietario extraído
- **Usar solo para** desarrollo de mods legítimos
- **Contactar soporte oficial** antes de ingeniería inversa
- **Documentar fuentes** de toda información obtenida

---

**¡Con estos recursos tienes todo lo necesario para completar la integración real!** 🚀
