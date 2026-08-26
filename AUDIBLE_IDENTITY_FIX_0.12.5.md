# Horror Castle v0.12.5 — Audible Identity Fix

## Why BIND and SACRIFICE still converged
When CRYPT and TOWER contain similar waveforms, both previous algorithms generated related nonlinear distortions.
The new SACRIFICE uses ring multiplication followed by DC blocking. For similar pitched tones, this emphasizes octave/sum/difference sidebands instead of warm fundamental-rich distortion.

## Curse identity target
Each Curse now changes the *topology* of the modulation contour, not merely its gain:

- CLEAN: source unchanged
- CORRUPT: asymmetric polarity breakage
- HAUNT: drifting eclipse + ghost contour
- POSSESSION: wavefolded modulation
- DECAY: narrow dying peaks
- MADNESS: hard staircase
- BLOOD: envelope-driven pumping + Wraith contamination

## Fast A/B test
Use one sustained note, HEX DEPTH = 1.00, Lane 1 = Pulse -> Pitch, Amount = +0.20.
Switch only the Curse selector. CLEAN, HAUNT, POSSESSION and MADNESS should be immediately recognizable.
Then use Pulse -> Crypt Cutoff, Amount = +0.75 to compare CORRUPT, DECAY and BLOOD.

For Ritual, set Mix = 0.75 and use the same CRYPT/TOWER patch. BIND should preserve the fundamental and thicken it. SACRIFICE should jump toward metallic/octave sideband content.
