# Story room characters (F7) — design notes

**Status:** ODD tracking only (`odd/tasks/story-room-characters.md`). Not implemented yet.

## Modes

| Mode | When | Characters | Story seat |
|------|------|------------|------------|
| **Story** | Main playthrough still active | Owner = **Geralt**; others = **humano / elfo / brujo / hechicera** (support, species qualities) | Geralt owner required |
| **Contracts / explore** | Main story finished; contracts remain | **Free pick** (e.g. Eredin, Emhyr, any CPC) | No Geralt-owner requirement |

## Intent

- Keep the main story coherent: one Geralt drives dialog/quest continuity; supports assist.
- After the story, unlock free casting for exploring and doing witcher contracts together.

## Story owner rules

- The **room owner is always Geralt** in story mode, even if their level is lower than other members.
- On **owner leave**: transfer the Geralt character / owner seat to the remaining member with the **highest level** (do not freeze the story while someone is still in the room).
- If the room is empty after leave: no owner until a new member joins and claims (edge case).

## Working assumptions (pending T1)

| Topic | Assumption |
|-------|------------|
| Room | Existing party/session on the dedicated server (F2 social unit) |
| Mode flag | Explicit `story` \| `contracts` (host sets, or later auto-detect) — **who sets it still open** |
| Level source | Character level already tracked for that player/session (confirm in impl) |
| Free pick catalog | CPC / Custom Player Characters appearances (not unlimited custom meshes) |

## Related

- Party world / dialog: [party-world-slice.md](party-world-slice.md)
- Identity: [session-identity.md](session-identity.md)
- Persistence hooks: [shared-persistence.md](shared-persistence.md)
- Risks: [odd/risks.md](../../odd/risks.md)
