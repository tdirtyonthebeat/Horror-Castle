# Horror Castle v0.13 — Signature Sound Alpha

v0.13 is the first pass where CRYPT, TOWER and GRAVE deliberately stop sharing a generic sonic identity.

## CRYPT — DREAD

CRYPT is the corporeal chamber: darker, less stable and more scarred.

- stronger note-to-note detune
- slow per-voice pitch wander
- asymmetric waveform deformation
- restrained odd-harmonic scars
- character-controlled body low-pass and saturation
- darker default filter tuning

The **DREAD** control scales those traits from restrained to unstable.

## TOWER — AETHER

TOWER is the elevated chamber: tighter pitch, brighter edges and glass-like upper partials.

- much smaller pitch drift
- subtle high-frequency motion
- frequency-aware 2nd/5th harmonic injection
- post-filter air emphasis
- brighter default filters
- Wavetable/FM/Vector-biased default generator stack

The **AETHER** control scales those traits.

## Independent filter histories

CRYPT and TOWER now have separate per-voice filter memory. A chamber can no longer inherit the other chamber's previous filter sample state.

## GRAVE — Castle chamber

JUCE's stock reverb has been removed from the active signal path. GRAVE now uses Horror Castle's own four-line dark feedback chamber:

- uneven delay paths
- orthogonal sign feedback mixing
- logarithmic damping controlled by TONE
- stereo decorrelation without modulation LFOs
- stable bounded feedback

The existing GRAVE controls still work, so presets and automation remain simple.

## Band-limited edges

VA and Chip generators now use a small PolyBLEP correction at discontinuities. This is a standard anti-aliasing technique implemented directly for Horror Castle.

## Product engineering

Saved state now carries `stateVersion = 13`.

An optional `HorrorCastleSignatureCheck` console target can:

- assert CRYPT and TOWER render differently
- assert BIND/SACRIFICE/SUMMON render differently
- assert every non-CLEAN Curse renders measurably differently from CLEAN
- stress-test numerical finiteness
- render fourteen WAV reference files for listening comparisons

Run:

```bash
./build_horror_castle.command test
./build_horror_castle.command render
```

The renderer writes WAVs to `SignatureRenders/`.


## 0.13.1 Chamber Identity follow-up
The first listening pass proved the chambers were technically different but still too close perceptually. v0.13.1 intentionally separates their pitch register, spectral topology, stereo behavior and filter defaults.
