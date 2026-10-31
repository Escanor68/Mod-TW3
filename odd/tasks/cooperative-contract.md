# cooperative-contract (F15)

Objective: Pilot one cooperative witcher contract (or short shared quest) with server-defined states/transitions, shared vs personal progress, and idempotent rewards — without syncing the whole TW3 quest graph. Branch: `feat/cooperative-contract`. Delivery: merge-to-main + delete work branch. Runner: unit tests for state machine + dual-client checklist.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit §9: choose a bounded pilot; separate personal vs group progress; stable IDs; no double rewards; define leader disconnect.
- S3: Align with F7: mid-story contracts under story cast; leftovers after “historia terminada” free-pick (do not invent new product rules).
- S4: Server state machine for one contract id (T1 picks which); clients request transitions; server accepts/rejects.
- S5: Rewards via F10 ops when available; otherwise stub grant with idempotent key.
- S6: Depends on F7, F9 (party), F8; prefer after F10/F14. Risk: contracts, data, concurrency.
- S7: Assumption: no full NPC AI on server — only decisions that affect shared progress/rewards.

Acceptance: two clients cannot double-claim reward; disconnect mid-contract defined; unit tests for transitions; operator dual-client.

## Tasks

- [ ] T1 (S2, S3) — Pick contract id + state diagram — route: inline — status: pending — commit: —
- [ ] T2 (S4, S5) — Protocol + server state machine + reward idempotency — route: inline — status: pending — commit: —
- [ ] T3 (S4) — Client/script hooks — route: inline — status: pending — commit: —
- [ ] T4 (S1, S6) — Tests + docs + checklist — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (misión cooperativa piloto).
