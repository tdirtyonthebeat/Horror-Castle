# Horror Castle v1.0 — Product Candidate

v1.0 integrates the experimental branches into one coherent instrument rather than treating each feature as a separate prototype.

## v0.15 — POSSESSION

A dedicated pre-Ritual cross-chamber processor was added:

- **Blood Feed** — CRYPT envelope energy drives TOWER intensity and edge.
- **Aether Leak** — TOWER high-frequency residue contaminates CRYPT while rejecting most low body.
- **Soul Exchange** — bounded bidirectional cross-phase interaction.
- **Haunt** — fixed, unequal delayed imprints of each chamber are injected into the opposite chamber without regenerative feedback.

HEX 2.0 can modulate all four Possession controls.

## v0.16 — RITUALS

Rituals is a note-performance engine, not a conventional up/down-only arpeggiator.

Patterns:

1. Procession
2. Ascension
3. Descent
4. Circle
5. Seance
6. Possession
7. Sacrifice
8. Chaos

Controls: enabled, pattern, rate, BPM, gate, probability, swing and octave range.

The core MIDI dispatcher was changed so generated and incoming note events are handled at their actual sample positions inside the block. Rituals also contributes a bounded performance imprint to chamber balance, Haunt and Ritual intensity.

## v0.17 — GRIMOIRE

- state version `100` / product version `1.0.0`
- legacy v0.13/v0.14 state migration marker
- factory spell system
- user `.hcg` save/load in `Documents/SleepFighterStudios/Horror Castle/Grimoire`
- eight factory starting points

## v0.18 — THE CASTLE

The editor now exposes the Possession Matrix and Rituals directly. The Grimoire selector is in the header. Cross FM, Cross Ring and both filter drives are also visible. Previously silent generator pan/spread, global glide/unison and cross-scene switches now affect the active engine.

## v0.19 — EXORCISM

The headless regression tool now covers:

- chamber/exclusive-engine spectral separation
- Ritual and Curse differentiation
- Possession stability and audible change
- HEX 2.0 identity/Possession destinations
- Rituals note performance and release
- all factory Grimoire spells remaining finite
- v1 state round-trip
- 44.1 kHz / 96 kHz and 64 / 1024 sample blocks
- extreme combined Possession + Ritual + Grave settings

A zero-dependency static source validator and `qa` launcher mode were also added.

## v1.0

The version number is now 1.0.0 in CMake/plugin metadata. This package should be treated as a **product candidate until it passes the Mac compile, DSP test, AU validation, host tests and provenance checklist**.
