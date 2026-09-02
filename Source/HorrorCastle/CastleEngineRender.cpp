#include "CastleEngine.h"
#include <algorithm>

namespace horrorcastle {
namespace { static float clip(float x){return std::tanh(x);} }

void CastleEngine::render(juce::AudioBuffer<float>& b, juce::MidiBuffer& m)
{
    b.clear(); juce::MidiBuffer performanceMidi; rituals.process(m, performanceMidi, b.getNumSamples()); auto midiIt=performanceMidi.begin(); const auto midiEnd=performanceMidi.end();
    auto* L=b.getWritePointer(0); auto* R=b.getNumChannels()>1?b.getWritePointer(1):nullptr; delay.setDelay(delayTimeSamples);
    std::array<float, CurseMatrix::Lanes> lanePeak{}; std::array<float, CurseMatrix::Destinations> destinationPeak{}; std::array<float,EcologyMeterCount> ecologyPeak{};
    for(int n=0;n<b.getNumSamples();++n){
        while(midiIt!=midiEnd&&(*midiIt).samplePosition<=n){const auto meta=*midiIt;const auto msg=meta.getMessage();if(msg.isNoteOn())on(msg.getNoteNumber(),msg.getVelocity());else if(msg.isNoteOff())off(msg.getNoteNumber());else if(msg.isPitchWheel())pitchBendSemitones=((float)msg.getPitchWheelValue()-8192.f)/8192.f*2.f;else if(msg.isController()&&msg.getControllerNumber()==1)modWheel=juce::jlimit(0.f,1.f,msg.getControllerValue()/127.f);else if(msg.isChannelPressure())channelPressure=juce::jlimit(0.f,1.f,msg.getChannelPressureValue()/127.f);else if(msg.isAftertouch()){const int note=msg.getNoteNumber();const float pressure=juce::jlimit(0.f,1.f,msg.getAfterTouchValue()/127.f);for(auto&voice:voices)if(voice.active&&voice.note==note)voice.polyPressure=pressure;}else if(msg.isAllNotesOff()||msg.isAllSoundOff())reset();++midiIt;}
        float cryptL=0,cryptR=0,towerL=0,towerR=0; std::array<float,CurseMatrix::Destinations> busHex{}; int activeVoices=0;
        for(auto&v:voices){if(!v.active)continue;++activeVoices;v.amp.next(sr);v.iron.next(sr);if(globalGlide>.001f){const float glideSec=.004f+globalGlide*.72f;const float gg=1.f-std::exp(-1.f/(glideSec*(float)sr));v.pitchNote+=((float)v.note-v.pitchNote)*gg;}else v.pitchNote=(float)v.note;
            const float mod=(v.iron.value-.5f)*.22f,key=juce::jlimit(-1.f,1.f,(v.note-60)/36.f),random=rnd(),blood=v.amp.value,wraith=v.iron.value,pressure=juce::jlimit(0.f,1.f,std::max(channelPressure,v.polyPressure));
            auto hx=hex.evaluate(blood,wraith,v.velocity,key,random,modWheel,pressure,1.f/(float)sr);const float hexScale=juce::jlimit(0.f,1.f,patch.hexAmount);for(int d=0;d<CurseMatrix::Destinations;++d){hx[(size_t)d]*=hexScale;busHex[(size_t)d]+=hx[(size_t)d];destinationPeak[(size_t)d]=std::max(destinationPeak[(size_t)d],std::abs(hx[(size_t)d]));}for(int i=0;i<CurseMatrix::Lanes;++i)lanePeak[(size_t)i]=std::max(lanePeak[(size_t)i],std::abs(hex.getLaneValue(i)*hexScale));
            // The inbox is generated from the previous physical snapshot. Producers
            // then publish a fresh snapshot while both scenes render: one-sample
            // causality prevents zero-delay ecological recursion.
            v.ecology.process(v.ecologySnapshot,v.ecologyInbox);
            float cl=0,cr=0,tl=0,tr=0;scene(v,patch.crypt,cl,cr,mod,hx,true);scene(v,patch.tower,tl,tr,-mod,hx,false);
            ecologyPeak[VortexEvent]=std::max(ecologyPeak[VortexEvent],v.ecologySnapshot[VortexCreature].get(CreatureStateBus::Signal::Event));
            ecologyPeak[SirenPressure]=std::max(ecologyPeak[SirenPressure],v.ecologySnapshot[SirenCreature].get(CreatureStateBus::Signal::Pressure));
            ecologyPeak[PoltergeistInstability]=std::max(ecologyPeak[PoltergeistInstability],v.ecologySnapshot[PoltergeistCreature].get(CreatureStateBus::Signal::Instability));
            ecologyPeak[AuroraField]=std::max(ecologyPeak[AuroraField],v.ecologySnapshot[AuroraCreature].get(CreatureStateBus::Signal::Field));
            if(patch.crypt.crossSceneFM||patch.tower.crossSceneFM){const float ncL=std::sin(cl*juce::MathConstants<float>::pi+tl*1.35f),ncR=std::sin(cr*juce::MathConstants<float>::pi+tr*1.35f),ntL=std::sin(tl*juce::MathConstants<float>::pi-cl*1.05f),ntR=std::sin(tr*juce::MathConstants<float>::pi-cr*1.05f);cl=.58f*cl+.42f*ncL;cr=.58f*cr+.42f*ncR;tl=.58f*tl+.42f*ntL;tr=.58f*tr+.42f*ntR;}if(patch.crypt.crossSceneRing||patch.tower.crossSceneRing){const float ringL=std::tanh(cl*tl*5.f),ringR=std::tanh(cr*tr*5.f);cl=.72f*cl+.28f*ringL;cr=.72f*cr+.28f*ringR;tl=.72f*tl-.28f*ringL;tr=.72f*tr-.28f*ringR;}
            cryptL+=cl;cryptR+=cr;towerL+=tl;towerR+=tr;if(!v.amp.value&&v.releasing)v.active=false;}
        if(activeVoices>1)for(auto&x:busHex)x/=(float)activeVoices;
        const float ritualBalance=rituals.getBalanceMod(),ritualCurse=rituals.getCursePulse();cryptL*=1.f-ritualBalance*.18f;cryptR*=1.f-ritualBalance*.18f;towerL*=1.f+ritualBalance*.18f;towerR*=1.f+ritualBalance*.18f;
        float possessedCL=cryptL,possessedCR=cryptR,possessedTL=towerL,possessedTR=towerR;possession.processSample(cryptL,cryptR,towerL,towerR,busHex[17],busHex[18],busHex[19],busHex[20]+ritualCurse*.12f,possessedCL,possessedCR,possessedTL,possessedTR);
        const float ritualAccent=rituals.getIntensityMod()*.18f+std::abs(ritualCurse)*.08f;float ritualL=0,ritualR=0;ritual.processSample(possessedCL,possessedCR,possessedTL,possessedTR,busHex[7],busHex[13],busHex[14]+ritualAccent,busHex[15],ritualL,ritualR);
        const float graveMod=busHex[8],effectiveDelayFeedback=juce::jlimit(0.f,.95f,delayFeedback+busHex[21]*.22f);const float dl=delay.popSample(0),dr=delay.popSample(1);delay.pushSample(0,ritualL+dr*effectiveDelayFeedback);delay.pushSample(1,ritualR+dl*effectiveDelayFeedback);const float outL=ritualL+dl*delayMix,outR=ritualR+dr*delayMix;
        const float graveTone01=juce::jlimit(0.f,1.f,(graveTone-.02f)/.46f+busHex[16]*.32f);grave.setParameters(juce::jlimit(0.f,1.f,patch.graveDepth+graveMod*.28f),graveTone01,ritualParams.width);float graveWetL=0,graveWetR=0;grave.processSample(outL,outR,graveWetL,graveWetR);const float graveMix=juce::jlimit(0.f,1.f,patch.graveDepth+graveMod*.45f),graveDry=1.f-.32f*graveMix;L[n]=clip((outL*graveDry+graveWetL*graveMix)*master);if(R)R[n]=clip((outR*graveDry+graveWetR*graveMix)*master);
    }
    for(int i=0;i<CurseMatrix::Lanes;++i)hexLaneTelemetry[(size_t)i].store(juce::jlimit(0.f,1.f,lanePeak[(size_t)i]),std::memory_order_relaxed);for(int d=0;d<CurseMatrix::Destinations;++d)hexDestinationTelemetry[(size_t)d].store(juce::jlimit(0.f,1.f,destinationPeak[(size_t)d]),std::memory_order_relaxed);for(int i=0;i<EcologyMeterCount;++i)ecologyTelemetry[(size_t)i].store(juce::jlimit(0.f,1.f,ecologyPeak[(size_t)i]),std::memory_order_relaxed);
}

} // namespace horrorcastle
