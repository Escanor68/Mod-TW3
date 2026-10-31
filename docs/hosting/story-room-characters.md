# Story room characters (F7) — design notes

**Status:** ODD tracking only (`odd/tasks/story-room-characters.md`). Not implemented yet.

## Intent

- One **Geralt** = **room owner** so the main story can continue (dialog / quest seat).
- Others build a character as **humano / elfo / brujo / hechicera** with species-appropriate qualities, as **support**.

## Working assumptions (pending T1)

| Topic | Assumption |
|-------|------------|
| Room | Existing party/session on the dedicated server (F2 social unit) |
| Owner | CPC Geralt; sole story authority for the room |
| Support | Non-owner species kits; cannot claim owner while another Geralt holds the seat |
| Owner leave | TBD in T1: transfer to another Geralt, or freeze story until a Geralt reclaims |

## Related

- Party world / dialog: [party-world-slice.md](party-world-slice.md)
- Identity: [session-identity.md](session-identity.md)
- Persistence hooks: [shared-persistence.md](shared-persistence.md)
- Risks: [odd/risks.md](../../odd/risks.md)
