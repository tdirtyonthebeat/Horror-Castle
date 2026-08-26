#include "HorrorCastleEditor.h"
#include "../Core/HorrorCastleProcessor.h"
#include "../UI/Theme/CastleTheme.h"
#include "../UI/Theme/CastleGraphics.h"
#include <BinaryData.h>
#include <cmath>

namespace horrorcastle {
using namespace juce;

namespace {
Image loadAsset(const void* data, int size)
{
    return ImageFileFormat::loadFrom(data, (size_t) size);
}

void labelIt(Label& l, const String& text)
{
    l.setText(text, dontSendNotification);
    l.setFont(FontOptions(9.5f));
    l.setJustificationType(Justification::centred);
    l.setColour(Label::textColourId, theme::bone().withAlpha(.84f));
}

}

CastleLookAndFeel::CastleLookAndFeel()
{
    setColour(Slider::thumbColourId,theme::gold());
    setColour(Slider::trackColourId,theme::cryptRed());
    setColour(Slider::textBoxTextColourId,theme::bone());
    setColour(Slider::textBoxBackgroundColourId,Colour(0xff050608));
    setColour(Slider::textBoxOutlineColourId,Colour(0xff51453f));
    setColour(ComboBox::backgroundColourId,Colour(0xff07090c));
    setColour(ComboBox::outlineColourId,Colour(0xff51453f));
    setColour(ComboBox::textColourId,theme::bone());
    setColour(Label::textColourId,theme::bone());
    setColour(PopupMenu::backgroundColourId,Colour(0xff07090c));
    setColour(PopupMenu::textColourId,theme::bone());
    setColour(TextButton::textColourOffId,theme::bone());
    setColour(TextButton::textColourOnId,theme::parchment());
}

void CastleLookAndFeel::drawRotarySlider(Graphics& g,int x,int y,int w,int h,float pos,float,float,Slider& s)
{
    auto b=Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(6.f);
    auto c=b.getCentre();
    const float r=jmin(b.getWidth(),b.getHeight())*.5f;
    const float a0=MathConstants<float>::pi*1.25f;
    const float a1=MathConstants<float>::pi*2.75f;
    const auto accent=theme::accentFor(s);

    // Deep shadow + carved bezel.
    g.setColour(Colours::black.withAlpha(.62f));
    g.fillEllipse(b.translated(0,2.0f).expanded(2.0f));
    g.setColour(Colour(0xff050608));
    g.fillEllipse(b);
    g.setColour(Colour(0xff4a4545));
    g.drawEllipse(b,1.5f);
    g.setColour(theme::gold().withAlpha(.20f));
    g.drawEllipse(b.reduced(2.5f),.8f);

    // Small engraved ticks.
    g.setColour(theme::parchment().withAlpha(.20f));
    for(int i=0;i<9;++i){
        const float t=(float)i/8.0f;
        const float a=a0+(a1-a0)*t;
        const float r0=r-3.0f, r1=r-6.0f;
        g.drawLine(c.x+std::cos(a)*r0,c.y+std::sin(a)*r0,
                   c.x+std::cos(a)*r1,c.y+std::sin(a)*r1,.65f);
    }

    auto inner=b.reduced(r*.19f);
    ColourGradient metal(Colour(0xff2a2a2d),inner.getX(),inner.getY(),
                         Colour(0xff07080a),inner.getRight(),inner.getBottom(),false);
    g.setGradientFill(metal);
    g.fillEllipse(inner);
    g.setColour(Colour(0xff111217));
    g.drawEllipse(inner.reduced(1.0f),1.0f);

    Path glow;
    auto arc=b.reduced(1.5f);
    glow.addCentredArc(arc.getCentreX(),arc.getCentreY(),arc.getWidth()*.5f,arc.getHeight()*.5f,
                      0,a0,a0+(a1-a0)*pos,true);
    g.setColour(accent.withAlpha(.13f));
    g.strokePath(glow,PathStrokeType(7.f));
    g.setColour(accent.withAlpha(.92f));
    g.strokePath(glow,PathStrokeType(2.0f));

    const float a=a0+(a1-a0)*pos;
    g.setColour(theme::bone().withAlpha(.94f));
    g.drawLine(c.x,c.y,c.x+std::cos(a)*(r-8),c.y+std::sin(a)*(r-8),1.65f);
    g.setColour(theme::gold().withAlpha(.72f));
    g.fillEllipse(c.x-2.0f,c.y-2.0f,4.0f,4.0f);
}

void CastleLookAndFeel::drawLinearSlider(Graphics& g,int x,int y,int w,int h,float pos,float,float,Slider::SliderStyle,Slider& s)
{
    auto r=Rectangle<float>((float)x,(float)y,(float)w,(float)h).reduced(2.f);
    auto accent=theme::accentFor(s);
    g.setColour(Colour(0xff05070a));g.fillRoundedRectangle(r,2);
    g.setColour(theme::gold().withAlpha(.18f));g.drawRoundedRectangle(r,2,1);
    auto fill=r.withRight(jlimit(r.getX(),r.getRight(),pos));
    g.setColour(accent.withAlpha(.32f));g.fillRoundedRectangle(fill,2);
    g.setColour(accent.withAlpha(.95f));g.fillEllipse(pos-3.5f,r.getCentreY()-4.0f,7.0f,8.0f);
}

void CastleLookAndFeel::drawToggleButton(Graphics& g,ToggleButton& b,bool hov,bool down)
{
    auto r=b.getLocalBounds().toFloat().reduced(1.5f);
    auto accent=theme::accentFor(b);
    CastleGraphics::drawStonePanel(g,r,accent,3.0f);
    if(b.getToggleState()||hov){
        g.setColour(accent.withAlpha(b.getToggleState()?.27f:.10f));
        g.fillRoundedRectangle(r.reduced(2),2);
    }
    g.setColour(b.getToggleState()?accent:theme::bone().withAlpha(.76f));
    g.setFont(Font(FontOptions(8.5f)).boldened());
    g.drawText(b.getButtonText(),r.toNearestInt(),Justification::centred);
}

void CastleLookAndFeel::drawComboBox(Graphics& g,int w,int h,bool,int,int,int,int,ComboBox& box)
{
    auto r=Rectangle<float>(0,0,(float)w,(float)h).reduced(.5f);
    auto accent=theme::accentFor(box);
    g.setColour(Colour(0xff06080b));g.fillRoundedRectangle(r,2);
    g.setColour(theme::gold().withAlpha(.18f));g.drawRoundedRectangle(r,2,1);
    g.setColour(accent.withAlpha(.58f));g.drawRoundedRectangle(r.reduced(1.0f),1.5f,.7f);
    Path arrow;
    arrow.startNewSubPath(w-15.0f,h*.42f);
    arrow.lineTo(w-11.0f,h*.58f);
    arrow.lineTo(w-7.0f,h*.42f);
    g.setColour(accent.withAlpha(.92f));g.strokePath(arrow,PathStrokeType(1.2f));
}

void CastleLookAndFeel::positionComboBoxText(ComboBox& b,Label& l)
{
    l.setBounds(7,1,b.getWidth()-25,b.getHeight()-2);
    l.setFont(FontOptions(9.0f));
}

void CastleLookAndFeel::drawButtonBackground(Graphics& g,Button& b,const Colour&,bool hov,bool down)
{
    auto r=b.getLocalBounds().toFloat().reduced(1);
    auto accent=theme::accentFor(b);
    CastleGraphics::drawStonePanel(g,r,accent,3.0f);
    if(hov||down){
        g.setColour(accent.withAlpha(down?.28f:.13f));
        g.fillRoundedRectangle(r.reduced(2),2);
    }
}

void CastleLookAndFeel::drawButtonText(Graphics& g,TextButton& b,bool,bool)
{
    g.setColour(theme::bone());
    g.setFont(Font(FontOptions(8.8f)).boldened());
    g.drawFittedText(b.getButtonText(),b.getLocalBounds().reduced(5,2),Justification::centred,2,.82f);
}

void HorrorCastleEditor::makeControl(Control& c,const String& id,const String& name)
{
    c.label=std::make_unique<Label>();labelIt(*c.label,name);
    c.slider=std::make_unique<Slider>();
    c.slider->setSliderStyle(Slider::RotaryHorizontalVerticalDrag);
    c.slider->setTextBoxStyle(Slider::TextBoxBelow,false,52,15);
    c.slider->setLookAndFeel(&look);
    c.attachment=std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(apvts,id,*c.slider);
    addAndMakeVisible(*c.label);addAndMakeVisible(*c.slider);
}
void HorrorCastleEditor::makeChoice(Choice& c,const String& id,const String& name)
{
    c.label=std::make_unique<Label>();labelIt(*c.label,name);
    c.box=std::make_unique<ComboBox>();c.box->setLookAndFeel(&look);
    if(auto* choice=dynamic_cast<AudioParameterChoice*>(apvts.getParameter(id)))
        for(int i=0;i<choice->choices.size();++i)c.box->addItem(choice->choices[i],i+1);
    c.box->setTextWhenNothingSelected("SELECT");
    c.attachment=std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(apvts,id,*c.box);
    addAndMakeVisible(*c.label);addAndMakeVisible(*c.box);
}
void HorrorCastleEditor::makeToggle(Toggle& t,const String& id,const String& name)
{
    t.button=std::make_unique<ToggleButton>(name);t.button->setLookAndFeel(&look);
    t.attachment=std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(apvts,id,*t.button);
    addAndMakeVisible(*t.button);
}
void HorrorCastleEditor::makeMasterControl(Control& c,const String& id,const String& name){makeControl(c,id,name);}
void HorrorCastleEditor::setAccent(Control& c,const String& key){if(c.slider)c.slider->getProperties().set("castleAccent",key);if(c.label)c.label->getProperties().set("castleAccent",key);}
void HorrorCastleEditor::setAccent(Choice& c,const String& key){if(c.box)c.box->getProperties().set("castleAccent",key);}
void HorrorCastleEditor::setAccent(Toggle& c,const String& key){if(c.button)c.button->getProperties().set("castleAccent",key);}

void HorrorCastleEditor::ScenePanel::bind(AudioProcessorValueTreeState&,const char*){}
void HorrorCastleEditor::ScenePanel::paint(Graphics& g)
{
    auto r=getLocalBounds().toFloat();
    const bool isCrypt=accentKey=="crypt";
    const auto accent=isCrypt?theme::cryptRed():theme::towerPurple();
    CastleGraphics::drawStonePanel(g,r.reduced(2),accent,5);
    CastleGraphics::drawGothicArch(g,r,accent);
    CastleGraphics::drawRuneBorder(g,r,accent,.12f);
    if(frameImage.isValid()){
        g.setOpacity(.96f);
        g.drawImageWithin(frameImage,0,0,getWidth(),getHeight(),RectanglePlacement::stretchToFit);
        g.setOpacity(1.0f);
    }
    // Interior veil lets the architecture remain visible while preserving control contrast.
    auto interior=r.reduced(isCrypt?126.0f:18.0f,86.0f);
    if(!isCrypt) interior=interior.withTrimmedRight(108.0f);
    else interior=interior.withTrimmedRight(18.0f);
    g.setColour(theme::stoneBlack().withAlpha(.36f));
    g.fillRoundedRectangle(interior,5.0f);
    g.setColour(accent.withAlpha(.96f));
    g.setFont(Font(FontOptions(17.0f)).boldened());
    g.drawText(title.getText(),isCrypt?132:22,10,getWidth()-170,25,Justification::centredLeft);
}
void HorrorCastleEditor::ScenePanel::resized(){}

HorrorCastleEditor::HorrorCastleEditor(HorrorCastleProcessor& p)
:AudioProcessorEditor(p),processor(p),apvts(p.getParameterState()),grimoire(p),hexMatrix(p.getParameterState()),curseInspector(p.getParameterState())
{
    setLookAndFeel(&look);
    startTimerHz(20);

    backdropImage=loadAsset(BinaryData::backdrop_png,BinaryData::backdrop_pngSize);
    ritualFrameImage=loadAsset(BinaryData::ritual_grave_frame_png,BinaryData::ritual_grave_frame_pngSize);
    undercroftImage=loadAsset(BinaryData::undercroft_png,BinaryData::undercroft_pngSize);

    addAndMakeVisible(header);
    addAndMakeVisible(hexMatrix);
    addAndMakeVisible(curseInspector);
    addAndMakeVisible(grimoire);grimoire.setVisible(false);

    grimoireToggle.setLookAndFeel(&look);
    grimoireToggle.getProperties().set("castleAccent","grimoire");
    addAndMakeVisible(grimoireToggle);
    grimoireToggle.onClick=[this]{setGrimoireVisible(!grimoire.isVisible());};

    undercroftToggle.setLookAndFeel(&look);
    undercroftToggle.getProperties().set("castleAccent","gold");
    addAndMakeVisible(undercroftToggle);
    undercroftToggle.onClick=[this]{setUndercroftVisible(!undercroftVisible);};

    grimoire.onClose=[this]{setGrimoireVisible(false);};
    grimoire.onStatus=[this](const String& s){status.setText(s,dontSendNotification);};

    curseInspector.selectLane(0);
    hexMatrix.onLaneSelected=[this](int lane){
        inspectedLane=lane;curseInspector.selectLane(lane);
        status.setText("HEX LANE "+String(lane+1)+" // CURSE INSPECTOR OPEN",dontSendNotification);
    };

    auto setupScene=[&](ScenePanel& panel,const char* s,const char* name){
        const String key=s;panel.title.setText(name,dontSendNotification);panel.accentKey=key;addAndMakeVisible(panel);
        auto addG=[&](int i){
            makeChoice(panel.genType[i],param::id(s,i+1,"type"),"GEN "+String(i+1));
            makeControl(panel.genLevel[i],param::id(s,i+1,"level"),"LEVEL");
            makeControl(panel.genShape[i],param::id(s,i+1,"shape"),"SHAPE");
            makeControl(panel.genTune[i],param::id(s,i+1,"tune"),"TUNE");
            setAccent(panel.genType[i],key);setAccent(panel.genLevel[i],key);setAccent(panel.genShape[i],key);setAccent(panel.genTune[i],key);
        };
        for(int i=0;i<3;++i)addG(i);
        makeControl(panel.noise,param::noise(s,"level"),"NOISE");
        makeControl(panel.f1Cut,param::fid(s,1,"cutoff"),"CUTOFF A");
        makeControl(panel.f1Res,param::fid(s,1,"resonance"),"RES A");
        makeControl(panel.f2Cut,param::fid(s,2,"cutoff"),"CUTOFF B");
        makeControl(panel.f2Res,param::fid(s,2,"resonance"),"RES B");
        makeControl(panel.f1Drive,param::fid(s,1,"drive"),"DRIVE A");
        makeControl(panel.f2Drive,param::fid(s,2,"drive"),"DRIVE B");
        makeChoice(panel.route,param::route(s),"ROUTING");
        makeControl(panel.master,param::scene(s,"master"),"MASTER");
        makeControl(panel.balance,param::scene(s,"balance"),"BALANCE");
        makeControl(panel.character,param::scene(s,"character"),key=="crypt"?"DREAD":"AETHER");
        makeToggle(panel.crossFm,param::scene(s,"crossfm"),"CROSS FM");
        makeToggle(panel.crossRing,param::scene(s,"crossring"),"CROSS RING");
        for(auto* c:{&panel.noise,&panel.f1Cut,&panel.f1Res,&panel.f2Cut,&panel.f2Res,&panel.f1Drive,&panel.f2Drive,&panel.master,&panel.balance,&panel.character})setAccent(*c,key);
        setAccent(panel.route,key);setAccent(panel.crossFm,key);setAccent(panel.crossRing,key);
    };
    setupScene(crypt,"crypt","CRYPT");
    setupScene(tower,"tower","TOWER");
    crypt.setFrameImage(loadAsset(BinaryData::crypt_frame_png,BinaryData::crypt_frame_pngSize));
    tower.setFrameImage(loadAsset(BinaryData::tower_frame_png,BinaryData::tower_frame_pngSize));

    centerSpine.setImage(loadAsset(BinaryData::center_spine_png,BinaryData::center_spine_pngSize),RectanglePlacement::stretchToFit);
    centerSpine.setInterceptsMouseClicks(false,false);
    addAndMakeVisible(centerSpine);

    makeChoice(ritualMode,"ritual.mode","MODE");
    ritualMode.box->onChange=[this]{status.setText("RITUAL // "+ritualMode.box->getText().toUpperCase(),dontSendNotification);};
    makeControl(ritualMix,"ritual.mix","MIX");makeControl(ritualDepth,"ritual.depth","DEPTH");makeControl(ritualDrive,"ritual.drive","FURY");
    makeControl(ritualWidth,"ritual.width","WIDTH");makeControl(ritualFeedback,"ritual.feedback","FEEDBACK");
    for(auto* c:{&ritualMix,&ritualDepth,&ritualDrive,&ritualWidth,&ritualFeedback})setAccent(*c,"ritual");
    setAccent(ritualMode,"ritual");

    makeControl(graveReverb,"grave.reverb","REVERB");makeControl(graveDelay,"grave.delay","DELAY");
    makeControl(graveFeedback,"grave.feedback","FEEDBACK");makeControl(graveTone,"grave.cutoff","TONE");makeControl(graveOutput,"grave.output","OUTPUT");
    for(auto* c:{&graveReverb,&graveDelay,&graveFeedback,&graveTone,&graveOutput})setAccent(*c,"grave");
    makeControl(hexAmount,"global.hex","HEX DEPTH");setAccent(hexAmount,"hex");

    // Spectral Corpse altar.
    makeControl(corpsePosition,"corpse.position","POSITION");makeControl(corpseRot,"corpse.rot","ROT");
    makeControl(corpseFormant,"corpse.formant","FORMANT");makeControl(corpseInharmonic,"corpse.inharmonic","INHARMONIC");
    for(auto* c:{&corpsePosition,&corpseRot,&corpseFormant,&corpseInharmonic})setAccent(*c,"crypt");

    makeControl(bloodFeed,"possession.bloodFeed","BLOOD FEED");makeControl(aetherLeak,"possession.aetherLeak","AETHER LEAK");
    makeControl(soulExchange,"possession.soulExchange","SOUL XCHG");makeControl(possessionHaunt,"possession.haunt","HAUNT");
    for(auto* c:{&bloodFeed,&aetherLeak,&soulExchange,&possessionHaunt})setAccent(*c,"hex");

    makeToggle(ritualsEnabled,"rituals.enabled","RITUALS");
    makeChoice(ritualsPattern,"rituals.pattern","PATTERN");makeChoice(ritualsRate,"rituals.rate","RATE");
    makeControl(ritualsBpm,"rituals.bpm","BPM");makeControl(ritualsGate,"rituals.gate","GATE");
    makeControl(ritualsProbability,"rituals.probability","CHANCE");makeControl(ritualsSwing,"rituals.swing","SWING");makeControl(ritualsOctaves,"rituals.octaves","OCTAVES");
    setAccent(ritualsEnabled,"grimoire");setAccent(ritualsPattern,"grimoire");setAccent(ritualsRate,"grimoire");
    for(auto* c:{&ritualsBpm,&ritualsGate,&ritualsProbability,&ritualsSwing,&ritualsOctaves})setAccent(*c,"grimoire");

    auto titleStyle=[&](Label& l,const String& text,Colour colour,float size){
        l.setText(text,dontSendNotification);l.setFont(Font(FontOptions(size)).boldened());
        l.setColour(Label::textColourId,colour);addAndMakeVisible(l);
    };
    titleStyle(ritualTitle,"RITUAL BUS",theme::cryptEmber(),13.0f);
    titleStyle(graveTitle,"GRAVE MASTER",theme::towerGlow(),13.0f);
    titleStyle(undercroftTitle,"UNDERCROFT // HIDDEN MACHINERY",theme::gold(),15.0f);
    titleStyle(corpseTitle,"CORPSE ALTAR // SPECTRAL RESYNTHESIS",theme::cryptEmber(),13.0f);
    titleStyle(advancedCryptTitle,"CRYPT FORGE",theme::cryptEmber(),11.0f);
    titleStyle(advancedTowerTitle,"TOWER FORGE",theme::towerGlow(),11.0f);
    titleStyle(possessionTitle,"POSSESSION MATRIX",theme::cryptEmber(),13.0f);
    titleStyle(ritualsTitle,"RITUALS",theme::towerGlow(),13.0f);

    corpseStatus.setText("24 PARTIALS  //  FRAME MORPH  //  FORMANT  //  ROT",dontSendNotification);
    corpseStatus.setFont(FontOptions(8.5f));corpseStatus.setColour(Label::textColourId,theme::parchment().withAlpha(.58f));addAndMakeVisible(corpseStatus);

    status.setText("STONE & SHADOW  //  SPECTRAL CORPSE ACTIVE",dontSendNotification);
    status.setFont(FontOptions(8.5f));status.setColour(Label::textColourId,theme::parchment().withAlpha(.54f));addAndMakeVisible(status);

    uiReady=true;
    setAdvancedComponentsVisible(false);
    setSize(1448,1086);
}

HorrorCastleEditor::~HorrorCastleEditor()
{
    stopTimer();
    for(auto* s:{&crypt,&tower}){
        for(auto* group:{&s->genLevel,&s->genShape,&s->genTune})
            for(auto& c:*group)if(c.slider)c.slider->setLookAndFeel(nullptr);
        for(auto* c:{&s->noise,&s->f1Cut,&s->f1Res,&s->f2Cut,&s->f2Res,&s->f1Drive,&s->f2Drive,&s->master,&s->balance,&s->character})
            if(c->slider)c->slider->setLookAndFeel(nullptr);
        if(s->crossFm.button)s->crossFm.button->setLookAndFeel(nullptr);
        if(s->crossRing.button)s->crossRing.button->setLookAndFeel(nullptr);
        for(auto& c:s->genType)if(c.box)c.box->setLookAndFeel(nullptr);
        if(s->route.box)s->route.box->setLookAndFeel(nullptr);
    }
    for(auto* c:{&ritualMix,&ritualDepth,&ritualDrive,&ritualWidth,&ritualFeedback,&graveReverb,&graveDelay,&graveFeedback,&graveTone,&graveOutput,&hexAmount,
                  &corpsePosition,&corpseRot,&corpseFormant,&corpseInharmonic,
                  &bloodFeed,&aetherLeak,&soulExchange,&possessionHaunt,&ritualsBpm,&ritualsGate,&ritualsProbability,&ritualsSwing,&ritualsOctaves})
        if(c->slider)c->slider->setLookAndFeel(nullptr);
    for(auto* c:{&ritualMode,&ritualsPattern,&ritualsRate})if(c->box)c->box->setLookAndFeel(nullptr);
    if(ritualsEnabled.button)ritualsEnabled.button->setLookAndFeel(nullptr);
    grimoireToggle.setLookAndFeel(nullptr);undercroftToggle.setLookAndFeel(nullptr);setLookAndFeel(nullptr);
}

void HorrorCastleEditor::setAdvancedComponentsVisible(bool show)
{
    auto setControlVisible=[&](Control& c){if(c.label)c.label->setVisible(show);if(c.slider)c.slider->setVisible(show);};
    for(auto* c:{&corpsePosition,&corpseRot,&corpseFormant,&corpseInharmonic,
                  &crypt.f1Drive,&crypt.f2Drive,&tower.f1Drive,&tower.f2Drive,
                  &bloodFeed,&aetherLeak,&soulExchange,&possessionHaunt,
                  &ritualsBpm,&ritualsGate,&ritualsProbability,&ritualsSwing,&ritualsOctaves})
        setControlVisible(*c);

    for(auto* t:{&crypt.crossFm,&crypt.crossRing,&tower.crossFm,&tower.crossRing,&ritualsEnabled})
        if(t->button)t->button->setVisible(show);

    for(auto* c:{&ritualsPattern,&ritualsRate}){
        if(c->label)c->label->setVisible(show);
        if(c->box)c->box->setVisible(show);
    }

    for(auto* l:{&undercroftTitle,&corpseTitle,&corpseStatus,&advancedCryptTitle,&advancedTowerTitle,&possessionTitle,&ritualsTitle})
        l->setVisible(show);
}

void HorrorCastleEditor::setGrimoireVisible(bool show)
{
    if(show)setUndercroftVisible(false);
    grimoire.setVisible(show);
    grimoireToggle.setButtonText(show?"RETURN TO CASTLE":"GRIMOIRE");
    hexMatrix.setVisible(!show && !undercroftVisible);
    curseInspector.setVisible(!show && !undercroftVisible);
    if(show)grimoire.toFront(false);
}

void HorrorCastleEditor::setUndercroftVisible(bool show)
{
    undercroftVisible=show;
    if(show && grimoire.isVisible()){
        grimoire.setVisible(false);
        grimoireToggle.setButtonText("GRIMOIRE");
    }
    setAdvancedComponentsVisible(show);
    undercroftToggle.setButtonText(show?"RETURN TO CASTLE":"UNDERCROFT");
    hexMatrix.setVisible(!show && !grimoire.isVisible());
    curseInspector.setVisible(!show && !grimoire.isVisible());
    repaint();
}

void HorrorCastleEditor::resized()
{
    if(!uiReady)return;

    const float sx=(float)getWidth()/1448.0f;
    const float sy=(float)getHeight()/1086.0f;
    auto R=[&](int x,int y,int w,int h){
        return Rectangle<int>((int)std::lround(x*sx),(int)std::lround(y*sy),
                              (int)std::lround(w*sx),(int)std::lround(h*sy));
    };
    auto B=[&](Component& c,int x,int y,int w,int h){c.setBounds(R(x,y,w,h));};

    B(header,0,0,1448,96);
    B(undercroftToggle,1120,28,140,26);
    B(grimoireToggle,1270,28,150,26);

    B(crypt,4,88,634,432);
    B(tower,810,88,634,432);
    B(centerSpine,620,92,208,538);

    auto sceneLayout=[&](ScenePanel& p,bool isCrypt){
        const int start=isCrypt?148:834;
        const int step=126;
        for(int i=0;i<3;++i){
            const int bx=start+i*step;
            B(*p.genType[i].label,bx,132,110,15); B(*p.genType[i].box,bx,148,108,22);
            B(*p.genLevel[i].label,bx,178,52,14); B(*p.genLevel[i].slider,bx-4,191,58,64);
            B(*p.genShape[i].label,bx+56,178,52,14); B(*p.genShape[i].slider,bx+52,191,58,64);
            B(*p.genTune[i].label,bx+20,263,55,14); B(*p.genTune[i].slider,bx+15,276,60,64);
        }

        const int yLab=348,yKnob=361;
        const int x0=start;
        std::array<Control*,5> row{&p.noise,&p.f1Cut,&p.f1Res,&p.f2Cut,&p.f2Res};
        for(int i=0;i<5;++i){B(*row[(size_t)i]->label,x0+i*67,yLab,62,14);B(*row[(size_t)i]->slider,x0+i*67-1,yKnob,60,59);}
        B(*p.route.label,x0+335,yLab,72,14);B(*p.route.box,x0+331,365,88,22);

        const int my=432;
        B(*p.master.label,x0+68,my,65,14);B(*p.master.slider,x0+70,my+13,62,61);
        B(*p.balance.label,x0+157,my,70,14);B(*p.balance.slider,x0+160,my+13,62,61);
        B(*p.character.label,x0+246,my,70,14);B(*p.character.slider,x0+250,my+13,62,61);
    };
    sceneLayout(crypt,true);sceneLayout(tower,false);

    B(ritualTitle,36,527,150,20);B(graveTitle,760,527,170,20);
    B(*ritualMode.label,38,551,105,14);B(*ritualMode.box,38,567,112,22);
    auto knob=[&](Control& c,int x,int y){B(*c.label,x,y,68,14);B(*c.slider,x,y+13,68,58);};
    int x=170;for(auto* c:{&ritualMix,&ritualDepth,&ritualDrive,&ritualWidth,&ritualFeedback}){knob(*c,x,548);x+=80;}
    knob(hexAmount,585,548);
    x=765;for(auto* c:{&graveReverb,&graveDelay,&graveFeedback,&graveTone,&graveOutput}){knob(*c,x,548);x+=86;}
    B(status,1190,608,230,15);

    B(hexMatrix,8,636,952,438);
    B(curseInspector,968,636,472,438);
    B(grimoire,20,100,1408,966);

    // UNDERCROFT / advanced page occupies the lower Castle only.
    B(undercroftTitle,36,654,400,24);
    B(corpseTitle,38,688,360,20);B(corpseStatus,38,708,360,18);
    int cx=38;for(auto* c:{&corpsePosition,&corpseRot,&corpseFormant,&corpseInharmonic}){knob(*c,cx,733);cx+=88;}

    B(advancedCryptTitle,410,688,160,20);knob(crypt.f1Drive,410,716);knob(crypt.f2Drive,482,716);
    B(*crypt.crossFm.button,410,800,132,24);B(*crypt.crossRing.button,548,800,132,24);

    B(advancedTowerTitle,700,688,160,20);knob(tower.f1Drive,700,716);knob(tower.f2Drive,772,716);
    B(*tower.crossFm.button,700,800,132,24);B(*tower.crossRing.button,838,800,132,24);

    B(possessionTitle,990,688,220,20);
    int px=990;for(auto* c:{&bloodFeed,&aetherLeak,&soulExchange,&possessionHaunt}){knob(*c,px,716);px+=92;}

    B(ritualsTitle,38,856,160,20);B(*ritualsEnabled.button,38,884,92,26);
    B(*ritualsPattern.label,144,858,100,14);B(*ritualsPattern.box,144,876,132,22);
    B(*ritualsRate.label,286,858,72,14);B(*ritualsRate.box,286,876,82,22);
    int rx=390;for(auto* c:{&ritualsBpm,&ritualsGate,&ritualsProbability,&ritualsSwing,&ritualsOctaves}){knob(*c,rx,856);rx+=100;}
}

void HorrorCastleEditor::timerCallback()
{
    atmospherePhase+=.025f;
    updateHexVisuals();
    repaint();
}

void HorrorCastleEditor::updateHexVisuals()
{
    std::array<float,22> destinationValues{};
    std::array<float,8> amounts{};
    std::array<int,8> destinations{};
    auto read=[&](const String& id,float fallback){if(auto* p=apvts.getRawParameterValue(id))return p->load();return fallback;};
    for(int i=1;i<=8;++i){
        const auto prefix=String("hex.curse")+String(i)+".";
        const int source=jlimit(0,6,(int)std::lround(read(prefix+"source",0.f)));
        const int curse=jlimit(0,6,(int)std::lround(read(prefix+"curse",0.f)));
        const int destination=jlimit(0,21,(int)std::lround(read(prefix+"destination",0.f)));
        const float amount=read(prefix+"amount",0.f);
        amounts[(size_t)(i-1)]=amount;destinations[(size_t)(i-1)]=destination;
        HorrorCastle::CurseLane lane{};
        lane.source=(HorrorCastle::ModSource)source;lane.curse=(HorrorCastle::CurseType)curse;
        lane.destination=(HorrorCastle::ModDestination)destination;lane.amount=amount;lane.enabled=(source!=0&&destination!=0);
        hexMatrix.setLane(i-1,lane);hexMatrix.setLaneValue(i-1,processor.getHexLaneActivity(i-1));
        if(destination>0&&destination<22&&destination!=10)destinationValues[(size_t)destination]+=amount;
    }
    float chainInfluence=0.f;
    for(int i=0;i<inspectedLane;++i)if(destinations[(size_t)i]==10)chainInfluence+=amounts[(size_t)i]*.35f;
    chainInfluence=jlimit(-1.f,1.f,chainInfluence);destinationValues[10]=chainInfluence;
    for(size_t i=0;i<destinationValues.size();++i)
        hexMatrix.setDestinationValue((HorrorCastle::ModDestination)i,processor.getHexDestinationValue((int)i));
    curseInspector.setLiveActivity(processor.getHexLaneActivity(jlimit(0,7,inspectedLane)));
    curseInspector.setChainInfluence(chainInfluence);
}

void HorrorCastleEditor::paint(Graphics& g)
{
    g.fillAll(theme::stoneBlack());

    if(backdropImage.isValid()){
        g.setOpacity(.44f);
        g.drawImageWithin(backdropImage,0,82,getWidth(),getHeight()-82,RectanglePlacement::fillDestination);
        g.setOpacity(1.0f);
    }

    ColourGradient veil(Colour(0xff030508).withAlpha(.35f),0,90,
                        Colour(0xff09070b).withAlpha(.72f),(float)getWidth(),(float)getHeight(),false);
    g.setGradientFill(veil);g.fillRect(getLocalBounds().withTrimmedTop(84));

    if(ritualFrameImage.isValid()){
        g.setOpacity(.94f);
        g.drawImageWithin(ritualFrameImage,0,(int)std::lround(505.0*getHeight()/1086.0),
                          getWidth(),(int)std::lround(137.0*getHeight()/1086.0),RectanglePlacement::stretchToFit);
        g.setOpacity(1.0f);
    }else{
        CastleGraphics::drawStonePanel(g,{8,515,(float)getWidth()-16,118},theme::gold(),5);
    }

    if(undercroftVisible){
        auto area=Rectangle<float>(8.0f,636.0f,(float)getWidth()-16.0f,(float)getHeight()-648.0f);
        if(undercroftImage.isValid()){
            g.setOpacity(.38f);g.drawImageWithin(undercroftImage,(int)area.getX(),(int)area.getY(),
                                                (int)area.getWidth(),(int)area.getHeight(),RectanglePlacement::fillDestination);g.setOpacity(1.0f);
        }
        CastleGraphics::drawStonePanel(g,area,theme::gold(),7.0f);
        CastleGraphics::drawRuneBorder(g,area,theme::gold(),.16f);
        g.setColour(theme::cryptRed().withAlpha(.14f));g.fillRect(area.getX()+20.0f,area.getY()+42.0f,350.0f,1.0f);
        g.setColour(theme::towerPurple().withAlpha(.14f));g.fillRect(area.getX()+690.0f,area.getY()+42.0f,350.0f,1.0f);
    }

    // Restrained moving mist: visual only.
    for(int i=0;i<5;++i){
        const float travel=(float)getWidth()+420.0f;
        const float x=std::fmod(atmospherePhase*(11.0f+i*2.2f)+i*310.0f,travel)-210.0f;
        const float y=110.0f+i*188.0f;
        g.setColour(theme::fog().withAlpha(.008f+(i%2)*.004f));
        g.fillEllipse(x,y,420.0f,72.0f);
    }
}

} // namespace horrorcastle
