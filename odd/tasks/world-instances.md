# world-instances (F6)

Objective: Add instance/shard or proximity interest-management so the server can scale beyond a single flat broadcast of all players/NPCs. Branch: `feat/world-instances`. Delivery: `ask-on-risk`. Runner: multi-client load smoke; metrics from server stats counters.

## Specs

- S1: Enable many concurrent players in an MMO-like deployment after earlier features exist.
- S2: First slice (assumption): interest management (only broadcast nearby entities) OR simple region instances — pick one in a blocking product decision before code.
- S3: Must not break F0 join path or F1 identity; party members (F2) should remain co-instanced when possible.
- S4: Risk: concurrency, contracts (routing), delivery/ops (multi-process later OUT unless authorized).
- S5: Depends on F0–F3 at minimum; do not start implementation before those are green unless user reorders.

Acceptance: N clients (agreed N) stay playable with bounded broadcast cost vs baseline; parties stay together per rules. Checks: server stats (broadcast ticks/packets) before/after + playability notes.

## Tasks

- [ ] T1 (S2, S3) — Blocking decision: interest mgmt vs region instances — route: inline — status: pending — commit: —
- [ ] T2 (S2, S4) — Design + implement chosen slice — route: inline — status: pending — commit: —
- [ ] T3 (S3, S5) — Regression with party + NPC presence — route: inline — status: pending — commit: —
- [ ] T4 (S1) — Load smoke + stats evidence — route: inline — status: pending — commit: —

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F6. Implementation not authorized yet; tracking only.
