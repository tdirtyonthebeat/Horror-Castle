#include "CastleHeaderComponent.h"
#include "../Theme/CastleTheme.h"
#include "../Theme/CastleGraphics.h"
#include <BinaryData.h>

namespace horrorcastle {
using namespace juce;

CastleHeaderComponent::CastleHeaderComponent()
{
    headerImage=ImageFileFormat::loadFrom(BinaryData::header_png,
                                          BinaryData::header_pngSize);
    setInterceptsMouseClicks(false,false);
}

void CastleHeaderComponent::paint(Graphics& g)
{
    auto r=getLocalBounds().toFloat();
    g.fillAll(theme::stoneBlack());

    if(headerImage.isValid()){
        g.setOpacity(.98f);
        g.drawImageWithin(headerImage,0,0,getWidth(),getHeight(),RectanglePlacement::fillDestination);
        g.setOpacity(1.0f);
    }

    ColourGradient veil(Colour(0xff020407).withAlpha(.08f),0,0,
                        Colour(0xff020407).withAlpha(.62f),0,(float)getHeight(),false);
    g.setGradientFill(veil);g.fillRect(r);

    g.setColour(theme::bone());
    g.setFont(Font(FontOptions(39.0f)).boldened());
    g.drawFittedText("HORROR CASTLE",300,7,getWidth()-600,47,Justification::centred,1,.88f);

    g.setFont(FontOptions(9.4f));
    g.setColour(theme::parchment().withAlpha(.78f));
    g.drawText("THE CRYPT   //   THE TOWER   //   THE RITUAL   //   THE GRAVE",
               300,55,getWidth()-600,17,Justification::centred);

    g.setFont(Font(FontOptions(7.8f)).boldened());
    g.setColour(theme::gold().withAlpha(.64f));
    g.drawText("STONE & SHADOW   //   SPECTRAL CORPSE",
               300,73,getWidth()-600,14,Justification::centred);

    g.setColour(theme::cryptRed().withAlpha(.42f));g.fillRect(0,getHeight()-3,getWidth()/2,2);
    g.setColour(theme::towerPurple().withAlpha(.42f));g.fillRect(getWidth()/2,getHeight()-3,getWidth()/2,2);
}
} // namespace horrorcastle
