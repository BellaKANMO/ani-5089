#ifndef COUCHES_H
#define COUCHES_H

#include <vector>

enum class Espace { Local, Stage };

struct Vue {
    float yawDegres = 0.f;
    float decalageX = 0.f;
};

struct CoucheProjection {
    Espace espace = Espace::Stage;
    Vue gauche;
    Vue droite;
};

class Compositeur {
public:
    explicit Compositeur(int images) : images_(images) {}

    void DebutDeTrame() {
        if (ouverte_) {
            ++debutsSurTrameOuverte_;
        }
        ouverte_ = true;
        couches_.clear();
    }

    void Soumettre(const CoucheProjection &couche) {
        couches_.push_back(couche);
    }

    int Dessiner() {
        if (images_ < 1 || couches_.empty()) {
            return -1;
        }
        const int index = suivant_;
        suivant_ = (suivant_ + 1) % images_;
        return index;
    }

    void FinDeTrame() {
        couches_.clear();
        ouverte_ = false;
        ++fins_;
    }

    int Suivant() const { return suivant_; }
    int CouchesSoumises() const { return (int)couches_.size(); }
    int Fins() const { return fins_; }
    int DebutsSurTrameOuverte() const { return debutsSurTrameOuverte_; }

private:
    int images_;
    int suivant_ = 0;
    int fins_ = 0;
    int debutsSurTrameOuverte_ = 0;
    bool ouverte_ = false;
    std::vector<CoucheProjection> couches_;
};

#endif
