# Session identity & moderation (F1)

Operator notes for bans, whitelist, kicks, and username holds on `WitcherServer`.

## Quick path

1. Start the server (JDK 21+ / Nexus JAR). Console shows `Session identity: bans=…`.
2. Use console commands below.
3. Clients still identify only by `Username` in `config.xml` (no passwords in F1).

## Behavior map

| Mechanism | What it does | Persistence |
|-----------|--------------|-------------|
| Username key | Case-insensitive trim (`Alice` = `alice`) | In-memory session map |
| Username hold | After timeout (~60s), name reserved to **previous IP** | In-memory until expiry |
| Ban | Blocks UDP from IP; kicks matching players | `banned-players.json` |
| Whitelist | When enabled, only listed IPs join | IPs → `whitelisted-players.json`; **on/off → `server.properties` `whitelist=`** |
| Kick | Disconnect + notice; no ban | Ephemeral |

Errors clients may see: `ERROR BANNED`, `ERROR NOT_WHITELISTED`, `ERROR USERNAME_TAKEN`, `ERROR INVALID_USERNAME`.

**Limit (R3):** bans are **IP-based**. A banned player on a new IP can reconnect with the same display name.

## Console commands

| Command | Effect |
|---------|--------|
| `kick <name\|ip>` | Disconnect player |
| `ban <name\|ip>` | Ban IP (+ kick if online) |
| `unban <ip>` | Remove ban |
| `bans` | List banned IPs |
| `whitelist on\|off` | Toggle + **persist** to `server.properties` |
| `whitelist <ip>` | Add IP |
| `whitelist remove <ip>` | Remove IP |
| `whitelist` / `list` / `status` | Show flag + IPs |
| `reserved` | Show username holds |
| `list` / `stats` | Players / counters |

## Verification matrix

| Case | Expected |
|------|----------|
| Two clients same username, different IP | Second gets `USERNAME_TAKEN` while first online |
| Soft timeout then same IP reconnect | Name restored from reservation |
| Soft timeout then other IP within hold | `USERNAME_TAKEN` |
| `ban` online player | Kick + IP in `bans`; further packets `BANNED` |
| `whitelist on` without IP | Client `NOT_WHITELISTED` |
| `whitelist on` + listed IP | Join OK; survives **server restart** (flag in properties) |
| Restart after `whitelist off` | Stays off (persisted) |

Automated (rules only): `server/scripts/run-identity-tests.sh`.

## Related

- Reachability: [dedicated-hosting.md](dedicated-hosting.md)
- Risks: [odd/risks.md](../../odd/risks.md) (R3, R4, R5)
