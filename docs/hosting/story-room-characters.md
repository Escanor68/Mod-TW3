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

## Working assumptions (pending T1)

| Topic | Assumption |
|-------|------------|
| Room | Existing party/session on the dedicated server (F2 social unit) |
| Mode flag | Explicit `story` \| `contracts` (host sets, or later auto-detect) |
| Story owner leave | TBD: transfer to another Geralt, or freeze until reclaim |
| Free pick catalog | CPC / Custom Player Characters appearances (not unlimited custom meshes) |

## Related

- Party world / dialog: [party-world-slice.md](party-world-slice.md)
- Identity: [session-identity.md](session-identity.md)
- Persistence hooks: [shared-persistence.md](shared-persistence.md)
- Risks: [odd/risks.md](../../odd/risks.md)
