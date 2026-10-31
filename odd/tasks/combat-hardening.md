# combat-hardening (F11)

Objective: Harden F4 combat — fail-closed positioning (shared with F8), server-derived damage (not client-declared), DPS budget, HITACK only to interested recipients, richer reject reasons. Branch: `feat/combat-hardening`. Delivery: merge-to-main + delete work branch. Runner: `run-combat-authority-tests.sh` extended + dual-client.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit: client declares damage; clamp 0.35 + 200ms ≈ kill in ~0.6s; HITACK flooded to all endpoints vs interest.
- S3: Server derives damage from hitType / allowed action table (client may suggest type, not magnitude).
- S4: Add per-attacker damage budget / longer cooldown policy (T1 numbers).
- S5: Validate attacker/target exist, same party-or-interest instance, not dead/disconnected (expand rules).
- S6: HITACK (and combat errors) respect F6 interest (+ party); no global flood.
- S7: Deterministic resolution when two hits race (document lock/order).
- S8: Depends on F4, F6, F8. Risk: security, contracts, concurrency.
- S9: Assumption: still PvP player targets first; PvE NPC HP authority OUT unless extended.

Acceptance: unit tests for derived damage + budget + interest-scoped HITACK; abuse checklist updated; dual-client residual operator-owned.

## Tasks

- [ ] T1 (S3, S4) — Damage table + DPS budget constants — route: inline — status: pending — commit: —
- [ ] T2 (S5, S7) — Expanded CombatAuthorityRules + handler — route: inline — status: pending — commit: —
- [ ] T3 (S6) — HITACK/error recipients via interest — route: inline — status: pending — commit: —
- [ ] T4 (S1, S8) — Tests + docs; reopen/mitigate R7 as needed — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (autoridad de combate).
