# Documentación de Red - Witcher3-MP

## Arquitectura de Red

### Cliente-Servidor
- **Servidor**: Maneja estado del juego, sincronización, lógica de combate
- **Cliente**: Envía input del jugador, recibe actualizaciones del servidor
- **Protocolo**: TCP para confiabilidad, UDP para datos en tiempo real

### Componentes Principales
- **net_server.cpp**: Servidor principal
- **net_client.cpp**: Cliente de red
- **net_connection.cpp**: Gestión de conexiones
- **net_message.cpp**: Sistema de mensajes
- **NetworkOptimizer.cpp**: Optimizaciones de red

## Tipos de Mensajes

### Mensajes de Conexión
```cpp
ClientConnect     // Cliente se conecta
ClientDisconnect  // Cliente se desconecta
ClientPing        // Ping del cliente
ServerPong        // Respuesta del servidor
```

### Mensajes de Jugador
```cpp
PlayerJoin        // Jugador se une
PlayerLeave       // Jugador se va
PlayerMove        // Movimiento del jugador
PlayerUpdate      // Actualización de estado
PlayerAttack      // Ataque del jugador
PlayerDefend      // Defensa del jugador
```

### Mensajes de NPC
```cpp
NpcSpawn          // Spawn de NPC
NpcDespawn        // Despawn de NPC
NpcMove           // Movimiento de NPC
NpcUpdate         // Actualización de NPC
NpcAttack         // Ataque de NPC
NpcDeath          // Muerte de NPC
```

### Mensajes de Mundo
```cpp
WorldUpdate       // Actualización del mundo
WeatherChange     // Cambio de clima
TimeUpdate        // Actualización de tiempo
QuestUpdate       // Actualización de quest
```

## Optimizaciones de Red

### Compresión
- **Algoritmo**: LZ4 (rápido y eficiente)
- **Nivel**: Adaptativo según tipo de mensaje
- **Umbral**: Solo comprimir mensajes > 64 bytes

### Batching
- **Agrupación**: Múltiples mensajes en un paquete
- **Intervalo**: 16ms (60 FPS)
- **Prioridad**: Mensajes críticos se envían inmediatamente

### Predicción de Movimiento
- **Cliente**: Predice movimiento local
- **Servidor**: Valida y corrige predicciones
- **Interpolación**: Suaviza movimiento entre actualizaciones

## Configuración de Red

### Puerto del Servidor
```json
{
  "server": {
    "port": 60000,
    "max_connections": 4,
    "timeout": 30,
    "keep_alive": true
  }
}
```

### Optimizaciones
```json
{
  "network": {
    "tcp_nodelay": true,
    "compression": true,
    "batching": true,
    "packet_priority": true,
    "latency_target": 30.0,
    "packet_loss_threshold": 3.0
  }
}
```

## Logging de Red

### Archivos de Log
- **network.log**: Eventos de red, latencia, compresión
- **mp_session.log**: Sesión del mod, errores, warnings

### Niveles de Log
- **DEBUG**: Información detallada de red
- **INFO**: Eventos importantes
- **WARNING**: Problemas menores
- **ERROR**: Errores críticos

### Métricas Monitoreadas
- **Latencia**: Tiempo de respuesta cliente-servidor
- **Pérdida de Paquetes**: Porcentaje de paquetes perdidos
- **Compresión**: Ratio de compresión, tiempo de compresión
- **Conexiones**: Número de clientes conectados

## Testing de Red

### Test de Conexión Local
```cmd
# Compilar tests
cd scripts
build_test.bat

# Ejecutar tests
cd build\Release
test_network_connection.exe
```

### Test de Latencia
1. Ejecutar servidor
2. Conectar cliente
3. Enviar ping
4. Medir tiempo de respuesta

### Test de Compresión
1. Enviar mensaje grande
2. Verificar compresión
3. Verificar descompresión
4. Validar integridad de datos

## Solución de Problemas de Red

### Conexión Fallida
```cmd
# Verificar puerto
netstat -an | findstr "60000"

# Verificar firewall
# Agregar excepción para Witcher3-MP.exe
```

### Alta Latencia
1. Verificar conexión de red
2. Reducir `latency_target` en configuración
3. Habilitar compresión
4. Verificar carga del servidor

### Pérdida de Paquetes
1. Verificar estabilidad de red
2. Ajustar `packet_loss_threshold`
3. Verificar configuración de TCP
4. Monitorear logs de red

### Desconexiones Frecuentes
1. Verificar `timeout` en configuración
2. Verificar `keep_alive`
3. Verificar estabilidad de red
4. Verificar logs de errores

## Protocolo de Comunicación

### Handshake Inicial
1. Cliente envía `ClientConnect`
2. Servidor responde con `ServerPong`
3. Cliente envía `PlayerJoin`
4. Servidor confirma con `PlayerUpdate`

### Sincronización de Estado
1. Servidor envía `WorldUpdate` periódicamente
2. Cliente envía `PlayerMove` en tiempo real
3. Servidor valida y responde con `PlayerUpdate`
4. Cliente interpola movimiento

### Manejo de Errores
1. Timeout de conexión
2. Reconexión automática
3. Validación de mensajes
4. Logging de errores

## Seguridad de Red

### Validación de Datos
- **Cliente**: Valida datos del servidor
- **Servidor**: Valida datos del cliente
- **Checksums**: Verificación de integridad

### Prevención de Cheating
- **Servidor**: Lógica autoritativa
- **Validación**: Verificación de acciones
- **Rate Limiting**: Límite de mensajes por segundo

### Autenticación
- **Identificación**: ID único del jugador
- **Validación**: Verificación de identidad
- **Sesión**: Manejo de sesiones

## Rendimiento de Red

### Métricas Clave
- **Latencia**: < 50ms (local), < 100ms (internet)
- **Pérdida de Paquetes**: < 1%
- **Compresión**: > 50% para mensajes grandes
- **Throughput**: > 1MB/s

### Optimizaciones
- **Compresión**: LZ4 para datos grandes
- **Batching**: Agrupar mensajes pequeños
- **Priorización**: Mensajes críticos primero
- **Caching**: Reutilizar buffers

### Monitoreo
- **Logs**: Revisar logs de red regularmente
- **Métricas**: Monitorear latencia y pérdida
- **Alertas**: Configurar alertas para problemas

---

**Nota**: Esta documentación está diseñada para desarrolladores que trabajan con el sistema de red de Witcher3-MP.
