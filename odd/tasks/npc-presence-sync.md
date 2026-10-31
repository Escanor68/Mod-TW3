# npc-presence-sync (F3)

Objective: Synchronize NPC/enemy presence (and related transforms) between players so the world feels shared — the largest technical gap vs a true online RPG. Branch: `feat/npc-presence-sync`. Delivery: `ask-on-risk`. Runner: `server/scripts/run-party-scope-tests.sh` + dual-client checklist.

## Specs

- S1: User wants an experience closer to online titles ("Where Winds Meet" style shared world), without importing another game's server: prior ask "como puedo hacer para traerlo a este mod".
- S2: Upstream README: "NPC sync is NOT implemented" — this feature exists to change that for an authorized slice.
- S3: First slice: **party-only** hostile/friendly NPC presence + basic transform fields — not full AI authority or quest NPC state.
- S4: Touches `client/` + `witcher/` + `server/` — Risk: contracts (new `NPC` opcode), concurrency.
- S5: Depends on F0; uses F2 party model for scope.

Acceptance: two party clients see a remote NPC ghost for the publisher’s combat target. T5 = in-game dual-client proof.

## Tasks

- [x] T1 (S2, S4) — Map native hook surface and current opcodes — route: inline — status: done — commit: 3fb23f9
- [x] T2 (S1, S3) — Product refine: party-only vs proximity global — route: inline — status: done — commit: 7c92f59
  - User confirmed party-only ("perfecto" after recommendation).
- [x] T3 (S3, S4) — Protocol + server relay for NPC presence slice — route: inline — status: done — commit: 7c92f59
- [x] T4 (S3) — Client/script apply remote NPC presence — route: inline — status: done — commit: pending
  - Spawn/teleport/despawn ghosts + auto-publish local hostile target while in party.
- [ ] T5 (S3) — Dual-client verification evidence — route: inline — status: pending — commit: —

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F3.
- L3: Related ask WWM → bring model to this mod.
- L4: 2026-10-31 — User: "siguei con F3". T1 done.
- L5: 2026-10-31 — Recommended party-only; user: "perfecto". Implemented NPC opcode + party filter + client/script scaffold.
- L6: Risk: item 3 (new NPC protocol contract); item 4 (party recipient selection). Item 6: PartyScopeRules tests; T5 in-game still open.
- L7: 2026-10-31 — User: "seguimos con spawn de entidades". Implemented remoteNpc ghost spawn + local target publish.
