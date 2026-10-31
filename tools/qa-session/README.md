# QA session runner (Windows + Linux)

Starts (or prepares) a Mod-TW3 / Witcher Online play session, **logs what happens**, and writes a **REPORT** you can zip and bring back for debugging.

It does **not** launch The Witcher 3 itself (Steam/GOG/Epic owns that). It does:

- build/start the **Java UDP server** (host mode)
- capture environment + server console
- optionally point `config.xml` at the host (client mode)
- write `REPORT.md` + checklist for what you saw in-game

## Quick start

### You host at home (Linux)

```bash
cd /path/to/mod-TheWitcher3
chmod +x tools/qa-session/run-session.sh
./tools/qa-session/run-session.sh host
```

### You host at home (Windows)

```powershell
cd C:\path\to\mod-TheWitcher3
powershell -ExecutionPolicy Bypass -File tools\qa-session\run-session.ps1 host
```

Or double-click / run: `tools\qa-session\run-session.bat host`

### Friend joins from their house (client)

1. Same Tailscale account / tailnet as the host (recommended), **or** host VPS public IP.
2. On their PC (repo clone or at least `witcher/bin/WitcherOnline/config.xml`):

```bash
# Linux
SERVER_IP=100.x.y.z USERNAME=FriendName ./tools/qa-session/run-session.sh client

# Windows PowerShell
$env:SERVER_IP="100.x.y.z"; $env:USERNAME_OVERRIDE="FriendName"
powershell -ExecutionPolicy Bypass -File tools\qa-session\run-session.ps1 client
```

3. Start TW3 Next Gen 4.04+ with Witcher Online mods loaded; load a save.

## How you + a friend play from different houses

| Role | What to run | Network |
|------|-------------|---------|
| **Host** | `run-session … host` → leaves UDP **40000** listening | Install [Tailscale](https://tailscale.com/) on host; note `tailscale ip -4` |
| **Friend** | `run-session … client` with `SERVER_IP=<host Tailscale IP>` | Same Tailscale tailnet; unique `Username` |

Alternative: VPS with public IP + open **UDP 40000** (see [dedicated-hosting.md](../../docs/hosting/dedicated-hosting.md)).  
Home port-forward often fails under CGNAT — prefer Tailscale.

If the host set `authToken` in `server.properties`, both clients need matching `<AuthToken>` in `config.xml`.

## Output folder

Each run creates:

```text
qa-sessions/<timestamp>-<role>/
  environment.txt     # OS, Java, git, IPs
  session.log         # runner actions
  server.log          # host only — Java console
  NOTES.md            # fill while playing
  REPORT.md           # auto summary + your NOTES (generated on stop)
  config.xml.bak      # client mode backup when rewriting ServerIP
```

Zip that folder and send it (do **not** commit it — `qa-sessions/` is gitignored). Redact passwords/tokens if you pasted any into NOTES.

## Requirements

- **Host:** JDK **21+** on `PATH` (or `JAVA_HOME`)
- **Both:** TW3 Next Gen 4.04+, Witcher Online mods + Script Merger (see repo README)
- **Friend reachability:** Tailscale or VPS (documented above)

## Modes

| Mode | Args | Behavior |
|------|------|----------|
| `host` | default | Build `server/out/WitcherServer.jar` if needed, start server, log until you press Enter / Ctrl+C, write REPORT |
| `client` | | Patch `witcher/bin/WitcherOnline/config.xml` ServerIP/Port/Username, write checklist + REPORT skeleton |
| `report` | | Finalize REPORT from an existing session dir (`SESSION_DIR=...`) |

Environment overrides: `SERVER_IP`, `PORT` (default 40000), `USERNAME` / `USERNAME_OVERRIDE` (Windows), `JAVA_HOME`, `SESSION_DIR`.
