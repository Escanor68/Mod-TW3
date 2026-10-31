# shared-persistence (F5)

Objective: Persist online character-relevant state on the server (inventory/progress slice) so the character is not only a local save ghost. Branch: `feat/shared-persistence`. Delivery: `ask-on-risk`. Runner: server integration checks + dual-client reconnect; DB migrations if introduced are high-risk.

## Specs

- S1: Move toward true online RPG identity of character data under "llevar a un mmorpg al juego".
- S2: First slice (assumption): server-side inventory and/or key progress flags tied to F1 identity — NOT full quest graph migration.
- S3: Local TW3 saves remain; define clear conflict rules (what wins on mismatch) before coding.
- S4: Risk: data/persistence format, security (item duping), contracts (login/load protocol).
- S5: Depends on F1; trading features should use server inventory once this lands.

Acceptance: disconnect/reconnect restores the agreed persisted slice from server; duping path for covered items fails. Checks: persistence round-trip + one abuse case.

## Tasks

- [ ] T1 (S2, S3) — Product decision: which fields persist + conflict rules (blocking) — route: inline — status: pending — commit: —
- [ ] T2 (S2, S4) — Storage choice + schema/migration plan — route: inline — status: pending — commit: —
- [ ] T3 (S2, S4) — Load/save protocol + server APIs — route: inline — status: pending — commit: —
- [ ] T4 (S2) — Reconnect + anti-dupe verification — route: inline — status: pending — commit: —

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F5. Implementation not authorized yet; tracking only.
