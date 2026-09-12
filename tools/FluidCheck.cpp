#include <JuceHeader.h>
#include "../Source/HorrorCastle/VortexEngine.h"
#include "../Source/HorrorCastle/SirenEngine.h"
#include "../Source/HorrorCastle/CreatureRoutingMatrix.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
struct Stats { double rms=0.0; float peak=0.0f; bool finite=true; };

template<class Engine,class State,class Render>
Stats render(Engine& e,State& s,Render&& r){
    constexpr int N=48000; double ss=0.0; float peak=0.0f; bool finite=true;
    for(int i=0;i<N;++i){ float y=r(e,s); if(!std::isfinite(y)){finite=false;y=0.0f;} ss+=(double)y*y; peak=std::max(peak,std::abs(y)); }
    return {std::sqrt(ss/(double)N),peak,finite};
}

bool separation(double a,double b,double minDelta){ return std::abs(a-b)>=minDelta; }
}

int main(){
    bool ok=true;
    horrorcastle::VortexEngine vortex; horrorcastle::SirenEngine siren;
    horrorcastle::VortexEngine::VoiceState v0{},vT{},vD{},vE{},vX{};
    horrorcastle::SirenEngine::VoiceState s0{},sA{},sAe{},sE{},sX{};

    auto vr=[&](auto& st,float t,float d,float e,float vel){return render(vortex,st,[&](auto& eng,auto& state){return eng.renderSample(state,110.0f,t,d,e,vel,48000.0);});};
    auto sr=[&](auto& st,float ap,float ae,float e,float vel){return render(siren,st,[&](auto& eng,auto& state){return eng.renderSample(state,220.0f,ap,ae,e,vel,48000.0);});};

    auto vb=vr(v0,.25f,.25f,.45f,.78f); auto vt=vr(vT,.90f,.25f,.45f,.78f); auto vd=vr(vD,.25f,.90f,.45f,.78f); auto ve=vr(vE,.25f,.25f,.95f,.78f); auto vx=vr(vX,1.0f,1.0f,1.0f,1.0f);
    auto sb=sr(s0,.25f,.25f,.45f,.78f); auto sa=sr(sA,.88f,.25f,.45f,.78f); auto sae=sr(sAe,.25f,.92f,.45f,.78f); auto se=sr(sE,.25f,.25f,.95f,.78f); auto sx=sr(sX,1.0f,1.0f,1.0f,1.0f);

    auto check=[&](bool cond,const char* label,double a=0,double b=0){ std::cout<<(cond?"PASS  ":"FAIL  ")<<label; if(a||b)std::cout<<"  "<<a<<" -> "<<b; std::cout<<"\n"; ok&=cond; };
    check(vb.finite&&vb.rms>1.0e-4,"VORTEX audible / finite",vb.rms,vb.peak);
    check(separation(vb.rms,vt.rms,2.0e-4),"VORTEX TURBULENCE",vb.rms,vt.rms);
    check(separation(vb.rms,vd.rms,2.0e-4),"VORTEX DREAD",vb.rms,vd.rms);
    check(separation(vb.rms,ve.rms,2.0e-4),"VORTEX expression",vb.rms,ve.rms);
    check(vx.finite&&vx.peak<=1.0001f,"VORTEX bounded extreme coupling",vx.rms,vx.peak);

    check(sb.finite&&sb.rms>1.0e-4,"SIREN audible / finite",sb.rms,sb.peak);
    check(separation(sb.rms,sa.rms,2.0e-4),"SIREN APERTURE",sb.rms,sa.rms);
    check(separation(sb.rms,sae.rms,2.0e-4),"SIREN AETHER",sb.rms,sae.rms);
    check(separation(sb.rms,se.rms,2.0e-4),"SIREN expression",sb.rms,se.rms);
    check(sx.finite&&sx.peak<=1.0001f,"SIREN bounded extreme coupling",sx.rms,sx.peak);

    horrorcastle::CreatureRoutingMatrix wetMatrix, dryMatrix;
    horrorcastle::CreatureRoutingMatrix::Route route;
    route.sourceCreature=0;
    route.sourceSignal=horrorcastle::CreatureStateBus::Signal::Event;
    route.destinationCreature=1;
    route.destinationSignal=horrorcastle::CreatureStateBus::Signal::Event;
    route.amount=.85f;
    route.enabled=true;
    wetMatrix.setRoute(0,route);
    route.enabled=false;
    dryMatrix.setRoute(0,route);

    horrorcastle::CreatureRoutingMatrix::StateArray sources{}, wetInbox{}, dryInbox{};
    horrorcastle::VortexEngine::VoiceState ecologyV{};
    horrorcastle::SirenEngine::VoiceState ecologyDry{}, ecologyWet{}, ecologyExtreme{};
    constexpr int N=48000;
    double drySS=0.0, wetSS=0.0, diffSS=0.0; float wetPeak=0.0f, eventPeak=0.0f; bool ecologyFinite=true;
    for(int i=0;i<N;++i){
        vortex.renderSample(ecologyV,82.41f,1.0f,1.0f,1.0f,1.0f,48000.0);
        sources[0]=horrorcastle::VortexEngine::stateBus(ecologyV);
        eventPeak=std::max(eventPeak,sources[0].get(horrorcastle::CreatureStateBus::Signal::Event));
        wetMatrix.process(sources,wetInbox);
        dryMatrix.process(sources,dryInbox);
        const float wetEvent=wetInbox[1].get(horrorcastle::CreatureStateBus::Signal::Event);
        const float dryEvent=dryInbox[1].get(horrorcastle::CreatureStateBus::Signal::Event);
        const float dry=siren.renderSample(ecologyDry,220.0f,.42f,.78f,.68f,.90f,48000.0,dryEvent,1.0f);
        const float wet=siren.renderSample(ecologyWet,220.0f,.42f,.78f,.68f,.90f,48000.0,wetEvent,1.0f);
        if(!std::isfinite(dry)||!std::isfinite(wet)){ ecologyFinite=false; continue; }
        drySS+=(double)dry*dry; wetSS+=(double)wet*wet; const double d=(double)wet-dry; diffSS+=d*d; wetPeak=std::max(wetPeak,std::abs(wet));
        const float extreme=siren.renderSample(ecologyExtreme,220.0f,1.0f,1.0f,1.0f,1.0f,48000.0,1.0f,1.0f);
        if(!std::isfinite(extreme)||std::abs(extreme)>1.0001f) ecologyFinite=false;
    }
    const double dryRms=std::sqrt(drySS/N), wetRms=std::sqrt(wetSS/N), diffRms=std::sqrt(diffSS/N);
    check(eventPeak>1.0e-4f,"VORTEX publishes collapse EVENT",eventPeak,0.0);
    check(ecologyFinite&&wetPeak<=1.0001f,"Ecology route finite / bounded",wetRms,wetPeak);
    check(diffRms>2.0e-4,"Routing Matrix: VORTEX EVENT -> SIREN plenum is audible",dryRms,wetRms);
    check(std::abs(ecologyDry.ecologicalImpulse)<1.0e-7f,"Disabled matrix route remains disconnected",ecologyDry.ecologicalImpulse,0.0);
    check(horrorcastle::SirenEngine::stateBus(ecologyWet).get(horrorcastle::CreatureStateBus::Signal::Pressure)>0.0f,"SIREN republishes disturbed physical state");

    return ok?0:1;
}
