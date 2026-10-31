# MMORPG-path roadmap (ODD index)

Living index of feature docs under `odd/tasks/`. Implement in order unless a dependency says otherwise.

| ID | Feature | Status | Doc |
|----|---------|--------|-----|
| F0 | dedicated-hosting | done (ops residual R1/R12) | [dedicated-hosting.md](tasks/dedicated-hosting.md) |
| F1 | session-identity | done (R3 accepted) | [session-identity.md](tasks/session-identity.md) |
| F2 | party-world-slice | done | [party-world-slice.md](tasks/party-world-slice.md) |
| F3 | npc-presence-sync | done (T5 operator) | [npc-presence-sync.md](tasks/npc-presence-sync.md) |
| F4 | combat-authority | done (dual-client open) | [combat-authority.md](tasks/combat-authority.md) |
| F5 | shared-persistence | done (residuals → F8/F10) | [shared-persistence.md](tasks/shared-persistence.md) |
| F6 | world-instances | done (interest; load smoke open) | [world-instances.md](tasks/world-instances.md) |
| F7 | story-room-characters | done (dual-client open) | [story-room-characters.md](tasks/story-room-characters.md) |
| F8 | harden-base | **next** — tracking | [harden-base.md](tasks/harden-base.md) |
| F9 | party-membership | tracking | [party-membership.md](tasks/party-membership.md) |
| F10 | economy-operations | tracking | [economy-operations.md](tasks/economy-operations.md) |
| F11 | combat-hardening | tracking | [combat-hardening.md](tasks/combat-hardening.md) |
| F12 | session-hardening | tracking | [session-hardening.md](tasks/session-hardening.md) |
| F13 | client-resilience | tracking | [client-resilience.md](tasks/client-resilience.md) |
| F14 | authority-matrix | tracking | [authority-matrix.md](tasks/authority-matrix.md) |
| F15 | cooperative-contract | tracking | [cooperative-contract.md](tasks/cooperative-contract.md) |
| F16 | observability | tracking | [observability.md](tasks/observability.md) |
| F17 | scale-experience | tracking | [scale-experience.md](tasks/scale-experience.md) |

Sources for F8+: technical audits (31 Oct 2026) + gap analysis vs `main`. Risks: [risks.md](risks.md).

Suggested implement order: **F8 → F12 → F11 → F9 → F10 → F13 → F14 → F16 → F15 → F17**.
