#include "CastleEngine.h"
#include <algorithm>

namespace horrorcastle {
namespace {
static float clip(float x){return std::tanh(x);}
}

void CastleEngine::render(juce::AudioBuffer<float>& b, juce::MidiBuffer& m)
{
    b.clear();
    juce::MidiBuffer performanceMidi;
    rituals.process(m, performanceMidi, b.getNumSamples());
    auto midiIt = performanceMidi.begin();
    const auto midiEnd = performanceMidi.end();

    auto* L = b.getWritePointer(0);
    auto* R = b.getNumChannels() > 1 ? b.getWritePointer(1) : nullptr;
    delay.setDelay(delayTimeSamples);

    std::array<float, CurseMatrix::Lanes> lanePeak{};
    std::array<float, CurseMatrix::Destinations> destinationPeak{};

    for (int n = 0; n < b.getNumSamples(); ++n)
    {
        while (midiIt != midiEnd && (*midiIt).samplePosition <= n)
        {
            const auto meta = *midiIt;
            const auto msg = meta.getMessage();
            if (msg.isNoteOn()) on(msg.getNoteNumber(), msg.getVelocity());
            else if (msg.isNoteOff()) off(msg.getNoteNumber());
            else if (msg.isPitchWheel())
                pitchBendSemitones = ((float) msg.getPitchWheelValue() - 8192.0f) / 8192.0f * 2.0f;
            else if (msg.isController() && msg.getControllerNumber() == 1)
                modWheel = juce::jlimit(0.0f, 1.0f, msg.getControllerValue() / 127.0f);
            else if (msg.isChannelPressure())
                channelPressure = juce::jlimit(0.0f, 1.0f, msg.getChannelPressureValue() / 127.0f);
            else if (msg.isAftertouch())
            {
                const int note = msg.getNoteNumber();
                const float pressure = juce::jlimit(0.0f, 1.0f, msg.getAfterTouchValue() / 127.0f);
                for (auto& voice : voices)
                    if (voice.active && voice.note == note) voice.polyPressure = pressure;
            }
            else if (msg.isAllNotesOff() || msg.isAllSoundOff()) reset();
            ++midiIt;
        }
        float cryptL = 0.f, cryptR = 0.f, towerL = 0.f, towerR = 0.f;
        std::array<float, CurseMatrix::Destinations> busHex{};
        int activeVoices = 0;

        for (auto& v : voices)
        {
            if (!v.active) continue;
            ++activeVoices;

            v.amp.next(sr); v.iron.next(sr);
            if(globalGlide>.001f){const float glideSec=.004f+globalGlide*.72f;const float g=1.f-std::exp(-1.f/(glideSec*(float)sr));v.pitchNote+=((float)v.note-v.pitchNote)*g;}else v.pitchNote=(float)v.note;
            const float mod = (v.iron.value - .5f) * .22f;
            float cl=0.f, cr=0.f, tl=0.f, tr=0.f;
            const float key = juce::jlimit(-1.f, 1.f, (v.note - 60) / 36.f);
            const float random = rnd();
            const float blood = v.amp.value;
            const float wraith = v.iron.value;

            const float pressure = juce::jlimit(0.f, 1.f, std::max(channelPressure, v.polyPressure));
            auto hx = hex.evaluate(blood, wraith, v.velocity, key, random, modWheel, pressure, 1.f / (float)sr);
            const float hexScale = juce::jlimit(0.f, 1.f, patch.hexAmount);
            for (int d = 0; d < CurseMatrix::Destinations; ++d)
            {
                hx[(size_t)d] *= hexScale;
                busHex[(size_t)d] += hx[(size_t)d];
                destinationPeak[(size_t)d] = std::max(destinationPeak[(size_t)d], std::abs(hx[(size_t)d]));
            }
            for (int i = 0; i < CurseMatrix::Lanes; ++i)
                lanePeak[(size_t)i] = std::max(lanePeak[(size_t)i], std::abs(hex.getLaneValue(i) * hexScale));

            scene(v, patch.crypt, cl, cr, mod, hx, true);
            scene(v, patch.tower, tl, tr, -mod, hx, false);

            if(patch.crypt.crossSceneFM||patch.tower.crossSceneFM){const float ncL=std::sin(cl*juce::MathConstants<float>::pi+tl*1.35f);const float ncR=std::sin(cr*juce::MathConstants<float>::pi+tr*1.35f);const float ntL=std::sin(tl*juce::MathConstants<float>::pi-cl*1.05f);const float ntR=std::sin(tr*juce::MathConstants<float>::pi-cr*1.05f);cl=.58f*cl+.42f*ncL;cr=.58f*cr+.42f*ncR;tl=.58f*tl+.42f*ntL;tr=.58f*tr+.42f*ntR;}
            if(patch.crypt.crossSceneRing||patch.tower.crossSceneRing){const float ringL=std::tanh(cl*tl*5.f),ringR=std::tanh(cr*tr*5.f);cl=.72f*cl+.28f*ringL;cr=.72f*cr+.28f*ringR;tl=.72f*tl-.28f*ringL;tr=.72f*tr-.28f*ringR;}

            cryptL += cl; cryptR += cr;
            towerL += tl; towerR += tr;

            if (!v.amp.value && v.releasing) v.active = false;
        }

        if (activeVoices > 1)
            for (auto& x : busHex) x /= (float)activeVoices;

        // RITUALS carries a small performance imprint beyond note order: its
        // circular balance and curse pulse animate the pre-Ritual relationship.
        const float ritualBalance=rituals.getBalanceMod();
        const float ritualCurse=rituals.getCursePulse();
        cryptL*=1.f-ritualBalance*.18f;cryptR*=1.f-ritualBalance*.18f;
        towerL*=1.f+ritualBalance*.18f;towerR*=1.f+ritualBalance*.18f;

        // POSSESSION infects the chambers before they enter the Ritual bus.
        float possessedCL=cryptL, possessedCR=cryptR, possessedTL=towerL, possessedTR=towerR;
        possession.processSample(cryptL, cryptR, towerL, towerR,
                                 busHex[17], busHex[18], busHex[19], busHex[20]+ritualCurse*.12f,
                                 possessedCL, possessedCR, possessedTL, possessedTR);

        const float ritualAccent = rituals.getIntensityMod() * .18f + std::abs(ritualCurse)*.08f;
        float ritualL = 0.f, ritualR = 0.f;
        ritual.processSample(possessedCL, possessedCR, possessedTL, possessedTR,
                             busHex[7], busHex[13], busHex[14] + ritualAccent, busHex[15],
                             ritualL, ritualR);

        const float graveMod = busHex[8];
        const float effectiveDelayFeedback = juce::jlimit(0.f,.95f,delayFeedback + busHex[21]*.22f);
        const float dl = delay.popSample(0), dr = delay.popSample(1);
        delay.pushSample(0, ritualL + dr * effectiveDelayFeedback);
        delay.pushSample(1, ritualR + dl * effectiveDelayFeedback);
        const float outL = ritualL + dl * delayMix;
        const float outR = ritualR + dr * delayMix;

        const float graveTone01=juce::jlimit(0.f,1.f,(graveTone-.02f)/.46f + busHex[16]*.32f);
        grave.setParameters(juce::jlimit(0.f,1.f,patch.graveDepth+graveMod*.28f), graveTone01, ritualParams.width);
        float graveWetL=0.f,graveWetR=0.f;
        grave.processSample(outL,outR,graveWetL,graveWetR);
        const float graveMix=juce::jlimit(0.f,1.f,patch.graveDepth+graveMod*.45f);
        const float graveDry=1.f-.32f*graveMix;
        L[n]=clip((outL*graveDry+graveWetL*graveMix)*master);
        if(R)R[n]=clip((outR*graveDry+graveWetR*graveMix)*master);
    }

    for (int i = 0; i < CurseMatrix::Lanes; ++i)
        hexLaneTelemetry[(size_t)i].store(juce::jlimit(0.f,1.f,lanePeak[(size_t)i]), std::memory_order_relaxed);
    for (int d = 0; d < CurseMatrix::Destinations; ++d)
        hexDestinationTelemetry[(size_t)d].store(juce::jlimit(0.f,1.f,destinationPeak[(size_t)d]), std::memory_order_relaxed);
}

} // namespace horrorcastle
