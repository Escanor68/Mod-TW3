# session-hardening (F12)

Objective: Real session binding — session id, invalidate prior endpoint on reconnect, optional challenge handshake; stop treating shared authToken as per-player identity. Branch: `feat/session-hardening`. Delivery: merge-to-main + delete work branch. Runner: identity/auth unit tests + dual-client reconnect checklist.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit: shared secret ≠ identity; no nonce/replay protection; authenticated defaults true; R5 “resolved” overstated.
- S3: On AUTH success (or join): issue `sessionId` (or generation) that gameplay packets must carry / bind.
- S4: Reconnect: invalidate previous endpoint for same username; reject stale packets from old generation.
- S5: Optional challenge-response handshake (server nonce) before AUTH completes — T1 chooses minimal vs full HMAC-per-packet (HMAC later OUT unless authorized).
- S6: Rotate/configure secrets without client recompile (properties/env); never log tokens.
- S7: Distinguish ERROR codes: AUTH_REQUIRED / AUTH_FAILED / SESSION_STALE (client F13 maps correctly).
- S8: Depends on F1, F8. Risk: security, contracts.
- S9: Assumption: full account passwords still OUT (future); this hardens UDP session layer.

Acceptance: two endpoints cannot drive one username; stale session rejected; docs + tests; R5 status revised honestly.

## Tasks

- [ ] T1 (S3, S5) — Decide handshake depth (sessionId vs challenge) — route: inline — status: pending — commit: —
- [ ] T2 (S3, S4, S6) — Server session generation + invalidate old endpoint — route: inline — status: pending — commit: —
- [ ] T3 (S7) — Protocol ERROR codes + client mapping (coord F13) — route: inline — status: pending — commit: —
- [ ] T4 (S1, S8) — Tests + docs + risk update — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (sesión / replay / identidad).
