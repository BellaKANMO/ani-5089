#ifndef CHAINE_H
#define CHAINE_H

class Chaine {
public:
    static constexpr int kMax = 8;

    static bool Creer(int images, Chaine &chaine) {
        if (images < 1 || images > kMax) {
            return false;
        }
        chaine.images_ = images;
        chaine.suivant_ = 0;
        chaine.emprunte_ = false;
        chaine.emprunt_ = -1;
        return true;
    }

    bool Emprunte(int &index) {
        if (images_ == 0 || emprunte_) {
            return false;
        }
        emprunte_ = true;
        emprunt_ = suivant_;
        index = emprunt_;
        return true;
    }

    bool Rend(int &index) {
        if (images_ == 0 || !emprunte_) {
            return false;
        }
        emprunte_ = false;
        index = emprunt_;
        suivant_ = (suivant_ + 1) % images_;
        return true;
    }

private:
    int images_ = 0;
    int suivant_ = 0;
    bool emprunte_ = false;
    int emprunt_ = -1;
};

#endif
