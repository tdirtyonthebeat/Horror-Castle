cmake_minimum_required(VERSION 3.22)

project(HorrorCastle VERSION 1.3.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "Minimum macOS version")

include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.15 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(HorrorCastle VERSION "1.3.0" COMPANY_NAME "SleepFighterStudios" COMPANY_WEBSITE "" FORMATS VST3 AU Standalone PLUGIN_MANUFACTURER_CODE "Slfs" PLUGIN_CODE "Hcst" PRODUCT_NAME "Horror Castle" IS_SYNTH TRUE NEEDS_MIDI_INPUT TRUE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD TRUE)
juce_generate_juce_header(HorrorCastle)
juce_add_binary_data(HorrorCastleAssets SOURCES Assets/castle_reference.png Assets/StoneShadow/backdrop.png Assets/StoneShadow/header.png Assets/StoneShadow/crypt_frame.png Assets/StoneShadow/tower_frame.png Assets/StoneShadow/center_spine.png Assets/StoneShadow/ritual_grave_frame.png Assets/StoneShadow/hex_frame.png Assets/StoneShadow/inspector_frame.png Assets/StoneShadow/undercroft.png)

target_sources(HorrorCastle PRIVATE
    Source/Core/HorrorCastleProcessor.cpp
    Source/HorrorCastle/HorrorCastleArchitecture.cpp Source/HorrorCastle/CastleEngineCore.cpp Source/HorrorCastle/CastleEngineScene.cpp Source/HorrorCastle/CastleEngineRender.cpp
    Source/HorrorCastle/SpectralCorpseEngine.cpp Source/HorrorCastle/RitualFMEngine.cpp Source/HorrorCastle/BoneResonatorEngine.cpp Source/HorrorCastle/WraithBreathEngine.cpp Source/HorrorCastle/ReliquaryEngine.cpp
    Source/HorrorCastle/CoffinBodyEngine.cpp Source/HorrorCastle/ChoirBodyEngine.cpp Source/HorrorCastle/MarrowEngine.cpp Source/HorrorCastle/OrreryEngine.cpp Source/HorrorCastle/MirrorSpectralEngine.cpp Source/HorrorCastle/AbyssWaveguideEngine.cpp
    Source/HorrorCastle/PoltergeistEngine.cpp Source/HorrorCastle/AuroraEngine.cpp Source/HorrorCastle/VortexEngine.cpp Source/HorrorCastle/SirenEngine.cpp
    Source/HorrorCastle/RitualEngine.cpp Source/HorrorCastle/GraveChamber.cpp Source/HorrorCastle/PossessionEngine.cpp Source/HorrorCastle/RitualsEngine.cpp Source/HorrorCastle/Grimoire.cpp Source/HorrorCastle/CurseMatrix.cpp Source/HorrorCastle/CurseChain.cpp Source/HorrorCastle/HorrorCastleEditor.cpp
    Source/UI/Theme/CastleGraphics.cpp Source/UI/Components/CastleHeaderComponent.cpp Source/UI/Components/GrimoireComponent.cpp Source/UI/HexMatrixComponent.cpp Source/UI/CurseInspectorComponent.cpp)

target_compile_definitions(HorrorCastle PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0 JUCE_DISPLAY_SPLASH_SCREEN=0)
target_link_libraries(HorrorCastle PRIVATE HorrorCastleAssets juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags juce::juce_recommended_lto_flags)

juce_add_console_app(HorrorCastleSignatureCheck PRODUCT_NAME "Horror Castle Signature Check")
juce_generate_juce_header(HorrorCastleSignatureCheck)
target_sources(HorrorCastleSignatureCheck PRIVATE tools/SignatureCheck.cpp Source/HorrorCastle/HorrorCastleArchitecture.cpp Source/HorrorCastle/CastleEngineCore.cpp Source/HorrorCastle/CastleEngineScene.cpp Source/HorrorCastle/CastleEngineRender.cpp Source/HorrorCastle/SpectralCorpseEngine.cpp Source/HorrorCastle/RitualFMEngine.cpp Source/HorrorCastle/BoneResonatorEngine.cpp Source/HorrorCastle/WraithBreathEngine.cpp Source/HorrorCastle/ReliquaryEngine.cpp Source/HorrorCastle/CoffinBodyEngine.cpp Source/HorrorCastle/ChoirBodyEngine.cpp Source/HorrorCastle/MarrowEngine.cpp Source/HorrorCastle/OrreryEngine.cpp Source/HorrorCastle/MirrorSpectralEngine.cpp Source/HorrorCastle/AbyssWaveguideEngine.cpp Source/HorrorCastle/PoltergeistEngine.cpp Source/HorrorCastle/AuroraEngine.cpp Source/HorrorCastle/VortexEngine.cpp Source/HorrorCastle/SirenEngine.cpp Source/HorrorCastle/RitualEngine.cpp Source/HorrorCastle/GraveChamber.cpp Source/HorrorCastle/PossessionEngine.cpp Source/HorrorCastle/RitualsEngine.cpp Source/HorrorCastle/Grimoire.cpp Source/HorrorCastle/CurseMatrix.cpp Source/HorrorCastle/CurseChain.cpp)
target_compile_definitions(HorrorCastleSignatureCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleSignatureCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleLivingEnginesCheck PRODUCT_NAME "Horror Castle Living Engines Check")
juce_generate_juce_header(HorrorCastleLivingEnginesCheck)
target_sources(HorrorCastleLivingEnginesCheck PRIVATE tools/LivingEnginesCheck.cpp Source/HorrorCastle/RitualFMEngine.cpp Source/HorrorCastle/BoneResonatorEngine.cpp Source/HorrorCastle/WraithBreathEngine.cpp Source/HorrorCastle/ReliquaryEngine.cpp)
target_compile_definitions(HorrorCastleLivingEnginesCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleLivingEnginesCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleCoffinCheck PRODUCT_NAME "Horror Castle Coffin Check")
juce_generate_juce_header(HorrorCastleCoffinCheck)
target_sources(HorrorCastleCoffinCheck PRIVATE tools/CoffinCheck.cpp Source/HorrorCastle/CoffinBodyEngine.cpp)
target_compile_definitions(HorrorCastleCoffinCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleCoffinCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleChoirCheck PRODUCT_NAME "Horror Castle Choir Check")
juce_generate_juce_header(HorrorCastleChoirCheck)
target_sources(HorrorCastleChoirCheck PRIVATE tools/ChoirCheck.cpp Source/HorrorCastle/ChoirBodyEngine.cpp)
target_compile_definitions(HorrorCastleChoirCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleChoirCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleBestiaryCheck PRODUCT_NAME "Horror Castle Bestiary Check")
juce_generate_juce_header(HorrorCastleBestiaryCheck)
target_sources(HorrorCastleBestiaryCheck PRIVATE tools/BestiaryCheck.cpp Source/HorrorCastle/MarrowEngine.cpp Source/HorrorCastle/OrreryEngine.cpp)
target_compile_definitions(HorrorCastleBestiaryCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleBestiaryCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleMirrorAbyssCheck PRODUCT_NAME "Horror Castle Mirror Abyss Check")
juce_generate_juce_header(HorrorCastleMirrorAbyssCheck)
target_sources(HorrorCastleMirrorAbyssCheck PRIVATE tools/MirrorAbyssCheck.cpp Source/HorrorCastle/MirrorSpectralEngine.cpp Source/HorrorCastle/AbyssWaveguideEngine.cpp)
target_compile_definitions(HorrorCastleMirrorAbyssCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleMirrorAbyssCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleElectromagneticCheck PRODUCT_NAME "Horror Castle Electromagnetic Check")
juce_generate_juce_header(HorrorCastleElectromagneticCheck)
target_sources(HorrorCastleElectromagneticCheck PRIVATE tools/ElectromagneticCheck.cpp Source/HorrorCastle/PoltergeistEngine.cpp Source/HorrorCastle/AuroraEngine.cpp)
target_compile_definitions(HorrorCastleElectromagneticCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleElectromagneticCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleFluidCheck PRODUCT_NAME "Horror Castle Fluid Check")
juce_generate_juce_header(HorrorCastleFluidCheck)
target_sources(HorrorCastleFluidCheck PRIVATE tools/FluidCheck.cpp Source/HorrorCastle/VortexEngine.cpp Source/HorrorCastle/SirenEngine.cpp)
target_compile_definitions(HorrorCastleFluidCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleFluidCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)

juce_add_console_app(HorrorCastleCreatureStateBusCheck PRODUCT_NAME "Horror Castle Creature State Bus Check")
juce_generate_juce_header(HorrorCastleCreatureStateBusCheck)
target_sources(HorrorCastleCreatureStateBusCheck PRIVATE tools/CreatureStateBusCheck.cpp)
target_compile_definitions(HorrorCastleCreatureStateBusCheck PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0)
target_link_libraries(HorrorCastleCreatureStateBusCheck PRIVATE juce::juce_audio_utils juce::juce_dsp juce::juce_recommended_config_flags)
