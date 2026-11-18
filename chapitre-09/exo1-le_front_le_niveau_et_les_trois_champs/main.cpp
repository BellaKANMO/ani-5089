#include <iostream>

int main() {
    int n = 0;
    std::cin >> n;

    int precedent = 0;
    int fronts = 0;
    int niveaux = 0;

    for (int image = 1; image <= n; ++image) {
        int presse = 0;
        int lie = 0;
        std::cin >> presse >> lie;

        const int actif = lie;
        const int courant = actif ? presse : 0;
        const int change = courant != precedent ? 1 : 0;

        if (courant == 1 && change == 1) {
            ++fronts;
        }
        if (courant == 1) {
            ++niveaux;
        }

        std::cout << image << " " << courant << " " << change << " " << actif << "\n";
        precedent = courant;
    }

    std::cout << "FRONTS " << fronts << "\n";
    std::cout << "NIVEAUX " << niveaux << "\n";
    return 0;
}