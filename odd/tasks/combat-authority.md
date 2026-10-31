# combat-authority (F4)

Objective: Move combat resolution toward server-validated outcomes to reduce desync and trivial client-side cheating — required before credible MMO PvE/PvP. Branch: `feat/combat-authority`. Delivery: `ask-on-risk`. Runner: dual-client combat scenarios; optional Java unit tests for validation rules if extracted.

## Specs

- S1: Supports MMORPG direction: "con la base de este mod lo podemos llevar a un mmorpg al juego" by making combat not purely local.
- S2: Scope for first slice (assumption): validate hit/damage application between players (and synced NPCs from F3 if present); full physics rewrite OUT of scope.
- S3: Server becomes authority for the agreed combat events; clients render confirmed results.
- S4: Risk: security (anti-cheat surface), contracts (new/changed opcodes), concurrency.
- S5: Depends on F3 for NPC targets; player-vs-player slice may start earlier if explicitly ordered.

Acceptance: forged/local-only damage does not stick for covered cases; both clients show server-confirmed HP/outcome. Checks: abuse-case checklist + happy path duel/fight.

## Tasks

- [ ] T1 (S2, S4) — Inventory current combat-related packets/scripts — route: inline — status: pending — commit: —
- [ ] T2 (S2, S3) — Define validation rules for slice — route: inline — status: pending — commit: —
- [ ] T3 (S3, S4) — Server validation + client apply path — route: inline — status: pending — commit: —
- [ ] T4 (S3) — Abuse + happy-path verification — route: inline — status: pending — commit: —

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F4. Implementation not authorized yet; tracking only.
