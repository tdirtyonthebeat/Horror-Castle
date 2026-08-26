# Horror Castle v0.12.2 — HEX Signal Fix

## Root cause
The eight Curse lanes were still being evaluated, but the final matrix output was multiplied by `global.hex`.
The Independent Core parameter default was 0.0 and the editor created the HEX master control without laying it out, so the user could not raise it.

## Fixes
1. `global.hex` defaults to 1.0 for new instances.
2. `HEX DEPTH` is visible between Ritual and Grave.
3. Existing saved states that intentionally contain a zero HEX depth remain respected; turn HEX DEPTH up to enable modulation.
4. The `FM Depth` destination now changes FM/PM generator modulation depth.

## Quick validation patch
- Set HEX DEPTH to 1.00.
- Lane 1: Pulse -> Clean -> Pitch, amount +0.15.
- Hold a note: audible periodic pitch movement should be obvious.
- Then switch destination to Crypt Cutoff and raise amount to +0.75.
- Switch Curse Clean -> Madness / Haunt / Corrupt and compare the modulation contour.
