# Horror Castle v1.3 — Living Engines

This milestone deepens performance expression without replacing the v1.2 Stone & Shadow architecture.

The design rule for this branch is simple: a supernatural object should have its own acoustic behavior, excitation model, energy storage, decay law, coupling rules, and performance response. New engines should not be ordinary oscillators with horror-themed names.

## Ritual FM

The shared FM generator is rendered by an independent four-operator engine with a continuous serial -> branching -> dual-carrier graph morph, chamber-specific ratio families, bounded feedback, subtle operator motion, and performance-controlled complexity.

## Bone Resonator 2.0 — active CRYPT Chamber III

Bone is a ten-mode physical body. Shape behaves as material stiffness; DREAD changes strike/read geometry and decay; expression adds restrained bow-like excitation. The historical generator index remains unchanged.

## Chamber V — appended compatibility-safe object pair

Generator index 12 is appended as `ChamberV`; indices 0..11 keep their existing meanings.

### CRYPT Chamber V — WRAITH

WRAITH is a breath-excited membrane / air-column organism.

1. A filtered breath/noise stream continuously injects energy.
2. Six non-harmonic membrane modes and three sparse air-column modes store that energy.
3. Membrane and air column exchange energy bidirectionally.
4. VEIL changes membrane stiffness, excitation position, damping, and breath bandwidth.
5. DREAD/HAUNT changes the coupling law itself.
6. Mod wheel and pressure change both excitation and coupling, not merely output level.
7. Body-boundary feedback is saturated so eerie self-reinforcement remains bounded.

### TOWER Chamber V — RELIQUARY

RELIQUARY is a sympathetic glass / cavity object.

1. A sparse fundamental-plus-inharmonic exciter feeds the body rather than becoming the final sound directly.
2. Eight irrational-ish glass modes create the upper resonant family.
3. Three lower cavity modes store and return energy to the glass modes.
4. APERTURE (generator Shape) moves virtual excitation/read positions, changing which modes survive.
5. AETHER changes decay, slow modal drift, and glass/cavity coupling strength.
6. Pressure/mod wheel alter excitation and coupling so the object opens under performance energy.
7. Glass/cavity feedback boundaries are bounded, and output gain is deliberately kept below permanent saturation so the object retains dynamic headroom.

The Chamber V pair follows one shared design principle but opposite physics: CRYPT breathes through a coupled membrane and air column; TOWER rings through sympathetic glass and cavities.

## Castle engine structure

The engine is separated into `CastleEngineCore.cpp`, `CastleEngineScene.cpp`, and `CastleEngineRender.cpp`, keeping voice/parameter logic, supernatural chamber DSP, and MIDI/bus rendering independently reviewable.

## Expressive MIDI

The Castle responds sample-accurately to pitch bend, CC1 mod wheel, channel pressure, and polyphonic aftertouch. Mod Wheel and Aftertouch are appended HEX sources, preserving previous source indices.

## Grimoire auditioning

Factory spells recall immediately when clicked. `RE-SUMMON` remains available for repeat recall.

## Living Engines regression gate

`HorrorCastleLivingEnginesCheck` measures:

- Ritual FM topology, chamber identity, and expression
- Bone bounded output, material separation, and expression
- Wraith VEIL, coupling, pressure response, audibility, and boundedness
- Reliquary APERTURE, AETHER coupling, expression response, audibility, and boundedness

The macOS workflow also compiles VST3, AU, Standalone, and the full SignatureCheck target.

## Compatibility

- Existing generator indices 0..11 are unchanged; Chamber V is appended at 12.
- Existing HEX source indices 0..6 are unchanged.
- Existing state migration remains in place.
- Spectral Corpse, Stone & Shadow, Ritual, Grave, Possession, Rituals, and the Grimoire remain present.

## Reference research

`OPENAUDIO_REFERENCE_LEDGER.md` records the external projects studied conceptually. Horror Castle implementations remain independent unless a future dependency explicitly passes license/provenance review.

OpenAudio is our library of textbooks. Horror Castle writes its own spells.
