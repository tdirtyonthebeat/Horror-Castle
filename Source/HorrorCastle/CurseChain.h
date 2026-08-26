#pragma once
#include <array>
#include <cmath>
#include <algorithm>

namespace HorrorCastle
{
    enum class CurseType
    {
        Clean = 0,
        Corrupt,
        Haunt,
        Possession,
        Decay,
        Madness,
        Blood
    };

    enum class ModSource
    {
        None = 0,
        Blood,
        Wraith,
        Velocity,
        Key,
        Random,
        Pulse
    };

    enum class ModDestination
    {
        None = 0,
        CryptCutoff,
        TowerCutoff,
        CryptShape,
        TowerShape,
        FMDepth,
        FilterDrive,
        Ritual,
        Grave,
        Pitch,
        CurseDepth,
        CryptDread,
        TowerAether,
        RitualDepth,
        RitualFury,
        RitualFeedback,
        GraveTone,
        BloodFeed,
        AetherLeak,
        SoulExchange,
        HauntEngine,
        GraveFeedback
    };

    struct CurseLane
    {
        ModSource source = ModSource::None;
        CurseType curse = CurseType::Clean;
        ModDestination destination = ModDestination::None;
        float amount = 0.0f;
        float phase = 0.0f;
        float decay = 1.0f;
        float heldRandom = 0.0f;
        float pulsePhase = 0.0f;
        bool enabled = false;
    };

    class CurseChain
    {
    public:
        static constexpr int maxLanes = 8;
        static constexpr int maxDepth = 3;

        CurseChain();

        void reset();
        void noteOn(float velocity, float key01, float random01);
        void noteOff();
        void setSampleRate(float sr);

        void setLane(int index, const CurseLane& lane);
        const CurseLane& getLane(int index) const noexcept;

        void process(float blood, float wraith, float velocity, float key01,
                     float random01, float deltaSeconds);

        float getDestination(ModDestination d) const noexcept;

        // A controlled second-order path:
        // lane A can modulate the effective depth of lane B when
        // lane B targets CurseDepth and its index is encoded by the
        // destination value supplied by the caller.
        float getLaneValue(int index) const noexcept;

    private:
        std::array<CurseLane, maxLanes> lanes {};
        std::array<float, maxLanes> laneValues {};
        std::array<float, 22> destinations {};

        float sampleRate = 44100.0f;
        float velocityHeld = 0.0f;
        float keyHeld = 0.0f;
        float randomHeld = 0.0f;
        float chainEnergy = 0.0f;

        static float clampBipolar(float v);
        static float applyCurse(CurseType curse, float x, CurseLane& lane,
                                float blood, float wraith, float dt);

        void clearDestinations();
    };
}
