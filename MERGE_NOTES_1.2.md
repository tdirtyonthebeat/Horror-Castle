# Horror Castle v1.2 — Stone & Shadow Merge Notes

## Inputs

This v1.2 tree merges:

1. The attached `HorrorCastle_v1.2_SpectralCorpseAlpha` build as the concrete code baseline for the Spectral Corpse work.
2. The Stone & Shadow UI/product direction developed in the current Horror Castle conversation.
3. The approved 1448×1086 gothic interface concept as the visual target for the hybrid skin.

The externally shared ChatGPT conversation URL supplied with the merge request could not be retrieved by the build environment. Therefore this package does not claim to reproduce any non-code prose or design decisions that may exist only on that shared page. The attached Spectral Corpse Alpha source is treated as the authoritative concrete implementation produced by that thread.

## Spectral Corpse integration

The attached alpha's spectral engine is retained and surfaced in the product UI:

- 24 phase-continuous additive partials
- spectral-frame interpolation
- formant remapping without pitch shift
- ROT harmonic-family decomposition
- controlled inharmonic partial stretching
- spectral-body smoothing
- offline periodic-audio analysis API
- existing CORPSE generator index retained for state/preset compatibility

The four live controls are exposed in the UNDERCROFT / CORPSE ALTAR:

- Position
- ROT
- Formant
- Inharmonicity

## Stone & Shadow integration

The primary view is reorganized around a 1448×1086 cinematic Castle layout:

- hybrid raster architectural plates + procedural JUCE + live controls
- CRYPT and TOWER architectural frames
- central cathedral / sigil / gargoyle spine
- castle header with moonlit skyline treatment
- Ritual / Grave architectural strip
- framed HEX and Curse Inspector areas
- advanced systems moved into the UNDERCROFT to reduce primary-view crowding

## Grimoire

The factory Grimoire grows to 40 spells.

Eight new Spectral Corpse spells are included:

- Choir of Ash
- The Last Breath
- Mummified Tape
- Rot Cathedral
- Hollow Saint
- Formant Crypt
- Broken Anatomy
- Spectral Reliquary

## QA expansion

The signature checker now:

- tests every factory spell rather than only the original first eight
- requires at least 40 factory spells
- tests Spectral Corpse Position, ROT, Formant and Inharmonicity independently
- checks those renders remain finite and measurably different
- renders four additional Spectral Corpse reference WAVs

## Build status

Static validation and source/shell sanity checks pass in the packaging environment.

A full local JUCE compile was not possible in the packaging environment because JUCE FetchContent could not reach GitHub. The macOS AppleClang build remains the runtime/compiler checkpoint.
