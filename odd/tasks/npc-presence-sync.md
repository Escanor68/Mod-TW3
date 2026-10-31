# npc-presence-sync (F3)

Objective: Synchronize NPC/enemy presence (and related transforms) between players so the world feels shared — the largest technical gap vs a true online RPG. Branch: `feat/npc-presence-sync`. Delivery: `ask-on-risk` (expect chain if >~400 LOC). Runner: in-game dual-client scenarios; native/client changes may need Windows MSVC build.

## Specs

- S1: User wants an experience closer to online titles ("Where Winds Meet" style shared world), without importing another game's server: prior ask "como puedo hacer para traerlo a este mod".
- S2: Upstream README: "NPC sync is NOT implemented" — this feature exists to change that for an authorized slice.
- S3: First slice (assumption until product refine): party-visible hostile/friendly NPC presence + basic position/anim, not full AI authority or quest NPC state.
- S4: Likely touches `client/` native bridge + `witcher/` scripts + `server/` broadcast opcodes — Risk items: contracts (protocol), concurrency.
- S5: Depends on F0; strongly prefer F2 party model before global NPC broadcast.

Acceptance: two clients in range see the same target NPC appear/move for the agreed slice cases. Checks: dual-client checklist; protocol fields documented.

## Tasks

- [x] T1 (S2, S4) — Map native hook surface and current opcodes — route: inline — status: done — commit: pending
  - Inventory: `docs/hosting/npc-presence-sync.md`. `presence` tag ≠ NPC sync; server only MOVE/UPDATE1–4.
- [ ] T2 (S1, S3) — Product refine: party-only vs proximity global (blocking decision before code) — route: inline — status: blocked — commit: —
- [ ] T3 (S3, S4) — Protocol + server relay for NPC presence slice — route: inline — status: pending — commit: —
- [ ] T4 (S3) — Client/script apply remote NPC presence — route: inline — status: pending — commit: —
- [ ] T5 (S3) — Dual-client verification evidence — route: inline — status: pending — commit: —

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F3. Implementation not authorized yet; tracking only.
- L3: Related ask: "y como funciona en los demas juegos como where winds meet ... como puedo hacer para traerlo a este mod?"
- L4: 2026-10-31 — User: "siguei con F3". T1 inventory completed; T2 blocking on scope choice.
