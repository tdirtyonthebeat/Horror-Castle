#include <JuceHeader.h>
#include "../Source/HorrorCastle/ChoirBodyEngine.h"
#include <cmath>
#include <iostream>
#include <vector>

namespace {
using horrorcastle::ChoirBodyEngine;
std::vector<float> render(float vowel,float aether,float expression,float velocity=0.82f){constexpr double sampleRate=48000.0;constexpr int samples=48000;ChoirBodyEngine engine;ChoirBodyEngine::VoiceState state;std::vector<float> out;out.reserve(samples);for(int i=0;i<samples;++i)out.push_back(engine.renderSample(state,110.0f,vowel,aether,expression,velocity,sampleRate));return out;}
bool finite(const std::vector<float>& x){for(const auto s:x)if(!std::isfinite(s)||std::abs(s)>1.001f)return false;return true;}
float rms(const std::vector<float>& x){double sum=0.0;for(const auto s:x)sum+=(double)s*(double)s;return x.empty()?0.0f:(float)std::sqrt(sum/(double)x.size());}
float difference(const std::vector<float>& a,const std::vector<float>& b){const auto n=std::min(a.size(),b.size());double sum=0.0;for(size_t i=0;i<n;++i){const double d=(double)a[i]-(double)b[i];sum+=d*d;}const float absolute=n?(float)std::sqrt(sum/(double)n):0.0f;return absolute/std::max(0.0001f,std::max(rms(a),rms(b)));}
}

int main(){
    int failures=0;auto check=[&](bool ok,const char* name){std::cout<<(ok?"PASS  ":"FAIL  ")<<name<<'\n';if(!ok)++failures;};
    const auto closed=render(0.05f,0.55f,0.45f),open=render(0.95f,0.55f,0.45f);const float vowelDifference=difference(closed,open);
    std::cout<<"INFO  CHOIR VOWEL tract difference="<<vowelDifference<<" rms(low/high)="<<rms(closed)<<"/"<<rms(open)<<'\n';
    check(finite(closed)&&finite(open)&&rms(closed)>0.001f&&rms(open)>0.001f,"CHOIR remains bounded and audible");
    check(vowelDifference>0.15f,"CHOIR tract geometry changes the vocal body");
    const auto clustered=render(0.50f,0.05f,0.50f),dispersed=render(0.50f,0.95f,0.50f);const float aetherDifference=difference(clustered,dispersed);
    std::cout<<"INFO  CHOIR AETHER difference="<<aetherDifference<<'\n';check(finite(dispersed)&&aetherDifference>0.12f,"CHOIR AETHER changes mouth drift and congregation alignment");
    const auto whisper=render(0.52f,0.72f,0.03f),pressure=render(0.52f,0.72f,1.00f);const float expressionDifference=difference(whisper,pressure);
    std::cout<<"INFO  CHOIR expression difference="<<expressionDifference<<'\n';check(finite(pressure)&&expressionDifference>0.12f,"CHOIR expression changes breath and inter-mouth coupling");
    const auto extreme=render(1.0f,1.0f,1.0f,1.0f);check(finite(extreme),"CHOIR extreme coupling remains finite");
    std::cout<<(failures==0?"\nHorror Castle CHOIR check passed.\n":"\nHorror Castle CHOIR check FAILED.\n");return failures==0?0:1;
}
