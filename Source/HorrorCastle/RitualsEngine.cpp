#include "RitualsEngine.h"
#include <algorithm>
#include <cmath>

namespace horrorcastle {

void RitualsEngine::prepare(double sampleRate) { sr = juce::jmax(1000.0, sampleRate); reset(); }
void RitualsEngine::reset() { held.fill(Held{}); heldCount=0; step=0; currentNote=-1; noteOffCountdown=-1; samplesToNext=0.0; cursePulse=balanceMod=intensityMod=0.f; }
float RitualsEngine::random01() noexcept { rng^=rng<<13; rng^=rng>>17; rng^=rng<<5; return (float)(rng & 0x00ffffffu)/(float)0x01000000u; }
void RitualsEngine::addHeld(int note, juce::uint8 velocity){ for(int i=0;i<heldCount;++i)if(held[(size_t)i].note==note){held[(size_t)i].velocity=velocity;return;} if(heldCount<(int)held.size())held[(size_t)heldCount++]={note,velocity}; }
void RitualsEngine::removeHeld(int note){ for(int i=0;i<heldCount;++i)if(held[(size_t)i].note==note){for(int j=i;j<heldCount-1;++j)held[(size_t)j]=held[(size_t)(j+1)];held[(size_t)(--heldCount)]={};break;} }

double RitualsEngine::stepSamples() const noexcept {
    static constexpr double beats[] {1.0, .5, .25, .125};
    const int r=juce::jlimit(0,3,params.rate);
    const double base=sr*(60.0/juce::jlimit(30.f,300.f,params.bpm))*beats[r];
    const double swing=(step&1)?(1.0+params.swing*.32):(1.0-params.swing*.32);
    return juce::jmax(8.0,base*swing);
}

RitualsEngine::Held RitualsEngine::chooseNote(){
    if(heldCount<=0)return {};
    std::array<Held,16> sorted=held;
    std::sort(sorted.begin(),sorted.begin()+heldCount,[](const Held&a,const Held&b){return a.note<b.note;});
    const int count=heldCount;
    int idx=0;
    switch(juce::jlimit(0,7,params.pattern)){
        case Ascension: idx=step%count; break;
        case Descent: idx=(count-1)-(step%count); break;
        case Circle:{const int period=juce::jmax(1,count*2-2);const int p=step%period;idx=p<count?p:period-p;break;}
        case Seance: idx=(step%4==3)?(count-1):(step%count); break;
        case Possession: idx=(int)(random01()*count)%count; break;
        case Sacrifice: idx=(step&1)?(count-1):0; break;
        case Chaos: idx=(int)(random01()*count)%count; break;
        default: idx=step%count; break;
    }
    auto h=sorted[(size_t)idx];
    const int oct=juce::jlimit(1,4,params.octaves);
    int octave=0;
    if(params.pattern==Ascension||params.pattern==Procession)octave=(step/count)%oct;
    else if(params.pattern==Descent)octave=(oct-1)-((step/count)%oct);
    else if(params.pattern==Seance)octave=(step%oct);
    else if(params.pattern==Chaos||params.pattern==Possession)octave=(int)(random01()*oct)%oct;
    h.note=juce::jlimit(0,127,h.note+12*oct);
    return h;
}

void RitualsEngine::process(const juce::MidiBuffer& input, juce::MidiBuffer& output, int numSamples){
    output.clear();
    if(!params.enabled){
        if(currentNote>=0){output.addEvent(juce::MidiMessage::noteOff(1,currentNote),0);currentNote=-1;}
        heldCount=0;noteOffCountdown=-1;samplesToNext=0.0;
        output.addEvents(input,0,numSamples,0);return;
    }

    auto it=input.begin(); const auto end=input.end();
    for(int n=0;n<numSamples;++n){
        while(it!=end && (*it).samplePosition<=n){
            const auto meta=*it; const auto msg=meta.getMessage();
            if(msg.isNoteOn())addHeld(msg.getNoteNumber(),msg.getVelocity());
            else if(msg.isNoteOff())removeHeld(msg.getNoteNumber());
            else if(msg.isAllNotesOff()||msg.isAllSoundOff()){heldCount=0;if(currentNote>=0){output.addEvent(juce::MidiMessage::noteOff(1,currentNote),n);currentNote=-1;}}
            else output.addEvent(msg,n);
            ++it;
        }

        if(noteOffCountdown==0 && currentNote>=0){output.addEvent(juce::MidiMessage::noteOff(1,currentNote),n);currentNote=-1;noteOffCountdown=-1;}
        if(noteOffCountdown>0)--noteOffCountdown;

        if(samplesToNext<=0.0){
            if(currentNote>=0){output.addEvent(juce::MidiMessage::noteOff(1,currentNote),n);currentNote=-1;}
            if(heldCount>0 && random01()<=juce::jlimit(0.f,1.f,params.probability)){
                const auto chosen=chooseNote();
                if(chosen.note>=0){
                    currentNote=chosen.note; output.addEvent(juce::MidiMessage::noteOn(1,currentNote,chosen.velocity),n);
                    noteOffCountdown=juce::jmax(1,(int)std::lround(stepSamples()*juce::jlimit(.05f,.98f,params.gate)));
                }
            }
            cursePulse=(step%4==0)?1.f:((step&1)?.25f:-.25f);
            balanceMod=std::sin(juce::MathConstants<float>::twoPi*(float)(step%8)/8.f);
            intensityMod=(params.pattern==Sacrifice&&((step&3)==3))?1.f:(params.pattern==Possession?.65f:.25f);
            samplesToNext+=stepSamples(); ++step;
        }
        samplesToNext-=1.0;
    }
}

} // namespace horrorcastle
