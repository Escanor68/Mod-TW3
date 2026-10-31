# harden-base (F8)

Objective: Close P0 audit gaps — atomic fail-closed persistence, production auth gate, fail-closed HIT/interest without positions, server username validation, QA session token hygiene. Branch: `feat/harden-base`. Delivery: merge-to-main + delete work branch. Runner: existing `run-*-tests.sh` + new unit cases + QA script smoke.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos" (backlog from audits).
- S2: Audit: SharedPersistenceStore load swallows IOException then flush can wipe file; `Files.writeString` not atomic; invalid rows dropped silently.
- S3: Persist with temp file + atomic replace; fail-closed if load fails (do not flush empty over a corrupt/unreadable store without explicit operator recovery).
- S4: Audit: CombatAuthorityRules / InterestManagementRules return true when position missing (fail-open).
- S5: Without valid MOVE samples for both parties: **reject** HIT; interest **deny** relay (party exception unchanged).
- S6: Audit: authToken empty leaves UDP open; `PlayerSession.authenticated = true` by default.
- S7: Production profile: refuse start (or refuse gameplay) when `requireAuth=true` / non-empty token policy; LAN/dev remains explicit opt-out. Document in `server.properties.example`.
- S8: Audit: username only trim/lower on server — validate `[A-Za-z0-9_]{2,16}` (or agreed pattern) server-side before session create.
- S9: Audit: `tools/qa-session` copies `config.xml.bak` with raw AuthToken into zipable session folder — redact bak (and any copied secrets).
- S10: Risk: security, data (persistence), contracts (stricter rejects), delivery (config flags). Does not replace F10 economy ops or F12 handshake.

Acceptance: unit tests for atomic write + fail-closed load; HIT/interest fail-closed tests; username reject tests; QA bak has no plaintext token; docs updated; dual-client residual operator-owned.

## Tasks

- [ ] T1 (S2, S3) — Atomic persistence write + fail-closed load policy — route: inline — status: pending — commit: —
- [ ] T2 (S4, S5) — Fail-closed HIT range + interest without positions — route: inline — status: pending — commit: —
- [ ] T3 (S6, S7) — Production auth gate + properties/docs — route: inline — status: pending — commit: —
- [ ] T4 (S8) — Server username charset/length validation — route: inline — status: pending — commit: —
- [ ] T5 (S9) — Redact AuthToken in QA bak / session copies — route: inline — status: pending — commit: —
- [ ] T6 (S1, S10) — Unit tests + hosting notes + risk row updates — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Feature doc created from audit P0 (auditoría técnica + revisión Escanor68). Tracking only.
