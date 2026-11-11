#include <iostream>

#include "chaine.h"

int main() {
    Chaine chaine;
    if (!Chaine::Creer(3, chaine)) {
        std::cout << "creation d'une chaine de 3 images : refusee\n";
        return 1;
    }
    std::cout << "chaine de 3 images creee\n";

    for (int tour = 1; tour <= 6; ++tour) {
        int index = -1;
        if (chaine.Emprunte(index)) {
            std::cout << "tour " << tour << " : emprunte " << index << "\n";
        } else {
            std::cout << "tour " << tour << " : emprunt refuse\n";
        }
        if (chaine.Rend(index)) {
            std::cout << "tour " << tour << " : rend " << index << "\n";
        } else {
            std::cout << "tour " << tour << " : rendu refuse\n";
        }
    }

    int index = -1;

    chaine.Emprunte(index);
    if (chaine.Emprunte(index)) {
        std::cout << "double emprunt : accepte\n";
    } else {
        std::cout << "double emprunt : refuse\n";
    }
    chaine.Rend(index);

    if (chaine.Rend(index)) {
        std::cout << "rendu sans emprunt : accepte\n";
    } else {
        std::cout << "rendu sans emprunt : refuse\n";
    }

    Chaine trop;
    if (Chaine::Creer(Chaine::kMax + 1, trop)) {
        std::cout << "chaine de " << Chaine::kMax + 1 << " images : acceptee\n";
    } else {
        std::cout << "chaine de " << Chaine::kMax + 1 << " images : refusee, le maximum est "
                  << Chaine::kMax << "\n";
    }

    return 0;
}