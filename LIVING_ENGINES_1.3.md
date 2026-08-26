# Horror Castle v1.3 — Living Engines

This milestone deepens performance expression without replacing the v1.2 Stone & Shadow architecture.

The design rule for this branch is simple: a supernatural object should have its own acoustic behavior, excitation model, energy storage, decay law, coupling rules, and performance response. New engines should not be ordinary oscillators with horror-themed names.

## Ritual FM

The shared FM generator is now rendered by an independent four-operator Ritual FM engine.

- phase-continuous four-operator synthesis
- no fixed algorithm table
- continuous graph morph: serial chain -> branching network -> dual carrier
- CRYPT and TOWER use different smoothly related ratio families
- subtle per-voice operator motion keeps long notes alive
- bounded feedback and index scaling remain finite at extremes
- Shape controls topology; chamber Character and live performance expression control complexity

## Bone Resonator 2.0 — active CRYPT body

CRYPT Chamber III now uses an independently designed modal physical body while preserving the existing generator index for patch compatibility.

Acoustic rules:

1. A short noisy strike injects energy into ten damped modes.
2. Shape behaves as material stiffness and continuously stretches modal spacing.
3. DREAD changes virtual strike/read position and modal decay.
4. Pressure/mod-wheel expression adds restrained continuous bow-like excitation so held notes remain alive.
5. Higher modes lose energy faster than lower modes.
6. Every mode remains inside the stable unit circle and the summed body is explicitly bounded.

This replaces the previous fixed two-partial Bone approximation without changing the saved generator identity.

## Wraith — coupled membrane / air-column prototype

WRAITH is the next supernatural acoustic object under development. It is compiled and covered by the Living Engines regression gate but is not yet assigned to a public generator index.

Acoustic rules:

1. WRAITH has no impulse-only source; a filtered breath/noise stream continuously excites the body.
2. Six membrane modes use non-harmonic spacing and material-dependent stretch.
3. Three air-column modes form a sparse odd-resonance family.
4. The membrane and air column exchange energy in both directions.
5. HAUNT controls coupling strength, so the structure itself changes as the control rises.
6. VEIL changes membrane stiffness, excitation position, damping, and breath bandwidth.
7. Pressure changes both excitation energy and membrane/air coupling rather than merely amplitude.
8. Coupling boundaries use bounded feedback so self-reinforcement remains numerically safe.

The intended result is a playable object that can whisper, breathe, flutter, choke, and bloom without becoming a generic granular or noise oscillator.

## Castle engine structure

The former monolithic Castle engine has been separated into three compiled translation units:

- `CastleEngineCore.cpp` — voice allocation, parameter loading, basic oscillators and filters
- `CastleEngineScene.cpp` — CRYPT/TOWER chamber synthesis and supernatural bodies
- `CastleEngineRender.cpp` — sample-accurate MIDI, HEX, Possession, Ritual, Grave, and output rendering

This makes future acoustic bodies easier to isolate, regression-test, and review without destabilizing unrelated systems.

## Expressive MIDI

The Castle engine responds sample-accurately to:

- pitch bend (±2 semitones)
- CC1 mod wheel
- channel pressure
- polyphonic aftertouch

Mod wheel and aftertouch also appear as new HEX source choices. Existing HEX source indices remain unchanged; the new sources are appended for preset compatibility.

## Grimoire auditioning

Factory spells recall immediately when their card is clicked. The old `SUMMON SPELL` action remains as `RE-SUMMON` for repeat recall, but browsing no longer requires a second confirmation click.

## Living Engines regression gate

`HorrorCastleLivingEnginesCheck` directly measures the current object families independently of the GUI:

- Ritual FM topology separation
- Ritual FM CRYPT/TOWER identity
- Ritual FM expression response
- Bone bounded output, material separation, and expression response
- Wraith bounded output, VEIL separation, HAUNT coupling separation, and pressure response

The regular macOS workflow also compiles VST3, AU, Standalone, and the full signature checker.

## Compatibility

- Existing generator indices are unchanged.
- Existing HEX source indices 0..6 are unchanged.
- Existing state migration remains in place.
- Spectral Corpse, Stone & Shadow assets, Ritual, Grave, Possession, and Curse systems remain present.

## Reference research

`OPENAUDIO_REFERENCE_LEDGER.md` records the external open-source projects being studied conceptually for synthesis, modulation, physical modeling, spectral processing, nonlinear processing, and spatial DSP. Horror Castle implementations remain independent unless a future change explicitly passes a dependency/license review.

OpenAudio is our library of textbooks. Horror Castle writes its own spells.
