#include <iostream>
#include <map>
#include <sstream>
#include <string>

int main() {
    const std::map<std::string, int> mains = {
        {"SELECT", 0},
        {"GRAB", 0},
        {"AIM_POSE", 0},
        {"GRIP_POSE", 0},
        {"HAPTIC", 0},
        {"MOVE_RIGHT", 0},
        {"SELECT_LEFT", 1},
        {"GRAB_LEFT", 1},
        {"AIM_POSE_LEFT", 1},
        {"GRIP_POSE_LEFT", 1},
        {"HAPTIC_LEFT", 1},
        {"MENU", 1},
        {"MOVE", 1},
    };

    int n = 0;
    std::cin >> n;
    std::string ligne;
    std::getline(std::cin, ligne);

    int deuxMains = 0;

    for (int i = 0; i < n; ++i) {
        std::getline(std::cin, ligne);
        std::istringstream mots(ligne);

        std::string nom;
        mots >> nom;

        int droite = 0;
        int gauche = 0;
        std::string usage;
        while (mots >> usage) {
            const auto trouve = mains.find(usage);
            if (trouve == mains.end()) {
                continue;
            }
            if (trouve->second == 0) {
                ++droite;
            } else {
                ++gauche;
            }
        }

        const bool deux = droite > 0 && gauche > 0;
        if (deux) {
            ++deuxMains;
        }

        std::cout << nom << " " << droite << " " << gauche << " "
                  << (deux ? "DEUX MAINS" : "UNE MAIN") << "\n";
    }

    std::cout << "DEUX MAINS " << deuxMains << "\n";
    return 0;
}
