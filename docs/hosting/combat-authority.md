# Combat authority (F4)

**Scope (first slice):** server-validated **player-vs-player** hits. Full combat physics, NPC HP authority, and account auth remain out of scope.

## Rules (`CombatAuthorityRules`)

| Rule | Value |
|------|--------|
| Health | Normalized `0..1` on the server (`PlayerSession.authHealth`) |
| Damage clamp | Reject `< 0.01`; cap at `0.35` per accepted hit |
| Cooldown | `200ms` per attacker (`lastAuthHitNanos`) |
| Range | Max `8.0` world units from last `MOVE` samples; if either side has no position yet, range check is skipped (clamp + cooldown still apply) |

## Protocol

### Script → DLL
```
hit <targetUsername> <damage> <hitType>
```
Published while in combat when the local hostile target is a remote player ghost (`maybePublishCombatHitOnTarget`, ~4 Hz max).

### DLL → UDP
```
HIT\t<playerId>\t<username>\t<targetUsername>\t<damage>\t<hitType>
```

### Server
- Accepts opcode `HIT` (min 3 fields).
- Resolves target by normalized username; rejects self-hits and offline targets.
- Applies clamp → cooldown → range → `authHealth`.
- Broadcasts `HITACK` to **all** online endpoints (including attacker and victim).

### UDP → DLL → Script
```
HITACK\t<attackerId>\t<attackerUsername>\t<targetUsername>\t<newHealth>\t<appliedDamage>\t<hitType>\t<attackerUsername>
```
`WO_Poll` opcode **11** → `WO_ApplyHitAck` → `applyCombatHitAck(...)`.

Victim applies server HP to local vitality; all clients refresh remote ghost `health` / `lastHit`.

## Abuse checklist (T4)

- [ ] Damage `0.9` request → applied ≤ `0.35`
- [ ] Spam hits faster than 200ms → extras dropped
- [ ] Hit from >8u (both have MOVE samples) → rejected
- [ ] Hit unknown / self username → no `HITACK`
- [ ] Happy path: A hits B → both see B HP drop via `HITACK`
- [ ] Optional: `MPGhosts_DebugCombat` toast on victim

## Limits

- Does **not** stop local single-player damage animations; it overlays authoritative HP for online PvP targets.
- UDP is still forgeable (R5); this slice only removes trivial client-declared HP for covered hits.
- NPC combat authority is deferred (depends on deeper F3).
