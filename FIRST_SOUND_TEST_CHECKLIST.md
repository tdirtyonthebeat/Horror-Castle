# v0.12 First Sound / Stability Checklist

Record PASS / FAIL plus a short note.

## 1. Launch and basic audio
- Standalone launches and remains open for 30 seconds.
- Audio device opens normally.
- One MIDI note produces sound.
- Note-off releases normally.
- 8-10 simultaneous notes do not crash or produce stuck notes.
- All-notes-off / transport stop does not leave a runaway tone.

## 2. CRYPT
For GEN 1, 2 and 3:
- Generator menu shows: VA, Wavetable, FM, PM, Vector, Chip, Noise, Resonator.
- Every generator type produces a perceptible change.
- Level works across its range.
- Shape produces a perceptible change.
- Tune works above and below zero.

Then:
- Noise changes the scene.
- Cutoff A / Resonance A work.
- Cutoff B / Resonance B work.
- Routing menu shows Serial / Parallel / Split / Crossfeed.
- Each routing mode changes the response.
- Master and Balance work.

## 3. TOWER
Repeat the same checks as CRYPT.

## 4. RITUAL
The MODE menu must show:
- Bind
- Sacrifice
- Summon
- Possess
- Devour

For every mode:
- Raise MIX above 0.
- Sweep DEPTH.
- Sweep FURY.
- Sweep WIDTH.
- Carefully raise FEEDBACK.
- Confirm each mode has a distinct audible behavior.
- Confirm no mode creates uncontrolled output at moderate settings.

## 5. GRAVE
- Reverb works.
- Delay works.
- Feedback increases repeats without immediate runaway behavior.
- Tone changes the Grave character.
- Output reaches silence near zero and normal level near the default.

## 6. HEX / CURSES
For at least one lane:
- Source menu works.
- Curse menu works.
- Destination menu works.
- Intensity works positive and negative.
- Curse Inspector follows lane selection.

Check all Curses:
- Clean
- Corrupt
- Haunt
- Possession
- Decay
- Madness
- Blood

Check several destinations:
- Crypt Cutoff
- Tower Cutoff
- Crypt Shape
- Tower Shape
- Filter Drive
- Ritual
- Grave
- Pitch
- Curse Depth

## 7. Stress / failure notes
- Rapidly change generator types while holding notes.
- Rapidly change Ritual modes while holding a chord.
- Change Curse type/destination during playback.
- Max Grave feedback briefly and bring it back down.
- Resize/reopen the editor if your host supports it.

Do not spend time making patches sound beautiful yet. The goal of this pass
is to find crashes, silent controls, parameter inversions, runaway levels,
stuck notes, or controls that do not match their labels.


## v0.12.2 HEX regression test
- Confirm HEX DEPTH is visible between Ritual and Grave.
- Confirm its default is 1.00 in a fresh instance.
- Pulse -> Clean -> Pitch at +0.15 audibly modulates pitch.
- Pulse -> Madness -> Pitch audibly differs from Clean.
- Wraith -> Haunt -> Crypt Cutoff at +0.75 audibly moves the filter.
- Velocity -> Blood -> Filter Drive produces velocity-dependent drive.
- An FM generator responds to HEX destination FM Depth.

## v0.12.3 Ritual separation test
Use a sustained note or chord with Ritual Mix around 0.65.

- BIND: sweep Depth/Fury. Listen for thicker harmonic fusion while the fundamental remains recognizable.
- SACRIFICE: switch from Bind without changing the source patch. Listen for a noticeably more metallic/ring-modulated tone and reduced fundamental.
- SUMMON: hold a note and sweep Depth. You should hear the resonant peak move in pitch. Raise Feedback carefully; persistence should increase.
- POSSESS: should remain phase-bent and animated.
- DEVOUR: should remain folded/destructive.

PASS requires all five modes to be identifiable by ear without looking at the selector.


## v0.14 Chamber Engines
- CRYPT dropdowns include Undercrypt, Corpse, Bone Resonator, Rotator.
- TOWER dropdowns include Bell Glass, Spectral Spire, Astral FM, Prism.
- Undercrypt: confirm obvious subharmonic weight.
- Corpse: confirm rough/decomposed movement unlike Undercrypt.
- Bone Resonator: confirm hollow pitched resonances.
- Rotator: confirm rotating/ringed movement as Shape changes.
- Bell Glass: confirm bright inharmonic bell structure.
- Spectral Spire: confirm sparse upper partials.
- Astral FM: confirm Shape and HEX FM Depth strongly change complexity.
- Prism: confirm refracted spectral motion distinct from Astral FM.
- Run `./build_horror_castle.command test`.
- Optional: run `./build_horror_castle.command render` and audition WAVs 15–22.
