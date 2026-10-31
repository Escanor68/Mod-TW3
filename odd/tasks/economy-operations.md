# economy-operations (F10)

Objective: Replace client-trusted INVSAVE snapshots with server-validated economic operations (add/remove crowns, bag mutations) that are idempotent and audit-logged. Branch: `feat/economy-operations`. Delivery: merge-to-main + delete work branch. Runner: persistence + new economy unit tests.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit: crowns/bag reported by client; “server wins on load” does not prevent dupe via INVSAVE; R8 residual underestimated.
- S3: Model explicit ops: `ECON_ADD` / `ECON_REMOVE` / `ECON_SET_BAG_ITEM` (names TBD) with server preconditions (balance, allowlist, capacity).
- S4: Idempotent op ids so reconnect retries do not double-apply.
- S5: Persist confirmed results only (builds on F8 atomic store); return authoritative snapshot to client.
- S6: Minimal audit log for crown/bag changes (session/player/op/result).
- S7: Assumption: SQLite optional later — first slice stays file store + ops journal if needed. Full TW3 inventory still OUT.
- S8: Depends on F5, F8. Risk: security, data, contracts.

Acceptance: client cannot set arbitrary crowns via INVSAVE (deprecated or ignored for covered fields); ops path unit-tested; dual-client checklist for crowns/bag.

## Tasks

- [ ] T1 (S3, S7) — Op catalog + deprecate/ignore raw INVSAVE for crowns — route: inline — status: pending — commit: —
- [ ] T2 (S3, S4, S5) — Server apply + idempotency + persist — route: inline — status: pending — commit: —
- [ ] T3 (S6) — Audit log slice — route: inline — status: pending — commit: —
- [ ] T4 (S3) — Client migrate to ops — route: inline — status: pending — commit: —
- [ ] T5 (S1, S8) — Tests + docs; update R8 status — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (persistir operaciones, no snapshots).
