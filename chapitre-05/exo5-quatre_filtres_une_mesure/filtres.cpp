#include <cstdio>
#include <fstream>
#include "NKImage/Core/NkImage.h"
#include "NKTime/NkChrono.h"
#include <utility>

using namespace nkentseu;

static const int NB_FILTRES = 4;
static const int REPETITIONS = 20;
static const char* NOMS[NB_FILTRES]  = { "NK_NEAREST", "NK_BILINEAR", "NK_BICUBIC", "NK_LANCZOS3" };
static const char* FICHIERS[NB_FILTRES] = { "nearest.png", "bilinear.png", "bicubic.png", "lanczos3.png" };

static NkResizeFilter Filtre(int i) {
    switch (i) {
        case 0: return NkResizeFilter::NK_NEAREST;
        case 1: return NkResizeFilter::NK_BILINEAR;
        case 2: return NkResizeFilter::NK_BICUBIC;
        default: return NkResizeFilter::NK_LANCZOS3;
    }
}

int main() {
    NkImage photo;
    if (!photo.Load("murs/photo.jpeg")) {              
        std::printf("Echec du chargement de photo.jpeg\n");
        return 1;
    }

    NkImage source = photo.Resize(2048, 2048, NkResizeFilter::NK_BILINEAR);
    std::printf("source : %d x %d\n", (int)source.Width(), (int)source.Height());

    NkImage resultats[NB_FILTRES];
    double moyennes[NB_FILTRES];

    for (int i = 0; i < NB_FILTRES; ++i) {
        double total = 0.0;
        for (int r = 0; r < REPETITIONS; ++r) {
            NkChrono chrono;
            NkImage petit = source.Resize(256, 256, Filtre(i));
            total += (double)chrono.Elapsed().ToMicroseconds();
            if (r == REPETITIONS - 1) resultats[i] = std::move(petit);
        }
        moyennes[i] = total / REPETITIONS;
        resultats[i].SavePNG(FICHIERS[i]);
        std::printf("%s : %d x %d\n", NOMS[i], (int)resultats[i].Width(), (int)resultats[i].Height());
    }

    std::ofstream out("mesures.txt");
    const NkImage& ref = resultats[1];
    for (int i = 0; i < NB_FILTRES; ++i) {
        long long differences = 0;
        for (int y = 0; y < (int)ref.Height(); ++y) {
            for (int x = 0; x < (int)ref.Width(); ++x) {
                auto a = ref.GetPixel(x, y);
                auto b = resultats[i].GetPixel(x, y);
                if (a.r != b.r || a.g != b.g || a.b != b.b || a.a != b.a) ++differences;
            }
        }
        out << NOMS[i] << " : " << (long long)moyennes[i] << " us, differences : " << differences << "\n";
        std::printf("%s : %lld us, differences : %lld\n", NOMS[i], (long long)moyennes[i], differences);
    }
    return 0;
}