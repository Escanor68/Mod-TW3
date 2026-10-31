# story-room-characters (F7)

Objective: Two room modes — **Story**: one Geralt owns the room and support species (human/elf/witcher/sorceress) assist; **Post-story / contracts**: free character pick (e.g. Eredin, Emhyr, any CPC) for explore and group contracts. Branch: `feat/story-room-characters`. Delivery: merge-to-main + delete work branch. Runner: server unit tests for role/mode rules + dual-client checklist (operator-owned).

## Specs

- S1: User: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- S2: **Story mode** — non-owners: **humano**, **elfo**, **brujo**, **hechicera** with species-appropriate qualities (not free-for-all).
- S3: **Story mode** — exactly one **Geralt** per room is the **room owner**; owns story continuity (quest/dialog authority).
- S4: **Story mode** — non-Geralt members are **support**; do not replace the owner as story driver.
- S5: User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas"
- S6: **Contracts / explore mode** (main story finished, contracts remain) — **libre elección** of characters (examples: Eredin, Emhyr, or any other available CPC/appearance); use cases: explore, group contracts, and similar post-story play. Geralt-owner rule does **not** apply in this mode.
- S7 (assumption): Room mode is an explicit room/party flag (`story` | `contracts`) set by host/owner or detected later; default for unfinished playthrough = `story`.
- S8 (assumption): “Sala” = existing **party / session room** (F2 social unit).
- S9 (assumption): Story Geralt = CPC `ENR_PlayerGeralt`; support/free picks use CPC / Custom Player Characters catalog.
- S10: Depends on F1, F2, F5. Does not replace F3 NPC AI authority or full quest-graph sync.
- S11: Risk: contracts (mode + role protocol), security (spoofing story owner / fake “contracts mode”), data (persist mode + role).

Acceptance (when implemented):
- Story mode: no story sync without owner Geralt; second owner claim rejected/demoted; support kits constrained.
- Contracts mode: free character selection allowed; group explore/contracts playable without Geralt owner requirement.
- Mode switch rules enforced server-side (cannot silently drop into free pick while story is marked active).

## Tasks

- [ ] T1 (S7, S8) — Product refine: who sets `story` vs `contracts`; owner leave/transfer in story mode — route: inline — status: pending — commit: —
- [ ] T2 (S2, S9) — Story-mode species matrix vs CPC types — route: inline — status: pending — commit: —
- [ ] T3 (S3, S4, S6, S11) — Protocol + server: mode flag, Geralt owner rules, contracts free-pick allowlist — route: inline — status: pending — commit: —
- [ ] T4 (S3, S6, S10) — Client/script: enforce mode gates (story vs free pick) — route: inline — status: pending — commit: —
- [ ] T5 (S1, S5) — Dual-client checklist + unit tests for both modes — route: inline — status: pending — commit: —

## Log

- L1: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- L2: 2026-10-31 — Feature doc created (F7). Tracking only; implementation not started.
- L3: Assumption S8/S9 marked until T1 confirms sala binding and leave/transfer policy.
- L4: 2026-10-31 — User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas". Added S5–S6 (contracts/explore free pick); rewrote tasks for dual mode.
