# story-room-characters (F7)

Objective: Two room modes — **Story**: Geralt owner + support species; contracts may be done **during** the story under those cast rules. **Post-story**: after “historia terminada”, unfinished main-story contracts (and explore) are group-completable with **free character pick**. Branch: `feat/story-room-characters`. Delivery: merge-to-main + delete work branch. Runner: `server/scripts/run-room-character-tests.sh` + dual-client checklist (operator-owned).

## Specs

- S1: User: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- S2: **Story mode** — non-owners: **humano**, **elfo**, **brujo**, **hechicera** with species-appropriate qualities (not free-for-all).
- S3: **Story mode** — exactly one **Geralt** per room is the **room owner**; owns story continuity (quest/dialog authority). Owner level does **not** need to be the highest in the room — the owner **is** Geralt regardless of being lower level than supports.
- S4: **Story mode** — non-Geralt members are **support**; do not replace the owner as story driver.
- S5: User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas"
- S6: **Contracts / explore mode** (after “historia terminada”) — **libre elección** of characters (examples: Eredin, Emhyr, or any other available CPC/appearance); each player may be any character. Use cases: explore and complete remaining contracts as a group. Geralt-owner rule does **not** apply in this mode.
- S7: User: "congelamos la historia hasta que otro Geralt reclame, o transferimos al siguiente Geralt si hay uno? se tranfiere el personaje de geralt a la siguiente persona con mayor nivel pero no importa que el dueno de la sala sea de un nivel mas bajo el sera geralt"
- S8: **Story mode — owner leave**: do **not** freeze. Transfer the **Geralt character / owner seat** to the remaining member with the **highest level**. That player becomes room owner and **plays as Geralt**. If the room empties (no remaining members), story has no owner until someone joins and claims (edge case for T1/T3).
- S9: User: "¿lo marca el dueño a mano, o querés que se active solo cuando digan “historia terminada”? solo cuando se diga historia terminada"
- S10: Room mode switches to `contracts` **only** when **“historia terminada”** is declared (not a free manual toggle by the owner to unlock free pick early). Default until then = `story`.
- S11 (assumption): “Sala” = existing **party / session room** (F2 social unit).
- S12: Story Geralt = CPC `ENR_PlayerGeralt`; support map HUMAN/ELF/WITCHER/SORCERESS → CPC names (HUMAN/ELF ordinals depend on CPC pack). Level = `ROLECLAIM` field (client-reported). Who may declare “historia terminada” = room owner.
- S13: Depends on F1, F2, F5. Does not replace F3 NPC AI authority or full quest-graph sync.
- S14: Risk: contracts (mode + role + transfer protocol), security (spoofing story owner / fake “historia terminada” / fake level for transfer), data (persist mode + role + owner).
- S15: User: "obviamente los contratos se puede ir completando tambien a lo largo de la historia pero los contratos que no se terminaron dentro de la historia principal eso ya pueden ser completados por el grupo de jugadores y cada jugar puede ser cualquier personaje"
- S16: **Contracts during story**: witcher contracts may be progressed/completed **while still in story mode**, under story cast rules (Geralt owner + support species). Completing contracts mid-story does **not** by itself flip the room to free-pick mode.
- S17: **Leftover contracts after main story**: contracts **not finished during the main story** remain available after “historia terminada” for the **player group** to complete; in that mode **each player may be any character**.

Acceptance (when implemented):
- Story mode: no story sync without owner Geralt; second owner claim rejected/demoted; support kits constrained; owner may be lower level than supports and still be Geralt; contracts playable mid-story without unlocking free pick.
- Owner disconnect/leave: Geralt seat transfers to highest-level remaining member (not freeze).
- Contracts mode: activates only after “historia terminada”; leftover unfinished contracts + explore are group-playable with free character selection per player.
- Mode switch rules enforced server-side (cannot enter free pick without the declared story-finished transition).

## Tasks

- [x] T1 (S9, S10, S8) — Product refine: mode = only on “historia terminada”; leave = highest-level Geralt transfer — route: inline — status: done — commit: 098e4d2
- [x] T2 (S2, S12) — Story-mode species matrix vs CPC types — route: inline — status: done — commit: c7e7968
- [x] T3 (S3, S4, S6, S8, S10, S14, S16, S17) — Protocol + server: “historia terminada”→contracts, Geralt owner, leave transfer, free-pick — route: inline — status: done — commit: c7e7968
- [x] T4 (S3, S6, S8, S13, S16, S17) — Client/script: mode gates + Geralt handoff + contract play in both modes — route: inline — status: done — commit: c7e7968
- [x] T5 (S1, S5, S7, S9, S15) — Unit tests for rules; dual-client checklist operator-owned — route: inline — status: done (unit) / pending (dual-client) — commit: c7e7968

## Log

- L1: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- L2: 2026-10-31 — Feature doc created (F7). Tracking only; implementation not started.
- L3: Assumption S11/S12 (sala, CPC, level source); leave/transfer resolved in L5; mode trigger in L6.
- L4: 2026-10-31 — User: "en el caso del que juego ya se haya termina y solo quedan los contratos ahi si se puede hacer una libre eleccion de los personajes cualquira como eredin o emhyr o cualquier otro para explorar, hacer contratos en grupo o demas". Added S5–S6 (contracts/explore free pick); rewrote tasks for dual mode.
- L5: 2026-10-31 — User: "congelamos la historia hasta que otro Geralt reclame, o transferimos al siguiente Geralt si hay uno? se tranfiere el personaje de geralt a la siguiente persona con mayor nivel pero no importa que el dueno de la sala sea de un nivel mas bajo el sera geralt". Decision: transfer (no freeze) to highest-level remaining; owner is always Geralt even if lower level than supports. Specs S3/S7/S8 updated.
- L6: 2026-10-31 — User: "¿lo marca el dueño a mano, o querés que se active solo cuando digan “historia terminada”? solo cuando se diga historia terminada". Decision: contracts mode only on declared “historia terminada” (not early manual unlock). T1 closed.
- L7: 2026-10-31 — User: "obviamente los contratos se puede ir completando tambien a lo largo de la historia pero los contratos que no se terminaron dentro de la historia principal eso ya pueden ser completados por el grupo de jugadores y cada jugar puede ser cualquier personaje". Specs S15–S17: mid-story contracts under story cast; leftovers after main story = group + any character.
- L8: 2026-10-31 — User authorized implementation. Implemented RoomCharacterRules + WitcherServer ROLECLAIM/STORYDONE/ROOMSTATE, client DLL+script helpers, unit tests PASS.
