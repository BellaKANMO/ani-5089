#include "NKAudio/NkAudio.h"

#include <cmath>
#include <cstdio>
#include <vector>

using namespace nkentseu;

static bool Mesure(const char* nom, audio::AudioSample& pluie,
                   float x, float y, float z, bool positional, FILE* out)
{
    audio::AudioEngine& eng = audio::AudioEngine::Instance();

    audio::VoiceParams p;
    p.looping = true;
    p.source3d.positional  = positional;
    p.source3d.position[0] = x;
    p.source3d.position[1] = y;
    p.source3d.position[2] = z;
    p.source3d.minDistance = 1.0f;
    p.source3d.maxDistance = 8.0f;
    audio::AudioHandle h = eng.Play(pluie, p);

    const int total = 48000;
    const int block = 256;
    std::vector<float> buf((size_t)block * 2); 
    double sumL = 0.0;
    double sumR = 0.0;
    int done = 0;

    while (done < total)
    {
        int n = (total - done < block) ? (total - done) : block;  
        eng.RenderToBuffer(buf.data(), n, 2);
        for (int i = 0; i < n; ++i)
        {
            double l = buf[(size_t)i * 2];
            double r = buf[(size_t)i * 2 + 1];
            sumL += l * l;
            sumR += r * r;
        }
        done += n;
    }

    eng.Stop(h);

    double rmsL = std::sqrt(sumL / total);
    double rmsR = std::sqrt(sumR / total);

    std::fprintf(out, "%s %d : G %.6f D %.6f\n", nom, positional ? 1 : 0, rmsL, rmsR);
    std::printf("%s %d : G %.6f D %.6f\n", nom, positional ? 1 : 0, rmsL, rmsR);
    return true;
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

    audio::AudioSample pluie =
        audio::AudioGenerator::GenerateNoise(4.f, audio::WaveformType::NOISE_PINK);

    FILE* out = std::fopen("mesures.txt", "w");
    if (!out) return 2;

    Mesure("left",   pluie, -2.f, 0.f,  0.f, true,  out);
    Mesure("left",   pluie, -2.f, 0.f,  0.f, false, out);
    Mesure("devant", pluie,  0.f, 0.f, -2.f, true,  out);
    Mesure("devant", pluie,  0.f, 0.f, -2.f, false, out);
    Mesure("right",  pluie,  2.f, 0.f,  0.f, true,  out);
    Mesure("right",  pluie,  2.f, 0.f,  0.f, false, out);

    std::fclose(out);

    eng.Shutdown();
    audio::AudioLoader::Free(pluie); 
    return 0;
}