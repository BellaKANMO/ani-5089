#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::string ligne;
    int n = 0;
    if (!std::getline(std::cin, ligne)) {
        std::cout << "FAUTES 0\n";
        return 0;
    }
    n = std::atoi(ligne.c_str());

    int fautes = 0;
    int lus = 0;
    while (lus < n && std::getline(std::cin, ligne)) {
        if (!ligne.empty() && ligne.back() == '\r') {
            ligne.pop_back();
        }
        std::istringstream flux(ligne);
        std::vector<std::string> mots;
        std::string mot;
        while (flux >> mot) {
            mots.push_back(mot);
        }
        if (mots.size() < 3) {
            continue;
        }
        ++lus;

        const std::string &pose = mots[mots.size() - 1];
        const std::string &intention = mots[mots.size() - 2];
        std::string nom;
        for (std::size_t i = 0; i + 2 < mots.size(); ++i) {
            if (i > 0) {
                nom += " ";
            }
            nom += mots[i];
        }

        std::string resultat = "OK";
        if (intention == "TENIR" && pose == "AIM") {
            resultat = "FLOTTE";
            ++fautes;
        } else if (intention == "POINTER" && pose == "GRIP") {
            resultat = "PART DE TRAVERS";
            ++fautes;
        }
        std::cout << nom << " " << resultat << "\n";
    }
    std::cout << "FAUTES " << fautes << "\n";
    return 0;
}
