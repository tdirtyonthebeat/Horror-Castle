#include <JuceHeader.h>
#include "../Source/HorrorCastle/MarrowEngine.h"
#include "../Source/HorrorCastle/OrreryEngine.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
using horrorcastle::MarrowEngine;
using horrorcastle::OrreryEngine;
constexpr double sampleRate=48000.0;
constexpr int samples=48000;

std::vector<float> renderMarrow(float viscosity,float dread,float expression){
    MarrowEngine e; MarrowEngine::VoiceState s; std::vector<float> out; out.reserve(samples);
    for(int i=0;i<samples;++i) out.push_back(e.renderSample(s,110.0f,viscosity,dread,expression,0.82f,sampleRate)); return out;
}
std::vector<float> renderOrrery(float orbit,float aether,float expression){
    OrreryEngine e; OrreryEngine::VoiceState s; std::vector<float> out; out.reserve(samples);
    for(int i=0;i<samples;++i) out.push_back(e.renderSample(s,110.0f,orbit,aether,expression,0.82f,sampleRate)); return out;
}
bool finite(const std::vector<float>& x){for(float v:x)if(!std::isfinite(v)||std::abs(v)>1.001f)return false;return true;}
float rms(const std::vector<float>& x){double z=0;for(float v:x)z+=(double)v*v;return x.empty()?0.0f:(float)std::sqrt(z/x.size());}
float diff(const std::vector<float>& a,const std::vector<float>& b){const auto n=std::min(a.size(),b.size());double z=0;for(size_t i=0;i<n;++i){double d=(double)a[i]-b[i];z+=d*d;}const float d=n?(float)std::sqrt(z/n):0.0f;return d/std::max(0.0001f,std::max(rms(a),rms(b)));}
}

int main(){
    int failures=0; auto check=[&](bool ok,const char* name){std::cout<<(ok?"PASS  ":"FAIL  ")<<name<<'\n';if(!ok)++failures;};
    auto thin=renderMarrow(0.08f,0.65f,0.55f), viscous=renderMarrow(0.92f,0.65f,0.55f);
    check(finite(thin)&&finite(viscous)&&rms(thin)>0.001f&&rms(viscous)>0.001f,"MARROW remains bounded and audible");
    std::cout<<"INFO  MARROW viscosity difference="<<diff(thin,viscous)<<'\n'; check(diff(thin,viscous)>0.12f,"MARROW viscosity changes string/fluid body");
    auto shallow=renderMarrow(0.55f,0.08f,0.55f), deep=renderMarrow(0.55f,0.92f,0.55f); check(diff(shallow,deep)>0.10f,"MARROW DREAD changes internal coupling");
    auto whisper=renderMarrow(0.55f,0.65f,0.03f), bowed=renderMarrow(0.55f,0.65f,1.0f); check(diff(whisper,bowed)>0.12f,"MARROW expression changes stick-slip regime");

    auto tight=renderOrrery(0.05f,0.60f,0.50f), wandering=renderOrrery(0.95f,0.60f,0.50f);
    check(finite(tight)&&finite(wandering)&&rms(tight)>0.001f&&rms(wandering)>0.001f,"ORRERY remains bounded and audible");
    std::cout<<"INFO  ORRERY orbit difference="<<diff(tight,wandering)<<'\n'; check(diff(tight,wandering)>0.08f,"ORRERY orbit depth changes resonant relationships");
    auto earth=renderOrrery(0.60f,0.05f,0.50f), ether=renderOrrery(0.60f,0.95f,0.50f); check(diff(earth,ether)>0.08f,"ORRERY AETHER changes orbital coupling field");
    auto still=renderOrrery(0.60f,0.60f,0.03f), pressed=renderOrrery(0.60f,0.60f,1.0f); check(diff(still,pressed)>0.08f,"ORRERY expression changes resonant field response");
    std::cout<<(failures?"\nHorror Castle Bestiary check FAILED.\n":"\nHorror Castle Bestiary check passed.\n"); return failures?1:0;
}
