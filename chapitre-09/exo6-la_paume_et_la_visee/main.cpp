#include <iostream>

int main() {
    long long seuil = 0;
    long long maximum = 0;
    int n = 0;
    std::cin >> seuil >> maximum >> n;

    const long long course = 1000 - seuil;
    int morts = 0;

    for (int tour = 0; tour < n; ++tour) {
        long long valeur = 0;
        std::cin >> valeur;

        const long long absolue = valeur < 0 ? -valeur : valeur;

        long long vitesse = 0;
        if (absolue < seuil) {
            ++morts;
        } else if (course > 0) {
            const long long reste = absolue - seuil;
            vitesse = maximum * reste * reste / (course * course);
            if (valeur < 0) {
                vitesse = -vitesse;
            }
        }

        std::cout << vitesse << "\n";
    }

    std::cout << "MORTS " << morts << "\n";
    return 0;
}
