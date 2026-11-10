#include <iostream>
#include <string>

struct Anneau {
    int suivant = 0;
    bool emprunte = false;
    int emprunt = -1;
    int rendus = 0;
};

int main() {
    int images = 0;
    int n = 0;
    std::cin >> images >> n;

    Anneau gauche;
    Anneau droit;
    int desync = 0;

    for (int tour = 1; tour <= n; ++tour) {
        std::string oeil;
        std::string commande;
        std::cin >> oeil >> commande;

        Anneau &a = (oeil == "G") ? gauche : droit;

        std::cout << tour << " " << oeil << " " << commande << " ";

        if (commande == "EMPRUNTE") {
            if (!a.emprunte) {
                a.emprunte = true;
                a.emprunt = a.suivant;
                std::cout << a.emprunt << "\n";
            } else {
                std::cout << "REFUSE\n";
            }
        } else {
            if (a.emprunte) {
                a.emprunte = false;
                std::cout << a.emprunt << "\n";
                a.suivant = (a.suivant + 1) % images;
                ++a.rendus;
            } else {
                std::cout << "REFUSE\n";
            }
        }

        int ecart = gauche.rendus - droit.rendus;
        if (ecart < 0) {
            ecart = -ecart;
        }
        if (ecart > 1 && desync == 0) {
            desync = tour;
        }
    }

    int decalage = gauche.rendus - droit.rendus;
    if (decalage < 0) {
        decalage = -decalage;
    }

    std::cout << "DECALAGE " << decalage << "\n";
    std::cout << "DESYNC " << desync << "\n";
    return 0;
}