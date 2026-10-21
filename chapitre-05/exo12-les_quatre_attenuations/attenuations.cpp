#include "NKAudio/NkAudio.h"

#include <cmath>
#include <cstdio>
#include <vector>

using namespace nkentseu;

static const int   kModels = 4;
static const int   kDists  = 5;
static const int   kRolls  = 2;

static const audio::AttenuationModel kModel[kModels] = {
    audio::AttenuationModel::NONE,
    audio::AttenuationModel::INVERSE,
    audio::AttenuationModel::LINEAR,
    audio::AttenuationModel::EXPONENTIAL
};
static const char*  kName[kModels]  = {"NONE", "INVERSE", "LINEAR", "EXPONENTIAL"};
static const float  kDist[kDists]   = {0.5f, 1.f, 2.f, 4.f, 8.f};
static const float  kRoll[kRolls]   = {1.f, 2.f};

static double MesureRms(audio::AudioSample& son, audio::AttenuationModel modele,
                        float distance, float rolloff)
{
    audio::AudioEngine& eng = audio::AudioEngine::Instance();

    audio::VoiceParams p;
    p.looping = true;
    p.source3d.positional        = true;
    p.source3d.minDistance       = 1.0f;
    p.source3d.maxDistance       = 100.0f;
    p.source3d.rolloffFactor     = rolloff;
    p.source3d.attenuationModel  = modele;
    p.source3d.position[0] = 0.f;
    p.source3d.position[1] = 0.f;
    p.source3d.position[2] = -distance;
    audio::AudioHandle h = eng.Play(son, p);

    const int total = 48000;
    const int block = 256;
    std::vector<float> buf((size_t)block * 2);  
    double somme = 0.0;
    int done = 0;
    while (done < total)
    {
        int n = (total - done < block) ? (total - done) : block; 
        eng.RenderToBuffer(buf.data(), n, 2);
        for (int i = 0; i < 2 * n; ++i)
            somme += (double)buf[(size_t)i] * buf[(size_t)i];
        done += n;
    }

    eng.Stop(h); 
    return std::sqrt(somme / (2.0 * total));
}

int main()
{
    audio::AudioEngineConfig cfg;
    cfg.sampleRate = 48000;
    cfg.backend = audio::AudioBackendType::NULL_OUTPUT;  

    audio::AudioEngine& eng = audio::AudioEngine::Instance();
    if (!eng.Initialize(cfg)) return 1;

    eng.SetListenerPosition(0.f, 0.f, 0.f);
    eng.SetListenerOrientation(0.f, 0.f, -1.f, 0.f, 1.f, 0.f);

    audio::AudioSample son =
        audio::AudioGenerator::GenerateTone(440.f, 2.f, audio::WaveformType::SINE, 48000, 0.1f);

    double rms[kRolls][kModels][kDists];
    for (int r = 0; r < kRolls; ++r)
        for (int d = 0; d < kDists; ++d)
            for (int m = 0; m < kModels; ++m)
                rms[r][m][d] = MesureRms(son, kModel[m], kDist[d], kRoll[r]);

    FILE* out = std::fopen("mesures.txt", "w");
    if (!out) return 2;

    for (int r = 0; r < kRolls; ++r)
        for (int m = 0; m < kModels; ++m)
            for (int d = 0; d < kDists; ++d)
            {
                double gain = rms[r][m][d] / rms[r][0][d];
                std::fprintf(out, "%s, rolloff %g, %g m : %.3f\n",
                             kName[m], (double)kRoll[r], (double)kDist[d], gain);
                std::printf("%s, rolloff %g, %g m : %.3f   (rms %.6f, rms NONE %.6f)\n",
                            kName[m], (double)kRoll[r], (double)kDist[d], gain,
                            rms[r][m][d], rms[r][0][d]);
            }
    std::fclose(out);

    eng.Shutdown();
    audio::AudioLoader::Free(son);   
    return 0;
}