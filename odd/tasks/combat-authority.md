# combat-authority (F4)

Objective: Move combat resolution toward server-validated outcomes to reduce desync and trivial client-side cheating — required before credible MMO PvE/PvP. Branch: `feat/combat-authority`. Delivery: `ask-on-risk`. Runner: `server/scripts/run-combat-authority-tests.sh` + dual-client combat checklist.

## Specs

- S1: Supports MMORPG direction: "con la base de este mod lo podemos llevar a un mmorpg al juego" by making combat not purely local.
- S2: Scope for first slice (assumption): validate hit/damage application between players (and synced NPCs from F3 if present); full physics rewrite OUT of scope.
- S3: Server becomes authority for the agreed combat events; clients render confirmed results.
- S4: Risk: security (anti-cheat surface), contracts (new/changed opcodes), concurrency.
- S5: Depends on F3 for NPC targets; player-vs-player slice may start earlier if explicitly ordered.

Acceptance: forged/local-only damage does not stick for covered cases; both clients show server-confirmed HP/outcome. Checks: abuse-case checklist + happy path duel/fight.

## Tasks

- [x] T1 (S2, S4) — Inventory current combat-related packets/scripts — route: inline — status: done — commit: e1fb713
  - Pre-F4: no HIT path; combat was local. Surface: MOVE positions + UPDATE* + F3 NPC. New opcodes: `HIT` / `HITACK`, script poll `11`.
- [x] T2 (S2, S3) — Define validation rules for slice — route: inline — status: done — commit: e1fb713
  - `CombatAuthorityRules`: clamp 0.01–0.35, 200ms cooldown, 8u range, HP 0..1.
- [x] T3 (S3, S4) — Server validation + client apply path — route: inline — status: done — commit: e1fb713
  - Server `handleHitRequest`; DLL `hit`/`HITACK`; script publish + `applyCombatHitAck`.
- [ ] T4 (S3) — Abuse + happy-path verification — route: inline — status: partial — commit: e1fb713 (unit); dual-client open
  - Unit tests: `run-combat-authority-tests.sh`. Dual-client checklist in `docs/hosting/combat-authority.md` (operator-owned, same as F3 T5).

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F4. Implementation not authorized yet; tracking only.
- L3: 2026-10-31 — User: "despues lo pruebo, seguimos con F4" (defer F3 T5; authorize F4).
- L4: 2026-10-31 — Implemented PvP HIT→HITACK authority slice + docs + unit tests. In-game duel proof deferred (R12).
- L5: Risk: item 2 (anti-cheat surface for covered hits); item 3 (HIT/HITACK contract); item 4 (session HP/cooldown concurrency). Item 6: Java unit tests; dual-client T4 still open.
