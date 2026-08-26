# Spectral Corpse / Resynthesis Engine — Alpha

This build replaces CRYPT Chamber II (CORPSE) with Horror Castle's independent spectral-frame resynthesis engine.

## Engine
- 24 phase-continuous additive partials per CORPSE generator voice.
- Up to 32 spectral frames per analysed model.
- Continuous interpolation between adjacent frames.
- Spectral formant remapping without changing played pitch.
- ROT decomposes harmonic families and upper partials instead of acting as a conventional low-pass filter.
- Controlled partial stretching introduces inharmonic decomposition.
- A short spectral-body integrator keeps frame transitions organic while remaining inside the resynthesis voice.

## Analysis API
`SpectralCorpseEngine::analysePeriodicAudio()` accepts a JUCE AudioBuffer, source sample rate, and approximate fundamental pitch. It windows overlapping FFT frames, harvests energy around integer harmonic bins, normalizes each frame, and returns a playable model.

The audio analysis method is intentionally offline/non-audio-thread. Sample/model import UI is the next layer; this alpha ships an original procedural default corpse model so CORPSE remains immediately playable.

## Parameters
- `corpse.position`
- `corpse.rot`
- `corpse.formant`
- `corpse.inharmonic`

Existing patches remain compatible because CORPSE retains its existing CRYPT Chamber II generator index.
