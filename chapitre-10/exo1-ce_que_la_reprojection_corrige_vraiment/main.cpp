#include <cstdlib>
#include <iostream>

int main() {
    long long n = 0;
    if (!(std::cin >> n)) {
        std::cout << "JUSTES 0\n";
        std::cout << "PIRES 0\n";
        return 0;
    }

    long long justes = 0;
    long long pires = 0;
    for (long long i = 0; i < n; ++i) {
        long long rendu = 0;
        long long reel = 0;
        long long soumis = 0;
        if (!(std::cin >> rendu >> reel >> soumis)) {
            break;
        }

        const long long sans = rendu - reel;
        const long long avec = rendu - soumis;

        const char *verdict = "PIRE";
        if (avec == 0) {
            verdict = "JUSTE";
            ++justes;
        } else if (std::llabs(avec) < std::llabs(sans)) {
            verdict = "PARTIEL";
        } else if (std::llabs(avec) == std::llabs(sans)) {
            verdict = "SANS EFFET";
        } else {
            ++pires;
        }

        std::cout << "SANS " << sans << " AVEC " << avec << " " << verdict << "\n";
    }

    std::cout << "JUSTES " << justes << "\n";
    std::cout << "PIRES " << pires << "\n";
    return 0;
}
