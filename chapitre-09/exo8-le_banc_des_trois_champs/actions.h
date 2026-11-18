#pragma once
#include <string>
#include <vector>

struct EtatAction {
    bool courant = false;
    bool change = false;
    bool actif = false;
};

struct Action {
    std::string nom;
    int touche = -1;
    EtatAction etat;
};

class SystemeActions {
public:
    int Creer(const std::string &nom, int touche) {
        Action a;
        a.nom = nom;
        a.touche = touche;
        actions.push_back(a);
        return (int)actions.size() - 1;
    }

    void MettreAJour(const std::vector<int> &touchesEnfoncees) {
        for (Action &a : actions) {
            const bool avant = a.etat.courant;
            a.etat.actif = a.touche >= 0;
            bool enfoncee = false;
            for (int t : touchesEnfoncees) {
                if (t == a.touche) enfoncee = true;
            }
            a.etat.courant = a.etat.actif && enfoncee;
            a.etat.change = a.etat.courant != avant;
        }
    }

    const Action &Obtenir(int index) const { return actions[index]; }
    int Nombre() const { return (int)actions.size(); }

private:
    std::vector<Action> actions;
};
