#include <iostream>
#include <string>

enum Etat { REPOS, ATTENDUE, COMMENCEE };

int main() {
    int n = 0;
    if (!(std::cin >> n)) {
        std::cout << "FAUTES 0\n";
        std::cout << "BLOQUE 0\n";
        return 0;
    }

    Etat etat = REPOS;
    int fautes = 0;
    int bloque = 0;

    for (int i = 1; i <= n; ++i) {
        std::string geste;
        if (!(std::cin >> geste)) {
            break;
        }

        std::string verdict = "OK";
        if (geste == "ATTEND") {
            if (etat == REPOS) {
                etat = ATTENDUE;
            } else if (etat == ATTENDUE) {
                verdict = "DEJA ATTENDUE";
            } else {
                verdict = "TRAME EN COURS";
                if (bloque == 0) {
                    bloque = i;
                }
            }
        } else if (geste == "COMMENCE") {
            if (etat == ATTENDUE) {
                etat = COMMENCEE;
            } else if (etat == REPOS) {
                verdict = "PAS ATTENDUE";
            } else {
                verdict = "DEJA COMMENCEE";
            }
        } else if (geste == "FINIT") {
            if (etat == COMMENCEE) {
                etat = REPOS;
            } else {
                verdict = "PAS COMMENCEE";
            }
        }

        if (verdict != "OK") {
            ++fautes;
        }
        std::cout << i << " " << geste << " " << verdict << "\n";
    }

    std::cout << "FAUTES " << fautes << "\n";
    std::cout << "BLOQUE " << bloque << "\n";
    return 0;
}
