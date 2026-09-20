# OpenAudio Reference Ledger — Horror Castle Living Engines

Reference catalogs:

- https://openaudio.webprofusion.com
- https://github.com/webprofusion/OpenAudio

## Provenance rule

OpenAudio is a discovery catalog, not a blanket license grant. Horror Castle uses these projects to study synthesis concepts, signal-flow patterns, performance behavior, testing strategy, and UX. Active Horror Castle DSP is independently implemented unless a future change explicitly documents a permissively licensed dependency after license/provenance review.

Do not copy source from GPL/copyleft projects into proprietary Horror Castle. Do not assume a project is permissive because it appears in OpenAudio. Audit the exact repository, commit, dependency graph, and license before any direct reuse.

## High-value synthesis references

### Frequency / phase modulation

- Dexed — six-operator DX-style FM architecture; study operator envelopes, ratios, feedback behavior, voice scaling, and patch ergonomics.
- hexter — another DX-oriented implementation; useful for comparing operator/algorithm design choices rather than treating one implementation as canonical.
- Oxe FM Synth — eight-operator FM; useful for larger operator-network and multitimbral ideas.
- ADLplug / JuceOPLVSTi / VST2413 — chip-FM families; useful for constrained ratio sets, feedback character, and deliberately limited timbral identities.
- OctaSine / WebDX7 — additional FM implementations for parameterization and performance comparison.

Horror Castle direction: Ritual FM remains an in-house continuous graph-morph engine rather than a DX algorithm clone.

### Virtual analog / subtractive / modulation architecture

- Helm — modulation-heavy polyphonic synth; study modulation routing, smoothing, polyphonic voice behavior, and performance-oriented control.
- amsynth / OB-Xd / Odin2 / Synister — compare oscillator/filter/envelope organization and analog-style voice architecture.
- Surge — wavetable/subtractive hybrid architecture, modulation, oscillator variety, and production-grade voice management.
- JS80P — performance-oriented synth behavior and expressive control philosophy.
- CMBNEX — per-parameter modulation slots and oscillator-combiner concepts.

Horror Castle direction: preserve CRYPT/TOWER chamber topology, but deepen per-voice expression, modulation smoothing, and nonlinear filter identity.

### Wavetable / spectral / additive

- Vital — spectral-warping wavetable concepts and high-density modulation UX.
- Vaporizer2 — hybrid wavetable/additive/subtractive/sampler architecture.
- Wavetable — compact two-oscillator wavetable implementation with flexible modulation.
- Aeolus / OwlBass / zynaddsubfx — additive synthesis organization and partial management.
- Coincident Spectra — spectral/microtonal additive ideas.
- Spectral Forge / DtBlkFx / ANATOMY — frequency-domain transformation and decomposition concepts.

Horror Castle direction: evolve Spectral Corpse toward animated spectral trajectories, controlled partial birth/death, formant-preserving motion, and chamber-aware spectral decay without cloning another spectral synth.

### Physical / modal / resonant synthesis

- Modal Synthesiser — resonator-bank physical modeling for material-like wood/metal/glass timbres.
- Resonarium — MPE-compatible coupled waveguide physical modeling and exploratory performance design.
- MechanOdd — polyphonic physical modeling.
- Harpejji-VST / Distrho Kars / EP-MK1 — string, delay-line, and electric-piano modeling approaches.
- Hammer & Meißel — keytracked modal-filter behavior.
- Chorus Ex Machina — physically modeled ensemble/chorus synthesis concepts.

Horror Castle direction: deepen Bone Resonator and future material engines through independently designed modal banks, damped exciters, coupling, and per-note expression.

### Phase distortion / waveshaping / nonlinear identity

- Digits — phase-distortion synthesis architecture.
- RCSiner — phase distortion plus staged waveshaping.
- Wolf Shaper / Overdraw — editable transfer-curve concepts.
- CHOW / Airwindows / Blackheart / Biztortion — nonlinear processing, saturation character, and experimental distortion topology.
- Disflux — phase-dispersion concepts.

Horror Castle direction: use nonlinear stages as chamber-specific topology changes, not generic post-distortion.

### Granular / time-domain texture

- Argotlunar — delay-line granulation.
- BORIS Granular Station — live-input granular processing.
- Eurorack collection — granular and organic processing references among a broader modular set.
- INTERSECT / Ninjas 2 / SAM-SPL 64 — slicing, triggering, and time/pitch organization.

Horror Castle direction: a future Wraith/Haunt texture engine should be voice-integrated and performance-controlled rather than a generic grain effect bolted onto the master bus.

### Reverb / space / delay networks

- Ambience — 16-channel feedback-delay-network reverb architecture.
- DF Zita Rev1 / RS Algorithmic Verb — FDN families and experimental spaces.
- HybridReverb2 — convolution + FDN hybrid thinking.
- Cloud Seed / Dragonfly Reverb / Gverb / MVerb — large algorithmic space and diffusion references.
- AE-LAPSE / IV-X Delay / Cocoa Delay — character-delay and time-domain motion.

Horror Castle direction: Grave remains an original dark-space engine; future work can add time-varying diffusion, frequency-dependent decay, and chamber-to-Grave excitation while retaining independent implementation.

### Expressive control / playability

- Audible Planets — expressive semi-modular instrument design.
- Breathalyzer — compact mapping from performance gestures to timbral dimensions.
- Resonarium — MPE physical-model control.
- Hand Control / GamepadMidi — non-keyboard performance input ideas.
- TeAr / ORchestra — evolving MIDI pattern and sequencer concepts.

Horror Castle direction: velocity, mod wheel, channel pressure, poly pressure, and later MPE should alter timbre/topology—not just amplitude.

## Engineering practices to harvest conceptually

Across the full OpenAudio catalog, evaluate projects for:

1. voice allocation and note-expression handling
2. parameter smoothing and sample-accurate modulation
3. oscillator anti-aliasing and mip/band limiting
4. nonlinear filter stability and oversampling strategy
5. modulation graph organization
6. state migration and preset compatibility
7. deterministic offline DSP tests
8. CPU behavior under high polyphony
9. denormal/NaN/feedback protection
10. UI feedback that reflects live DSP state

Each Horror Castle engine milestone should document which concepts were studied and how our implementation differs.
