#include "CastleGraphics.h"
#include "CastleTheme.h"

namespace horrorcastle::CastleGraphics {
using namespace juce;

void drawStonePanel(Graphics& g, Rectangle<float> r, Colour accent, float corner)
{
    ColourGradient bg(theme::stoneRaised().brighter(0.05f), r.getX(), r.getY(),
                      theme::stoneBlack(), r.getRight(), r.getBottom(), false);
    g.setGradientFill(bg);
    g.fillRoundedRectangle(r, corner);

    g.setColour(Colour(0xff030405));
    g.drawRoundedRectangle(r.reduced(1.0f), corner, 2.0f);
    g.setColour(theme::iron().withAlpha(0.92f));
    g.drawRoundedRectangle(r.reduced(3.0f), corner - 2.0f, 1.0f);
    g.setColour(accent.withAlpha(0.36f));
    g.drawRoundedRectangle(r.reduced(5.0f), corner - 3.0f, 1.0f);

    // Light procedural stone scratches: cheap, scalable, deterministic.
    g.setColour(Colour(0xff66707b).withAlpha(0.035f));
    for (int i=0;i<14;++i) {
        const float yy=r.getY()+8.0f+std::fmod(i*37.0f, jmax(12.0f,r.getHeight()-16.0f));
        const float xx=r.getX()+8.0f+std::fmod(i*83.0f, jmax(20.0f,r.getWidth()-80.0f));
        g.drawLine(xx,yy,jmin(r.getRight()-8.0f,xx+28.0f+(i%4)*19.0f),yy+(i%3-1)*2.0f,0.7f);
    }
}

void drawRuneBorder(Graphics& g, Rectangle<float> r, Colour accent, float alpha)
{
    r=r.reduced(8.0f);
    g.setColour(accent.withAlpha(alpha));
    const float step=34.0f;
    for(float x=r.getX()+12.0f;x<r.getRight()-12.0f;x+=step){
        Path p; p.startNewSubPath(x,r.getY()); p.lineTo(x+5,r.getY()+5); p.lineTo(x+10,r.getY());
        g.strokePath(p,PathStrokeType(0.8f));
        Path q; q.startNewSubPath(x,r.getBottom()); q.lineTo(x+5,r.getBottom()-5); q.lineTo(x+10,r.getBottom());
        g.strokePath(q,PathStrokeType(0.8f));
    }
    for(float y=r.getY()+18.0f;y<r.getBottom()-18.0f;y+=step){
        g.drawLine(r.getX(),y,r.getX()+5,y+5,0.8f); g.drawLine(r.getX()+5,y+5,r.getX(),y+10,0.8f);
        g.drawLine(r.getRight(),y,r.getRight()-5,y+5,0.8f); g.drawLine(r.getRight()-5,y+5,r.getRight(),y+10,0.8f);
    }
}

void drawGothicArch(Graphics& g, Rectangle<float> r, Colour accent)
{
    r=r.reduced(8.0f);
    const auto top=r.getY()+36.0f;
    const auto cx=r.getCentreX();
    Path arch;
    arch.startNewSubPath(r.getX(),r.getBottom());
    arch.lineTo(r.getX(),top+20.0f);
    arch.quadraticTo(r.getX()+r.getWidth()*0.17f,top-4.0f,cx,top-28.0f);
    arch.quadraticTo(r.getRight()-r.getWidth()*0.17f,top-4.0f,r.getRight(),top+20.0f);
    arch.lineTo(r.getRight(),r.getBottom());
    g.setColour(Colour(0xff050608).withAlpha(0.82f));
    g.strokePath(arch,PathStrokeType(4.0f));
    g.setColour(accent.withAlpha(0.30f));
    g.strokePath(arch,PathStrokeType(1.0f));
}

void drawSigil(Graphics& g, Point<float> c, float r, Colour accent, float phase)
{
    g.setColour(accent.withAlpha(0.10f)); g.fillEllipse(c.x-r*1.22f,c.y-r*1.22f,r*2.44f,r*2.44f);
    g.setColour(accent.withAlpha(0.62f));
    g.drawEllipse(c.x-r,c.y-r,r*2,r*2,1.2f);
    g.drawEllipse(c.x-r*.72f,c.y-r*.72f,r*1.44f,r*1.44f,0.8f);
    Path star;
    for(int i=0;i<10;++i){
        const float rr=(i%2==0)?r*.82f:r*.34f;
        const float a=phase-MathConstants<float>::halfPi+i*MathConstants<float>::pi/5.0f;
        Point<float> p{c.x+std::cos(a)*rr,c.y+std::sin(a)*rr};
        if(i==0)star.startNewSubPath(p);else star.lineTo(p);
    }
    star.closeSubPath(); g.strokePath(star,PathStrokeType(1.0f));
    g.drawLine(c.x-r*.88f,c.y,c.x+r*.88f,c.y,0.6f);
    g.drawLine(c.x,c.y-r*.88f,c.x,c.y+r*.88f,0.6f);
}

void drawTorch(Graphics& g, Point<float> base, Colour flame, float pulse)
{
    g.setColour(Colour(0xff2b2420)); g.fillRect(base.x-2.0f,base.y-16.0f,4.0f,16.0f);
    const float rr=7.0f+2.0f*std::sin(pulse);
    ColourGradient glow(flame.withAlpha(0.45f),base.x,base.y-20.0f,flame.withAlpha(0.0f),base.x+rr*2,base.y-20.0f,true);
    g.setGradientFill(glow); g.fillEllipse(base.x-rr,base.y-28.0f-rr,rr*2,rr*2);
    g.setColour(flame.withAlpha(0.95f));
    Path p; p.startNewSubPath(base.x,base.y-18.0f); p.quadraticTo(base.x-5.0f,base.y-25.0f,base.x,base.y-35.0f); p.quadraticTo(base.x+6.0f,base.y-25.0f,base.x,base.y-18.0f); g.fillPath(p);
}

} // namespace horrorcastle::CastleGraphics
