# Análisis del Mod de Combate - Witcher3-MP

## 🎯 **RESUMEN DEL ANÁLISIS**

Se analizó el mod "Fix sounds of sword hits when adding effects" que proporciona información valiosa sobre el sistema de combate interno de TW3, incluyendo APIs reales para manejo de daño, animaciones y efectos.

---

## 🔍 **INFORMACIÓN CLAVE EXTRAÍDA**

### **1. Sistema de Daño (W3DamageAction)**

#### **APIs Reales Encontradas:**
```witcherscript
// Crear acción de daño
var action : W3DamageAction;
action = new W3DamageAction in theGame.damageMgr;

// Inicializar acción
action.Initialize(attacker, victim, carrier, sourceName, hitReactionType, powerStatType, 
                  ignoreImmortality, ignoreArmor, ignoreResistance, isEnvironment);

// Configurar daño
action.AddDamage(damageType, damageValue);
action.SetHitAnimationPlayType(EAHA_ForceYes); // o EAHA_ForceNo
action.SetCannotReturnDamage(true);
action.SetCanPlayHitParticle(false);
action.SetCriticalHit();

// Procesar acción
theGame.damageMgr.ProcessAction(action);
```

#### **Tipos de Hit Reaction:**
- `EHRT_Light` - Reacción ligera
- `EHRT_Reflect` - Reflejo de ataque
- `EHRT_None` - Sin reacción

#### **Tipos de Animation Play:**
- `EAHA_ForceYes` - Forzar animación
- `EAHA_ForceNo` - No reproducir animación

### **2. Sistema de Efectos (W3EffectManager)**

#### **APIs Reales Encontradas:**
```witcherscript
// Aplicar efectos desde acción de daño
effectManager.AddEffectsFromAction(action);

// Verificar si tiene buff
actor.HasBuff(EET_HeavyKnockdown)
actor.HasBuff(EET_Knockdown)
actor.HasBuff(EET_LongStagger)
actor.HasBuff(EET_Stagger)

// Configurar animación de hit
actor.SetCanPlayHitAnim(true);
```

### **3. Sistema de Combate (CActor)**

#### **APIs Reales Encontradas:**
```witcherscript
// Reaccionar a ser golpeado
actor.ReactToBeingHit(damageAction, buffNotApplied);

// Aplicar efectos de acción
actor.ApplyActionEffects(action);

// Procesar sonido de hit
actor.ProcessHitSound(damageAction, hitAnimPlayed);

// Reproducir efecto de hit
actor.PlayHitEffect(damageAction);

// Reproducir animación de hit
actor.PlayHitAnimation(damageAction, animType);
```

---

## 🚀 **APLICACIÓN A NUESTRO PROYECTO**

### **1. Mejoras para REDkitBridge.cpp**

#### **Función ExecuteAttack() - MEJORADA**
```cpp
bool REDkitBridge::ExecuteAttack(uint32_t playerId, uint32_t attackType, uint32_t weaponType, const Vector4F& targetPosition)
{
    // Crear JSON con parámetros reales de W3DamageAction
    Json::Value attackParams;
    attackParams["attacker"] = playerId;
    attackParams["victim"] = targetId; // Obtener del targetPosition
    attackParams["sourceName"] = "multiplayer_attack";
    attackParams["hitReactionType"] = "EHRT_Light";
    attackParams["powerStatType"] = "CPS_AttackPower";
    attackParams["damageType"] = attackType;
    attackParams["damageValue"] = CalculateDamage(attackType, weaponType);
    attackParams["hitAnimationPlayType"] = "EAHA_ForceYes";
    attackParams["canPlayHitParticle"] = true;
    attackParams["isCriticalHit"] = IsCriticalHit(playerId);
    
    // Enviar a WitcherScript
    std::string jsonParams = Json::writeString(builder, attackParams);
    return ExecuteREDkitFunction("ExecuteAttack", jsonParams);
}
```

#### **Función PlayAttackAnimation() - MEJORADA**
```cpp
bool REDkitBridge::PlayAttackAnimation(uint32_t playerId, uint32_t animationType)
{
    Json::Value animParams;
    animParams["playerId"] = playerId;
    animParams["animationType"] = animationType;
    animParams["hitAnimationPlayType"] = "EAHA_ForceYes";
    animParams["canPlayHitParticle"] = true;
    
    std::string jsonParams = Json::writeString(builder, animParams);
    return ExecuteREDkitFunction("PlayAttackAnimation", jsonParams);
}
```

### **2. Mejoras para WitcherScriptBridge.cpp**

#### **Función ExecuteCombatAction() - MEJORADA**
```cpp
bool WitcherScriptBridge::ExecuteCombatAction(uint32_t playerId, const std::string& action, const std::string& parameters)
{
    // Parsear parámetros JSON
    Json::Value params;
    Json::Reader reader;
    if (!reader.parse(parameters, params)) return false;
    
    // Crear script Lua que use las APIs reales
    std::string luaScript = R"(
        local action = W3DamageAction.new()
        action:Initialize(
            attacker, victim, carrier, sourceName,
            hitReactionType, powerStatType,
            ignoreImmortality, ignoreArmor, ignoreResistance, isEnvironment
        )
        action:AddDamage(damageType, damageValue)
        action:SetHitAnimationPlayType(EAHA_ForceYes)
        theGame.damageMgr:ProcessAction(action)
    )";
    
    // Ejecutar en Lua
    lua_State* L = static_cast<lua_State*>(m_witcherScriptEngine);
    return luaL_dostring(L, luaScript.c_str()) == 0;
}
```

### **3. Nuevas Funciones Basadas en el Mod**

#### **Función ProcessHitReaction() - NUEVA**
```cpp
bool REDkitBridge::ProcessHitReaction(uint32_t playerId, uint32_t attackerId, float damage, bool isCritical)
{
    Json::Value reactionParams;
    reactionParams["victim"] = playerId;
    reactionParams["attacker"] = attackerId;
    reactionParams["damage"] = damage;
    reactionParams["isCritical"] = isCritical;
    reactionParams["hitReactionType"] = isCritical ? "EHRT_Light" : "EHRT_None";
    reactionParams["hitAnimationPlayType"] = "EAHA_ForceYes";
    
    std::string jsonParams = Json::writeString(builder, reactionParams);
    return ExecuteREDkitFunction("ProcessHitReaction", jsonParams);
}
```

#### **Función CheckCombatState() - NUEVA**
```cpp
bool REDkitBridge::CheckCombatState(uint32_t playerId)
{
    Json::Value stateParams;
    stateParams["playerId"] = playerId;
    stateParams["checkBuffs"] = true;
    stateParams["buffsToCheck"] = {
        "EET_HeavyKnockdown",
        "EET_Knockdown", 
        "EET_LongStagger",
        "EET_Stagger"
    };
    
    std::string jsonParams = Json::writeString(builder, stateParams);
    return ExecuteREDkitFunction("CheckCombatState", jsonParams);
}
```

---

## 📋 **FUNCIONES WITCHERSCRIPT NECESARIAS**

### **1. MP_CombatSystem.ws - ACTUALIZADO**
```witcherscript
// Función para ejecutar ataque usando APIs reales
public function ExecuteAttack(attackerId: int32, victimId: int32, attackType: int32, weaponType: int32) : bool
{
    var action: W3DamageAction;
    var attacker: CActor;
    var victim: CActor;
    
    attacker = GetActorById(attackerId);
    victim = GetActorById(victimId);
    
    if (!attacker || !victim) return false;
    
    action = new W3DamageAction in theGame.damageMgr;
    action.Initialize(attacker, victim, NULL, "multiplayer_attack", EHRT_Light, CPS_AttackPower, false, false, false, false);
    action.AddDamage(GetDamageType(attackType), CalculateDamage(attackType, weaponType));
    action.SetHitAnimationPlayType(EAHA_ForceYes);
    action.SetCanPlayHitParticle(true);
    
    if (IsCriticalHit(attackerId, victimId))
    {
        action.SetCriticalHit();
    }
    
    theGame.damageMgr.ProcessAction(action);
    delete action;
    
    return true;
}

// Función para procesar reacción de hit
public function ProcessHitReaction(victimId: int32, attackerId: int32, damage: float, isCritical: bool) : bool
{
    var victim: CActor;
    var attacker: CActor;
    
    victim = GetActorById(victimId);
    attacker = GetActorById(attackerId);
    
    if (!victim || !attacker) return false;
    
    // Verificar si puede recibir daño
    if (victim.HasBuff(EET_HeavyKnockdown) || 
        victim.HasBuff(EET_Knockdown) || 
        victim.HasBuff(EET_LongStagger) || 
        victim.HasBuff(EET_Stagger))
    {
        return false;
    }
    
    // Configurar para reproducir animación
    victim.SetCanPlayHitAnim(true);
    
    return true;
}
```

---

## 🎯 **BENEFICIOS OBTENIDOS**

### **1. APIs Reales Identificadas**
- ✅ **W3DamageAction**: Sistema completo de daño
- ✅ **W3EffectManager**: Sistema de efectos y buffs
- ✅ **CActor**: Sistema de combate y animaciones
- ✅ **theGame.damageMgr**: Gestor de daño del juego

### **2. Patrones de Implementación**
- ✅ **Creación de acciones**: `new W3DamageAction in theGame.damageMgr`
- ✅ **Inicialización**: `Initialize(attacker, victim, ...)`
- ✅ **Configuración**: `SetHitAnimationPlayType()`, `SetCriticalHit()`
- ✅ **Procesamiento**: `theGame.damageMgr.ProcessAction(action)`

### **3. Tipos de Datos Reales**
- ✅ **EHitReactionType**: Tipos de reacción
- ✅ **EAnimationHitAction**: Tipos de animación
- ✅ **EDamageType**: Tipos de daño
- ✅ **ECombatPowerStat**: Tipos de poder

---

## 🚀 **PRÓXIMOS PASOS**

### **Fase 1: Implementar APIs Reales (1 semana)**
1. **Actualizar REDkitBridge.cpp** con APIs reales
2. **Crear funciones WitcherScript** usando patrones del mod
3. **Implementar sistema de daño** completo
4. **Agregar sistema de efectos** y buffs

### **Fase 2: Testing y Validación (1 semana)**
1. **Probar funciones** con datos reales
2. **Validar animaciones** y efectos
3. **Corregir bugs** encontrados
4. **Optimizar rendimiento**

### **Fase 3: Integración Completa (1 semana)**
1. **Integrar** con sistemas existentes
2. **Documentar** nuevas APIs
3. **Crear tests** unitarios
4. **Validar** en entorno real

---

## ⚠️ **CONSIDERACIONES IMPORTANTES**

### **Limitaciones**
- **Mod dependencies**: Requiere que el mod esté instalado
- **Version compatibility**: APIs pueden cambiar entre versiones
- **Performance**: Procesamiento de daño puede ser costoso

### **Riesgos**
- **API changes**: CDPR puede cambiar APIs internas
- **Mod conflicts**: Otros mods pueden interferir
- **Save compatibility**: Cambios pueden afectar saves

### **Mitigaciones**
- **Fallback**: Mantener implementación mock
- **Validation**: Verificar APIs antes de usar
- **Logging**: Información detallada para debugging
- **Documentation**: Documentar todas las APIs usadas

---

## 🎉 **CONCLUSIÓN**

El análisis del mod proporciona información invaluable sobre el sistema de combate interno de TW3. Con estas APIs reales, podemos implementar un sistema de combate multiplayer mucho más robusto y funcional.

**Estado actual**: **APIs reales identificadas y listas para implementación**
**Próximo objetivo**: **Implementar sistema de combate real**

---

*Análisis completado el 2024-12-19*
*Basado en mod "Fix sounds of sword hits when adding effects"*
