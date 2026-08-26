# Horror Castle v1.0 — Release Checklist

## Required on the Mac build machine

1. `./build_horror_castle.command static`
2. `./build_horror_castle.command clean run`
3. Confirm Standalone launches, remains open and produces audio.
4. Audition all CRYPT/TOWER exclusive engines.
5. Audition all five Ritual modes.
6. Audition all seven Curse identities and several HEX 2.0 destinations.
7. Test all four Possession controls individually and in combination.
8. Enable Rituals; test every pattern, note release, probability, swing and octave range.
9. Load all eight factory Grimoire spells; save and reload at least one user `.hcg` spell.
10. `./build_horror_castle.command qa`

## DAW / plug-in checks before distribution

- VST3 scan and instantiate in at least two VST3 hosts.
- AU scan/instantiate in Logic or another AU host; `auval` must pass.
- automate major controls, including HEX, Possession and Rituals.
- save/reopen sessions and confirm state recall.
- test 44.1, 48, 88.2 and 96 kHz where the host supports them.
- test common buffer sizes including 64, 128, 256, 512 and 1024.
- stress 32-note input, rapid note stealing, all-notes-off and transport restarts.
- check CPU at normal and extreme patches.
- check editor open/close/reopen and repeated plug-in instantiate/destroy.

## Product / provenance checks

- confirm the release archive contains only intended Horror Castle source and documentation.
- run the source naming/provenance scan in `static` validation.
- review all third-party notices and select the appropriate JUCE licensing path.
- do not describe the project as “clean-room” unless the development process actually satisfies that standard.
- complete an independent source/provenance and licensing review before a proprietary commercial release.

## Signing / distribution

Ad-hoc signing used during development is not a distribution strategy. Before shipping, establish the intended Apple Developer ID/notarization workflow and installer/package strategy.
