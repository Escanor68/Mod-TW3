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

## Game version (Next Gen)

**Target:** *The Witcher 3: Wild Hunt* **Next Gen v4.04+** (Steam / GOG / Epic). Upstream does **not** support Classic or builds older than v4.04 (REDkit June 2024).

| Check | Status |
|-------|--------|
| Upstream requirement | Next Gen **4.04+** ([Getting Started](https://rejuvenate.gitbook.io/witcheronline/guides/getting-started)) |
| Classic / pre-4.04 | Unsupported |
| Required mods | [Community Patch – Shared Imports (Next Gen)](https://rejuvenate.gitbook.io/witcheronline/guides/client-setup/required-mods), [Custom Player Characters](https://www.nexusmods.com/witcher3/mods/5940), [Chill Out (Next Gen 4.04)](https://www.nexusmods.com/witcher3/mods/6402) — install manually, then Script Merger |
| This repo layout | Next Gen style (`mods/` + `dlc/` + `user_config_matrix`); client hooks via **WitcherNativeBridge** (x64) |
| Server (Java) | Game-version agnostic; still **JDK 21+** |
| In-game dual-client proof | Not run in this environment — verify on your 4.04+ install |

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
| Story room characters (story Geralt owner; post-story free pick) | [story-room-characters.md](docs/hosting/story-room-characters.md) — F7 ROLECLAIM / STORYDONE |

**Still local / residual:** full quest graph, full inventory, and deep NPC AI are not fully server-owned. Dual-client in-game checklists are operator-run (`odd/risks.md`).

Public hosts should set `authToken` in `server.properties` and matching `<AuthToken>` in client `config.xml`.

## QA session runner (you + a friend)

Use **`tools/qa-session/`** to start the dedicated server (host), point the client config (friend), and collect logs into a zipable **REPORT**.

| Who | Command |
|-----|---------|
| Host (Linux) | `./tools/qa-session/run-session.sh host` |
| Host (Windows) | `tools\qa-session\run-session.bat host` |
| Friend | `SERVER_IP=<host Tailscale IP> USERNAME=FriendName ./tools/qa-session/run-session.sh client` (or PowerShell equivalents) |

**Different houses:** both install [Tailscale](https://tailscale.com/) on the same tailnet; host runs `host` mode and shares `tailscale ip -4`; friend runs `client` with that IP, then launches TW3. Full steps: [tools/qa-session/README.md](tools/qa-session/README.md). Reachability alternatives (VPS / port-forward): [docs/hosting/dedicated-hosting.md](docs/hosting/dedicated-hosting.md).

Session folders under `qa-sessions/` are gitignored — zip the folder and send the REPORT back; do not commit tokens.

## License

This repository ships under **GPL-3.0** (see [`LICENSE`](LICENSE)). That file was **not** changed in the MMORPG-path / Next Gen README updates; it remains the upstream license already on `main`.
