# Horror Castle v1.3 — Living Engines

This milestone deepens performance expression without replacing the v1.2 Stone & Shadow architecture.

## Ritual FM

The shared FM generator is now rendered by an independent four-operator Ritual FM engine.

- phase-continuous four-operator synthesis
- no fixed algorithm table
- continuous graph morph: serial chain -> branching network -> dual carrier
- CRYPT and TOWER use different smoothly related ratio families
- subtle per-voice operator motion keeps long notes alive
- bounded feedback and index scaling remain finite at extremes
- Shape controls topology; chamber Character and live performance expression control complexity

## Expressive MIDI

The Castle engine now responds sample-accurately to:

- pitch bend (±2 semitones)
- CC1 mod wheel
- channel pressure
- polyphonic aftertouch

Mod wheel and aftertouch also appear as new HEX source choices. Existing HEX source indices remain unchanged; the new sources are appended for preset compatibility.

## Grimoire auditioning

Factory spells now recall immediately when their card is clicked. The old `SUMMON SPELL` action remains as `RE-SUMMON` for repeat recall, but browsing no longer requires a second confirmation click.

## Compatibility

- Existing generator indices are unchanged.
- Existing HEX source indices 0..6 are unchanged.
- Existing state migration remains in place.
- Spectral Corpse, Stone & Shadow assets, Ritual, Grave, Possession, and Curse systems remain present.

## Reference research

`OPENAUDIO_REFERENCE_LEDGER.md` records the external open-source projects being studied conceptually for synthesis, modulation, physical modeling, spectral processing, nonlinear processing, and spatial DSP. Horror Castle implementations remain independent unless a future change explicitly passes a dependency/license review.
