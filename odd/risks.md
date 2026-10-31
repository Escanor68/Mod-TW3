# Risk register (MMORPG path)

Living log of risks for the WitcherOnline → shared-online roadmap.  
**Fill while working. Resolve when the owning feature closes** (or explicitly accept residual risk).

Status: `open` | `mitigating` | `resolved` | `accepted`

| ID | Risk | Feature | Severity | Status | Mitigation / resolution |
|----|------|---------|----------|--------|-------------------------|
| R1 | Home host unreachable (CGNAT / no public IP) | F0 dedicated-hosting | high | accepted | Tailscale/VPS documented; live peer reachability remains operator-owned (no code can fix CGNAT) |
| R2 | JDK mismatch: repo needs 21+, some hosts only have 8 | F0 / build | medium | resolved | Shared `require-jdk21.sh` gates all server test scripts; docs call out JDK 21+ |
| R3 | Ban is IP-only; username spoof after IP change | F1 session-identity | medium | accepted | Documented in session-identity.md; full accounts deferred to F5 |
| R4 | Whitelist on/off was runtime-only (lost on restart) | F1 session-identity | medium | resolved | `whitelist on/off` now persists to `server.properties` |
| R5 | UDP protocol forgeable (no auth) | F1→F4 | high | open | F4 clamps HIT damage/range/cooldown; full packet auth still deferred |
| R6 | NPC desync / no shared world | F3 npc-presence-sync | high | mitigating | Party NPC ghosts spawn; first slice uses shared geralt_npc visual; T5 in-game open |
| R7 | Client-authoritative combat / cheating | F4 combat-authority | high | mitigating | Server `authHealth` + HIT/HITACK; local animations can still play; dual-client T4 open |
| R8 | Item duping via local saves | F5 shared-persistence | high | mitigating | Crowns+key-bag server snapshot; covered qty overwrite; full inventory still local |
| R9 | Broadcast cost grows O(n²) with players | F6 world-instances | medium | resolved | Interest radius 100u + party exception; heartbeat logs interest vs baseline (`092924f`) |
| R10 | Exposing status HTTP (40001) on public VPS | F0 ops | low | resolved | Default `svgBind=127.0.0.1`; public bind requires explicit `svgBind` + firewall |
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
- 2026-10-31 — **R7 mitigating** during F4: HIT/HITACK + `CombatAuthorityRules`; residual local anim + UDP forge (R5).
- 2026-10-31 — **R8 mitigating** during F5: crowns+key-bag TSV snapshots; covered items overwrite on load.
- 2026-10-31 — **R9 mitigating** during F6: interest radius + party exception; heartbeat compares interest vs baseline.
- 2026-10-31 — **R9 resolved**: F6 interest slice accepted; residual multi-process shards remain out of scope.
- 2026-10-31 — **R10 resolved**: status HTTP defaults to loopback bind (`svgBind`).
- 2026-10-31 — **R2 resolved**: `require-jdk21.sh` fail-fast on JDK &lt; 21 for all `run-*-tests.sh`.
- 2026-10-31 — **R1 accepted**: CGNAT/public reachability is an operator path (Tailscale/VPS); residual accepted for F0.
- _(append dated entries when marking resolved/accepted)_

## How to use

1. When starting a feature, add/update rows for new risks.
2. While implementing, set status to `mitigating` and note what you did.
3. When the feature’s acceptance is met, set owning risks to `resolved` or `accepted` and add a line under **Resolution log**.
