#!/usr/bin/env python3
from pathlib import Path
import sys, re
root=Path(__file__).resolve().parents[1]
fail=[]
def require(cond,msg):
    if not cond: fail.append(msg)

def text(path): return (root/path).read_text(errors='replace')
cmake=text('CMakeLists.txt'); params=text('Source/HorrorCastle/CastleParameters.h'); engine=text('Source/HorrorCastle/CastleEngine.cpp'); curse=text('Source/HorrorCastle/CurseMatrix.h'); processor=text('Source/Core/HorrorCastleProcessor.cpp')
require('VERSION 1.3.0' in cmake and 'VERSION "1.3.0"' in cmake,'CMake version is not 1.3.0')
for f in ['PossessionEngine.cpp','PossessionEngine.h','RitualsEngine.cpp','RitualsEngine.h','Grimoire.cpp','Grimoire.h','GraveChamber.cpp']:
    require((root/'Source/HorrorCastle'/f).exists(),f'missing {f}')
for pid in ['corpse.position','corpse.rot','corpse.formant','corpse.inharmonic','possession.bloodFeed','possession.aetherLeak','possession.soulExchange','possession.haunt','rituals.enabled','rituals.pattern','rituals.rate','rituals.bpm']:
    require(pid in params,f'missing parameter {pid}')
require('Destinations = 22' in curse,'HEX 2.0 destination count is not 22')
require('busHex[19]' in engine and 'busHex[20]' in engine and 'busHex[21]' in engine,'new HEX destinations are not consumed')
require('Grimoire::CurrentStateVersion' in processor,'versioned state marker missing')
require('juce::Reverb' not in engine,'stock juce::Reverb returned to active engine')
require('process(m, performanceMidi' in engine,'RITUALS MIDI path missing')
require((root/'Source/HorrorCastle/SpectralCorpseEngine.cpp').exists(),'missing SpectralCorpseEngine.cpp')
require((root/'Source/HorrorCastle/RitualFMEngine.cpp').exists(),'missing RitualFMEngine.cpp')
require('ritualFM.renderSample' in engine,'Ritual FM is not wired into the generator path')
require('isPitchWheel' in engine and 'getControllerNumber() == 1' in engine and 'isChannelPressure' in engine and 'isAftertouch' in engine,'expressive MIDI path is incomplete')
require('Mod Wheel' in params and 'Aftertouch' in params,'HEX expressive sources are not exposed')
require('spectralCorpse.renderSample' in engine,'CRYPT CORPSE is not wired to spectral resynthesis')
require('samplePosition <= n' in engine,'sample-accurate MIDI event dispatch missing')
require('modWheel, pressure' in engine,'HEX live expression values are not forwarded')
for f in ['Source/UI/Theme/CastleTheme.h','Source/UI/Theme/CastleGraphics.cpp','Source/UI/Components/CastleHeaderComponent.cpp','Source/UI/Components/GrimoireComponent.cpp','Assets/castle_reference.png','Assets/StoneShadow/backdrop.png','Assets/StoneShadow/header.png','Assets/StoneShadow/crypt_frame.png','Assets/StoneShadow/tower_frame.png','Assets/StoneShadow/center_spine.png','Assets/StoneShadow/ritual_grave_frame.png','Assets/StoneShadow/hex_frame.png','Assets/StoneShadow/inspector_frame.png','Assets/StoneShadow/undercroft.png']:
    require((root/f).exists(),f'missing v1.1 UI asset/module {f}')
require('getFactorySpells' in text('Source/HorrorCastle/Grimoire.cpp'),'expanded Grimoire metadata missing')
require(text('Source/HorrorCastle/Grimoire.cpp').count('S{') >= 40,'Grimoire has fewer than 40 factory spell metadata entries')
editor=text('Source/HorrorCastle/HorrorCastleEditor.cpp')
for pid in ['corpse.position','corpse.rot','corpse.formant','corpse.inharmonic']:
    require(pid in editor,f'Spectral Corpse control {pid} is not exposed in editor')
require('UNDERCROFT' in editor and 'CORPSE ALTAR' in editor,'Stone & Shadow secondary-room UI is missing')
require('center_spine_png' in editor,'Stone & Shadow cathedral spine asset is not wired')
# Release source must not contain copied historical project names.
source_hits=[]
for f in (root/'Source').rglob('*'):
    if f.is_file() and f.suffix in {'.h','.cpp','.c','.mm'}:
        low=f.read_text(errors='ignore').lower()
        for forbidden in ('odin','surge','galdr','wavewarden'):
            if forbidden in low: source_hits.append(f'{f.relative_to(root)}:{forbidden}')
require(not source_hits,'reference-project names found in release Source/: '+', '.join(source_hits))
# Shell syntax is checked by the launcher itself on macOS; ensure file is present/executable-ish.
require((root/'build_horror_castle.command').exists(),'missing build launcher')
if fail:
    print('HORROR CASTLE STATIC VALIDATION FAILED')
    for x in fail: print('FAIL ',x)
    sys.exit(1)
print('HORROR CASTLE STATIC VALIDATION PASSED')
print('PASS  v1.3 version metadata')
print('PASS  Possession / Rituals / Grimoire source modules')
print('PASS  HEX 2.0 destination architecture')
print('PASS  sample-accurate MIDI dispatch')
print('PASS  independent release Source/ naming scan')
print('PASS  Stone & Shadow hybrid skin + 40-spell Grimoire + Spectral Corpse UI')
print('PASS  Living Engines Ritual FM + expressive MIDI + one-click Grimoire')
