# Horror Castle v0.12.1 — Independent Core Compile Fix

The first v0.12 Independent Core Mac build exposed two JUCE 8 API qualification issues in
`HorrorCastleProcessor.cpp`.

JUCE declares `copyXmlToBinary` and `getXmlFromBinary` as static members of
`juce::AudioProcessor`.

This patch changes:

- `juce::copyXmlToBinary(...)`
  to `juce::AudioProcessor::copyXmlToBinary(...)`

- `juce::getXmlFromBinary(...)`
  to `juce::AudioProcessor::getXmlFromBinary(...)`

No sound-engine behavior changed.
