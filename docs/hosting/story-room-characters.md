# Story room characters (F7)

**Status:** Server rules + protocol implemented. Dual-client QA is operator-owned.

## Modes

| Mode | When | Characters | Story seat | Contracts |
|------|------|------------|------------|-----------|
| **Story** | Until owner declares “historia terminada” | Owner = **GERALT**; support = **HUMAN / ELF / WITCHER / SORCERESS** | Geralt owner required | May progress mid-story (cast stays story) |
| **Contracts / explore** | Only after `STORYDONE` | Free pick (any species token, e.g. Eredin/Emhyr/CPC#) | No Geralt-owner requirement | Leftover contracts + explore as a group |

## Species ↔ CPC matrix (story)

| F7 species | CPC name (scripts) | Notes |
|------------|--------------------|-------|
| GERALT | `ENR_PlayerGeralt` | Room owner only |
| WITCHER | `ENR_PlayerWitcher` | Support |
| SORCERESS | `ENR_PlayerSorceress` | Support |
| HUMAN | `ENR_PlayerHuman` | Support — confirm ordinal in your CPC pack |
| ELF | `ENR_PlayerElf` | Support — confirm ordinal in your CPC pack |

Free-pick (contracts mode) accepts any non-empty token. Client maps unknown CPC types to `CPC` + ordinal.

## Owner leave

Transfer Geralt seat to the remaining party member with the **highest reported level** (tie → username ascending). Owner may be lower level than supports while seated.

## Protocol (script → DLL → UDP)

```
roleclaim <SPECIES> [level]
storydone
roomstatereq
```

Opcodes: `ROLECLAIM`, `STORYDONE`, `ROOMSTATEREQ`.

### Server → client

```
ROOMSTATE	<playerId>	<username>	<STORY|CONTRACTS>	<ownerUsername>	<species>	<level>
```

Inbound opcode **13** → `WO_ApplyRoomState`.

Soft errors (session stays up): `ERROR ROLE_INVALID`, `ROLE_SPECIES`, `ROLE_OWNER_TAKEN`, `STORYDONE_DENIED`.

## Client helpers (`client.ws`)

- `claimRoomRole(level)` — maps current CPC → species and sends `roleclaim`
- `declareHistoriaTerminada()` — owner-only `storydone`
- `requestRoomState()` / `isContractsRoomMode()`

## Related

- Party: [party-world-slice.md](party-world-slice.md)
- Identity: [session-identity.md](session-identity.md)
- Persistence: [shared-persistence.md](shared-persistence.md)
- ODD: [odd/tasks/story-room-characters.md](../../odd/tasks/story-room-characters.md)
- Tests: `server/scripts/run-room-character-tests.sh`
