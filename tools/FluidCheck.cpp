#include <JuceHeader.h>
#include "../Source/HorrorCastle/VortexEngine.h"
#include "../Source/HorrorCastle/SirenEngine.h"
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

    return ok?0:1;
}
