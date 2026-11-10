#include <iostream>
#include <sstream>
#include <string>

int main() {
    int n = 0;
    std::cin >> n;
    std::string ligne;
    std::getline(std::cin, ligne);

    int survivants = 0;

    for (int i = 0; i < n; ++i) {
        std::getline(std::cin, ligne);
        std::istringstream mots(ligne);

        std::string nom;
        mots >> nom;

        int position = 0;
        int detruit = -1;
        int recree = -1;
        bool reenregistre = false;

        std::string mot;
        while (mots >> mot) {
            ++position;
            if (mot == "DETRUIT" && detruit < 0) {
                detruit = position;
            } else if (mot == "RECREE" && recree < 0) {
                recree = position;
            } else if (mot == "REENREGISTRE") {
                reenregistre = true;
            }
        }

        const char *verdict = "SURVIT";
        if (detruit >= 0 && recree >= 0 && recree < detruit) {
            verdict = "MAUVAIS ORDRE";
        } else if (detruit < 0 || recree < 0) {
            verdict = "CHAINE PERIMEE";
        } else if (!reenregistre) {
            verdict = "POIGNEES PERDUES";
        } else {
            ++survivants;
        }

        std::cout << nom << " " << verdict << "\n";
    }

    std::cout << "SURVIVANTS " << survivants << "\n";
    return 0;
}