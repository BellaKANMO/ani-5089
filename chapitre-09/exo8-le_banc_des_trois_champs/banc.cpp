#include "actions.h"

#include <cstdio>
#include <vector>

int main() {
    SystemeActions systeme;
    const int saut = systeme.Creer("saut", 'A');
    const int tir = systeme.Creer("tir", 'B');
    const int libre = systeme.Creer("libre", -1);

    std::printf("trame | saut c ch a | tir c ch a | libre c ch a\n");
    for (int trame = 1; trame <= 6; ++trame) {
        std::vector<int> touches;
        if (trame >= 2 && trame <= 4) touches.push_back('A');
        systeme.MettreAJour(touches);

        std::printf("%5d |", trame);
        const int ids[3] = { saut, tir, libre };
        for (int i = 0; i < 3; ++i) {
            const EtatAction &e = systeme.Obtenir(ids[i]).etat;
            std::printf("%s %d  %d  %d", i > 0 ? " |" : "", e.courant, e.change, e.actif);
        }
        std::printf("\n");
    }
    return 0;
}
