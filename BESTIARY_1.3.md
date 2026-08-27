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

MIRROR and ABYSS are compiled with the real plugin and SignatureCheck, but they are not assigned generator index 15 until their dedicated macOS numerical gate passes.

## Regression policy

A supernatural object cannot become public merely because it compiles. Its gate must demonstrate:

- finite bounded output at aggressive settings;
- audible output;
- material separation between its structural controls;
- meaningful expression response;
- preservation of earlier generator indices and state compatibility.

## Why GitHub may email failed runs during development

The open pull request runs macOS CI every time its head changes. During active development an intermediate commit can fail a static assertion, compiler check, or acoustic regression test and GitHub may email that failure. A later corrective commit starts a new run; passing the new run does not erase the historical failed-run notification. We therefore prefer batched commits for tightly coupled engine/plumbing changes to reduce notification noise while retaining strict gates.
