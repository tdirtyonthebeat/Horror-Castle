#pragma once
#include <JuceHeader.h>
#include <array>

namespace horrorcastle {

class SirenEngine
{
public:
    static constexpr int Jets = 4;
    struct JetState { float phase=0.0f; float displacement=0.0f; float edgeMemory=0.0f; };
    struct VoiceState { std::array<JetState,Jets> jets{}; float plenum=0.0f; float field=0.0f; };

    float renderSample(VoiceState& state,
                       float fundamentalHz,
                       float aperture,
                       float aether,
                       float expression,
                       float velocity,
                       double sampleRate) noexcept;
};

} // namespace horrorcastle
