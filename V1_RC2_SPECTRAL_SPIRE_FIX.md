# Horror Castle v1.0 RC2 — Spectral Spire QA Fix

The first v1.0 Product Candidate compiled and launched successfully on AppleClang/JUCE.
Its QA suite had one failure: `SPECTRAL SPIRE lives above CORPSE`. All other v1 tests passed.

RC2 changes only the TOWER-exclusive Spectral Spire synthesis profile and the associated QA diagnostics.

Spectral Spire now concentrates energy in sparse upper partials near 3.019x, 5.071x, 9.173x, and 13.127x while keeping only a trace of the fundamental.

The QA checker now prints:

`INFO  exclusive brightness(CORPSE/SPIRE)=.../... ratio=...`

The release gate remains unchanged: Spire must exceed Corpse brightness by at least 10%.
