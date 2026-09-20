# Horror Castle v1.3 — Synthesis-Physics Bestiary

Horror Castle treats supernatural generators as acoustic species. Each species must define its own excitation model, energy storage, coupling topology, decay behavior, performance response, and numerical safety boundary.

OpenAudio is our library of textbooks. Horror Castle writes its own spells — and its own creatures.

## Active chamber species

- Chamber III / CRYPT — **BONE**: struck ten-mode solid with stiffness, strike/read geometry, pressure-fed sustain, and mode-dependent damping.
- Chamber V / CRYPT — **WRAITH**: breath-excited membrane coupled bidirectionally to an air column.
- Chamber V / TOWER — **RELIQUARY**: sympathetic glass modes coupled to resonant cavities.
- Chamber VI / CRYPT — **COFFIN**: wooden lid/body enclosure with cavity, sympathetic-string, scrape, and nonlinear rattle behavior.
- Chamber VI / TOWER — **CHOIR**: several drifting synthetic vocal tracts coupled as a congregation; no vocal samples.
- Chamber VII / CRYPT — **MARROW**: nonlinear stick/slip bow excitation coupling string-like modes into a viscous internal body.
- Chamber VII / TOWER — **ORRERY**: slowly interacting resonant bodies whose orbital phases perturb frequency and coupling relationships.

Generator indices remain append-only. Chamber VII is index 14; indices 0..13 keep their historical meanings.

## Chamber VIII research prototypes

### ABYSS — CRYPT candidate

ABYSS models propagation through a virtual subterranean waveguide network rather than a conventional oscillator.

Acoustic rules:

1. Four tunnels store travelling-wave energy in independent delay paths.
2. DEPTH changes the effective tunnel lengths, so pitch relationships and propagation time change together.
3. Tunnel exits meet at a scattering junction and exchange energy instead of being summed as ordinary delays.
4. DREAD changes wall absorption and junction feedback, making the enclosure darker and more persistent.
5. Expression changes continuous low-frequency excitation and inter-tunnel coupling.
6. All feedback is explicitly bounded before writing back into the tunnels.

### MIRROR — TOWER candidate

MIRROR is a recursive spectral-reflection system.

Acoustic rules:

1. Seven spectral rays begin from an asymmetric inharmonic family.
2. REFLECTION moves a spectral pivot and geometrically reflects partial ratios around that pivot in log-frequency space.
3. A ray contributes to a bounded recursive field that phase-bends later rays.
4. AETHER changes the recursive field depth and slow spectral precession.
5. Expression changes recursive phase coupling rather than merely output level.
6. Reflected and original relationships morph continuously; there is no fixed wavetable or algorithm table.

MIRROR and ABYSS are compiled with the real plugin and SignatureCheck, but remain unassigned to a public generator index until promoted.

## Chamber IX research prototypes

### POLTERGEIST — CRYPT candidate

POLTERGEIST models four resonant charged plates whose electrostatic field changes their mechanical relationships.

Acoustic rules:

1. Plate charges accumulate and leak continuously rather than acting as static modulation values.
2. Like charges repel and opposite charges attract, perturbing plate stress and resonant frequency.
3. CHARGE changes stored electrostatic energy and therefore interaction force.
4. DREAD increases field persistence and the likelihood of bounded discharge events.
5. Expression changes charging rate and discharge regime, not post-output amplitude.
6. Arc impulses and plate displacement are hard-bounded before feedback.

### AURORA — TOWER candidate

AURORA models charged resonant rings inside a shared electrostatic field.

Acoustic rules:

1. Five rings carry slowly redistributing charge.
2. FIELD controls electrostatic bending of the rings' frequency relationships.
3. AETHER changes redistribution rate, field motion, and cross-ring participation.
4. Expression changes field participation continuously rather than gating a separate oscillator.
5. The shared field stores slow state so repeated notes do not behave as fixed additive spectra.
6. Field and ring states are numerically bounded before feedback.

The electromagnetic pair passed the macOS plugin build and its dedicated regression gate before the next family was started.

## Chamber X research prototypes

### VORTEX — CRYPT candidate

VORTEX is a chaotic pressure-and-vortex network inspired by unstable cavity flow and vortex shedding.

Acoustic rules:

1. Four coupled fluid cells store pressure, flow velocity, and signed vortex strength.
2. TURBULENCE increases nonlinear coupling between local flow, neighbor pressure differences, and vortex shedding.
3. Each cell sheds an aeroacoustic tone whose frequency is continuously perturbed by its own vortex state.
4. DREAD increases fluid memory and cavity persistence, allowing pressure to accumulate toward collapse thresholds.
5. Crossing a pressure threshold creates bounded stochastic collapse bursts instead of unbounded feedback.
6. Expression changes forcing pressure and therefore the actual flow regime.

### SIREN — TOWER candidate

SIREN is a bounded jet/edge-tone aeroacoustic model rather than filtered noise.

Acoustic rules:

1. Four air jets are driven from a shared virtual plenum.
2. APERTURE changes jet speed and the edge operating point together, shifting both tone and nonlinear edge response.
3. Each jet stores displacement and edge memory, giving the flow a stateful oscillation rather than a fixed waveform.
4. AETHER changes slow plenum-field coupling and how strongly the jets participate in a shared flow field.
5. Expression changes breath pressure, which changes jet displacement and edge switching behavior.
6. Jet, field, and output states are explicitly bounded at extreme settings.

VORTEX and SIREN are compiled with the real plugin and SignatureCheck and have a dedicated fluid regression target, but they are not assigned a generator index while they remain research prototypes.

## The Nervous System — Creature State Bus

Advanced species may publish normalized physical state through a shared abstraction boundary rather than exposing private DSP internals. The bus vocabulary is **ENERGY, PRESSURE, MOTION, INSTABILITY, EVENT, FIELD**. Continuous signals are bounded and may be smoothed; EVENT is intended for short bounded pulses.

The first ecological route is **VORTEX collapse EVENT -> SIREN plenum disturbance**. VORTEX derives the event directly from its pressure/collapse simulation. SIREN consumes that event before jet-speed and edge-switching calculations, so the route alters the aeroacoustic operating regime instead of post-output amplitude. A zero route amount must leave the systems disconnected, and aggressive coupling must remain finite and bounded.

This route is still research plumbing: it does not assign VORTEX or SIREN public generator indices and therefore does not change preset/state meanings.

## Regression policy

A supernatural object cannot become public merely because it compiles. Its gate must demonstrate:

- finite bounded output at aggressive settings;
- audible output;
- material separation between its structural controls;
- meaningful expression response;
- preservation of earlier generator indices and state compatibility.

Cross-creature ecology adds three additional requirements:

- the producer must emit a measurable physical state from its real simulation;
- disabling a route must make the coupling inert;
- enabling a route must create a measurable behavioral/audio change without violating numerical bounds.

## Why GitHub may email failed runs during development

The open pull request runs macOS CI every time its head changes. During active development an intermediate commit can fail a static assertion, compiler check, or acoustic regression test and GitHub may email that failure. A later corrective commit starts a new run; passing the new run does not erase the historical failed-run notification. We therefore prefer batched commits for tightly coupled engine/plumbing changes to reduce notification noise while retaining strict gates.
