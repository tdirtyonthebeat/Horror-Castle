# Changelog

## 1.2.0 — Stone & Shadow

- Fixed AppleClang overload ambiguity in two UNDERCROFT `Graphics::fillRect` calls by making all rectangle arguments explicitly floating-point.
- Integrated the Spectral Corpse / resynthesis alpha into the product UI.
- Exposed Corpse Position, ROT, Formant and Inharmonicity in the UNDERCROFT.
- Switched the primary Castle view to a 1448×1086 art-directed layout.
- Added hybrid raster/procedural architecture plates for CRYPT, TOWER, the central cathedral spine, Ritual/Grave, HEX and Curse Inspector.
- Moved advanced filter drive, Cross FM/Ring, Possession and Rituals into the secondary UNDERCROFT room so the main view tracks the approved reference aesthetic more closely.
- Expanded the Grimoire from 32 to 40 factory spells with eight Spectral Corpse recipes.
- Extended signature QA to exercise the four Spectral Corpse dimensions and all factory spells.
- State/product metadata updated to 1.2.0 / state version 120.

## 1.1.0 — The Castle Awakens

- New gothic castle LookAndFeel and procedural stone/rune drawing system.
- Embedded supplied castle artwork in the header.
- CRYPT and TOWER now carry independent crimson/violet visual identities.
- Added subtle animated fog and torch ambience.
- Restyled HEX and Curse Inspector around ritual geometry.
- Added dedicated searchable/category-filtered Grimoire overlay.
- Expanded the factory Grimoire from 8 to 32 spells.
- User `.hcg` save/load retained.
- DSP engine files intentionally unchanged from v1.0 release.

## 1.0.0 — Release

- Promoted v1.0 RC2 to the first Horror Castle release after all automated signature tests passed.
- Apple Audio Unit validation succeeded.
- Spectral Spire / Corpse release-gate brightness ratio passed at 1.44479.
- No DSP algorithms were changed between RC2 and this release package.
- Release package branding, README, and release notes finalized.

## 1.0.0 RC2 — Spectral Spire QA Fix

- Raised SPECTRAL SPIRE into a more decisively upper-spectral identity.
- Reduced SPECTRAL SPIRE fundamental contribution.
- Moved its primary inharmonic partials from ~4.071x / 7.133x to ~5.071x / 9.173x and added a ~13.127x crown partial when sample rate permits.
- Added quantitative CORPSE/SPIRE brightness diagnostics to the signature checker.
- Did not alter CORPSE, Possession, HEX 2.0, Rituals, Grimoire, Ritual modes, Curses, Grave, or state IDs.
- Plugin/version metadata remains 1.0.0; RC2 identifies the source candidate/package.

## 1.0.0 — Product Candidate
- Integrated the v0.15–v0.19 roadmap into a single product candidate.
- Added Possession Matrix: Blood Feed, Aether Leak, Soul Exchange, Haunt.
- Expanded HEX from 11 to 22 stable destinations while retaining indices 0–10.
- Added Rituals performance sequencer with eight patterns, rate/BPM/gate/probability/swing/octaves.
- Changed active MIDI dispatch to honor sample positions inside each audio block.
- Added Grimoire state version 100, eight factory spells and user `.hcg` save/load.
- Added v1 state merge/migration path for legacy states.
- Exposed Possession, Rituals, Cross FM, Cross Ring and filter drive controls in the editor.
- Wired previously dormant generator pan/spread, global glide/unison and cross-scene controls into active DSP.
- Expanded regression QA for new systems, state round-trip, sample rates and buffer sizes.
- Added static source validation and `qa` launcher mode with AU validation when available.

## 0.19 — Exorcism / QA
- Regression suite expansion, static validation and release checklist.

## 0.18 — The Castle
- Integrated product UI for Possession, Rituals and Grimoire; additional parameter exposure.

## 0.17 — Grimoire
- Versioned preset/state system and factory spells.

## 0.16 — Rituals
- Performance sequencer and sample-position MIDI dispatch.

## 0.15 — Possession
- Cross-chamber infection engine and HEX 2.0 destinations.

## 0.14.0 — Chamber Engines

- Added four CRYPT-exclusive generator identities: Undercrypt, Corpse, Bone Resonator, Rotator.
- Added four TOWER-exclusive generator identities: Bell Glass, Spectral Spire, Astral FM, Prism.
- Preserved shared generator indices 0–7 for v0.13.x patch compatibility; exclusive engines occupy indices 8–11 and are interpreted per chamber.
- Added independent per-generator auxiliary phase clocks for stable subharmonic and inharmonic synthesis.
- FM Depth HEX modulation now also drives TOWER Astral FM.
- New default patch uses chamber-exclusive engines so CRYPT/TOWER identity is immediate.
- Expanded signature regression tests and offline renders for all eight exclusive engines.
- Retains v0.13.1 chamber separation, v0.12.5 Curse identity, and Ritual separation work.

## 0.13.1 — Chamber Identity

- Pushed CRYPT and TOWER into different synthesis domains instead of merely different coloration.
- Added CRYPT octave/sub-octave underbody controlled by DREAD.
- Added TOWER inharmonic bell partials and spectral stereo width controlled by AETHER.
- Increased CRYPT drift while tightening TOWER pitch stability.
- Changed default secondary generator tunings: CRYPT downward, TOWER upward.
- Changed default TOWER filter topology to high-pass -> low-pass; CRYPT remains low-pass -> low-pass.
- Strengthened automated chamber-identity tests with brightness and low-body proxies.
- Added max-character chamber reference renders.

## 0.13.0 — Signature Sound Alpha

- Added distinct CRYPT DREAD and TOWER AETHER scene-character DSP.
- Added DREAD/AETHER controls to the editor.
- Added band-limited VA and Chip oscillator edges.
- Split CRYPT/TOWER filter memory per voice.
- Replaced stock reverb path with independent GraveChamber feedback network.
- Added versioned state marker `stateVersion = 13`.
- Added optional signature regression console target and WAV renderer.
- Retains v0.12.5 audible Ritual/Curse identity behavior.

## 0.12.5 — Audible Identity Fix

- Rebuilt SACRIFICE as a DC-blocked ring/sideband generator so it remains clearly different from BIND even when CRYPT and TOWER use similar tones.
- Kept BIND as fundamental-preserving harmonic fusion.
- Increased perceptual separation of all seven Curse transfer functions.
- CORRUPT is now strongly asymmetric.
- HAUNT now combines amplitude eclipsing with an independent ghost contour.
- POSSESSION now wavefolds the modulation source.
- DECAY now creates narrow dying peaks controlled by Decay Time and Blood.
- MADNESS remains explicitly stepped.
- BLOOD now pumps with the amp envelope and Wraith contamination.
- Retains the v0.12.2 HEX DEPTH and FM Depth wiring fixes.

## 0.12.4 — Ritual Compile Fix

- Fixed the `CastleEngine` FM-depth modulation helper type from nonexistent `Generator` to `GeneratorSlot`.
- Retains the v0.12.3 BIND / SACRIFICE / SUMMON separation DSP unchanged.
- Retains the v0.12.2 HEX signal and visible HEX DEPTH fixes unchanged.

## 0.12.3 — Ritual Separation

- Reworked BIND so it harmonically fuses CRYPT and TOWER instead of acting primarily as a scene crossfade.
- Reworked SACRIFICE into a much stronger balanced-ring / sideband texture with reduced fundamental.
- Reworked SUMMON into a tuned, bounded regenerative resonator.
- SUMMON Depth now sweeps resonance pitch (~72 Hz to ~2.3 kHz).
- SUMMON Fury controls excitation; Feedback controls persistence/resonance.
- Retained POSSESS and DEVOUR algorithms because they were already perceptually distinct.
- Includes all v0.12.2 HEX signal fixes.

## 0.12.2 — HEX Signal Fix

- Fixed silent HEX/Curses in the Independent Core build.
- `global.hex` now defaults to 1.0 for new instances.
- Added a visible `HEX DEPTH` master control between Ritual and Grave.
- Wired the previously-unused `FM Depth` HEX destination into FM/PM generator modulation depth.
- No changes to Curse algorithms, Ritual algorithms, or Grave processing.

## 0.12.1 — Independent Core Compile Fix

- Fixed `AudioProcessor::copyXmlToBinary` qualification.
- Fixed `AudioProcessor::getXmlFromBinary` qualification.
- No DSP, parameter, Ritual, HEX, Curse, or UI behavior changes.

## 0.12.0 — Independent Core Alpha

- Added HorrorCastleProcessor as the active JUCE processor shell.
- Removed inactive historical source trees and binary asset packs from the build.
- Removed nonessential external dependencies; CMake now fetches JUCE only.
- Preserved CastleEngine / HEX / Curse / Ritual / Grave DSP behavior.
- Fixed empty APVTS choice ComboBoxes globally.
- Ritual MODE now exposes Bind, Sacrifice, Summon, Possess and Devour.
- Generator TYPE and filter ROUTING menus now populate correctly.
- Added a structured first-sound/stability test checklist.
