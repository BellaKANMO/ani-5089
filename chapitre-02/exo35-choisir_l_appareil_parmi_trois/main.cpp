#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
using namespace std;

struct Device {
    string serie;
    string etat;
    string modele;
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int D;
    cin >> D;
    vector<Device> devices(D);
    for (int i = 0; i < D; i++) {
        cin >> devices[i].serie >> devices[i].etat >> devices[i].modele;
    }

    string target;
    cin >> target;

    if (target != "-") {
        
        bool found = false;
        for (auto &dev : devices) {
            if (dev.serie == target) {
                found = true;
                if (dev.etat != "device") {
                    cout << "ERREUR " << dev.serie << " est " << dev.etat << "\n";
                } else {
                    cout << dev.serie << "\n";
                }
                break;
            }
        }
        if (!found) {
            cout << "ERREUR cible introuvable\n";
        }
    } else {

        vector<string> active;
        for (auto &dev : devices) {
            if (dev.etat == "device") {
                active.push_back(dev.serie);
            }
        }
        if (active.empty()) {
            cout << "ERREUR aucun appareil\n";
        } else if (active.size() == 1) {
            cout << active[0] << "\n";
        } else {
            sort(active.begin(), active.end());
            cout << "ERREUR plusieurs appareils\n";
            for (auto &s : active) {
                cout << s << "\n";
            }
        }
    }

    return 0;
}
