#include "NKWindow/NKMain.h"
#include "NKWindow/Core/NkWindow.h"
#include "NKWindow/Core/NkWindowConfig.h"
#include "NKEvent/NkEventSystem.h"
#include "NKEvent/NkWindowEvent.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <thread>

using namespace nkentseu;

static void ConfigureAppData(NkAppData &d) { d.appName = "DeuxChaines"; }
NK_REGISTER_ENTRY_APPDATA_UPDATER(ConfigureAppData)

namespace {

    constexpr int kImages = 3;
    constexpr int kMax = 8;

    class Chaine {
    public:
        bool Creer(int images) {
            if (images < 1 || images > kMax) {
                return false;
            }
            Vider();
            images_ = images;
            ++generation_;
            return true;
        }

        void Detruire() {
            Vider();
            images_ = 0;
        }

        bool Enregistre(int index, std::uint64_t poignee) {
            if (index < 0 || index >= images_) {
                return false;
            }
            poignees_[index] = poignee;
            enregistree_[index] = true;
            return true;
        }

        bool Acquiert(int &index) {
            if (images_ == 0 || acquise_ || !enregistree_[suivant_]) {
                return false;
            }
            acquise_ = true;
            index = suivant_;
            return true;
        }

        bool Dessine() const {
            return images_ > 0 && acquise_;
        }

        bool Rend() {
            if (images_ == 0 || !acquise_) {
                return false;
            }
            acquise_ = false;
            enAttente_ = true;
            suivant_ = (suivant_ + 1) % images_;
            return true;
        }

        bool Soumet() {
            if (!enAttente_) {
                return false;
            }
            enAttente_ = false;
            return true;
        }

        int Images() const { return images_; }
        int Generation() const { return generation_; }
        std::uint64_t Poignee(int index) const { return poignees_[index]; }

    private:
        void Vider() {
            suivant_ = 0;
            acquise_ = false;
            enAttente_ = false;
            for (int i = 0; i < kMax; ++i) {
                enregistree_[i] = false;
                poignees_[i] = 0;
            }
        }

        int images_ = 0;
        int suivant_ = 0;
        int generation_ = 0;
        bool acquise_ = false;
        bool enAttente_ = false;
        bool enregistree_[kMax] = {};
        std::uint64_t poignees_[kMax] = {};
    };

    std::uint64_t FabriquePoignee(int generation, int oeil, int index) {
        return ((std::uint64_t)generation << 16) | ((std::uint64_t)oeil << 8) | (std::uint64_t)index;
    }

    void Enregistrer(Chaine &chaine, int oeil) {
        for (int i = 0; i < chaine.Images(); ++i) {
            chaine.Enregistre(i, FabriquePoignee(chaine.Generation(), oeil, i));
        }
    }

    bool UneImage(Chaine &chaine, int &index) {
        return chaine.Acquiert(index) && chaine.Dessine() && chaine.Rend() && chaine.Soumet();
    }

    void Operation(std::FILE *f, const char *texte) {
        std::fprintf(f, "%s\n", texte);
        std::printf("%s\n", texte);
    }

    void Redimensionner(Chaine &gauche, Chaine &droite, std::FILE *f) {
        gauche.Detruire();
        Operation(f, "DETRUIT chaine gauche");
        droite.Detruire();
        Operation(f, "DETRUIT chaine droite");
        gauche.Creer(kImages);
        Operation(f, "RECREE chaine gauche");
        droite.Creer(kImages);
        Operation(f, "RECREE chaine droite");
        Enregistrer(gauche, 0);
        Operation(f, "REENREGISTRE images chaine gauche");
        Enregistrer(droite, 1);
        Operation(f, "REENREGISTRE images chaine droite");
    }

}

int nkmain(const NkEntryState &state) {
    (void)state;

    NkWindowConfig cfg;
    cfg.title = "Deux chaines";
    cfg.width = 1280;
    cfg.height = 720;
    cfg.centered = true;

    NkWindow fenetre(cfg);
    if (!fenetre.IsValid()) {
        std::printf("[deux_chaines] la fenetre n'a pas pu s'ouvrir\n");
        return 1;
    }

    Chaine gauche;
    Chaine droite;
    gauche.Creer(kImages);
    droite.Creer(kImages);
    Enregistrer(gauche, 0);
    Enregistrer(droite, 1);

    std::FILE *trace = std::fopen("trace.txt", "w");

    unsigned connuL = (unsigned)fenetre.GetSize().x;
    unsigned connuH = (unsigned)fenetre.GetSize().y;
    unsigned vuL = connuL;
    unsigned vuH = connuH;
    int stable = 0;

    int image = 0;
    int apres = 0;
    bool tourne = true;
    bool redimensionne = false;

    while (tourne && fenetre.IsOpen()) {
        NkEvent *ev = nullptr;
        while (NkEvents().PollEvent(ev)) {
            if (ev->As<NkWindowCloseEvent>() != nullptr) {
                tourne = false;
            }
        }
        if (!tourne) {
            break;
        }

        ++image;
        int indexGauche = -1;
        int indexDroit = -1;
        const bool okGauche = UneImage(gauche, indexGauche);
        const bool okDroit = UneImage(droite, indexDroit);
        if (!okGauche || !okDroit) {
            std::fprintf(stderr, "[deux_chaines] image %d : une etape refusee (gauche %d, droit %d)\n",
                         image, okGauche ? 1 : 0, okDroit ? 1 : 0);
        }

        std::printf("%simage : %d, gauche : %d, droit : %d\n",
                    redimensionne ? "apres redimensionnement, " : "", image, indexGauche, indexDroit);

        if (trace != nullptr && image <= 30) {
            std::fprintf(trace, "image : %d, gauche : %d, droit : %d\n", image, indexGauche, indexDroit);
            if (image == 30) {
                std::fclose(trace);
                trace = nullptr;
                std::printf("[deux_chaines] trente images tracees : redimensionne la fenetre maintenant\n");
            }
        }

        const unsigned l = (unsigned)fenetre.GetSize().x;
        const unsigned h = (unsigned)fenetre.GetSize().y;
        if (l != vuL || h != vuH) {
            vuL = l;
            vuH = h;
            stable = 0;
        } else {
            ++stable;
        }

        if (!redimensionne && l > 0 && h > 0 && stable >= 5 && (l != connuL || h != connuH)) {
            std::printf("[deux_chaines] fenetre %ux%u -> %ux%u\n", connuL, connuH, l, h);
            std::FILE *resize = std::fopen("resize.txt", "w");
            if (resize != nullptr) {
                Redimensionner(gauche, droite, resize);
                std::fclose(resize);
            }
            connuL = l;
            connuH = h;
            redimensionne = true;
        }

        if (redimensionne && ++apres >= 30) {
            tourne = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(image <= 30 ? 30 : 200));
    }

    if (trace != nullptr) {
        std::fclose(trace);
    }
    if (!redimensionne) {
        std::printf("[deux_chaines] aucun redimensionnement detecte : resize.txt non ecrit\n");
    }
    return 0;
}
