# session-identity (F1)

Objective: Strengthen player identity and session controls on the dedicated server (names, bans, whitelist) as the social base for later MMO features. Branch: `feat/session-identity`. Delivery: `ask-on-risk`. Runner: `server/scripts/run-identity-tests.sh` + manual console matrix in docs.

## Specs

- S1: Support the path toward "llevar a un mmorpg al juego" with durable player identity on the server (not full accounts/DB yet unless explicitly expanded later).
- S2: Build on existing server concepts: username reservation, ban list, whitelist (`WitcherServer.java`).
- S3: Operators can ban/kick/whitelist with clear console feedback; reserved names hold across brief reconnects (existing hold semantics preserved or improved with explicit tests).
- S4 (assumption): Full account login (password/OAuth) is OUT of F1 unless the user later authorizes it; F1 = reliable session identity + moderation hooks.
- S5: Depends on F0 operationally (a reachable server) but code may land in parallel after F0 runbook exists.

Acceptance: reconnect with same username reattaches predictably; banned IP/user cannot join; whitelist mode documented and verified. Checks: server console commands + join/reject observations.

## Tasks

- [x] T1 (S2) — Map current ban/whitelist/username reservation behavior — route: inline — status: done — commit: 96d9092
- [x] T2 (S1, S3) — Spec gaps vs desired operator UX; implement minimal hardening — route: inline — status: done — commit: 96d9092
  - Fixed whitelist already-in-list log bug; persist whitelist on/off; `bans` / `reserved` / `whitelist list`; `SessionIdentityRules` + startup identity summary.
- [x] T3 (S3, S4) — Verification matrix (ban, whitelist, name hold, kick) — route: inline — status: done — commit: 96d9092
  - Docs matrix + automated rules tests. Live UDP join matrix remains operator (needs running game clients).
- [x] T4 (S5) — Operator notes linked from F0 runbook — route: inline — status: done — commit: 96d9092

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F1. Implementation not authorized yet; tracking only.
- L3: Program intent: "con la base de este mod lo podemos llevar a un mmorpg al juego"
- L4: 2026-10-31 — User: "si hazlo, despues en la carpeta de odd armamos uno de riesgo y lo vamos completando y despues de terminado resolviendo"
- L5: 2026-10-31 — Created `odd/risks.md`; F1 hardening + docs. R4 resolved, R3 accepted. Risk: item 3 (console/protocol operator contracts + new helpers); item 6 covered by `run-identity-tests.sh` for extracted rules.
