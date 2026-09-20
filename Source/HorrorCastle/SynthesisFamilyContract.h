#pragma once
#include "HorrorCastleArchitecture.h"
#include <array>
#include <cstddef>

namespace horrorcastle {

// Synthesis Family Contract sits above the per-creature DSP implementation.
// A generator is not allowed to be "just another oscillator": it declares the
// behavioral laws that define how it is excited, how spectra move, how notes
// articulate, how it distorts, how it occupies stereo, and how MORPH travels.
//
// The text fields teach the UI. The numeric fields enforce the same contract in
// the audio path, so documentation and sound cannot silently drift apart.
struct SynthesisFamilyContract
{
    const char* creature;
    const char* family;

    const char* excitation;
    const char* synthesisMethod;
    const char* spectralMotion;
    const char* articulationLaw;
    const char* nonlinearity;
    const char* stereoLaw;
    const char* morphTrajectory;

    float morphCurve;          // <1 early transformation, >1 late transformation
    float articulationPower;   // amplitude-envelope curvature
    float transientBoost;      // IRON/transient emphasis
    float sustainBias;         // body weight after transient
    float clarity;             // local high-frequency recovery
    float nonlinearDrive;      // creature-contract intensity
    float stereoMotion;        // independent per-slot stereo behavior
};

namespace synthesis_contract {

inline constexpr std::array<SynthesisFamilyContract, 8> common {{
    { "WEREWOLF", "VA / Subtractive",
      "band-limited oscillator edge", "virtual analog waveform + subtractive body", "harmonic series thickens and duty bends",
      "fast organic bite into muscular sustain", "asymmetric soft saturation", "mostly centered with growl widening", "tame wave -> feral pulse",
      .82f, .82f, .16f, .92f, .09f, .72f, .12f },

    { "VAMPIRE", "Wavetable Spectral",
      "phase-coherent table voice", "spectral wavetable interpolation", "harmonic emphasis glides upward through the table",
      "silky onset and long controlled body", "elegant harmonic bite", "narrow core with polished halo", "dark fundamental -> bright aristocratic spectrum",
      1.08f, 1.28f, .05f, .98f, .15f, .42f, .18f },

    { "SORCERER", "FM / Nonlinear",
      "carrier struck by operator energy", "multi-rate frequency modulation", "sidebands multiply and spread inharmonically",
      "bright spell-strike into controlled decay", "feedback-like phase density", "focused carrier with orbiting sidebands", "simple ratio -> dense arcane sidebands",
      1.35f, .48f, .42f, .78f, .22f, .88f, .24f },

    { "WITCH", "Phase Modulation",
      "phase-warped carrier", "nonlinear phase modulation", "phase folds generate moving upper partials",
      "quick unstable onset with elastic sustain", "sine-through-phase folding", "slight wandering antiphase shimmer", "gentle bend -> warped spell geometry",
      1.22f, .62f, .34f, .82f, .20f, .82f, .20f },

    { "SHAPESHIFTER", "Vector Hybrid",
      "four-corner oscillator field", "continuous vector cross-synthesis", "energy migrates between harmonic geometries",
      "even body designed for continuous mutation", "crossfade plus restrained saturation", "motion follows vector travel", "sine -> triangle -> saw -> hollow octave",
      .92f, 1.08f, .10f, .94f, .12f, .38f, .30f },

    { "GREMLIN", "Digital / Bitwise",
      "hard digital edge", "pulse/staircase digital synthesis", "quantization and duty-cycle artifacts dominate",
      "instant brittle snap", "quantization and hard limiting", "small jittered digital width", "clean pulse -> crushed staircase",
      .72f, .34f, .50f, .70f, .24f, .95f, .16f },

    { "GHOUL", "Noise / Texture",
      "pitched stochastic breath", "noise excitation under periodic gating", "broadband energy becomes rhythmically colored",
      "ragged transient into decaying grit", "noise saturation", "diffuse unstable cloud", "breath -> hungry rasp",
      .76f, .72f, .58f, .64f, .10f, .76f, .38f },

    { "SKELETON", "Modal Additive",
      "impulse-like modal strike", "additive resonant partial bank", "stiffness spreads upper modes away from harmonics",
      "hard strike with ringing modal tail", "resonant soft clipping", "dry center with rattling side motion", "wooden body -> brittle bone",
      1.14f, .55f, .66f, .72f, .08f, .54f, .20f }
}};

inline constexpr std::array<SynthesisFamilyContract, 10> crypt {{
    { "BURIED GIANT", "Subharmonic / Subtractive",
      "subharmonic pressure pulse", "stacked subharmonics into dark subtractive body", "energy sinks toward octave and quarter-frequency pressure",
      "slow pressure swell", "heavy symmetric compression", "monolithic center", "shallow chamber -> crushing depth",
      1.28f, 1.46f, .12f, .92f, .03f, .78f, .05f },

    { "CORPSE", "Spectral Resynthesis",
      "spectral frame excitation", "partial-frame resynthesis", "partials move, rotate, preserve/remap formants and stretch inharmonicity",
      "frame bloom with spectral memory", "spectral redistribution before gentle limiting", "ghostly partial drift", "stable corpse -> decomposed spectral body",
      1.02f, 1.08f, .08f, .96f, .14f, .28f, .28f },

    { "BONE", "Modal Additive",
      "noise/impulse strike", "stiff modal resonator bank", "modal ratios stretch with material stiffness",
      "very fast strike and resonant tail", "body-limited modal saturation", "rattling asymmetric reflections", "hollow bone -> glassy rigid skeleton",
      .84f, .42f, .82f, .62f, .18f, .70f, .22f },

    { "ROTATOR", "AM / Rotational Hybrid",
      "coupled rotating oscillators", "ring/amplitude modulation with phase turning", "sidebands rotate around a mechanical center",
      "mechanical attack with churning sustain", "quadratic asymmetry", "explicit orbital rotation", "slow coupling -> violent rotor",
      1.10f, .78f, .30f, .84f, .08f, .64f, .42f },

    { "WRAITH", "Physical Breath Model",
      "continuous turbulent breath", "coupled membrane + air-column physical model", "breath excites drifting resonant membranes",
      "very slow inhaled bloom", "energy-limited resonator feedback", "diffuse hovering veil", "thin veil -> haunted coupled body",
      1.34f, 1.92f, .02f, 1.00f, .11f, .20f, .46f },

    { "COFFIN", "Body Resonance Model",
      "blunt body impact", "coupled wooden/body resonances", "body modes shift with lid/body geometry",
      "hard thud with wooden sustain", "body compression", "wide box reflections", "closed thump -> resonant opened coffin",
      .88f, .50f, .74f, .68f, .06f, .74f, .22f },

    { "MARROW", "Exciter + Modal",
      "wet noisy exciter", "nonlinear exciter feeding resonant body", "exciter brightness and modes diverge with pressure",
      "sharp wet snap into unstable resonance", "asymmetric exciter fold", "small nervous wobble", "dry click -> living marrow scream",
      .74f, .38f, .88f, .60f, .12f, .92f, .34f },

    { "ABYSS", "Waveguide Physical Model",
      "pressure-fed cavity", "coupled waveguide / subterranean cavity model", "tunnel modes lengthen and collapse under pressure",
      "high-inertia pressure rise", "junction compression and choking", "huge centered low-pressure field", "depth -> pressure -> collapse threshold",
      1.42f, 1.72f, .04f, 1.02f, .02f, .80f, .08f },

    { "POLTERGEIST", "Electrostatic Physical Model",
      "charge accumulation and discharge", "electrostatic plate/arc model", "sparse spikes erupt into broadband electrical sidebands",
      "instant discharge with repeated micro-arcs", "threshold arc folding", "unpredictable left/right spark jumps", "charge -> arc -> unstable haunting",
      .68f, .30f, .96f, .58f, .24f, 1.00f, .58f },

    { "VORTEX", "Chaotic Fluid Model",
      "pressure forcing into rotating cells", "coupled nonlinear fluid-cell model", "coherent rotation breaks into turbulent broadband shedding",
      "rotational spin-up and inertia", "pressure collapse saturation", "continuous circular motion", "flow -> turbulence -> collapse",
      1.24f, 1.38f, .16f, .94f, .08f, .66f, .62f }
}};

inline constexpr std::array<SynthesisFamilyContract, 10> tower {{
    { "BELL GLASS", "Additive / Bell Modal",
      "crystalline strike", "inharmonic additive bell partials", "fixed irrational partials bloom above the fundamental",
      "hard glass strike and ringing tail", "gentle glass limiting", "bright stereo reflection", "clear bell -> splintered glass",
      .88f, .40f, .80f, .66f, .18f, .46f, .26f },

    { "SPIRE", "Additive Spectral",
      "high partial injection", "sparse additive spectral tower", "energy climbs into widely spaced upper partials",
      "needle-fast bright onset", "spectral peak limiting", "tall narrow center with high shimmer", "shoulder -> crown -> spectral needle",
      1.18f, .58f, .52f, .78f, .25f, .54f, .30f },

    { "ASTRAL FM", "FM / Orbital Hybrid",
      "orbital modulators around a carrier", "irrational-ratio FM", "sidebands orbit through golden-ratio relationships",
      "bright arcane onset", "phase-density saturation", "orbiting sideband stereo", "simple orbit -> dense astral network",
      1.36f, .66f, .38f, .84f, .22f, .86f, .44f },

    { "PRISM", "Granular / Refraction",
      "overlapping micro-grain windows", "oscillator-granular refraction", "grain pitch, density and window sharpness scatter energy over time",
      "flickering grain onset", "grain-cloud soft limiting", "wide refracted grain cloud", "long grains -> dense crystalline spray",
      .70f, .92f, .22f, .90f, .26f, .64f, .66f },

    { "RELIQUARY", "Physical Resonator",
      "sympathetic body excitation", "coupled glass/cavity resonators", "glass and cavity modes trade energy",
      "restrained resonant wake", "feedback bounded at resonator boundaries", "sympathetic chamber width", "sealed relic -> singing vessel",
      1.10f, 1.32f, .08f, .98f, .09f, .30f, .32f },

    { "CHOIR", "Formant Additive",
      "harmonic glottal source", "multi-mouth formant resonator bank", "formant geometry moves while mouths drift apart/together",
      "slow collective vocal bloom", "tract resonance limiting", "ensemble mouth spread", "single body -> supernatural congregation",
      1.22f, 2.05f, .01f, 1.04f, .10f, .24f, .50f },

    { "ORRERY", "Additive / Orbital",
      "multiple irrational orbital bodies", "additive oscillator field with coupled orbital perturbation", "partial frequencies perturb around a moving barycenter",
      "measured mechanical bloom", "gravity-field soft warp", "deep orbital stereo travel", "clockwork order -> gravitational instability",
      1.16f, 1.12f, .10f, .96f, .09f, .44f, .56f },

    { "MIRROR", "Spectral Reflection",
      "reflected spectral frame", "recursive spectral reflection / inversion", "partials smear, invert and fracture across reflection passes",
      "reflected attack with regeneration", "spectral fracture fold", "asymmetric reflection field", "clean reflection -> shattered apparition",
      .82f, .76f, .44f, .82f, .20f, .82f, .60f },

    { "AURORA", "Field / Additive Hybrid",
      "electromagnetic field alignment", "moving additive bands coupled by a field model", "harmonic bands align, drift and radiate halos",
      "long luminous bloom", "soft field compression", "very wide slow halo", "aligned bands -> radiant unstable field",
      1.48f, 2.30f, .01f, 1.06f, .15f, .32f, .72f },

    { "SIREN", "Physical Air-Jet",
      "breath pressure into an aperture", "air-jet / edge-tone physical model", "edge locking crosses breathy, stable and overblown regimes",
      "pressure catch then sustained jet", "overblow edge saturation", "narrow jet opening into wide overblow", "closed breath -> edge lock -> violent overblow",
      1.30f, 1.58f, .12f, .98f, .17f, .76f, .48f }
}};

inline constexpr const SynthesisFamilyContract& get(GeneratorType type, bool isCrypt) noexcept
{
    const auto i = static_cast<std::size_t>(type);
    if (i < common.size()) return common[i];
    const auto chamber = i - common.size();
    return isCrypt ? crypt[chamber] : tower[chamber];
}

} // namespace synthesis_contract
} // namespace horrorcastle
