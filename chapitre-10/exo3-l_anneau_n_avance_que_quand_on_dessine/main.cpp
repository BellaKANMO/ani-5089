#include <iostream>

int main() {
    long long images = 0;
    long long trames = 0;
    if (!(std::cin >> images >> trames)) {
        std::cout << "RENDUES 0\n";
        std::cout << "FINS 0\n";
        return 0;
    }

    long long index = 0;
    long long rendues = 0;
    long long fins = 0;

    for (long long t = 1; t <= trames; ++t) {
        int dessine = 0;
        if (!(std::cin >> dessine)) {
            break;
        }

        if (dessine != 0) {
            std::cout << t << " OUI 1 " << index << "\n";
            ++rendues;
            if (images > 0) {
                index = (index + 1) % images;
            }
        } else {
            std::cout << t << " NON 0 -\n";
        }
        ++fins;
    }

    std::cout << "RENDUES " << rendues << "\n";
    std::cout << "FINS " << fins << "\n";
    return 0;
}
