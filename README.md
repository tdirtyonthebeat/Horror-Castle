# Horror Castle v1.2 — Stone & Shadow

Stone & Shadow merges the v1.1 Castle UI system with the Spectral Corpse / resynthesis alpha.

## Main Castle view

The default view now follows the illustrated Horror Castle reference more closely:

- 1448 × 1086 design canvas.
- Asset-backed CRYPT and TOWER architecture with live JUCE controls layered above it.
- A central cathedral / sigil / gargoyle spine between the chambers.
- Asset-backed Ritual / Grave, HEX, and Curse Inspector frames.
- Smaller forged-metal controls so the architecture dominates.
- Possession, advanced filter drive/cross-mod, Rituals, and Spectral Corpse controls moved into the **UNDERCROFT** secondary room.
- The Grimoire remains a dedicated overlay.

## Spectral Corpse

CRYPT Chamber II / CORPSE is a spectral-frame additive resynthesis engine:

- 24 phase-continuous additive partials.
- Up to 32 analysed spectral frames.
- Continuous frame interpolation.
- Spectral formant remapping.
- ROT harmonic-family decomposition.
- Inharmonic partial stretching.
- A short spectral-body integrator.

The UNDERCROFT exposes:

- POSITION
- ROT
- FORMANT
- INHARMONIC

The offline `analysePeriodicAudio()` API remains available for the future sample/model importer.

## Grimoire

The factory Grimoire expands from 32 to 40 spells, including eight Spectral Corpse recipes.

## Build

```bash
./build_horror_castle.command static
./build_horror_castle.command clean run
./build_horror_castle.command qa
```

## Release discipline

The v1.0/v1.1 core systems remain intact. Stone & Shadow changes presentation, exposes the Spectral Corpse controls already present in the attached alpha, expands presets, and adds regression coverage for the four Corpse dimensions.
