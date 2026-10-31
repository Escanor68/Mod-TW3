# dedicated-hosting (F0)

Objective: Make a WitcherOnline Java UDP server reachable by players outside one LAN. Branch: `feat/dedicated-hosting`. Delivery: `ask-on-risk`. Runner: `docs/hosting/smoke-local.sh` + manual peer join checklist; no automated TW3 suite.

## Specs

- S1: Players outside the LAN must be able to connect — user: "otras personas fuera de la red osea cada uno de su casa pueda conectarse".
- S2: Scope is hosting/reachability only (VPS, Tailscale/ZeroTier, or equivalent), not MMORPG world authority.
- S3: Clients keep using `witcher/bin/WitcherOnline/config.xml` (`ServerIP` + `Port`, default UDP `40000`).
- S4: Deliver a short English runbook: how to run the server, open/reach UDP 40000, and what friends put in `ServerIP`.
- S5 (assumption): Prefer documenting both Tailscale (friends) and VPS (public/always-on); product choice of which to operate first is deferred until implementation authorize.

Acceptance: two machines not on the same LAN join the same server session (or Tailscale-equivalent proof). Checks: server binds UDP 40000; client config points at reachable host; join observed.

## Tasks

- [x] T1 (S1–S3) — Document current listen/connect path (`WitcherServer` + `config.xml`) — route: inline — status: done — commit: pending
- [x] T2 (S1, S4, S5) — Author runbook for Tailscale path — route: inline — status: done — commit: pending
- [x] T3 (S1, S4, S5) — Author runbook for VPS + UDP 40000 firewall — route: inline — status: done — commit: pending
- [x] T4 (S1, S4) — Smoke-verify join from a non-LAN peer (or recorded Tailscale proof) — route: inline — status: partial — commit: pending
  - Evidence: `docs/hosting/smoke-local.sh` → PASS (config shape + UDP 40000 bind). Peer TW3 join not run here (no JDK 21 JAR build without sudo; no second game client).

## Log

- L1: "primero arrancamos con crear los ODD y despues vamos con lo demas"
- L2: 2026-10-31 — Feature doc created from roadmap F0. Implementation not authorized yet; tracking only.
- L3: Prior user intent for this feature: "como podria hacer para que otras personas fuera de la red osea cada uno de su casa pueda conectarse"
- L4: 2026-10-31 — User authorized F0 implementation: "si hazlo"
- L5: 2026-10-31 — Added `docs/hosting/dedicated-hosting.md`, `docs/hosting/smoke-local.sh`, `server/README.md`, `server/server.properties.example`; README link. Smoke local PASS. Open: live dual-client join from separate networks (operator).
- L6: Risk: none (documentation + local probe script; no protocol/runtime behavior change).
