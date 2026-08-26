cmake_minimum_required(VERSION 3.22)

project(HorrorCastle VERSION 1.2.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.13 CACHE STRING "Minimum macOS version")

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.15
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(HorrorCastle
    VERSION "1.2.0"
    COMPANY_NAME "SleepFighterStudios"
    COMPANY_WEBSITE ""
    FORMATS VST3 AU Standalone
    PLUGIN_MANUFACTURER_CODE "Slfs"
    PLUGIN_CODE "Hcst"
    PRODUCT_NAME "Horror Castle"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

juce_generate_juce_header(HorrorCastle)

juce_add_binary_data(HorrorCastleAssets
    SOURCES
        Assets/castle_reference.png
        Assets/StoneShadow/backdrop.png
        Assets/StoneShadow/header.png
        Assets/StoneShadow/crypt_frame.png
        Assets/StoneShadow/tower_frame.png
        Assets/StoneShadow/center_spine.png
        Assets/StoneShadow/ritual_grave_frame.png
        Assets/StoneShadow/hex_frame.png
        Assets/StoneShadow/inspector_frame.png
        Assets/StoneShadow/undercroft.png
)

target_sources(HorrorCastle PRIVATE
    Source/Core/HorrorCastleProcessor.cpp

    Source/HorrorCastle/HorrorCastleArchitecture.cpp
    Source/HorrorCastle/CastleEngine.cpp
    Source/HorrorCastle/SpectralCorpseEngine.cpp
    Source/HorrorCastle/RitualEngine.cpp
    Source/HorrorCastle/GraveChamber.cpp
    Source/HorrorCastle/PossessionEngine.cpp
    Source/HorrorCastle/RitualsEngine.cpp
    Source/HorrorCastle/Grimoire.cpp
    Source/HorrorCastle/CurseMatrix.cpp
    Source/HorrorCastle/CurseChain.cpp
    Source/HorrorCastle/HorrorCastleEditor.cpp

    Source/UI/Theme/CastleGraphics.cpp
    Source/UI/Components/CastleHeaderComponent.cpp
    Source/UI/Components/GrimoireComponent.cpp
    Source/UI/HexMatrixComponent.cpp
    Source/UI/CurseInspectorComponent.cpp
)

target_compile_definitions(HorrorCastle PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(HorrorCastle PRIVATE
    HorrorCastleAssets
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)


# Optional headless signature regression check / offline WAV renderer.
juce_add_console_app(HorrorCastleSignatureCheck
    PRODUCT_NAME "Horror Castle Signature Check"
)
juce_generate_juce_header(HorrorCastleSignatureCheck)
target_sources(HorrorCastleSignatureCheck PRIVATE
    tools/SignatureCheck.cpp
    Source/HorrorCastle/HorrorCastleArchitecture.cpp
    Source/HorrorCastle/CastleEngine.cpp
    Source/HorrorCastle/SpectralCorpseEngine.cpp
    Source/HorrorCastle/RitualEngine.cpp
    Source/HorrorCastle/GraveChamber.cpp
    Source/HorrorCastle/PossessionEngine.cpp
    Source/HorrorCastle/RitualsEngine.cpp
    Source/HorrorCastle/Grimoire.cpp
    Source/HorrorCastle/CurseMatrix.cpp
    Source/HorrorCastle/CurseChain.cpp
)
target_compile_definitions(HorrorCastleSignatureCheck PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)
target_link_libraries(HorrorCastleSignatureCheck PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_recommended_config_flags
)
