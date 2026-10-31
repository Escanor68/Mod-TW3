# authority-matrix (F14)

Objective: Publish and maintain a written authority matrix (who is source of truth per data/action) and keep `PlayerSession` vs domain state split as an agreed design before more features. Branch: `feat/authority-matrix`. Delivery: merge-to-main + delete work branch. Runner: doc review only (passive) unless code split authorized in T3.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit §5 matrix: identity, position, health, economy, party, quests, NPC rewards, VFX.
- S3: Doc under `docs/hosting/authority-matrix.md` matching current code + target state (now vs next).
- S4: Recommend split: `PlayerSession` (network) vs `PlayerState` (gameplay) — implementation optional follow-up task if authorized.
- S5: Handlers should map packets → domain commands (design note); no big-bang rewrite in this feature unless T3 scoped.
- S6: Risk: contracts/docs only unless T3 code; item 3 if public docs change expectations.

Acceptance: matrix merged; README links it; gaps explicitly list owning ODD (F9–F15).

## Tasks

- [ ] T1 (S2, S3) — Draft matrix now vs target — route: inline — status: pending — commit: —
- [ ] T2 (S4, S5) — Session vs state split design note — route: inline — status: pending — commit: —
- [ ] T3 (S4) — Optional: first code extraction of PlayerState (only if authorized) — route: inline — status: pending — commit: —
- [ ] T4 (S1) — README + roadmap cross-links — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (matriz de autoridad).
