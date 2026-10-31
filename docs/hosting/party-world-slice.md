# Party world slice (F2)

Party-scoped sync of **time**, **weather**, and **cutscene dialog choices**.  
This is **not** global world authority and **not** NPC sync (see F3).

Upstream playguide: [Playguide](https://rejuvenate.gitbook.io/witcheronline/general-information/playguide)

## Quick path

1. Two+ players on the same server (`config.xml` ServerIP/Port).
2. Followers join the **party leader** (player menu / console join party).
3. Leader’s game time + weather apply to members; leader’s dialog picks apply in cutscenes when close enough.

## Inventory (this repo)

| Piece | Path / symbol | Role |
|-------|----------------|------|
| Party join/leave | `client.ws` `joinParty` / `leaveParty` | Local party membership (`joinedParty` = leader username) |
| World sync tick | `client.ws` `updateWorldSync` | Applies leader time/weather; dialog follow |
| Time/weather apply | `client.ws` `applyPartyWorldTimeWeather` | Shared helper; uses ghost **or** global leader data |
| Outbound fields | `client.ws` `wo_get4` | Sends `inParty`, `joinedParty`, weather, day/hour/min/sec, dialog indexes |
| Inbound fields | `client.ws` `updatePlayerData4` | Stores party + world fields on global **and** ghost player |
| Dialog proximity | `isRemotePlayerCloseForDialogSync` | ~8 m horizontal, ~3 m height |
| Transport | Server UDP UPDATE chunks | Opaque relay (no party logic on Java server) |

## Authority model (S4)

- **Leader** = the username in `joinedParty` for followers.
- Followers pull time/weather from the leader’s last UPDATE4 payload.
- Dialog auto-select follows the leader when choices match and players are in range / in cutscene.

## Reliability notes (F2 hardening)

- Before: time/weather required a **spawned ghost** for the leader; far leaders broke sync.
- After: world fields are stored on the **global** player entry; `updateWorldSync` falls back to that when the ghost is missing. Dialog sync still needs the nearby ghost + proximity.

## Manual acceptance checklist

- [ ] A joins B’s party (notification OK)
- [ ] Change B’s time of day → A converges within ~90s tolerance
- [ ] Change B’s weather → A changes (throttle ~6s)
- [ ] B far away (no ghost) → A still gets time/weather
- [ ] Same cutscene, A near B → B’s dialog choice applies on A
- [ ] Leave party → A stops following B’s time/weather
- [ ] Confirm NPCs are still **not** shared (expected)

## Out of scope

NPC presence, combat authority, shared inventory — `odd/tasks/`.
