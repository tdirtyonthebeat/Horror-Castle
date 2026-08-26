# Horror Castle v0.14 — Chamber Engines

## The rule
A chamber-exclusive generator must make sound the other chamber cannot produce by selecting the same named engine.

The original eight shared generator indices remain intact for patch compatibility:

0. VA
1. Wavetable
2. FM
3. PM
4. Vector
5. Chip
6. Noise
7. Resonator

Indices 8–11 now belong to the chamber itself.

## CRYPT exclusive generators

### Undercrypt
Subharmonic body using independent octave and two-octave-below clocks, plus a scarred fundamental. Shape changes the wound component while DREAD increases density and saturation.

### Corpse
A decomposed oscillator built from near-half-speed rotation, near-double-speed splitting, nonlinear dead-zone remapping, and asymmetric CRYPT processing. Intended for unstable basses, drones, and decayed leads.

### Bone Resonator
A pitched body with non-integer resonant partials (2.702x and 4.113x) and a shape-dependent knock component. Intended to sound physical, hollow, and struck rather than glassy.

### Rotator
Crosses a fundamental against 0.75x and 1.25x rotating clocks. Shape moves between ringed body and phase-bent motion.

## TOWER exclusive generators

### Bell Glass
Stable carrier plus 2.414x / 3.732x inharmonic bells. Bright and elevated.

### Spectral Spire
Sparse upper structure around ~3.019x, 4.071x, and 7.133x. Designed to create thin, architectural high-frequency silhouettes.

### Astral FM
FM driven by an independent golden-ratio-ish 1.618x modulator with a slower 0.707x orbit. Shape and HEX FM Depth increase modulation index.

### Prism
Multiple inharmonic clocks are recombined and passed through a sine refraction stage. Shape increases spectral refraction instead of ordinary oscillator morphing.

## Compatibility
Old v0.13.x generator choices 0–7 keep their original meaning. New choices append at 8–11.

## Reference-source policy
The techniques here are independent Horror Castle implementations based on standard synthesis principles (subharmonics, additive/inharmonic partials, FM, ring/phase interaction). No third-party GPL source implementation is copied into these engines.
