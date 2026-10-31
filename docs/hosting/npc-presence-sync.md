# NPC presence sync (F3) — inventory

Status: **not implemented** for world NPCs (README). This doc maps what exists today before adding a slice.

## Quick take

Today the stack syncs **other players** (ghosts). Tags named `presence` are **menu/status strings**, not NPC entities. There is **no** NPC opcode on the Java UDP server yet.

## Layers

| Layer | What exists | NPC gap |
|-------|-------------|---------|
| WitcherScript | `client.ws` ghosts via `geralt_npc` / `female_npc` templates for **remote players** | No spawn/track of world NPCs/enemies for other clients |
| Native DLL | `WO_Send` / `WO_Poll` / `WO_Tick`; outbound tags `move`,`wo`…`wo4`,`presence`,`presence_transition` | `presence*` = local menu status (`Teleporting`, `IngameMenu`), injected into UPDATE1 menu field |
| Native Bridge API | `WNB_RegisterNative` — register script natives | No world-entity enumeration API in-tree |
| Server | Opcodes `MOVE`, `UPDATE1A/B`…`UPDATE4` only | Must add new opcode(s) + relay/filter policy |

## Inbound opcodes (script `WO_Poll`)

| Code | Meaning |
|------|---------|
| 1 | Move |
| 2–5 | Update1–4 |
| 6–9 | UsernameTaken / Kicked / Banned / NotWhitelisted |

No slot reserved for NPC payloads yet.

## Suggested first slice (pending product choice)

1. **Detect** nearby combat NPCs on the publishing client (script and/or future native hook).
2. **Publish** compact snapshots: id/template, pos, yaw, anim/state, hp (minimal).
3. **Relay** on server with a **scope filter** (party vs proximity — see decision below).
4. **Apply** on receivers as ephemeral remote actors (not quest authority).

Out of scope for first slice: AI authority, quest NPC state, loot tables, full anim graphs.

## Dependencies

- F0 reachability, F2 party model (if party-scoped).
- Windows MSVC build for client DLL changes.
- Dual-client TW3 verification (R12).

## Open product decision (blocks T3+)

**Party-only** vs **proximity-global** broadcast of NPC snapshots — recorded in `odd/tasks/npc-presence-sync.md` T2.
