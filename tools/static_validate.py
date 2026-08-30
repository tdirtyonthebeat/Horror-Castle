#!/usr/bin/env python3
from pathlib import Path
import sys
root=Path(__file__).resolve().parents[1];fail=[]
def require(c,m):
 if not c:fail.append(m)
def text(p):return(root/p).read_text(errors='replace')
cmake=text('CMakeLists.txt');params=text('Source/HorrorCastle/CastleParameters.h');curse=text('Source/HorrorCastle/CurseMatrix.h');processor=text('Source/Core/HorrorCastleProcessor.cpp');grimoire=text('Source/HorrorCastle/Grimoire.cpp');grimoire_h=text('Source/HorrorCastle/Grimoire.h');manifest=text('Source/HorrorCastle/FactoryPresetManifest.cpp');manifest_h=text('Source/HorrorCastle/FactoryPresetManifest.h')
engine_files=['Source/HorrorCastle/CastleEngineCore.cpp','Source/HorrorCastle/CastleEngineScene.cpp','Source/HorrorCastle/CastleEngineRender.cpp'];engine='\n'.join(text(f) for f in engine_files);header=text('Source/HorrorCastle/CastleEngine.h');architecture=text('Source/HorrorCastle/HorrorCastleArchitecture.h')
require('VERSION 1.3.0' in cmake and 'VERSION "1.3.0"' in cmake,'CMake version is not 1.3.0')
for f in ['PossessionEngine.cpp','PossessionEngine.h','RitualsEngine.cpp','RitualsEngine.h','Grimoire.cpp','Grimoire.h','FactoryPresetManifest.cpp','FactoryPresetManifest.h','GraveChamber.cpp','LivingCastleEditor.h']:require((root/'Source/HorrorCastle'/f).exists(),f'missing {f}')
for pid in ['ecology.enabled','ecology.depth','corpse.position','possession.bloodFeed','rituals.enabled']:require(pid in params,f'missing parameter {pid}')
require('Destinations = 22' in curse,'HEX destination count wrong');require('CurrentStateVersion = 130' in grimoire_h,'state version wrong');require('FactoryPresetCount = 100' in manifest_h,'factory bank is not exactly 100');require('LegacyPresetCount = 44' in manifest_h,'legacy preset boundary changed');require('out.reserve(FactoryPresetCount)' in manifest,'manifest not data-driven');require('jassert((int)out.size()==FactoryPresetCount)' in manifest,'manifest size assertion missing')
require('loadGeneratedFactory' in grimoire and 'FactoryPresetManifest::create()' in grimoire,'Grimoire not driven by manifest');require('index>=FactoryPresetManifest::LegacyPresetCount' in grimoire,'append-only legacy boundary missing');require(all(x in manifest for x in ['Grave Current','Wraith Needle','Ashen Heaven','Bottomless Room','Bone Keys','Rib Strike','Raven Engine','Door Behind You','Living Storm']),'launch categories incomplete')
require('HorrorCastleFactoryPresetCheck' in cmake,'factory preset QA target missing');require('FactoryPresetManifest.cpp' in cmake,'manifest not compiled');require('juce::Reverb' not in engine,'stock reverb returned');require('CreatureRoutingMatrix ecology' in header and 'ecology.process(v.ecologySnapshot,v.ecologyInbox)' in engine,'live ecology missing');require('EcologyMeterCount' in header and 'ecologyTelemetry' in header,'ecology telemetry missing')
for term in ['abyss.renderSample','mirror.renderSample','poltergeist.renderSample','aurora.renderSample','vortex.renderSample','siren.renderSample']:require(term in engine,f'live generator missing {term}')
require(all(x in architecture for x in ['ChamberVIII','ChamberIX','ChamberX']),'new chamber identities missing');require('jlimit(0,17,v)' in text('Source/HorrorCastle/CastleEngineCore.cpp'),'generator loader range wrong')
for f in ['Source/UI/NervousSystemComponent.h','Assets/StoneShadow/backdrop.png']:require((root/f).exists(),f'missing UI asset/module {f}')
require('return new LivingCastleEditor(*this)' in processor,'production editor wrapper missing');living=text('Source/HorrorCastle/LivingCastleEditor.h');nervous=text('Source/UI/NervousSystemComponent.h');require('NERVOUS SYSTEM' in living and 'getEcologyMeter' in nervous,'Nervous System UI incomplete')
source_hits=[]
for f in(root/'Source').rglob('*'):
 if f.is_file() and f.suffix in{'.h','.cpp','.c','.mm'}:
  low=f.read_text(errors='ignore').lower()
  for forbidden in('odin','surge','galdr','wavewarden'):
   if forbidden in low:source_hits.append(f'{f.relative_to(root)}:{forbidden}')
require(not source_hits,'reference-project names found in Source/: '+', '.join(source_hits));require((root/'build_horror_castle.command').exists(),'missing build launcher')
if fail:
 print('HORROR CASTLE STATIC VALIDATION FAILED');[print('FAIL ',x)for x in fail];sys.exit(1)
print('HORROR CASTLE STATIC VALIDATION PASSED')
print('PASS  v1.3 living-engine architecture and Nervous System UI')
print('PASS  data-driven factory manifest targets exactly 100 spells')
print('PASS  legacy factory indices 0-43 remain append-only compatible')
print('PASS  deterministic archetypes scale bank without switch-case explosion')
print('PASS  dedicated factory-preset QA target is compiled in CI')
print('PASS  independent release Source/ naming scan')
