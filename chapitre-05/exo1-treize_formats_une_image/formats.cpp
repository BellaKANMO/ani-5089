#include <cstdio>
#include <fstream>
#include <string>
#include "NKImage/Core/NkImage.h"

using namespace nkentseu;

static long long FileSize(const std::string& path) {
    std::ifstream f(path, std::ios::binary | std::ios::ate);
    if (!f) return 0;
    return static_cast<long long>(f.tellg());
}

static const int NB_FORMATS = 5;
static const char* NAMES[NB_FORMATS] = { "png", "jpeg", "bmp", "tga", "qoi" };
static const char* FILES[NB_FORMATS] = { "out.png", "out.jpg", "out.bmp", "out.tga", "out.qoi" };

static bool SaveAs(NkImage& img, int i) {
    switch (i) {
        case 0: return img.SavePNG(FILES[0]);
        case 1: return img.SaveJPEG(FILES[1]);
        case 2: return img.SaveBMP(FILES[2]);
        case 3: return img.SaveTGA(FILES[3]);
        case 4: return img.SaveQOI(FILES[4]);
    }
    return false;
}

int main() {
    NkImage photo;
    if (!photo.Load("photo.jpeg")) {            
        std::printf("Echec du chargement de photo.jpg\n");
        return 1;
    }
    const int W = photo.Width(), H = photo.Height();
    std::printf("photo : %d x %d\n", W, H);

    std::ofstream report("formats.txt");
    for (int i = 0; i < NB_FORMATS; ++i) {
        bool saved = SaveAs(photo, i);
        NkImage back;                             
        bool ok = saved && back.Load(FILES[i]);   
        long long size = FileSize(FILES[i]);

        if (ok) {
            report << NAMES[i] << " : " << size << " octets, "
                   << back.Width() << " x " << back.Height() << "\n";
            std::printf("%s : %lld octets, %d x %d %s\n", NAMES[i], size,
                        back.Width(), back.Height(),
                        (back.Width() == W && back.Height() == H) ? "(identique)" : "(DIMENSIONS DIFFERENTES)");
        } else {
            report << NAMES[i] << " : " << size << " octets, illisible\n";
            std::printf("%s : %lld octets, illisible\n", NAMES[i], size);
        }
    }
    return 0;
}