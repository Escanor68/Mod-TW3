# Witcher Online
<div align="left">
  <a href="https://www.youtube.com/watch?v=E36I77ardm4">
    <img
      src="https://github.com/user-attachments/assets/2f2c39ab-7d48-4095-8a89-56fbede324ef"
    >
  </a>
</div>

**Witcher Online** is a mod that adds multiplayer to *The Witcher 3: Wild Hunt*. Join servers with friends, customize characters with [Custom Player Characters](https://www.nexusmods.com/witcher3/mods/5940), relax with [Chill Out](https://www.nexusmods.com/witcher3/mods/6402), play Gwent, party up, chat, and emote.

This repository (`main`) carries the WitcherOnline line plus an **MMORPG-path** slice (hosting, identity, party world, NPC presence, combat authority, shared persistence, interest management). Tracking: `odd/tasks/`, risks: `odd/risks.md`.

## Quick path

1. Playable build: [Nexus — Witcher Online](https://www.nexusmods.com/witcher3/mods/11590).
2. Docs / wiki: [Witcher Online Wiki](https://rejuvenate.gitbook.io/witcheronline).
3. Host friends outside your LAN: [docs/hosting/dedicated-hosting.md](docs/hosting/dedicated-hosting.md) (Tailscale or VPS).
4. Build the Java server from source: [server/README.md](server/README.md) (**JDK 21+**).

Special thanks to Werasik2aa for [Witcher3-Multiplayer](https://github.com/werasik2aa/Witcher3-Multiplayer).

## Features / Playguide

Full movement, combat, and appearance sync. Emotes and chat via player menu or console. Morphs, horses, boats, Gwent (Timed/Normal + bets), parties (time / weather / dialog).

[Features](https://rejuvenate.gitbook.io/witcheronline/general-information/features) · [Playguide](https://rejuvenate.gitbook.io/witcheronline/general-information/playguide)

### MMO Experience

Join the public server to wander the world with other players.

![MMO Experience](assets/mmo.gif)

### Chill Out
Sit down in taverns and other locations throughout the world.

![Chill Out](assets/chill.gif)

### Party System
Join a party to synchronize time, weather, and dialog choices.

![Party System](assets/world.gif)

![Party System 2](assets/party.gif)

### Cutscene Sync
Party leader dialog choices apply to party members.

![Cutscene Sync](assets/dialog.gif)

### Gwent Duels

Play Gwent with other players using your decks.

![Gwent Duels](assets/gwent.gif)

### Player Riding

Ride along while another player drives.

![Player Riding](assets/riding.gif)

### Vehicle Passengers

Share horses and boats.

![Vehicle Passengers Horse](assets/horse.gif)

![Vehicle Passengers Boat](assets/boat.gif)

### Item Trading

Trade items with other players for a price.

![Item Trading](assets/trading.gif)

### Morphs

Transform into an owl, crow, fox, or cat.

![Morphs](assets/morphs.gif)

### Emotes

50+ player emotes, some with props.

![Emotes](assets/emotes.gif)

## Shared online slice (this `main`)

Operator / developer notes under `docs/hosting/`:

| Topic | Doc |
|-------|-----|
| Reachability (Tailscale / VPS) | [dedicated-hosting.md](docs/hosting/dedicated-hosting.md) |
| Bans / whitelist / kicks | [session-identity.md](docs/hosting/session-identity.md) |
| Party time / weather / dialog | [party-world-slice.md](docs/hosting/party-world-slice.md) |
| Party NPC presence ghosts | [npc-presence-sync.md](docs/hosting/npc-presence-sync.md) |
| Server-validated PvP hits | [combat-authority.md](docs/hosting/combat-authority.md) |
| Crowns + key-item bag persistence | [shared-persistence.md](docs/hosting/shared-persistence.md) |
| Proximity interest broadcasts | [world-instances.md](docs/hosting/world-instances.md) |

**Still local / residual:** full quest graph, full inventory, and deep NPC AI are not fully server-owned. Dual-client in-game checklists are operator-run (`odd/risks.md`).

Public hosts should set `authToken` in `server.properties` and matching `<AuthToken>` in client `config.xml`.

## Socials

- [Discord](https://discord.gg/AGXXvGNnH8)
- [Wolven Workshop](https://discord.com/invite/RzkhYYr7fk) (`#rejuvenate-rock-farm`)
- [Wiki](https://rejuvenate.gitbook.io/witcheronline)
- [YouTube](https://www.youtube.com/@rejuvenate7/videos)
- [Donate](https://ko-fi.com/rejuvenate)

## Credits
- [rejuvenate7](https://github.com/rejuvenate7) — Lead developer
- [x4lva](https://github.com/x4lva) — Co-developer, Gwent sync
- [Flawkee](https://github.com/flawkee) — Co-developer, Native hook / NPC sync
