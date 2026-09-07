#include "HorrorCastleProcessor.h"
#include "../HorrorCastle/LivingCastleEditor.h"

namespace horrorcastle {

static void mergeStateTree(juce::ValueTree& target, const juce::ValueTree& source)
{
    for (int i=0;i<source.getNumProperties();++i) {
        const auto name=source.getPropertyName(i);
        target.setProperty(name,source.getProperty(name),nullptr);
    }
    for (int i=0;i<source.getNumChildren();++i) {
        const auto src=source.getChild(i);
        juce::ValueTree dst;
        if(src.hasProperty("id")) {
            const auto wanted=src.getProperty("id");
            for(int j=0;j<target.getNumChildren();++j) {
                auto c=target.getChild(j);
                if(c.hasProperty("id")&&c.getProperty("id")==wanted){dst=c;break;}
            }
        }
        if(!dst.isValid()) {
            for(int j=0;j<target.getNumChildren();++j) { auto c=target.getChild(j); if(c.getType()==src.getType()){dst=c;break;} }
        }
        if(dst.isValid()) mergeStateTree(dst,src);
        else target.addChild(src.createCopy(),-1,nullptr);
    }
}

HorrorCastleProcessor::HorrorCastleProcessor()
    : juce::AudioProcessor(BusesProperties()
                              .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      parameters(*this, nullptr, juce::Identifier("HorrorCastle"), param::createLayout()),
      grimoire(parameters)
{
}

void HorrorCastleProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    engine.prepare(sampleRate, samplesPerBlock);
    engine.setParameters(parameters);
}

void HorrorCastleProcessor::releaseResources()
{
    engine.reset();
}

void HorrorCastleProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    engine.setParameters(parameters);
    engine.render(buffer, midi);

    // UI telemetry only: bounded, lock-free and allocation-free on the audio thread.
    const int n=buffer.getNumSamples();
    if(n>0){
        const float* l=buffer.getReadPointer(0); const float* r=buffer.getNumChannels()>1?buffer.getReadPointer(1):l;
        double side=0.0,mid=0.0;
        for(int i=0;i<n;++i){const size_t wi=soulWrite.fetch_add(1,std::memory_order_relaxed);soulWave[wi%soulWave.size()].store(.5f*(l[i]+r[i]),std::memory_order_relaxed);const float m=l[i]+r[i],s=l[i]-r[i];mid+=m*m;side+=s*s;}
        soulWidth.store((float)juce::jlimit(0.0,1.0,std::sqrt(side/std::max(1.0e-12,mid))),std::memory_order_relaxed);
        constexpr int bins=64;
        for(int k=0;k<bins;++k){
            const int a=k*n/bins,b=(k+1)*n/bins;double energy=0.0;
            for(int i=a;i<b;++i){const float x=.5f*(l[i]+r[i]);energy+=x*x;}
            const float e=(b>a)?(float)std::sqrt(energy/(b-a)):0.f;
            const float old=soulSpectrum[(size_t)k].load(std::memory_order_relaxed);
            soulSpectrum[(size_t)k].store(std::max(e,old*.86f),std::memory_order_relaxed);
        }
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HorrorCastleProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

juce::AudioProcessorEditor* HorrorCastleProcessor::createEditor()
{
    return new LivingCastleEditor(*this);
}

void HorrorCastleProcessor::copySoulGlass(std::array<float,512>& wave, std::array<float,64>& spectrum, float& width) const noexcept
{
    const size_t head=soulWrite.load(std::memory_order_relaxed);
    for(size_t i=0;i<wave.size();++i)wave[i]=soulWave[(head+i)%soulWave.size()].load(std::memory_order_relaxed);
    float peak=.0001f;for(size_t i=0;i<spectrum.size();++i){spectrum[i]=soulSpectrum[i].load(std::memory_order_relaxed);peak=std::max(peak,spectrum[i]);}
    for(auto& x:spectrum)x=juce::jlimit(0.f,1.f,x/peak);
    width=soulWidth.load(std::memory_order_relaxed);
}

void HorrorCastleProcessor::getStateInformation(juce::MemoryBlock& destination)
{
    auto state = parameters.copyState();
    state.setProperty("stateVersion", Grimoire::CurrentStateVersion, nullptr);
    state.setProperty("productVersion", "1.3.0", nullptr);
    if (auto xml = state.createXml())
        juce::AudioProcessor::copyXmlToBinary(*xml, destination);
}

void HorrorCastleProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = juce::AudioProcessor::getXmlFromBinary(data, sizeInBytes))
    {
        auto state = juce::ValueTree::fromXml(*xml);
        Grimoire::migrateState(state);
        if (state.isValid() && state.hasType(parameters.state.getType()))
        {
            auto merged = parameters.copyState();
            mergeStateTree(merged, state);
            Grimoire::migrateState(merged);
            parameters.replaceState(merged);
            engine.setParameters(parameters);
        }
    }
}

} // namespace horrorcastle

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new horrorcastle::HorrorCastleProcessor();
}
