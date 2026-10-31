# scale-experience (F17)

Objective: Improve multiplayer feel and scale — remote interpolation buffer, movement vs teleport separation, optional spatial grid beyond linear interest scan, LOD update rates; measure before multi-process shards. Branch: `feat/scale-experience`. Delivery: merge-to-main + delete work branch. Runner: interest/load notes + dual-client feel checklist.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit §8 / Fase 4: interpolation, measure update freq/recipients, spatial partition if N grows, LOD, queue drop stale.
- S3: Keep F6 party always-interested; LOD may reduce non-party far updates further.
- S4: Separate normal MOVE from teleport/respawn/zone-change events (protocol or flags).
- S5: Multi-process shards remain OUT until metrics from F16 justify (same as F6 S2).
- S6: Depends on F6, F13, F16. Risk: contracts, concurrency, delivery.

Acceptance: measurable recipient counts before/after; interpolation or documented client buffer; checklist for jitter; no shard requirement.

## Tasks

- [ ] T1 (S2, S3) — Baseline metrics from F16 + interest scan cost — route: inline — status: pending — commit: —
- [ ] T2 (S4) — Teleport/respawn event separation — route: inline — status: pending — commit: —
- [ ] T3 (S2) — Client remote interpolation buffer — route: inline — status: pending — commit: —
- [ ] T4 (S2, S5) — Optional spatial grid if T1 shows need — route: inline — status: pending — commit: —
- [ ] T5 (S1) — Docs + load notes — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (escala y experiencia).
