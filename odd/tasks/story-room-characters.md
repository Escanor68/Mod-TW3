# story-room-characters (F7)

Objective: Define online character roles so players build species-faithful loadouts (human, elf, witcher, sorceress) while **one Geralt owns the room** and carries the main story; others play as support. Branch: `feat/story-room-characters`. Delivery: merge-to-main + delete work branch (per current repo convention). Runner: server unit tests for role rules + dual-client checklist (operator-owned).

## Specs

- S1: User: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- S2: Allowed species/roles for non-owners: **humano**, **elfo**, **brujo**, **hechicera** — each with species-appropriate qualities (appearance / CPC type / allowed tools), not a free-for-all.
- S3: Exactly one **Geralt** per room is the **room owner** (`dueño de la sala`); that seat owns story continuity (quest/dialog authority path).
- S4: Non-Geralt members are **support** — present in the shared session, assist gameplay, do **not** replace the owner as story driver.
- S5 (assumption): “Sala” maps to the existing **party / session room** on the dedicated server (same social unit as F2), not a separate MMO shard — revisit if product wants global lobbies.
- S6 (assumption): Geralt owner aligns with CPC `ENR_PlayerGeralt` (and story/dialog leader from F2); support species map onto CPC / Custom Player Characters types where they already exist.
- S7: Depends on F1 (identity), F2 (party world/dialog), F5 (persistence hooks for role). Does not replace F3 NPC authority or full quest graph sync.
- S8: Risk: contracts (role/ownership protocol), security (spoofing owner), data (persisting role per username).

Acceptance (when implemented): a room cannot start story sync without an owner Geralt; a second Geralt claiming owner is rejected or demoted to support; support species are constrained to their species kit; owner leave transfers or freezes story per agreed rule.

## Tasks

- [ ] T1 (S1, S5, S6) — Product refine: sala = party vs lobby; owner leave/transfer rules — route: inline — status: pending — commit: —
- [ ] T2 (S2, S6) — Species quality matrix (humano / elfo / brujo / hechicera) vs CPC types — route: inline — status: pending — commit: —
- [ ] T3 (S3, S4, S8) — Protocol + server rules: claim/validate owner Geralt + support roles — route: inline — status: pending — commit: —
- [ ] T4 (S3, S7) — Client/script: enforce owner story seat + support loadout gates — route: inline — status: pending — commit: —
- [ ] T5 (S1) — Dual-client checklist + unit tests for role rules — route: inline — status: pending — commit: —

## Log

- L1: "podes agregar otro odd que sea personajes, armarse uno pero siguiendo las cualidades de su especie sea humano, elfo, brujo o hechicera pero tiene que haber un geralt que seria el dueno de la sala para que la historia pueda seguir y el resto como apoyo"
- L2: 2026-10-31 — Feature doc created (F7). Tracking only; implementation not started.
- L3: Assumption S5/S6 marked until T1 confirms sala binding and leave/transfer policy.
