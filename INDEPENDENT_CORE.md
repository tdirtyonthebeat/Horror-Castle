# Independent Core Migration

v0.12 introduces `HorrorCastleProcessor`, which owns only:

- Horror Castle APVTS parameter state
- CastleEngine
- MIDI/audio rendering
- preset/state serialization
- editor creation
- HEX telemetry

The build target no longer compiles the historical inactive processor,
voice, GUI, manual, factory-preset, or binary-data trees.

The current audio path is:

MIDI
-> HorrorCastleProcessor
-> CastleEngine
-> CRYPT + TOWER
-> HEX / Curses
-> Ritual
-> Grave
-> stereo output

This is an engineering migration milestone, not a final legal provenance
certification. A release candidate should still receive a dedicated
source/provenance and third-party-license audit before commercial distribution.
