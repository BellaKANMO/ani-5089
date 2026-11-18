#include <iostream>
#include <set>
#include <string>
#include <vector>

int main() {
    int nombreActions = 0;
    std::cin >> nombreActions;

    std::vector<std::string> noms;
    std::vector<std::string> usages;
    for (int i = 0; i < nombreActions; ++i) {
        std::string nom;
        std::string usage;
        std::cin >> nom >> usage;
        noms.push_back(nom);
        usages.push_back(usage);
    }

    int nombreFournis = 0;
    std::cin >> nombreFournis;

    std::set<std::string> fournis;
    for (int i = 0; i < nombreFournis; ++i) {
        std::string usage;
        std::cin >> usage;
        fournis.insert(usage);
    }

    int mortes = 0;
    for (size_t i = 0; i < noms.size(); ++i) {
        const bool active = fournis.count(usages[i]) > 0;
        if (!active) {
            ++mortes;
        }
        std::cout << noms[i] << " " << (active ? "ACTIVE" : "MORTE") << "\n";
    }

    std::cout << "MORTES " << mortes << "\n";
    return 0;
}