# Reference Techniques / Provenance Notes

Horror Castle 1.0 is an independent implementation. During development, GPL-licensed synthesizer projects supplied as references — including Odin 2 and Galdr — and the Surge project were studied for *classes of techniques*, architecture ideas, testing practice and product-engineering patterns. Their source implementations are not vendored into this release tree.

Independent Horror Castle implementations use standard synthesis/DSP ideas including:

- PolyBLEP-style discontinuity correction
- subharmonic and inharmonic oscillator structures
- additive partials, FM/PM and bounded cross-phase interaction
- per-voice pitch drift and glide
- feedback-delay-network reverb principles
- delay-based spectral imprints
- modulation waveshaping and quantisation
- versioned state and preset migration markers
- sample-position MIDI dispatch
- headless numerical/audio-difference tests
- offline reference rendering

Galdr specifically informed the decision to invest early in headless tests, offline rendering, versioned preset state and clearly separated modulation domains. Those are engineering ideas, not copied source.

No third-party synthesizer DSP source file is included in `Source/`.
