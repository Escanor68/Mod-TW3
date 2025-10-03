# 🎮 Modos de Juego Multijugador - Witcher3-MP

## 📋 Resumen

El sistema de modos de juego de Witcher3-MP permite a los jugadores elegir entre dos experiencias multijugador distintas:

1. **Historia Cooperativa** - Experiencia narrativa completa con quests principales
2. **Exploración Libre** - Mundo abierto sin restricciones de historia principal

---

## 🎯 Modo Historia Cooperativa

### Características Principales
- ✅ **Quests principales habilitadas** - Toda la campaña de The Witcher 3 disponible
- ✅ **Cutscenes sincronizadas** - Todos los jugadores ven las mismas cinemáticas
- ✅ **Diálogos compartidos** - El host controla las decisiones, otros observan
- ✅ **Progresión sincronizada** - XP y recompensas compartidas entre todos
- ✅ **Sincronización de quests** - Estado de misiones actualizado para todos

### Configuración
```json
{
  "gameMode": {
    "mode": "story",
    "story_mode": {
      "enable_main_quests": true,
      "enable_cutscenes": true,
      "enable_dialogs": true,
      "host_controls_decisions": true,
      "sync_xp_rewards": true,
      "sync_quest_progress": true
    }
  }
}
```

### Flujo de Juego
1. **Host inicia servidor** con modo "story"
2. **Jugadores se conectan** y sincronizan progreso
3. **Host controla decisiones** en diálogos y cutscenes
4. **Todos reciben XP** y recompensas de quests completadas
5. **Progresión guardada** en saves vanilla del juego

---

## 🌍 Modo Exploración Libre

### Características Principales
- ❌ **Quests principales deshabilitadas** - Sin campaña principal
- ❌ **Cutscenes deshabilitadas** - Sin cinemáticas obligatorias
- ❌ **Diálogos deshabilitados** - Sin conversaciones de historia
- ✅ **Contratos de brujo** - Sistema completo de contratos disponible
- ✅ **Cazas de tesoros** - Puntos de interés y tesoros desbloqueados
- ✅ **Eventos dinámicos** - Encuentros aleatorios y actividades
- ✅ **Mapa completo desbloqueado** - Todas las zonas disponibles desde el inicio
- ✅ **Inventario sincronizado** - Items compartidos entre jugadores
- ✅ **Progreso externo** - Guardado en `mp_config.json` sin afectar saves vanilla

### Configuración
```json
{
  "gameMode": {
    "mode": "exploration",
    "exploration_mode": {
      "enable_main_quests": false,
      "enable_cutscenes": false,
      "enable_dialogs": false,
      "enable_witcher_contracts": true,
      "enable_treasure_hunts": true,
      "enable_question_marks": true,
      "enable_dynamic_events": true,
      "unlock_all_areas": true,
      "sync_inventory": true,
      "sync_contracts": true,
      "save_external_progress": true
    }
  }
}
```

### Flujo de Juego
1. **Host inicia servidor** con modo "exploration"
2. **Jugadores se conectan** y reciben inventario base
3. **Mapa completo desbloqueado** - Acceso a todas las zonas
4. **Actividades disponibles**:
   - Contratos de brujo
   - Cazas de tesoros
   - Puntos de interés (?)
   - Eventos dinámicos
   - Combate contra monstruos
5. **Progreso guardado** en sistema externo

---

## ⚙️ Configuración Técnica

### Archivos de Configuración

#### `mod/config/mp_config.json`
```json
{
  "gameMode": {
    "mode": "story",
    "available_modes": ["story", "exploration"],
    "story_mode": {
      "enable_main_quests": true,
      "enable_cutscenes": true,
      "enable_dialogs": true,
      "host_controls_decisions": true,
      "sync_xp_rewards": true,
      "sync_quest_progress": true
    },
    "exploration_mode": {
      "enable_main_quests": false,
      "enable_cutscenes": false,
      "enable_dialogs": false,
      "enable_witcher_contracts": true,
      "enable_treasure_hunts": true,
      "enable_question_marks": true,
      "enable_dynamic_events": true,
      "unlock_all_areas": true,
      "sync_inventory": true,
      "sync_contracts": true,
      "save_external_progress": true
    }
  }
}
```

### Scripts Principales

#### `MP_GameModeManager.ws`
- Gestión principal de modos de juego
- Aplicación de configuraciones específicas
- Control de habilitación/deshabilitación de sistemas

#### `MP_QuestManager.ws`
- Control de quests basado en modo de juego
- Deshabilitación de main quests en modo exploración
- Sincronización de progreso entre jugadores

#### `MP_AreaManager.ws`
- Desbloqueo de áreas del mapa
- Control de acceso basado en modo de juego

#### `MP_ContractManager.ws`
- Gestión de contratos de brujo
- Sincronización de contratos entre jugadores

---

## 🔧 Implementación Técnica

### Sistema de Flags
El sistema utiliza flags booleanos para controlar qué características están habilitadas:

```witcherscript
// Verificar si main quests están habilitadas
if (gameModeManager.ShouldEnableQuest(QuestType.MainQuest))
{
    // Habilitar main quests
}

// Verificar si área debe estar desbloqueada
if (gameModeManager.ShouldUnlockArea("novigrad"))
{
    // Desbloquear área
}
```

### Logging
Todos los cambios de modo se registran en `logs/mp_session.log`:

```
[12:34:56] [INFO] Game mode changed to: Exploration
[12:34:56] [INFO] Main quests disabled
[12:34:56] [INFO] All areas unlocked
[12:34:56] [INFO] Witcher contracts enabled
```

### Integración con Menú
El menú de mods permite cambiar entre modos:
- Selector de modo en pestaña "Configuración"
- Aplicación inmediata de cambios
- Guardado automático de configuración

---

## 🎮 Experiencia de Usuario

### Cambio de Modo
1. **Abrir menú de mods** en el juego
2. **Ir a pestaña "Configuración"**
3. **Seleccionar modo deseado**:
   - "Historia Cooperativa" para modo story
   - "Exploración Libre" para modo exploration
4. **Guardar configuración**
5. **Reiniciar servidor** para aplicar cambios

### Indicadores Visuales
- **Estado del modo** mostrado en menú de mods
- **Logs en consola** indicando modo activo
- **Diferencias en UI** según modo seleccionado

---

## 🧪 Pruebas y Validación

### Pruebas Automáticas
```bash
# Ejecutar pruebas de modos de juego
witcher3_mp.bat test
```

### Validaciones Implementadas
- ✅ Modo story carga main quests
- ✅ Modo exploration omite main quests
- ✅ Contratos funcionan en ambos modos
- ✅ Áreas se desbloquean correctamente
- ✅ Inventario se sincroniza apropiadamente
- ✅ Logging funciona en ambos modos

### Casos de Prueba
1. **Iniciar en modo story** → Verificar que main quests están disponibles
2. **Cambiar a modo exploration** → Verificar que main quests se deshabilitan
3. **Completar contrato** → Verificar sincronización entre jugadores
4. **Desbloquear área** → Verificar acceso inmediato en modo exploration

---

## 🚨 Consideraciones Importantes

### Compatibilidad
- **Saves vanilla** no se ven afectados en modo exploration
- **Progreso externo** se guarda en `mp_config.json`
- **Mods existentes** pueden requerir adaptación

### Limitaciones
- **Cambio de modo** requiere reinicio del servidor
- **Progreso de main quests** no se transfiere entre modos
- **Cutscenes** no se pueden habilitar individualmente

### Recomendaciones
- **Usar modo story** para primera experiencia multijugador
- **Usar modo exploration** para sesiones casuales
- **Hacer backup** de configuración antes de cambios importantes

---

## 📞 Soporte

### Problemas Comunes
1. **Modo no cambia** → Verificar configuración en `mp_config.json`
2. **Áreas bloqueadas** → Verificar que modo exploration está activo
3. **Quests no aparecen** → Verificar que modo story está activo

### Logs de Depuración
Revisar `logs/mp_session.log` para información detallada sobre el estado del sistema.

---

**🎮 ¡Disfruta explorando The Witcher 3 con tus amigos en el modo que prefieras!**
