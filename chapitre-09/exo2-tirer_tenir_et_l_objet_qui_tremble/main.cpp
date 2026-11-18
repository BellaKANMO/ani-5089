#include <iostream>

int main() {
    int n = 0;
    std::cin >> n;

    int precedent = 0;
    int fronts = 0;
    int niveaux = 0;

    for (int image = 0; image < n; ++image) {
        int presse = 0;
        std::cin >> presse;

        if (presse == 1 && precedent == 0) {
            ++fronts;
        }
        if (presse == 1) {
            ++niveaux;
        }
        precedent = presse;
    }

    std::cout << "BALLES FRONT " << fronts << "\n";
    std::cout << "BALLES NIVEAU " << niveaux << "\n";
    std::cout << "TENU NIVEAU " << niveaux << "\n";
    std::cout << "TENU FRONT " << fronts << "\n";
    return 0;
}
