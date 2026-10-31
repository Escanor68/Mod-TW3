# NPC presence sync (F3)

**Scope decision (T2):** **party-only** — NPC snapshots relay only to the publisher’s party (same model as F2 world sync).

## Protocol (T3)

### Script → DLL
```
npc <npcKey> <template> <x> <y> <z> <yaw> <hp> <state>
```
`WO_Send(...)` / `publishNpcPresence(...)`.

### DLL → UDP
```
NPC\t<playerId>\t<username>\t<npcKey>\t<template>\t<x>\t<y>\t<z>\t<yaw>\t<hp>\t<state>
```

### Server
- Accepts opcode `NPC` (min 8 fields).
- Parses party from `UPDATE4` fields `[0]=inParty`, `[1]=joinedParty`.
- Broadcasts `NPC` only to **same party**, excluding publisher.
- Solo players (party of one) → no relay.

### DLL → Script
`WO_Poll` opcode **10** → `WO_ApplyNpc` → `applyRemoteNpcPresence(...)`.

### Entity spawn (T4)

Receivers spawn a non-persistent `CActor` ghost (`MPEntity` + `MPNpcEntity`), invulnerable / non-colliding (same idea as player ghosts).  
While in a party, each client auto-publishes its current hostile `GetTarget()` (~4 Hz) using the shared `geralt_npc.w2ent` visual for this slice.  
Stale entries prune after ~5s; `state=despawn` removes immediately. Cap: 24 remote NPCs.

## Inventory reminder

`presence` / `presence_transition` tags remain **menu status**, not world NPCs.

## Manual checklist (T5)

- [ ] A and B in same party; A fights an enemy → B sees a ghost near that enemy
- [ ] C not in party → no NPC ghost
- [ ] A alone → no self-echo / no broadcast
- [ ] Leave party / hide ghosts → remote NPC entities cleared
- [ ] Optional: `MPGhosts_DebugNpc` shows spawn toasts

## Residual risk (R6)

First slice uses a shared `geralt_npc` visual and party-only relay. Full AI/quest NPC authority is out of scope; residual **accepted** for F3.
