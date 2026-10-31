# Story room characters (F7) — design notes

**Status:** ODD tracking only (`odd/tasks/story-room-characters.md`). Not implemented yet.

## Modes

| Mode | When | Characters | Story seat |
|------|------|------------|------------|
| **Story** | Until “historia terminada” | Owner = **Geralt**; others = **humano / elfo / brujo / hechicera** (support) | Geralt owner required |
| **Contracts / explore** | Only after “historia terminada” is declared | **Free pick** (e.g. Eredin, Emhyr, any CPC) | No Geralt-owner requirement |

Mode does **not** switch because the owner toggles it early — only when the room declares **historia terminada**.

## Intent

- Keep the main story coherent: one Geralt drives dialog/quest continuity; supports assist.
- After the story is declared finished, unlock free casting for exploring and group contracts.

## Story owner rules

- The **room owner is always Geralt** in story mode, even if their level is lower than other members.
- On **owner leave**: transfer the Geralt character / owner seat to the remaining member with the **highest level** (do not freeze while someone remains).
- If the room is empty after leave: no owner until a new member joins and claims (edge case).

## Working assumptions (impl)

| Topic | Assumption |
|-------|------------|
| Room | Existing party/session on the dedicated server (F2 social unit) |
| Who declares “historia terminada” | Room owner (Geralt) until product says otherwise |
| Level source | Character level already tracked for that player/session |
| Free pick catalog | CPC / Custom Player Characters appearances |

## Related

- Party world / dialog: [party-world-slice.md](party-world-slice.md)
- Identity: [session-identity.md](session-identity.md)
- Persistence hooks: [shared-persistence.md](shared-persistence.md)
- Risks: [odd/risks.md](../../odd/risks.md)
