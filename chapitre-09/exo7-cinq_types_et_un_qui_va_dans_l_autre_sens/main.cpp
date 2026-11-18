#include <iostream>
#include <map>
#include <string>

int main() {
    int nombreActions = 0;
    std::cin >> nombreActions;

    std::map<std::string, bool> sortie;
    int entrees = 0;
    int sorties = 0;

    for (int i = 0; i < nombreActions; ++i) {
        std::string nom;
        std::string type;
        std::cin >> nom >> type;

        const bool estSortie = type == "HAPTIC";
        sortie[nom] = estSortie;
        if (estSortie) {
            ++sorties;
        } else {
            ++entrees;
        }
    }

    int nombreOperations = 0;
    std::cin >> nombreOperations;

    for (int i = 0; i < nombreOperations; ++i) {
        std::string nom;
        std::string operation;
        std::cin >> nom >> operation;

        const auto trouve = sortie.find(nom);
        if (trouve == sortie.end()) {
            continue;
        }

        if (operation == "LIT" && trouve->second) {
            std::cout << nom << " LIT UNE SORTIE\n";
        } else if (operation == "ECRIT" && !trouve->second) {
            std::cout << nom << " ECRIT UNE ENTREE\n";
        }
    }

    std::cout << "ENTREES " << entrees << "\n";
    std::cout << "SORTIES " << sorties << "\n";
    return 0;
}