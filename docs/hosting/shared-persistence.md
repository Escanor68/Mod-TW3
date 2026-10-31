# Shared persistence (F5)

**Scope decision (T1):** Persist **crowns/orens** + a small **key-item bag** (alchemy allowlist, max 12 stacks).  
**Conflict rule:** **server wins** on reconnect for the covered slice. Local TW3 save still owns everything else.  
**Anti-dupe:** replace quantities for covered names (never additive merge).

## Storage

File next to server data: `player-snapshots.tsv`

```
# usernameKey	crowns	bagBlob	updatedAtMs
alice	1234	Swallow=2;White+Honey=1	1710000000000
```

Spaces in item names use `+` on the wire (`White Honey` → `White+Honey`).

## Protocol

### Script → DLL → UDP
```
invloadreq
invsave <crowns> <bagBlob>
```

Opcodes: `INVLOADREQ`, `INVSAVE` (fields: crowns, bagBlob).

### Server → client (unicast)
```
INVLOAD	<playerId>	<username>	<status>	<crowns>	<bagBlob>
```

| status | Meaning |
|--------|---------|
| `ok` | Apply crowns + bag (server wins) |
| `missing` | No snapshot yet — keep local; next `INVSAVE` seeds the server |

`WO_Poll` opcode **12** → `WO_ApplyInvLoad` → `applySharedPersistenceLoad(...)`.

## Client behavior

1. Shortly after in-game, send `invloadreq`.
2. On `ok`, force-set money and covered item quantities.
3. On `missing`, keep local and save soon.
4. Periodic `invsave` (~15s) after load applied.

Key items (first slice allowlist): Swallow, White Honey, Thunderbolt, Tawny Owl, Blizzard, Petri Philter, Full Moon, Cat, Golden Oriole, Black Blood, Maribor Forest, Killer Whale.

## Abuse checklist (T4)

- [ ] Unit: bag parse overwrites same-name qty (no sum) — `run-shared-persistence-tests.sh`
- [ ] Reconnect with prior snapshot → crowns/items match server
- [ ] First join (`missing`) → local kept; after save, second join loads server
- [ ] Inflated local Swallow while server has 2 → after load, qty is 2
- [ ] Optional: `MPGhosts_DebugPersist` toast on load

## Limits

- Not full inventory / quest graph.
- UDP still forgeable (R5); username-bound file is the trust boundary for this slice.
- Item names outside the allowlist are not covered.

## Residual risk (R8)

Only crowns + allowlisted key items are server-authoritative. Other inventory can still diverge via local saves. Residual **accepted** for F5.
