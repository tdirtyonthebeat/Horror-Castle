#!/usr/bin/env python3
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]
fail=[]
def require(cond,msg):
    if not cond: fail.append(msg)
def text(path): return (root/path).read_text(errors='replace')
cmake=text('CMakeLists.txt'); params=text('Source/HorrorCastle/CastleParameters.h'); curse=text('Source/HorrorCastle/CurseMatrix.h'); processor=text('Source/Core/HorrorCastleProcessor.cpp')
engine_files=['Source/HorrorCastle/CastleEngineCore.cpp','Source/HorrorCastle/CastleEngineScene.cpp','Source/HorrorCastle/CastleEngineRender.cpp']
engine='\n'.join(text(f) for f in engine_files); header=text('Source/HorrorCastle/CastleEngine.h'); architecture=text('Source/HorrorCastle/HorrorCastleArchitecture.h')
require('VERSION 1.3.0' in cmake and 'VERSION "1.3.0"' in cmake,'CMake version is not 1.3.0')
for f in ['PossessionEngine.cpp','PossessionEngine.h','RitualsEngine.cpp','RitualsEngine.h','Grimoire.cpp','Grimoire.h','GraveChamber.cpp']:
    require((root/'Source/HorrorCastle'/f).exists(),f'missing {f}')
for f in engine_files: require((root/f).exists(),f'missing split engine module {f}')
for pid in ['corpse.position','corpse.rot','corpse.formant','corpse.inharmonic','possession.bloodFeed','possession.aetherLeak','possession.soulExchange','possession.haunt','rituals.enabled','rituals.pattern','rituals.rate','rituals.bpm']:
    require(pid in params,f'missing parameter {pid}')
require('Destinations = 22' in curse,'HEX 2.0 destination count is not 22')
require('busHex[19]' in engine and 'busHex[20]' in engine and 'busHex[21]' in engine,'new HEX destinations are not consumed')
require('Grimoire::CurrentStateVersion' in processor,'versioned state marker missing')
require('juce::Reverb' not in engine,'stock juce::Reverb returned to active engine')
require('process(m, performanceMidi' in engine,'RITUALS MIDI path missing')

engine_names=['SpectralCorpseEngine','RitualFMEngine','BoneResonatorEngine','WraithBreathEngine','ReliquaryEngine','CoffinBodyEngine','ChoirBodyEngine','MarrowEngine','OrreryEngine','MirrorSpectralEngine','AbyssWaveguideEngine','PoltergeistEngine','AuroraEngine']
for engine_name in engine_names:
    require((root/'Source/HorrorCastle'/f'{engine_name}.cpp').exists(),f'missing {engine_name}.cpp')

require('ritualFM.renderSample' in engine,'Ritual FM is not wired into the generator path')
require('boneResonator.renderSample' in engine and 'cryptBone' in header,'Bone Resonator 2.0 is not active in CRYPT Chamber III')
require('wraith.renderSample' in engine and 'cryptWraith' in header,'Wraith is not active in CRYPT Chamber V')
require('reliquary.renderSample' in engine and 'towerReliquary' in header,'Reliquary is not active in TOWER Chamber V')
require('coffin.renderSample' in engine and 'cryptCoffin' in header,'COFFIN is not active in CRYPT Chamber VI')
require('choir.renderSample' in engine and 'towerChoir' in header,'CHOIR is not active in TOWER Chamber VI')
require('marrow.renderSample' in engine and 'cryptMarrow' in header,'MARROW is not active in CRYPT Chamber VII')
require('orrery.renderSample' in engine and 'towerOrrery' in header,'ORRERY is not active in TOWER Chamber VII')
require(all(x in architecture for x in ['ChamberV','ChamberVI','ChamberVII']),'appended Chamber V/VI/VII generator identities are missing')
require(all(x in params for x in ['"Wraith"','"Reliquary"','"Coffin"','"Choir"','"Marrow"','"Orrery"']),'bestiary parameter choices are not exposed')
require('jlimit(0,14,v)' in text('Source/HorrorCastle/CastleEngineCore.cpp'),'generator loader does not accept appended index 14')

require('isPitchWheel' in engine and 'getControllerNumber() == 1' in engine and 'isChannelPressure' in engine and 'isAftertouch' in engine,'expressive MIDI path is incomplete')
require('Mod Wheel' in params and 'Aftertouch' in params,'HEX expressive sources are not exposed')
require('spectralCorpse.renderSample' in engine,'CRYPT CORPSE is not wired to spectral resynthesis')
require('samplePosition <= n' in engine,'sample-accurate MIDI event dispatch missing')
require('modWheel, pressure' in engine,'HEX live expression values are not forwarded')
require(all(f in cmake for f in ['CastleEngineCore.cpp','CastleEngineScene.cpp','CastleEngineRender.cpp']),'CMake is not using split Castle engine modules')
require('Source/HorrorCastle/CastleEngine.cpp' not in cmake,'legacy monolithic CastleEngine.cpp is still compiled')
require(all((name + '.cpp') in cmake for name in engine_names[3:]),'bestiary engines are not all compiled')
require(all(x in cmake for x in ['HorrorCastleCoffinCheck','HorrorCastleChoirCheck','HorrorCastleBestiaryCheck','HorrorCastleMirrorAbyssCheck','HorrorCastleElectromagneticCheck']),'bestiary regression targets are incomplete')
require((root/'tools/CoffinCheck.cpp').exists() and (root/'tools/ChoirCheck.cpp').exists() and (root/'tools/BestiaryCheck.cpp').exists() and (root/'tools/MirrorAbyssCheck.cpp').exists() and (root/'tools/ElectromagneticCheck.cpp').exists(),'missing bestiary regression checks')

coffin=text('tools/CoffinCheck.cpp'); choir=text('tools/ChoirCheck.cpp'); bestiary=text('tools/BestiaryCheck.cpp'); mirror_abyss=text('tools/MirrorAbyssCheck.cpp'); electromagnetic=text('tools/ElectromagneticCheck.cpp')
require(all(term in coffin for term in ['COFFIN LID','COFFIN DREAD','COFFIN expression','extreme coupling']),'COFFIN gate incomplete')
require(all(term in choir for term in ['CHOIR VOWEL','CHOIR AETHER','CHOIR expression','extreme coupling']),'CHOIR gate incomplete')
require(all(term in bestiary for term in ['MARROW viscosity','MARROW DREAD','stick-slip','ORRERY orbit','ORRERY AETHER']),'MARROW/ORRERY gate incomplete')
require(all(term in mirror_abyss for term in ['MIRROR REFLECTION','MIRROR AETHER','ABYSS DEPTH','ABYSS DREAD']),'MIRROR/ABYSS gate incomplete')
require(all(term in electromagnetic for term in ['POLTERGEIST CHARGE','POLTERGEIST DREAD','AURORA FIELD','AURORA AETHER']),'electromagnetic gate incomplete')

for f in ['Source/UI/Theme/CastleTheme.h','Source/UI/Theme/CastleGraphics.cpp','Source/UI/Components/CastleHeaderComponent.cpp','Source/UI/Components/GrimoireComponent.cpp','Assets/castle_reference.png','Assets/StoneShadow/backdrop.png','Assets/StoneShadow/header.png','Assets/StoneShadow/crypt_frame.png','Assets/StoneShadow/tower_frame.png','Assets/StoneShadow/center_spine.png','Assets/StoneShadow/ritual_grave_frame.png','Assets/StoneShadow/hex_frame.png','Assets/StoneShadow/inspector_frame.png','Assets/StoneShadow/undercroft.png']:
    require((root/f).exists(),f'missing v1.1 UI asset/module {f}')
require('getFactorySpells' in text('Source/HorrorCastle/Grimoire.cpp'),'expanded Grimoire metadata missing')
require(text('Source/HorrorCastle/Grimoire.cpp').count('S{') >= 40,'Grimoire has fewer than 40 factory spell metadata entries')
editor=text('Source/HorrorCastle/HorrorCastleEditor.cpp')
for pid in ['corpse.position','corpse.rot','corpse.formant','corpse.inharmonic']: require(pid in editor,f'Spectral Corpse control {pid} is not exposed in editor')
require('UNDERCROFT' in editor and 'CORPSE ALTAR' in editor,'Stone & Shadow secondary-room UI is missing')
require('center_spine_png' in editor,'Stone & Shadow cathedral spine asset is not wired')
source_hits=[]
for f in (root/'Source').rglob('*'):
    if f.is_file() and f.suffix in {'.h','.cpp','.c','.mm'}:
        low=f.read_text(errors='ignore').lower()
        for forbidden in ('odin','surge','galdr','wavewarden'):
            if forbidden in low: source_hits.append(f'{f.relative_to(root)}:{forbidden}')
require(not source_hits,'reference-project names found in release Source/: '+', '.join(source_hits))
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
print('PASS  Chamber V CRYPT Wraith + TOWER Reliquary')
print('PASS  Chamber VI CRYPT Coffin + TOWER Choir')
print('PASS  Chamber VII CRYPT Marrow + TOWER Orrery')
print('PASS  Chamber VIII research prototypes MIRROR + ABYSS')
print('PASS  Chamber IX research prototypes POLTERGEIST + AURORA')
print('PASS  generator indices 0..13 preserved; Chamber VII appended at 14')
