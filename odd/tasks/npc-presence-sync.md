# npc-presence-sync (F3)

Objective: Synchronize NPC/enemy presence (and related transforms) between players so the world feels shared — the largest technical gap vs a true online RPG. Branch: `feat/npc-presence-sync`. Delivery: `ask-on-risk`. Runner: `server/scripts/run-party-scope-tests.sh` + dual-client checklist.

## Specs

- S1: User wants an experience closer to online titles ("Where Winds Meet" style shared world), without importing another game's server: prior ask "como puedo hacer para traerlo a este mod".
- S2: Upstream README: "NPC sync is NOT implemented" — this feature exists to change that for an authorized slice.
- S3: First slice: **party-only** hostile/friendly NPC presence + basic transform fields — not full AI authority or quest NPC state.
- S4: Touches `client/` + `witcher/` + `server/` — Risk: contracts (new `NPC` opcode), concurrency.
- S5: Depends on F0; uses F2 party model for scope.

Acceptance: two party clients see the same published NPC pipe (receive apply path). Full entity spawn may remain scaffold until T5 evidence.

## Tasks

- [x] T1 (S2, S4) — Map native hook surface and current opcodes — route: inline — status: done — commit: 3fb23f9
- [x] T2 (S1, S3) — Product refine: party-only vs proximity global — route: inline — status: done — commit: pending
  - User confirmed party-only ("perfecto" after recommendation).
- [x] T3 (S3, S4) — Protocol + server relay for NPC presence slice — route: inline — status: done — commit: pending
- [x] T4 (S3) — Client/script apply remote NPC presence — route: inline — status: partial — commit: pending
  - Pipe end-to-end scaffolded; entity spawn deferred (debug flag).
- [ ] T5 (S3) — Dual-client verification evidence — route: inline — status: pending — commit: —

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F3.
- L3: Related ask WWM → bring model to this mod.
- L4: 2026-10-31 — User: "siguei con F3". T1 done.
- L5: 2026-10-31 — Recommended party-only; user: "perfecto". Implemented NPC opcode + party filter + client/script scaffold.
- L6: Risk: item 3 (new NPC protocol contract); item 4 (party recipient selection). Item 6: PartyScopeRules tests; T5 in-game still open.
