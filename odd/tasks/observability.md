# observability (F16)

Objective: Operable server — metrics for sessions, rejects, combat, persistence flush; structured logs with session/player/opcode (no secrets); health/degraded signals; document deploy/backup/rollback. Branch: `feat/observability`. Delivery: merge-to-main + delete work branch. Runner: metrics smoke via status JSON/console + unit where applicable.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit §10–11: packets in/out, rejects by reason, combat rejects, flush duration/errors, health endpoint, ops limits, backup/restore docs.
- S3: Extend existing status JSON / console stats (reuse svg/json port; keep default loopback).
- S4: Structured log fields: timestamp, sessionId (when F12 exists), playerId, opcode, errorCode.
- S5: Never log authToken / AuthToken / bag full dumps.
- S6: Document backup of `player-snapshots.tsv` + recovery after F8 fail-closed.
- S7: Risk: delivery/ops; low contract risk if read-only metrics.

Acceptance: status JSON exposes agreed counters; sample reject increments in unit/manual; hosting doc updated.

## Tasks

- [ ] T1 (S3, S4) — Metrics counters + status JSON fields — route: inline — status: pending — commit: —
- [ ] T2 (S4, S5) — Structured reject logging hygiene — route: inline — status: pending — commit: —
- [ ] T3 (S6) — Backup/restore + ops limits docs — route: inline — status: pending — commit: —
- [ ] T4 (S1) — Verify with QA session REPORT section optional — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (observabilidad).
