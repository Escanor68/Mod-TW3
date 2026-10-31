# world-instances (F6)

Objective: Add instance/shard or proximity interest-management so the server can scale beyond a single flat broadcast of all players/NPCs. Branch: `feat/world-instances`. Delivery: merge-to-main + delete work branch. Runner: `server/scripts/run-interest-management-tests.sh` + multi-client load notes.

## Specs

- S1: Enable many concurrent players in an MMO-like deployment after earlier features exist.
- S2: First slice: **interest management** (only broadcast nearby entities) — region instances OUT.
- S3: Must not break F0 join path or F1 identity; party members (F2) remain co-interested always.
- S4: Risk: concurrency, contracts (routing), delivery/ops (multi-process later OUT unless authorized).
- S5: Depends on F0–F3 at minimum; authorized after F5 closed.

Acceptance: N clients stay playable with bounded broadcast cost vs baseline; parties stay together per rules. Checks: server stats (broadcast ticks/packets) before/after + playability notes.

## Tasks

- [x] T1 (S2, S3) — Blocking decision: interest mgmt vs region instances — route: inline — status: done — commit: 092924f
  - User confirmed interest management ("si").
- [x] T2 (S2, S4) — Design + implement chosen slice — route: inline — status: done — commit: 092924f
  - `InterestManagementRules` + `snapshotInterestRecipients` on MOVE/UPDATE*/HITACK.
- [x] T3 (S3, S5) — Regression with party + NPC presence — route: inline — status: done — commit: 092924f
  - Party always relays; NPC stays party-only (unchanged).
- [ ] T4 (S1) — Load smoke + stats evidence — route: inline — status: partial — commit: 092924f (unit); load smoke open
  - Unit tests + heartbeat fields; dual-client load smoke operator-owned (R12).

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F6. Implementation not authorized yet; tracking only.
- L3: 2026-10-31 — User: F5 then F6 on separate branches; merge to main and delete work branch when done.
- L4: 2026-10-31 — Recommended interest management; user: "si".
- L5: Risk: item 4 (recipient selection concurrency); item 3 (routing behavior change, opcodes unchanged). Item 6: unit tests; in-game T4 open.
