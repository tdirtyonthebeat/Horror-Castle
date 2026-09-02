#include "HexMatrixComponent.h"
#include "Theme/CastleTheme.h"
#include "Theme/CastleGraphics.h"
#include <BinaryData.h>

namespace horrorcastle {
using namespace juce;
using namespace HorrorCastle;

static void styleCombo(ComboBox& c){ c.getProperties().set("castleAccent","hex"); c.setColour(ComboBox::backgroundColourId,Colour(0xff080a0d)); c.setColour(ComboBox::outlineColourId,theme::cryptRed().withAlpha(.55f)); c.setColour(ComboBox::textColourId,theme::bone()); }

HexMatrixComponent::HexMatrixComponent(AudioProcessorValueTreeState& state) : apvts(state)
{
    skinFrame=ImageFileFormat::loadFrom(BinaryData::hex_frame_png,BinaryData::hex_frame_pngSize);
    setupWidgets();startTimerHz(30);setOpaque(false);
}

void HexMatrixComponent::setupWidgets(){
    for(int i=0;i<8;++i){
        auto& w=widgets[(size_t)i]; const auto p=String("hex.curse")+String(i+1)+".";
        w.source=std::make_unique<ComboBox>(); w.curse=std::make_unique<ComboBox>(); w.destination=std::make_unique<ComboBox>(); w.amount=std::make_unique<Slider>();
        const StringArray sources{"Off","Blood","Wraith","Velocity","Key","Random","Pulse","Mod Wheel","Aftertouch"};
        const StringArray curses{"Clean","Corrupt","Haunt","Possession","Decay","Madness","Blood"};
        const StringArray dests{"Off","Crypt Cutoff","Tower Cutoff","Crypt Shape","Tower Shape","FM Depth","Filter Drive","Ritual Mix","Grave Mix","Pitch","Curse Depth","Crypt Dread","Tower Aether","Ritual Depth","Ritual Fury","Ritual Feedback","Grave Tone","Blood Feed","Aether Leak","Soul Exchange","Haunt","Grave Feedback"};
        for(int j=0;j<sources.size();++j)w.source->addItem(sources[j],j+1);
        for(int j=0;j<curses.size();++j)w.curse->addItem(curses[j],j+1);
        for(int j=0;j<dests.size();++j)w.destination->addItem(dests[j],j+1);
        w.amount->getProperties().set("castleAccent","hex"); w.amount->setSliderStyle(Slider::LinearHorizontal); w.amount->setTextBoxStyle(Slider::TextBoxRight,false,42,16); w.amount->setRange(-1.0,1.0,0.001); w.amount->setDoubleClickReturnValue(true,0.0);
        styleCombo(*w.source); styleCombo(*w.curse); styleCombo(*w.destination);
        w.sourceA=std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,p+"source",*w.source);
        w.curseA=std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,p+"curse",*w.curse);
        w.destinationA=std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,p+"destination",*w.destination);
        w.amountA=std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts,p+"amount",*w.amount);
        addAndMakeVisible(*w.source); addAndMakeVisible(*w.curse); addAndMakeVisible(*w.destination); addAndMakeVisible(*w.amount);
    }
}

void HexMatrixComponent::setLane(int i,const CurseLane& lane){if(i>=0&&i<8)lanes[(size_t)i]=lane;repaint();}
void HexMatrixComponent::setLaneValue(int i,float value){if(i>=0&&i<8)laneValues[(size_t)i]=value;repaint();}
void HexMatrixComponent::setDestinationValue(ModDestination d,float value){auto i=(size_t)d;if(i<destinationValues.size())destinationValues[i]=value;repaint();}
String HexMatrixComponent::sourceName(ModSource s)const{switch(s){case ModSource::Blood:return"BLOOD";case ModSource::Wraith:return"WRAITH";case ModSource::Velocity:return"VELOCITY";case ModSource::Key:return"KEY";case ModSource::Random:return"RANDOM";case ModSource::Pulse:return"PULSE";case ModSource::ModWheel:return"MOD WHEEL";case ModSource::Aftertouch:return"AFTERTOUCH";default:return"—";}}
String HexMatrixComponent::curseName(CurseType c)const{switch(c){case CurseType::Corrupt:return"CORRUPT";case CurseType::Haunt:return"HAUNT";case CurseType::Possession:return"POSSESSION";case CurseType::Decay:return"DECAY";case CurseType::Madness:return"MADNESS";case CurseType::Blood:return"BLOOD";default:return"CLEAN";}}
String HexMatrixComponent::destinationName(ModDestination d)const{switch(d){case ModDestination::CryptCutoff:return"CRYPT CUTOFF";case ModDestination::TowerCutoff:return"TOWER CUTOFF";case ModDestination::CryptShape:return"CRYPT SHAPE";case ModDestination::TowerShape:return"TOWER SHAPE";case ModDestination::FMDepth:return"FM DEPTH";case ModDestination::FilterDrive:return"FILTER DRIVE";case ModDestination::Ritual:return"RITUAL MIX";case ModDestination::Grave:return"GRAVE MIX";case ModDestination::Pitch:return"PITCH";case ModDestination::CurseDepth:return"CURSE DEPTH";case ModDestination::CryptDread:return"CRYPT DREAD";case ModDestination::TowerAether:return"TOWER AETHER";case ModDestination::RitualDepth:return"RITUAL DEPTH";case ModDestination::RitualFury:return"RITUAL FURY";case ModDestination::RitualFeedback:return"RITUAL FEEDBACK";case ModDestination::GraveTone:return"GRAVE TONE";case ModDestination::BloodFeed:return"BLOOD FEED";case ModDestination::AetherLeak:return"AETHER LEAK";case ModDestination::SoulExchange:return"SOUL EXCHANGE";case ModDestination::HauntEngine:return"HAUNT";case ModDestination::GraveFeedback:return"GRAVE FEEDBACK";default:return"—";}}
String HexMatrixComponent::curseGlyph(CurseType c)const{switch(c){case CurseType::Corrupt:return"X";case CurseType::Haunt:return"O";case CurseType::Possession:return"†";case CurseType::Decay:return"~";case CurseType::Madness:return"Ψ";case CurseType::Blood:return"♦";default:return"·";}}
void HexMatrixComponent::resized(){layoutNodes();auto b=getLocalBounds().reduced(12,48);const int panelW=250;auto controls=b.removeFromRight(panelW);const int rowH=std::max(28,controls.getHeight()/8);for(int i=0;i<8;++i){auto r=controls.removeFromTop(rowH).reduced(2);widgets[i].source->setBounds(r.removeFromLeft(58));widgets[i].curse->setBounds(r.removeFromLeft(64));widgets[i].destination->setBounds(r.removeFromLeft(78));widgets[i].amount->setBounds(r.reduced(1));}}
void HexMatrixComponent::layoutNodes(){auto b=getLocalBounds().toFloat().reduced(22,52);b.removeFromRight(270);float w=b.getWidth()/3.f,h=b.getHeight()/8.f;for(int i=0;i<8;++i){float y=b.getY()+h*(i+.5f);sourceNodes[i].pos={b.getX()+w*.15f,y};curseNodes[i].pos={b.getX()+w,y};destinationNodes[i].pos={b.getX()+w*1.85f,y};}}
void HexMatrixComponent::timerCallback(){animation+=.055f;for(int i=0;i<8;++i){float a=jlimit(0.f,1.f,std::abs(laneValues[i])+.06f*(.5f+.5f*std::sin(animation*1.6f+i*.77f)));sourceNodes[i].activity=a;curseNodes[i].activity=jlimit(0.f,1.f,a*1.25f);auto d=(size_t)lanes[i].destination;destinationNodes[i].activity=d<destinationValues.size()?jlimit(0.f,1.f,std::abs(destinationValues[d])):0.f;}repaint();}
void HexMatrixComponent::drawConnection(Graphics&g,const Node&a,const Node&b,float activity,bool cursed,bool secondary){Path p;float mx=(a.pos.x+b.pos.x)*.5f;p.startNewSubPath(a.pos);p.cubicTo(mx,a.pos.y,mx,b.pos.y,b.pos.x,b.pos.y);Colour c=secondary?Colour(0xffd5a84c):(cursed?Colour(0xffa92843):Colour(0xff705c52));g.setColour(c.withAlpha(.12f+.75f*jlimit(0.f,1.f,activity)));g.strokePath(p,PathStrokeType(secondary?2.2f:1.4f));float t=std::fmod(animation*(.12f+.35f*activity),1.f);auto m=a.pos+(b.pos-a.pos)*t;g.setColour(c.withAlpha(.85f));g.fillEllipse(m.x-2.5f,m.y-2.5f,5,5);}
void HexMatrixComponent::drawNode(Graphics&g,const Node&n,const String&label,const String&gl,bool cursed){float r=n.radius*(1+.16f*n.activity);Colour edge=n.selected?Colour(0xffd8ae59):(cursed?Colour(0xff9b2943):Colour(0xff594a43));g.setColour(edge.withAlpha(.2f+.55f*n.activity));g.drawEllipse(n.pos.x-r-4,n.pos.y-r-4,(r+4)*2,(r+4)*2,2);g.setColour(Colour(0xff120d13));g.fillEllipse(n.pos.x-r,n.pos.y-r,r*2,r*2);g.setColour(edge);g.drawEllipse(n.pos.x-r,n.pos.y-r,r*2,r*2,1.5f);g.setColour(Colour(0xffe3d4bb));g.setFont(FontOptions(11));g.drawText(gl, Rectangle<float>(n.pos.x-r, n.pos.y-r+1, r*2, r), Justification::centred);g.setFont(FontOptions(8.5f));g.drawText(label, Rectangle<float>(n.pos.x-r-42, n.pos.y+r+5, r*2+84, 15.0f), Justification::centred);}
void HexMatrixComponent::paint(Graphics&g)
{
    auto b=getLocalBounds().toFloat();
    CastleGraphics::drawStonePanel(g,b.reduced(3),theme::cryptRed(),5);
    if(skinFrame.isValid()){
        g.setOpacity(.96f);g.drawImageWithin(skinFrame,0,0,getWidth(),getHeight(),RectanglePlacement::stretchToFit);g.setOpacity(1.0f);
    }else CastleGraphics::drawRuneBorder(g,b.reduced(4),theme::cryptRed(),.22f);

    g.setColour(theme::cryptEmber());g.setFont(Font(FontOptions(18.0f)).boldened());
    g.drawText("HEX // THE CURSE ENGINE",28,16,b.getWidth()-320,24,Justification::centredLeft);

    auto mb=b.reduced(28,58);mb.removeFromRight(280);
    CastleGraphics::drawSigil(g,{mb.getX()+56.0f,mb.getY()+38.0f},28.0f,theme::towerPurple(),animation*.08f);
    g.setColour(theme::parchment().withAlpha(.62f));g.setFont(FontOptions(8.5f));
    float w=mb.getWidth()/3.f;
    g.drawText("SOURCE",mb.withWidth(w).toNearestInt(),Justification::centred);
    g.drawText("CURSE",mb.withX(mb.getX()+w).withWidth(w).toNearestInt(),Justification::centred);
    g.drawText("DESTINATION",mb.withX(mb.getX()+2*w).withWidth(w).toNearestInt(),Justification::centred);

    for(int i=0;i<8;++i){
        auto&l=lanes[i];if(!l.enabled||l.destination==ModDestination::None)continue;
        bool cursed=l.curse!=CurseType::Clean;
        drawConnection(g,sourceNodes[i],curseNodes[i],sourceNodes[i].activity,cursed,false);
        drawConnection(g,curseNodes[i],destinationNodes[i],curseNodes[i].activity,cursed,false);
        if(l.destination==ModDestination::CurseDepth)
            for(int j=i+1;j<8;++j)if(lanes[j].enabled)
                drawConnection(g,destinationNodes[i],curseNodes[j],std::abs(laneValues[i])*.75f,true,true);
        sourceNodes[i].selected=curseNodes[i].selected=destinationNodes[i].selected=(selectedLane==i);
        drawNode(g,sourceNodes[i],sourceName(l.source),"S",cursed);
        drawNode(g,curseNodes[i],curseName(l.curse),curseGlyph(l.curse),cursed);
        drawNode(g,destinationNodes[i],destinationName(l.destination),"D",cursed);
    }
}
void HexMatrixComponent::mouseDown(const MouseEvent&e){selectedLane=-1;for(int i=0;i<8;++i)if(sourceNodes[i].pos.getDistanceFrom(e.position)<32||curseNodes[i].pos.getDistanceFrom(e.position)<32||destinationNodes[i].pos.getDistanceFrom(e.position)<32){selectedLane=i;break;}if(selectedLane>=0&&onLaneSelected)onLaneSelected(selectedLane);repaint();}
} // namespace horrorcastle