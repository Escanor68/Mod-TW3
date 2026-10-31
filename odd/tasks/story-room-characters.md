# story-room-characters (F7)

Objective: Two room modes — **Story**: one Geralt owns the room and support species (human/elf/witcher/sorceress) assist; **Post-story / contracts**: free character pick (e.g. Eredin, Emhyr, any CPC) for explore and group contracts. Branch: `feat/story-room-characters`. Delivery: merge-to-main + delete work branch. Runner: server unit tests for role/mode rules + dual-client checklist (operator-owned).

## Specs

- S1: User: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- S2: **Story mode** — non-owners: **humano**, **elfo**, **brujo**, **hechicera** with species-appropriate qualities (not free-for-all).
- S3: **Story mode** — exactly one **Geralt** per room is the **room owner**; owns story continuity (quest/dialog authority). Owner level does **not** need to be the highest in the room — the owner **is** Geralt regardless of being lower level than supports.
- S4: **Story mode** — non-Geralt members are **support**; do not replace the owner as story driver.
- S5: User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas"
- S6: **Contracts / explore mode** (main story finished, contracts remain) — **libre elección** of characters (examples: Eredin, Emhyr, or any other available CPC/appearance); use cases: explore, group contracts, and similar post-story play. Geralt-owner rule does **not** apply in this mode.
- S7: User: "congelamos la historia hasta que otro Geralt reclame, o transferimos al siguiente Geralt si hay uno? se tranfiere el personaje de geralt a la siguiente persona con mayor nivel pero no importa que el dueno de la sala sea de un nivel mas bajo el sera geralt"
- S8: **Story mode — owner leave**: do **not** freeze. Transfer the **Geralt character / owner seat** to the remaining member with the **highest level**. That player becomes room owner and **plays as Geralt**. If the room empties (no remaining members), story has no owner until someone joins and claims (edge case for T1/T3).
- S9 (assumption): Room mode is an explicit room/party flag (`story` | `contracts`) set by host/owner or detected later; default for unfinished playthrough = `story`.
- S10 (assumption): “Sala” = existing **party / session room** (F2 social unit).
- S11 (assumption): Story Geralt = CPC `ENR_PlayerGeralt`; support/free picks use CPC / Custom Player Characters catalog. “Nivel” = the character level already tracked for that player/session (confirm source in T3).
- S12: Depends on F1, F2, F5. Does not replace F3 NPC AI authority or full quest-graph sync.
- S13: Risk: contracts (mode + role + transfer protocol), security (spoofing story owner / fake “contracts mode” / fake level for transfer), data (persist mode + role + owner).

Acceptance (when implemented):
- Story mode: no story sync without owner Geralt; second owner claim rejected/demoted; support kits constrained; owner may be lower level than supports and still be Geralt.
- Owner disconnect/leave: Geralt seat transfers to highest-level remaining member (not freeze).
- Contracts mode: free character selection allowed; group explore/contracts playable without Geralt owner requirement.
- Mode switch rules enforced server-side (cannot silently drop into free pick while story is marked active).

## Tasks

- [ ] T1 (S9, S10) — Product refine: who sets `story` vs `contracts` (leave/transfer = S8 done) — route: inline — status: pending — commit: —
- [ ] T2 (S2, S11) — Story-mode species matrix vs CPC types — route: inline — status: pending — commit: —
- [ ] T3 (S3, S4, S6, S8, S13) — Protocol + server: mode flag, Geralt owner, leave→highest-level transfer, contracts free-pick — route: inline — status: pending — commit: —
- [ ] T4 (S3, S6, S8, S12) — Client/script: mode gates + Geralt seat handoff on transfer — route: inline — status: pending — commit: —
- [ ] T5 (S1, S5, S7) — Dual-client checklist + unit tests (transfer by level, both modes) — route: inline — status: pending — commit: —

## Log

- L1: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- L2: 2026-10-31 — Feature doc created (F7). Tracking only; implementation not started.
- L3: Assumption S10/S11 marked until T1 confirms sala binding; leave/transfer resolved in L5.
- L4: 2026-10-31 — User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas". Added S5–S6 (contracts/explore free pick); rewrote tasks for dual mode.
- L5: 2026-10-31 — User: "congelamos la historia hasta que otro Geralt reclame, o transferimos al siguiente Geralt si hay uno? se tranfiere el personaje de geralt a la siguiente persona con mayor nivel pero no importa que el dueno de la sala sea de un nivel mas bajo el sera geralt". Decision: transfer (no freeze) to highest-level remaining; owner is always Geralt even if lower level than supports. Specs S3/S7/S8 updated.
