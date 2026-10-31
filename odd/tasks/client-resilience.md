# client-resilience (F13)

Objective: Harden C++ / script client — serialize socket lifecycle, reconnect path, fix AUTH error mapping, bound/cleanup `remotePlayers`, reduce SenderThread sleep jitter. Branch: `feat/client-resilience`. Delivery: merge-to-main + delete work branch. Runner: compile client where available + dual-client reconnect checklist (operator).

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit: race Closing socket while send; no reconnect; AUTH_FAILED mapped to NotWhitelisted; unknown ERROR closes session; remotePlayers unbounded + sequence stuck after server restart; Sleep(1) ~15ms on Windows.
- S3: Single owner / strand (or mutex) for socket open/close/send.
- S4: Reconnect policy: backoff, reset remote player sequence state, re-AUTH / session (coord F12).
- S5: Map AUTH_* and SESSION_* to distinct script signals; ROLE_/STORYDONE_ stay soft.
- S6: Cap or TTL-evict remotePlayers; clear on disconnect/reconnect.
- S7: Replace busy Sleep(1) with event/condition where feasible.
- S8: Assumption: full protocol schema codegen OUT; document fragile field indices for later.
- S9: Risk: contracts, concurrency; depends on F12 for session errors.

Acceptance: documented reconnect smoke; AUTH fail shows correct UX; remotePlayers does not grow without bound in checklist notes; unit/build as available.

## Tasks

- [ ] T1 (S3, S7) — Socket ownership + sender wake — route: inline — status: pending — commit: —
- [ ] T2 (S4, S6) — Reconnect + remotePlayers reset/evict — route: inline — status: pending — commit: —
- [ ] T3 (S5) — Error opcode mapping in DLL/script — route: inline — status: pending — commit: —
- [ ] T4 (S1, S8) — Docs + dual-client reconnect checklist — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (cliente C++).
