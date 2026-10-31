# World interest management (F6)

**Scope decision (T1):** **Interest management** (proximity broadcast filter) — not region/shard instances.

## Rules (`InterestManagementRules`)

| Rule | Value |
|------|--------|
| Interest radius | `100` world units (from last `MOVE` samples) |
| Party exception | Same party always relays (F2/F3 cohesion), even if far |
| Missing positions | Fail open — relay until both sides have MOVE samples |
| NPC | Unchanged: still **party-only** (F3) |

## What is filtered

| Opcode | Recipient selection |
|--------|---------------------|
| `MOVE` | Interest (+ party) |
| `UPDATE1A`…`UPDATE4` | Interest (+ party) per publisher |
| `HITACK` | Interest of attacker **plus** always the victim endpoint |
| `NPC` | Party-only (unchanged) |
| `INVLOAD` | Unicast (unchanged) |

## Ops / evidence

Broadcast heartbeat now logs:

- `interestRecipientSum` — sum of interest set sizes per publisher this tick
- `baselineGlobal` — `players²` style upper bound (players × players)
- `interestSkips` — cumulative skipped (publisher,recipient) pairs

Isolated players (no party, far apart) approach **1 recipient per publisher** instead of N.

## Checklist (T4)

- [ ] Unit: `run-interest-management-tests.sh`
- [ ] Two distant non-party clients: no remote ghost / no UPDATE noise from the other
- [ ] Same party far apart: still receive party world / NPC as before
- [ ] Nearby non-party: still see each other
- [ ] Heartbeat shows `interestRecipientSum` ≪ `baselineGlobal` with scattered players

## Limits

- Single process / single map; no shard routing.
- Fail-open before first MOVE can briefly look global.
- Radius is fixed for this slice (not config yet).
