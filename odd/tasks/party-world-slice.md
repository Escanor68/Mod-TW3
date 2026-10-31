# party-world-slice (F2)

Objective: Expand the existing party slice so co-op players share a small set of world conditions (time, weather, dialog choices) more reliably — first shared-world vertical, not full MMORPG authority. Branch: `feat/party-world-slice`. Delivery: `ask-on-risk`. Runner: in-game checklist in `docs/hosting/party-world-slice.md` (no WitcherScript automated suite).

## Specs

- S1: Advance shared experience beyond "ghost players only", toward the user's MMORPG direction: "con la base de este mod lo podemos llevar a un mmorpg al juego".
- S2: Scope is party-scoped sync already seeded in the mod (time, weather, cutscene dialog) — harden and document; do NOT claim global world state.
- S3: README constraint remains: NPC sync is a later feature (F3); this feature must not pretend NPCs are shared.
- S4 (assumption): Party leader remains authority for dialog choices unless user later decides otherwise.
- S5: Depends on F0 (reachable server); benefits from F1 (stable identities in party UI).

Acceptance: party of ≥2 observes matching time/weather and leader dialog choices per agreed cases. Checks: recorded in-game checklist in Log after runs.

## Tasks

- [x] T1 (S2, S3) — Inventory current party sync scripts/packets — route: inline — status: done — commit: pending
- [x] T2 (S1, S2, S4) — Close gaps for time/weather/dialog reliability — route: inline — status: done — commit: pending
  - Store world fields on global player; fall back in `updateWorldSync` when ghost missing.
- [x] T3 (S2) — Manual acceptance checklist + run evidence — route: inline — status: partial — commit: pending
  - Checklist published; in-game dual-client evidence still operator-owned (no TW3 runtime here).

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F2. Implementation not authorized yet; tracking only.
- L3: 2026-10-31 — User authorized F2 + push to https://github.com/Escanor68/Mod-TW3 for game-related work.
- L4: Risk: item 3 (client sync behavior change for party world fields); item 6 — no automated WS tests; checklist only.
