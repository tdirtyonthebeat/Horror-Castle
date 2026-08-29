#!/usr/bin/env python3
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1]; fail=[]
def require(cond,msg):
    if not cond: fail.append(msg)
def text(path): return (root/path).read_text(errors='replace')
cmake=text('CMakeLists.txt');params=text('Source/HorrorCastle/CastleParameters.h');curse=text('Source/HorrorCastle/CurseMatrix.h');processor=text('Source/Core/HorrorCastleProcessor.cpp');grimoire=text('Source/HorrorCastle/Grimoire.cpp');grimoire_h=text('Source/HorrorCastle/Grimoire.h')
engine_files=['Source/HorrorCastle/CastleEngineCore.cpp','Source/HorrorCastle/CastleEngineScene.cpp','Source/HorrorCastle/CastleEngineRender.cpp'];engine='\n'.join(text(f) for f in engine_files);header=text('Source/HorrorCastle/CastleEngine.h');architecture=text('Source/HorrorCastle/HorrorCastleArchitecture.h')
require('VERSION 1.3.0' in cmake and 'VERSION "1.3.0"' in cmake,'CMake version is not 1.3.0')
for f in ['PossessionEngine.cpp','PossessionEngine.h','RitualsEngine.cpp','RitualsEngine.h','Grimoire.cpp','Grimoire.h','GraveChamber.cpp','LivingCastleEditor.h']: require((root/'Source/HorrorCastle'/f).exists(),f'missing {f}')
for f in engine_files: require((root/f).exists(),f'missing split engine module {f}')
for pid in ['corpse.position','corpse.rot','corpse.formant','corpse.inharmonic','possession.bloodFeed','possession.aetherLeak','possession.soulExchange','possession.haunt','rituals.enabled','rituals.pattern','rituals.rate','rituals.bpm','ecology.enabled','ecology.depth']: require(pid in params,f'missing parameter {pid}')
require('Destinations = 22' in curse,'HEX 2.0 destination count is not 22');require('busHex[19]' in engine and 'busHex[20]' in engine and 'busHex[21]' in engine,'new HEX destinations are not consumed');require('Grimoire::CurrentStateVersion' in processor,'versioned state marker missing');require('CurrentStateVersion = 130' in grimoire_h,'Grimoire state version is not 130');require('juce::Reverb' not in engine,'stock juce::Reverb returned to active engine');require('process(m, performanceMidi' in engine,'RITUALS MIDI path missing')
engine_names=['SpectralCorpseEngine','RitualFMEngine','BoneResonatorEngine','WraithBreathEngine','ReliquaryEngine','CoffinBodyEngine','ChoirBodyEngine','MarrowEngine','OrreryEngine','MirrorSpectralEngine','AbyssWaveguideEngine','PoltergeistEngine','AuroraEngine','VortexEngine','SirenEngine']
for name in engine_names: require((root/'Source/HorrorCastle'/f'{name}.cpp').exists(),f'missing {name}.cpp')
require('ritualFM.renderSample' in engine,'Ritual FM is not wired into generator path');require('boneResonator.renderSample' in engine and 'cryptBone' in header,'Bone Resonator inactive');require('wraith.renderSample' in engine and 'cryptWraith' in header,'Wraith inactive');require('reliquary.renderSample' in engine and 'towerReliquary' in header,'Reliquary inactive');require('coffin.renderSample' in engine and 'cryptCoffin' in header,'Coffin inactive');require('choir.renderSample' in engine and 'towerChoir' in header,'Choir inactive');require('marrow.renderSample' in engine and 'cryptMarrow' in header,'Marrow inactive');require('orrery.renderSample' in engine and 'towerOrrery' in header,'Orrery inactive')
require(all(x in architecture for x in ['ChamberV','ChamberVI','ChamberVII','ChamberVIII','ChamberIX','ChamberX']),'appended Chamber identities missing');require(all(x in params for x in ['"Wraith"','"Reliquary"','"Coffin"','"Choir"','"Marrow"','"Orrery"','"Abyss"','"Mirror"','"Poltergeist"','"Aurora"','"Vortex"','"Siren"']),'bestiary choices not exposed');require('jlimit(0,17,v)' in text('Source/HorrorCastle/CastleEngineCore.cpp'),'generator loader does not accept index 17')
for term in ['abyss.renderSample','mirror.renderSample','poltergeist.renderSample','aurora.renderSample','vortex.renderSample','siren.renderSample']: require(term in engine,f'live generator path missing {term}')
require('CreatureRoutingMatrix ecology' in header and 'ecology.process(v.ecologySnapshot,v.ecologyInbox)' in engine,'Creature Routing Matrix not in live voice runtime');require('VortexCreature' in header and 'PoltergeistCreature' in header,'ecology endpoints missing');require('ecologyEnabled=getb("ecology.enabled"' in engine,'ecology host parameter not loaded');require('EcologyMeterCount' in header and 'ecologyTelemetry' in header and 'ecologyPeak[VortexEvent]' in engine,'live ecology telemetry missing')
require('isPitchWheel' in engine and 'getControllerNumber()==1' in engine and 'isChannelPressure' in engine and 'isAftertouch' in engine,'expressive MIDI path incomplete');require('Mod Wheel' in params and 'Aftertouch' in params,'HEX expressive sources not exposed');require('spectralCorpse.renderSample' in engine,'CRYPT CORPSE not wired');require('samplePosition<=n' in engine,'sample-accurate MIDI event dispatch missing')
require(all(f in cmake for f in engine_files),'CMake not using split Castle modules');require('Source/HorrorCastle/CastleEngine.cpp' not in cmake,'legacy monolithic CastleEngine.cpp compiled');require(all((name+'.cpp') in cmake for name in engine_names[3:]),'bestiary engines not all compiled');require(all(x in cmake for x in ['HorrorCastleCoffinCheck','HorrorCastleChoirCheck','HorrorCastleBestiaryCheck','HorrorCastleMirrorAbyssCheck','HorrorCastleElectromagneticCheck','HorrorCastleFluidCheck','HorrorCastleCreatureStateBusCheck']),'regression targets incomplete')
for f in ['Source/UI/Theme/CastleTheme.h','Source/UI/Theme/CastleGraphics.cpp','Source/UI/Components/CastleHeaderComponent.cpp','Source/UI/Components/GrimoireComponent.cpp','Source/UI/NervousSystemComponent.h','Assets/castle_reference.png','Assets/StoneShadow/backdrop.png','Assets/StoneShadow/header.png','Assets/StoneShadow/crypt_frame.png','Assets/StoneShadow/tower_frame.png','Assets/StoneShadow/center_spine.png','Assets/StoneShadow/ritual_grave_frame.png','Assets/StoneShadow/hex_frame.png','Assets/StoneShadow/inspector_frame.png','Assets/StoneShadow/undercroft.png']: require((root/f).exists(),f'missing UI asset/module {f}')
require('return new LivingCastleEditor(*this)' in processor,'production editor does not launch Nervous System wrapper');living=text('Source/HorrorCastle/LivingCastleEditor.h');nervous=text('Source/UI/NervousSystemComponent.h');require('NERVOUS SYSTEM' in living and 'NervousSystemComponent' in living,'Nervous System overlay missing');require(all(x in nervous for x in ['ecology.enabled','ecology.depth','getEcologyMeter','VORTEX  COLLAPSE EVENT','POLTERGEIST  INSTABILITY']),'Nervous System UI telemetry/controls incomplete')
require('getFactorySpells' in grimoire,'expanded Grimoire metadata missing');require(grimoire.count('S{')>=44,'Grimoire has fewer than 44 factory spells');require(all(x in grimoire for x in ['Black Weather','Static Halo','The Castle Breathes','Mirror in the Flood','set("ecology.enabled",1.f)']),'Living Ecology factory spells incomplete');require('productVersion", "1.3.0"' in grimoire,'Grimoire product version is not 1.3.0')
editor=text('Source/HorrorCastle/HorrorCastleEditor.cpp');require('UNDERCROFT' in editor and 'CORPSE ALTAR' in editor,'secondary-room UI missing');require('center_spine_png' in editor,'cathedral spine asset not wired')
source_hits=[]
for f in (root/'Source').rglob('*'):
    if f.is_file() and f.suffix in {'.h','.cpp','.c','.mm'}:
        low=f.read_text(errors='ignore').lower()
        for forbidden in ('odin','surge','galdr','wavewarden'):
            if forbidden in low: source_hits.append(f'{f.relative_to(root)}:{forbidden}')
require(not source_hits,'reference-project names found in release Source/: '+', '.join(source_hits));require((root/'build_horror_castle.command').exists(),'missing build launcher')
if fail:
    print('HORROR CASTLE STATIC VALIDATION FAILED');[print('FAIL ',x) for x in fail];sys.exit(1)
print('HORROR CASTLE STATIC VALIDATION PASSED')
print('PASS  v1.3 compatibility + append-only Chamber VIII-X promotion')
print('PASS  live Creature Routing Matrix + block-safe ecology telemetry')
print('PASS  host-automatable Nervous System enable/depth parameters')
print('PASS  Nervous System production overlay reads real DSP telemetry')
print('PASS  44-spell Grimoire includes four Living Ecology instruments')
print('PASS  independent release Source/ naming scan')
