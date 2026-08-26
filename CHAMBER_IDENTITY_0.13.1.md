# Horror Castle v0.13.1 — Chamber Identity

## Goal
CRYPT and TOWER must be recognizable as different synthesis spaces, not merely statistically different renders.

### CRYPT / DREAD
- true octave-below and two-octave-below underbody oscillators
- stronger note-to-note detune and slow wander
- asymmetric generator scarring/saturation
- progressively lower chamber ceiling as DREAD rises
- centered, monolithic stereo image
- secondary generators default downward (-12 semitones)
- low-pass / low-pass default filter topology

### TOWER / AETHER
- independent inharmonic bell oscillators at ~2.414x and ~3.732x
- much tighter pitch center
- high-frequency air extraction
- opposite-polarity bell side signal for spectral stereo width
- secondary generators default upward (+12 and +19 semitones)
- high-pass into low-pass default filter topology

## Regression bar
The signature tool now checks not only sample difference but also two simple perceptual proxies:
- TOWER must be measurably brighter than CRYPT
- CRYPT must carry measurably more low-body energy than TOWER

The renderer adds `02a-crypt-dread-max.wav` and `02b-tower-aether-max.wav` for fast A/B listening.
