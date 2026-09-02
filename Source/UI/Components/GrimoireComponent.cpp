#include "GrimoireComponent.h"
#include "../../Core/HorrorCastleProcessor.h"
#include "../Theme/CastleTheme.h"
#include "../Theme/CastleGraphics.h"
#include <algorithm>

namespace horrorcastle {
using namespace juce;

static void setupBookLabel(Label& l,float size,bool bold=false)
{
    l.setColour(Label::textColourId,theme::bone());
    l.setFont(bold ? Font(FontOptions(size)).boldened() : Font(FontOptions(size)));
    l.setJustificationType(Justification::centredLeft);
}

GrimoireComponent::GrimoireComponent(HorrorCastleProcessor& p):processor(p),spells(p.getFactoryPresetSpells())
{
    setOpaque(true);
    getProperties().set("castleAccent","grimoire");

    category.addItem("ALL SPELLS",1); category.addItem("CRYPT",2); category.addItem("TOWER",3);
    category.addItem("POSSESSION",4); category.addItem("HEXED",5); category.addItem("RITUALS",6);
    category.addItem("ATMOSPHERE",7); category.addItem("CORPSE",8); category.addItem("FAVORITES",9); category.setSelectedId(1,dontSendNotification);
    category.getProperties().set("castleAccent","grimoire"); addAndMakeVisible(category);

    search.setTextToShowWhenEmpty("SEARCH THE GRIMOIRE...",theme::bone().withAlpha(.35f));
    search.setColour(TextEditor::backgroundColourId,Colour(0xff090a0d)); search.setColour(TextEditor::textColourId,theme::bone());
    search.setColour(TextEditor::outlineColourId,theme::towerPurple().withAlpha(.55f)); addAndMakeVisible(search);

    for(int i=0;i<8;++i){
        auto& b=spellButtons[(size_t)i]; b.getProperties().set("castleAccent","grimoire"); addAndMakeVisible(b);
        b.onClick=[this,i]{ const int slot=page*8+i; if(slot<(int)filtered.size()) selectFactoryIndex(filtered[(size_t)slot]); };
    }

    for(auto* b:{&closeButton,&previousButton,&nextButton,&summonButton,&favouriteButton,&saveButton,&loadButton}){
        b->getProperties().set("castleAccent","grimoire"); addAndMakeVisible(*b);
    }
    closeButton.onClick=[this]{if(onClose)onClose();};
    previousButton.onClick=[this]{page=jmax(0,page-1);refreshButtons();};
    nextButton.onClick=[this]{page=jmin(pageCount()-1,page+1);refreshButtons();};
    summonButton.onClick=[this]{ if(selected>=0&&processor.loadFactoryPreset(selected)){ if(onStatus)onStatus("GRIMOIRE // "+spells[(size_t)selected].name.toUpperCase()+" SUMMONED"); } };
    favouriteButton.onClick=[this]{ if(selected>=0&&selected<(int)favourites.size()){ favourites[(size_t)selected]=!favourites[(size_t)selected]; updateDetail(); if(category.getSelectedId()==9)applyFilter(); } };
    saveButton.onClick=[this]{ const auto base=selected>=0?spells[(size_t)selected].name:"Horror Castle"; const auto f=processor.saveUserPreset(base+" User Spell"); if(onStatus)onStatus("SAVED // "+f.getFileName()); };
    loadButton.onClick=[this]{ chooser=std::make_unique<FileChooser>("Open a Grimoire spell",processor.getUserPresetDirectory(),"*.hcg"); chooser->launchAsync(FileBrowserComponent::openMode|FileBrowserComponent::canSelectFiles,[this](const FileChooser& fc){auto f=fc.getResult();if(f.existsAsFile()&&processor.loadUserPreset(f)&&onStatus)onStatus("LOADED // "+f.getFileName());}); };

    setupBookLabel(title,24,true); setupBookLabel(subtitle,14,true); setupBookLabel(description,12,false); setupBookLabel(tags,10,true); setupBookLabel(pageLabel,10,false);
    description.setJustificationType(Justification::topLeft); description.setMinimumHorizontalScale(.86f);
    addAndMakeVisible(title);addAndMakeVisible(subtitle);addAndMakeVisible(description);addAndMakeVisible(tags);addAndMakeVisible(pageLabel);

    category.onChange=[this]{page=0;applyFilter();}; search.onTextChange=[this]{page=0;applyFilter();};
    applyFilter();
}

GrimoireComponent::~GrimoireComponent()=default;

int GrimoireComponent::pageCount() const { return jmax(1,((int)filtered.size()+7)/8); }

void GrimoireComponent::applyFilter()
{
    filtered.clear();
    const auto cat=category.getText(); const auto q=search.getText().trim().toLowerCase();
    for(int i=0;i<(int)spells.size();++i){
        const auto& s=spells[(size_t)i];
        if(cat=="FAVORITES"&&!favourites[(size_t)i])continue;
        if(cat!="ALL SPELLS"&&cat!="FAVORITES"&&s.category!=cat)continue;
        const auto hay=(s.name+" "+s.subtitle+" "+s.description+" "+s.tags).toLowerCase();
        if(q.isNotEmpty()&&!hay.contains(q))continue;
        filtered.push_back(i);
    }
    page=jlimit(0,pageCount()-1,page);
    if(!filtered.empty()&&(selected<0||std::find(filtered.begin(),filtered.end(),selected)==filtered.end()))selected=filtered.front();
    if(filtered.empty())selected=-1;
    refreshButtons();updateDetail();repaint();
}

void GrimoireComponent::refreshButtons()
{
    for(int i=0;i<8;++i){
        auto& b=spellButtons[(size_t)i]; const int slot=page*8+i;
        if(slot<(int)filtered.size()){
            const int idx=filtered[(size_t)slot]; b.setVisible(true); b.setButtonText((idx==selected?"✦  ":"")+spells[(size_t)idx].name);
        }else{b.setVisible(false);b.setButtonText({});}
    }
    previousButton.setEnabled(page>0);nextButton.setEnabled(page+1<pageCount());
    pageLabel.setText("PAGE "+String(page+1)+" / "+String(pageCount())+"   •   "+String(filtered.size())+" SPELLS",dontSendNotification);
}

void GrimoireComponent::selectFactoryIndex(int i)
{
    selected=i;
    refreshButtons();
    updateDetail();
    repaint();

    // v1.3: browsing is auditioning. A single click recalls the spell so the
    // Grimoire behaves like an instrument browser instead of a two-step form.
    if(selected>=0 && selected<(int)spells.size() && processor.loadFactoryPreset(selected))
        if(onStatus) onStatus("GRIMOIRE // "+spells[(size_t)selected].name.toUpperCase()+" SUMMONED");
}

void GrimoireComponent::updateDetail()
{
    if(selected<0){title.setText("NO SPELLS FOUND",dontSendNotification);subtitle.setText({},dontSendNotification);description.setText("Change the category or search phrase.",dontSendNotification);tags.setText({},dontSendNotification);summonButton.setEnabled(false);favouriteButton.setEnabled(false);return;}
    const auto& s=spells[(size_t)selected]; title.setText(s.name.toUpperCase(),dontSendNotification); subtitle.setText(s.subtitle,dontSendNotification); description.setText(s.description,dontSendNotification); tags.setText(s.tags,dontSendNotification);
    summonButton.setEnabled(true); favouriteButton.setEnabled(true); favouriteButton.setButtonText(favourites[(size_t)selected]?"★ FAVORITE":"☆ FAVORITE");
}

void GrimoireComponent::paint(Graphics& g)
{
    g.fillAll(Colour(0xff040507)); auto outer=getLocalBounds().toFloat().reduced(6.0f);
    CastleGraphics::drawStonePanel(g,outer,theme::towerPurple(),12.0f); CastleGraphics::drawRuneBorder(g,outer,theme::towerPurple(),.34f);
    g.setColour(theme::towerGlow().withAlpha(.88f));g.setFont(Font(FontOptions(19.0f)).boldened());g.drawText("GRIMOIRE // THE CASTLE REMEMBERS",22,14,getWidth()-180,28,Justification::centredLeft);

    auto body=getLocalBounds().toFloat().reduced(22.0f);body.removeFromTop(54.0f);
    auto right=body.removeFromRight(body.getWidth()*.39f).reduced(7.0f); auto left=body.reduced(7.0f);
    CastleGraphics::drawStonePanel(g,left,theme::graveViolet(),8.0f);

    // Open book: two parchment leaves, darkened to keep text readable.
    auto book=right.reduced(4.0f);auto leftPage=book.withWidth(book.getWidth()*.50f);auto rightPage=book.withTrimmedLeft(book.getWidth()*.50f);
    g.setColour(Colour(0xff8f7b62));g.fillRoundedRectangle(book,8.0f);
    ColourGradient paper(Colour(0xffc5b08c),book.getX(),book.getY(),Colour(0xff8c775f),book.getRight(),book.getBottom(),false);g.setGradientFill(paper);g.fillRoundedRectangle(book.reduced(3.0f),7.0f);
    g.setColour(Colour(0xff2a1e18).withAlpha(.22f));g.fillRect(rightPage.reduced(5.0f));
    g.setColour(Colour(0xff4b372b).withAlpha(.55f));g.drawLine(book.getCentreX(),book.getY()+7.0f,book.getCentreX(),book.getBottom()-7.0f,2.0f);

    if(selected>=0){
        const auto cat=spells[(size_t)selected].category;
        const auto accent=(cat=="CRYPT"||cat=="CORPSE"||cat=="HEXED")?theme::cryptRed():theme::towerPurple();
        CastleGraphics::drawSigil(g,leftPage.getCentre(),jmin(leftPage.getWidth(),leftPage.getHeight())*.24f,accent,(float)spells[(size_t)selected].sigil*.11f);
        g.setColour(Colour(0xff1d1714).withAlpha(.70f));g.setFont(Font(FontOptions(9.0f)).boldened());g.drawText(spells[(size_t)selected].category,leftPage.getX(),leftPage.getBottom()-34.0f,leftPage.getWidth(),18.0f,Justification::centred);
    }
}

void GrimoireComponent::resized()
{
    const int w=getWidth(),h=getHeight(); closeButton.setBounds(w-94,14,72,28);
    category.setBounds(28,64,154,26);search.setBounds(190,64,jmax(180,w/2-250),26);
    previousButton.setBounds(28,98,36,26);nextButton.setBounds(70,98,36,26);pageLabel.setBounds(116,98,260,26);

    const int leftW=(int)(w*.57f)-34; const int gridY=136; const int gap=8; const int cellW=(leftW-44-gap)/2; const int cellH=48;
    for(int i=0;i<8;++i){const int col=i%2,row=i/2;spellButtons[(size_t)i].setBounds(28+col*(cellW+gap),gridY+row*(cellH+gap),cellW,cellH);}
    saveButton.setBounds(28,h-54,148,30);loadButton.setBounds(184,h-54,148,30);

    const int rightX=(int)(w*.61f); const int rightW=w-rightX-35;
    title.setBounds(rightX+rightW/2+18,92,rightW/2-34,34);
    subtitle.setBounds(rightX+rightW/2+18,126,rightW/2-34,26);
    description.setBounds(rightX+rightW/2+18,158,rightW/2-34,jmax(80,h-310));
    tags.setBounds(rightX+rightW/2+18,h-130,rightW/2-34,22);
    favouriteButton.setBounds(rightX+rightW/2+18,h-98,120,30);
    summonButton.setBounds(rightX+rightW/2+146,h-98,jmax(110,rightW/2-164),30);
}

} // namespace horrorcastle
