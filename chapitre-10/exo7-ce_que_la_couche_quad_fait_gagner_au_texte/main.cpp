#include <iostream>
#include <string>

int main() {
    long long rendu = 0, deformation = 0, reprojection = 0, seuil = 0;
    int n = 0;
    if (!(std::cin >> rendu >> deformation >> reprojection >> seuil >> n)) {
        std::cout << "GAGNES 0\n";
        std::cout << "PERDUS 0\n";
        return 0;
    }

    int gagnes = 0;
    int perdus = 0;

    for (int i = 0; i < n; ++i) {
        std::string nom;
        long long pixels = 0;
        if (!(std::cin >> nom >> pixels)) {
            break;
        }

        long long scene = pixels;
        scene = scene * rendu / 1000;
        scene = scene * deformation / 1000;
        scene = scene * reprojection / 1000;

        const bool lisibleScene = scene >= seuil;
        const bool lisibleQuad = pixels >= seuil;

        if (!lisibleScene && lisibleQuad) {
            ++gagnes;
        }
        if (lisibleScene && !lisibleQuad) {
            ++perdus;
        }

        std::cout << nom << " " << scene << " " << pixels << " "
                  << (lisibleScene ? "LISIBLE" : "FLOU") << " "
                  << (lisibleQuad ? "LISIBLE" : "FLOU") << "\n";
    }

    std::cout << "GAGNES " << gagnes << "\n";
    std::cout << "PERDUS " << perdus << "\n";
    return 0;
}
