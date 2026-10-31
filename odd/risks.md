# Risk register (MMORPG path)

Living log of risks for the WitcherOnline → shared-online roadmap.  
**Fill while working. Resolve when the owning feature closes** (or explicitly accept residual risk).

Status: `open` | `mitigating` | `resolved` | `accepted`

| ID | Risk | Feature | Severity | Status | Mitigation / resolution |
|----|------|---------|----------|--------|-------------------------|
| R1 | Home host unreachable (CGNAT / no public IP) | F0 dedicated-hosting | high | mitigating | Document Tailscale + VPS; local smoke only — live peer join still operator-owned |
| R2 | JDK mismatch: repo needs 21+, some hosts only have 8 | F0 / build | medium | mitigating | Document JDK 21+; portable JDK under `~/.local/jdk` for this machine |
| R3 | Ban is IP-only; username spoof after IP change | F1 session-identity | medium | accepted | Documented in session-identity.md; full accounts deferred to F5 |
| R4 | Whitelist on/off was runtime-only (lost on restart) | F1 session-identity | medium | resolved | `whitelist on/off` now persists to `server.properties` |
| R5 | UDP protocol forgeable (no auth) | F1→F4 | high | open | Deferred to combat-authority / later auth |
| R6 | NPC desync / no shared world | F3 npc-presence-sync | high | open | Tracked in F3; not started |
| R7 | Client-authoritative combat / cheating | F4 combat-authority | high | open | Tracked in F4 |
| R8 | Item duping via local saves | F5 shared-persistence | high | open | Tracked in F5 |
| R9 | Broadcast cost grows O(n²) with players | F6 world-instances | medium | open | Tracked in F6 |
| R10 | Exposing status HTTP (40001) on public VPS | F0 ops | low | open | Runbook: enable only if locked down |
| R11 | Party time/weather failed when leader ghost not spawned | F2 party-world-slice | medium | resolved | Store UPDATE4 world fields on global player; fallback in `updateWorldSync` |
| R12 | In-game dual-client proof unavailable in this environment | F2 / F0 | medium | accepted | Manual checklists; operator runs acceptance on TW3 clients |
| R13 | Publishing WitcherOnline work into Mod-TW3 must not wipe existing Witcher3-MP `main` | delivery | high | accepted | User authorized force-replace of `main` with WitcherOnline line (`ddf2cae`); prior Witcher3-MP `main` overwritten |

## Resolution log

- 2026-10-31 — **R4 resolved** during F1: whitelist flag persisted via `persistServerBoolean`.
- 2026-10-31 — **R3 accepted** for F1: IP-only bans documented; account auth out of scope until F5.
- 2026-10-31 — **R11 resolved** during F2: global-player world fields + sync fallback.
- 2026-10-31 — **R12 accepted** for F0/F2: no TW3 dual-client in CI/dev host; checklists only.
- 2026-10-31 — **R13 resolved**: remote `mod-tw3` → branch `witcheronline-mmorpg` (https://github.com/Escanor68/Mod-TW3/tree/witcheronline-mmorpg).
- 2026-10-31 — **R13 accepted (superseded)**: user asked to wipe prior `main` and force-push WitcherOnline (`ddf2cae`) to https://github.com/Escanor68/Mod-TW3/tree/main.
- _(append dated entries when marking resolved/accepted)_

## How to use

1. When starting a feature, add/update rows for new risks.
2. While implementing, set status to `mitigating` and note what you did.
3. When the feature’s acceptance is met, set owning risks to `resolved` or `accepted` and add a line under **Resolution log**.
