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
- S9: User: "¿lo marca el dueño a mano, o querés que se active solo cuando digan “historia terminada”? solo cuando se diga historia terminada"
- S10: Room mode switches to `contracts` **only** when **“historia terminada”** is declared (not a free manual toggle by the owner to unlock free pick early). Default until then = `story`.
- S11 (assumption): “Sala” = existing **party / session room** (F2 social unit).
- S12 (assumption): Story Geralt = CPC `ENR_PlayerGeralt`; support/free picks use CPC / Custom Player Characters catalog. “Nivel” = the character level already tracked for that player/session (confirm source in T3). Who may declare “historia terminada” (owner-only vs party) = owner-only until product says otherwise.
- S13: Depends on F1, F2, F5. Does not replace F3 NPC AI authority or full quest-graph sync.
- S14: Risk: contracts (mode + role + transfer protocol), security (spoofing story owner / fake “historia terminada” / fake level for transfer), data (persist mode + role + owner).

Acceptance (when implemented):
- Story mode: no story sync without owner Geralt; second owner claim rejected/demoted; support kits constrained; owner may be lower level than supports and still be Geralt.
- Owner disconnect/leave: Geralt seat transfers to highest-level remaining member (not freeze).
- Contracts mode: activates only after “historia terminada”; then free character selection for explore/group contracts.
- Mode switch rules enforced server-side (cannot enter free pick without the declared story-finished transition).

## Tasks

- [x] T1 (S9, S10, S8) — Product refine: mode = only on “historia terminada”; leave = highest-level Geralt transfer — route: inline — status: done — commit: 82db024
- [ ] T2 (S2, S12) — Story-mode species matrix vs CPC types — route: inline — status: pending — commit: —
- [ ] T3 (S3, S4, S6, S8, S10, S14) — Protocol + server: “historia terminada”→contracts, Geralt owner, leave transfer, free-pick — route: inline — status: pending — commit: —
- [ ] T4 (S3, S6, S8, S13) — Client/script: mode gates + Geralt seat handoff on transfer — route: inline — status: pending — commit: —
- [ ] T5 (S1, S5, S7, S9) — Dual-client checklist + unit tests (transfer by level, both modes) — route: inline — status: pending — commit: —

## Log

- L1: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- L2: 2026-10-31 — Feature doc created (F7). Tracking only; implementation not started.
- L3: Assumption S11/S12 (sala, CPC, level source); leave/transfer resolved in L5; mode trigger in L6.
- L4: 2026-10-31 — User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas". Added S5–S6 (contracts/explore free pick); rewrote tasks for dual mode.
- L5: 2026-10-31 — User: "congelamos la historia hasta que otro Geralt reclame, o transferimos al siguiente Geralt si hay uno? se tranfiere el personaje de geralt a la siguiente persona con mayor nivel pero no importa que el dueno de la sala sea de un nivel mas bajo el sera geralt". Decision: transfer (no freeze) to highest-level remaining; owner is always Geralt even if lower level than supports. Specs S3/S7/S8 updated.
- L6: 2026-10-31 — User: "¿lo marca el dueño a mano, o querés que se active solo cuando digan “historia terminada”? solo cuando se diga historia terminada". Decision: contracts mode only on declared “historia terminada” (not early manual unlock). T1 closed.
