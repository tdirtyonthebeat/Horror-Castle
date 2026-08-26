# Horror Castle v1.0 — Release Notes

Horror Castle v1.0 is the first product release of the independent Horror Castle core.

## Release gate

This source is promoted from v1.0 RC2 after the following gates passed on macOS / AppleClang:

- static source validation
- VST3 build
- Audio Unit build
- Standalone build and launch
- signature regression suite
- chamber spectral-separation checks
- all CRYPT-exclusive engines finite and distinct
- all TOWER-exclusive engines finite and distinct
- Spectral Spire brightness > Corpse brightness by the release threshold
- Ritual mode separation
- Curse identity checks
- Possession Matrix stability and audible-effect checks
- HEX 2.0 destination reachability
- Rituals performance and note-release tests
- Grimoire factory-spell finiteness
- 44.1/96 kHz and 64/1024-sample buffer tests
- v1 state round-trip
- extreme Ritual/Grave stability
- offline reference rendering
- Apple Audio Unit validation

## Core architecture

CRYPT -> POSSESSION <- TOWER
            |
          RITUAL
            |
          GRAVE
            |
          OUTPUT

HEX 2.0 can modulate the wider Castle architecture.
RITUALS provides performance sequencing.
GRIMOIRE provides factory and user state/preset handling.

## Exclusive chamber engines

CRYPT:
- Undercrypt
- Corpse
- Bone Resonator
- Rotator

TOWER:
- Bell Glass
- Spectral Spire
- Astral FM
- Prism

## v1.0 QA reference result

RC2 release-gate diagnostic:
- CORPSE brightness: 0.107133
- SPECTRAL SPIRE brightness: 0.154785
- ratio: 1.44479
- required ratio: > 1.10

## Product note

This release source preserves the independent Horror Castle implementation and provenance policy documented in the repository. Third-party reference projects were used for learning and architectural inspiration, not as code to transplant into the active proprietary product core.
