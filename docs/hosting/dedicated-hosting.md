# Dedicated hosting (reachability)

Get friends **outside your LAN** onto the same Witcher Online UDP server. This does **not** add MMORPG world authority — only network reachability.

Official public server (always on, if you do not want to host): `46.62.255.79:40000`  
Upstream guide: [Server setup](https://rejuvenate.gitbook.io/witcheronline/guides/server-setup)

## Quick path

1. Run `WitcherServer` (Nexus **Witcher Online Server** zip, or build from `server/` with **JDK 21+**).
2. Pick reachability: **Tailscale** (friends) or **VPS** (public IP).
3. Each client sets `bin/WitcherOnline/config.xml` → `ServerIP` + `Port` (default `40000`).
4. Load any save in TW3; usernames must be unique.

## How connect works (this repo)

| Piece | Role |
|-------|------|
| `server/src/WitcherServer.java` | UDP listen (default **40000**). Optional CLI port arg, or `port=` in `server.properties` next to the JAR. |
| Optional | `whitelist=true`, `svgEnabled=true` (HTTP status default **40001**). |
| `witcher/bin/WitcherOnline/config.xml` | Client auto-connect: `Username`, `ServerIP`, `Port`. |
| Protocol | UDP only for gameplay. TCP/HTTP is only for optional status SVG/JSON. |

Example client config:

```xml
<?xml version="1.0"?>
<Config>
	<Username>Player</Username>
	<ServerIP>100.x.y.z</ServerIP>
	<Port>40000</Port>
</Config>
```

Files beside the JAR (created/read by the server): `server.properties`, `banned-players.json`, `whitelisted-players.json`.  
See `server/server.properties.example` in this repo.

## Path A — Tailscale (recommended for a small group)

No router port-forward. Works behind CGNAT.

1. Install [Tailscale](https://tailscale.com/) on the **host** PC and on every **player** PC; sign into the same tailnet.
2. On the host, note Tailscale IPv4 (`tailscale ip -4`, e.g. `100.x.y.z`).
3. Start the server on the host (UDP **40000**). Host firewall: allow UDP 40000 from the Tailscale interface if needed.
4. Friends set `ServerIP` to that `100.x.y.z` and `Port` to `40000`.
5. Verify: host console shows joins; players see each other in-world.

**Tradeoff:** everyone installs Tailscale; not a public open server.

## Path B — VPS (always-on / public)

1. Create a small Linux VPS with a **public IPv4**.
2. Install **JDK 21+**. Place `WitcherServer.jar` (and optional `server.properties`) in a folder; run e.g. `java -jar WitcherServer.jar` (or `java -jar WitcherServer.jar 40000`).
3. Open **UDP 40000** on the cloud firewall **and** the OS firewall (`ufw allow 40000/udp`, security group, etc.).
4. Players set `ServerIP` to the VPS public IP and `Port` to `40000`.
5. Optional: enable `svgEnabled=true` and open TCP **40001** only if you want `/status.json` publicly (lock it down if exposed).

**Tradeoff:** monthly cost + you operate the box; no VPN required for players.

## Path C — Home port-forward (optional)

Forward **UDP 40000** to the host LAN IP; friends use your public IP (or DynDNS). Fails under **CGNAT**. Prefer A or B if unsure. See also upstream [Server setup](https://rejuvenate.gitbook.io/witcheronline/guides/server-setup).

## Verification checklist

- [ ] Server log shows `Starting Witcher Online server on *:40000` (or your port).
- [ ] `ss -ulnp | grep 40000` (Linux) or equivalent shows the Java process listening.
- [ ] From another network (or Tailscale peer): UDP reachability to host:port (game join, or a UDP probe).
- [ ] Client `config.xml` `ServerIP`/`Port` match the reachable host.
- [ ] Two distinct usernames join; both appear on the server (`list` / console help).

Local automation helper (bind + config shape only): `docs/hosting/smoke-local.sh`.

## Build from this repository

Requires **JDK 21+** (source uses APIs newer than Java 8). See `server/README.md`.

## Session identity / moderation

Bans, whitelist, kicks, username holds: [session-identity.md](session-identity.md).

## Out of scope (later ODD features)

NPC sync, combat authority, shared persistence, sharding — see `odd/tasks/`.

Program risks: [odd/risks.md](../../odd/risks.md).
