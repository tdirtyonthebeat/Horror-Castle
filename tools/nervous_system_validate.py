#!/usr/bin/env python3
from pathlib import Path
import sys

root = Path(__file__).resolve().parents[1]
fail = []

def require(cond, msg):
    if not cond:
        fail.append(msg)

def text(path):
    return (root / path).read_text(errors='replace')

required = [
    'Source/HorrorCastle/CreatureStateBus.h',
    'Source/HorrorCastle/CreatureRoutingMatrix.h',
    'Source/HorrorCastle/VortexEngine.h',
    'Source/HorrorCastle/VortexEngine.cpp',
    'Source/HorrorCastle/SirenEngine.h',
    'Source/HorrorCastle/SirenEngine.cpp',
    'Source/HorrorCastle/PoltergeistEngine.h',
    'Source/HorrorCastle/PoltergeistEngine.cpp',
    'Source/HorrorCastle/AuroraEngine.h',
    'Source/HorrorCastle/AuroraEngine.cpp',
    'tools/CreatureStateBusCheck.cpp',
    'tools/FluidCheck.cpp',
    'tools/ElectromagneticCheck.cpp',
]
for path in required:
    require((root / path).exists(), f'missing Nervous System file {path}')

bus = text('Source/HorrorCastle/CreatureStateBus.h')
router = text('Source/HorrorCastle/CreatureRoutingMatrix.h')
vortex_h = text('Source/HorrorCastle/VortexEngine.h')
siren_h = text('Source/HorrorCastle/SirenEngine.h')
polter_h = text('Source/HorrorCastle/PoltergeistEngine.h')
aurora_h = text('Source/HorrorCastle/AuroraEngine.h')
fluid = text('tools/FluidCheck.cpp')
electro = text('tools/ElectromagneticCheck.cpp')
check = text('tools/CreatureStateBusCheck.cpp')

for signal in ['Energy', 'Pressure', 'Motion', 'Instability', 'Event', 'Field']:
    require(signal in bus, f'missing Creature State Bus signal {signal}')

require('MaxRoutes = 32' in router, 'routing matrix capacity changed or missing')
require('sourceSnapshot' in router and 'destinationInbox' in router, 'snapshot/inbox cycle boundary missing')
require('sourceSignal != CreatureStateBus::Signal::Event' in router, 'EVENT bypass semantics missing')
require('std::array<Route, MaxRoutes>' in router, 'routing matrix is not fixed-capacity')

for header, name in [(vortex_h, 'VORTEX'), (siren_h, 'SIREN'), (polter_h, 'POLTERGEIST'), (aurora_h, 'AURORA')]:
    require('CreatureStateBus::State creatureState' in header, f'{name} does not publish creature state')
    require('stateBus(' in header, f'{name} stateBus accessor missing')

require('CreatureRoutingMatrix' in fluid and 'VORTEX EVENT -> SIREN plenum' in fluid,
        'VORTEX/SIREN ecology is not routed through matrix regression')
require('CreatureRoutingMatrix' in electro and 'POLTERGEIST disturbs AURORA FIELD audibly' in electro,
        'POLTERGEIST/AURORA ecology regression missing')
require('snapshot cycle semantics' in check and 'EVENT routing failed' in check,
        'routing matrix safety regression incomplete')

cmake = text('CMakeLists.txt')
workflow = text('.github/workflows/macos-build.yml')
require('HorrorCastleCreatureStateBusCheck' in cmake, 'Nervous System CMake regression target missing')
require('HorrorCastleCreatureStateBusCheck' in workflow, 'Nervous System CI gate missing')

if fail:
    print('HORROR CASTLE NERVOUS SYSTEM VALIDATION FAILED')
    for item in fail:
        print('FAIL ', item)
    sys.exit(1)

print('HORROR CASTLE NERVOUS SYSTEM VALIDATION PASSED')
print('PASS  normalized Creature State Bus contract')
print('PASS  allocation-free fixed-capacity routing matrix')
print('PASS  snapshot cycle boundary + EVENT pulse semantics')
print('PASS  VORTEX -> SIREN routed ecology')
print('PASS  POLTERGEIST -> AURORA routed ecology')
print('PASS  dedicated CMake + macOS CI regression gate')
