#include "HorrorCastleProcessor.h"
#include "../HorrorCastle/HorrorCastleEditor.h"

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
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HorrorCastleProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}
#endif

juce::AudioProcessorEditor* HorrorCastleProcessor::createEditor()
{
    return new HorrorCastleEditor(*this);
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
            // Merge legacy state into a fresh v1 tree so parameters introduced after
            // v0.14 keep their defaults instead of disappearing from the state.
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
