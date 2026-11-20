#include <iostream>
#include <string>

int main() {
    const std::string symptomes[6] = {
        "BLOQUE_AU_WAIT",
        "PAS_DE_REPRISE",
        "OEIL_NOIR",
        "TREMBLEMENT",
        "CORRECTION_DE_TRAVERS",
        "SCENE_DECALEE"
    };
    const std::string causes[6] = {
        "FIN DE TRAME",
        "FIN SANS RENDU",
        "CHAINE MANQUANTE",
        "MAUVAIS INSTANT",
        "POSE FRAICHE",
        "MAUVAIS ESPACE"
    };

    int n = 0;
    if (!(std::cin >> n)) {
        std::cout << "DIAGNOSTIQUES 0\n";
        std::cout << "INCONNUS 0\n";
        return 0;
    }

    int diagnostiques = 0;
    int inconnus = 0;

    for (int i = 0; i < n; ++i) {
        std::string symptome;
        if (!(std::cin >> symptome)) {
            break;
        }

        std::string cause = "INCONNU";
        for (int k = 0; k < 6; ++k) {
            if (symptome == symptomes[k]) {
                cause = causes[k];
                break;
            }
        }

        if (cause == "INCONNU") {
            ++inconnus;
        } else {
            ++diagnostiques;
        }
        std::cout << symptome << " " << cause << "\n";
    }

    std::cout << "DIAGNOSTIQUES " << diagnostiques << "\n";
    std::cout << "INCONNUS " << inconnus << "\n";
    return 0;
}
