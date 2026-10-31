# shared-persistence (F5)

Objective: Persist online character-relevant state on the server (inventory/progress slice) so the character is not only a local save ghost. Branch: `feat/shared-persistence`. Delivery: merge-to-main + delete work branch. Runner: `server/scripts/run-shared-persistence-tests.sh` + dual-client reconnect checklist.

## Specs

- S1: Move toward true online RPG identity of character data under "llevar a un mmorpg al juego".
- S2: First slice: server-side **crowns + key-item bag** tied to F1 username — NOT full quest graph migration.
- S3: Local TW3 saves remain; **server wins** on mismatch for the covered slice.
- S4: Risk: data/persistence format, security (item duping), contracts (INVSAVE/INVLOAD protocol).
- S5: Depends on F1; trading features should use server inventory once this lands.

Acceptance: disconnect/reconnect restores the agreed persisted slice from server; duping path for covered items fails. Checks: persistence round-trip + one abuse case.

## Tasks

- [x] T1 (S2, S3) — Product decision: which fields persist + conflict rules (blocking) — route: inline — status: done — commit: a5e0f65
  - User confirmed: crowns + key bag; server wins; overwrite anti-dupe ("si, la cerramos con esa regla").
- [x] T2 (S2, S4) — Storage choice + schema/migration plan — route: inline — status: done — commit: a5e0f65
  - `player-snapshots.tsv` via `SharedPersistenceStore` (no DB).
- [x] T3 (S2, S4) — Load/save protocol + server APIs — route: inline — status: done — commit: a5e0f65
  - `INVSAVE` / `INVLOADREQ` / `INVLOAD` + script/DLL apply path.
- [ ] T4 (S2) — Reconnect + anti-dupe verification — route: inline — status: partial — commit: a5e0f65 (unit); dual-client open
  - Unit tests green; dual-client checklist in `docs/hosting/shared-persistence.md` (operator-owned / R12).

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F5. Implementation not authorized yet; tracking only.
- L3: 2026-10-31 — User: F5 then F6 on separate branches; when finished push to main and delete work branch.
- L4: 2026-10-31 — Recommended crowns+key bag / server-wins; user: "si, la cerramos con esa regla".
- L5: Risk: item 1 (persisted snapshot format); item 2 (anti-dupe surface); item 3 (INV* contract). Item 6: unit tests; in-game T4 open.
