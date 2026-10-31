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

Spawn of real entities is **scaffolded** (debug notification behind `MPGhosts_DebugNpc`); full ghost-NPC lifecycle is the next hardening pass (T4/T5 in-game).

## Inventory reminder

`presence` / `presence_transition` tags remain **menu status**, not world NPCs.

## Manual checklist (T5)

- [ ] A and B in same party; A publishes NPC → B receives opcode 10 / debug toast if debug on
- [ ] C not in party → no NPC packet
- [ ] A alone → no self-echo / no broadcast
- [ ] After leave party → stop receiving
