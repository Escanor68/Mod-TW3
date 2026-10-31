# party-membership (F9)

Objective: Server-authoritative party membership (invite/accept/leave) so clients cannot spoof `joinedParty` to bypass interest or hijack room scope. Branch: `feat/party-membership`. Delivery: merge-to-main + delete work branch. Runner: new party-membership unit tests + dual-client checklist.

## Specs

- S1: User: "arma todos los odd para despues seguir implementandolos"
- S2: Audit: `PlayerSession.joinedParty` / `inParty` come from client UPDATE4; `PartyScopeRules.sameParty` trusts them — spoof bypasses interest radius and party NPC/room keys.
- S3: Membership changes only via server-validated opcodes (e.g. `PARTYINVITE` / `PARTYACCEPT` / `PARTYLEAVE` / `PARTYKICK`); UPDATE4 party fields ignored or treated as cosmetic until confirmed.
- S4: Leader/owner rules aligned with F2 dialog seat and F7 story owner where applicable; document conflicts.
- S5: On disconnect: remove from party; transfer leader if needed (policy explicit in T1).
- S6: Depends on F2, F6, F8 (auth/username). Risk: contracts, security, concurrency.
- S7: Assumption: first slice = in-memory party graph keyed by leader username; persistence of party OUT unless authorized later.

Acceptance: forged UPDATE4 cannot join another player’s interest/party relays; invite→accept path works; unit tests cover spoof reject.

## Tasks

- [ ] T1 (S3, S5) — Product refine: invite flow + leader transfer on leave — route: inline — status: pending — commit: —
- [ ] T2 (S3, S6) — Protocol + server party graph + ignore spoofed UPDATE4 membership — route: inline — status: pending — commit: —
- [ ] T3 (S4) — Wire F3/F6/F7 partyKey to authoritative membership — route: inline — status: pending — commit: —
- [ ] T4 (S3) — Client DLL + script helpers — route: inline — status: pending — commit: —
- [ ] T5 (S1, S7) — Unit tests + dual-client checklist — route: inline — status: pending — commit: —

## Log

- L1: "arma todos los odd para despues seguir implementandolos"
- L2: 2026-10-31 — Tracking only from audit (party autodeclarado).
