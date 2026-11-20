#include <cstdio>

#include "couches.h"

namespace {

    constexpr int kTrames = 6;
    constexpr int kImages = 3;

    const bool kRend[kTrames] = {true, true, false, false, true, true};
    const int kIndexAttendu[kTrames] = {0, 1, -1, -1, 2, 0};
    const int kCouchesAttendues[kTrames] = {1, 1, 0, 0, 1, 1};
    const int kSuivantAttendu[kTrames] = {1, 2, 2, 2, 0, 1};

    CoucheProjection FabriqueCouche(int trame) {
        CoucheProjection couche;
        couche.espace = Espace::Stage;
        couche.gauche.yawDegres = (float)trame;
        couche.gauche.decalageX = -0.032f;
        couche.droite.yawDegres = (float)trame;
        couche.droite.decalageX = 0.032f;
        return couche;
    }

}

int main() {
    Compositeur compositeur(kImages);
    int ecarts = 0;

    std::printf("image | on rend | couches soumises | index d'image\n");
    for (int t = 0; t < kTrames; ++t) {
        compositeur.DebutDeTrame();

        const CoucheProjection couche = FabriqueCouche(t + 1);
        int index = -1;
        if (kRend[t]) {
            compositeur.Soumettre(couche);
            index = compositeur.Dessiner();
        }

        const int soumises = compositeur.CouchesSoumises();
        const int suivant = compositeur.Suivant();
        compositeur.FinDeTrame();

        char colonne[16];
        if (index < 0) {
            std::snprintf(colonne, sizeof(colonne), "-");
        } else {
            std::snprintf(colonne, sizeof(colonne), "%d", index);
        }
        std::printf("%5d | %7d | %16d | %13s\n", t + 1, kRend[t] ? 1 : 0, soumises, colonne);

        if (index != kIndexAttendu[t] || soumises != kCouchesAttendues[t]
            || suivant != kSuivantAttendu[t]) {
            ++ecarts;
        }
    }

    const bool finsOk = compositeur.Fins() == kTrames && compositeur.DebutsSurTrameOuverte() == 0;

    Compositeur sansCouche(kImages);
    sansCouche.DebutDeTrame();
    const int avant = sansCouche.Suivant();
    const int refus = sansCouche.Dessiner();
    const bool refuseOk = refus == -1 && sansCouche.Suivant() == avant;
    sansCouche.FinDeTrame();

    int ecartsAnneau = 0;
    int suivantFautif = 0;
    for (int t = 0; t < kTrames; ++t) {
        if (kRend[t] && suivantFautif % kImages != kIndexAttendu[t]) {
            ++ecartsAnneau;
        }
        ++suivantFautif;
    }

    Compositeur finDansLeIf(kImages);
    for (int t = 0; t < kTrames; ++t) {
        finDansLeIf.DebutDeTrame();
        if (kRend[t]) {
            finDansLeIf.Soumettre(FabriqueCouche(t + 1));
            finDansLeIf.Dessiner();
            finDansLeIf.FinDeTrame();
        }
    }

    std::printf("verification : %s (%d ecart)\n", ecarts == 0 ? "OK" : "ECHEC", ecarts);
    std::printf("fins de trame : %d sur %d, %d debut sur une trame ouverte\n", compositeur.Fins(),
                kTrames, compositeur.DebutsSurTrameOuverte());
    std::printf("dessin sans couche : %s, anneau inchange : %s\n", refus == -1 ? "refuse" : "ACCEPTE",
                refuseOk ? "oui" : "NON");
    std::printf("version fautive, l'anneau avance a chaque trame : %d ecarts detectes\n", ecartsAnneau);
    std::printf("version fautive, fin de trame dans le if : %d fins sur %d, %d debuts sur une trame ouverte\n",
                finDansLeIf.Fins(), kTrames, finDansLeIf.DebutsSurTrameOuverte());

    const bool fautesDetectees = ecartsAnneau > 0 && finDansLeIf.DebutsSurTrameOuverte() > 0;
    return ecarts == 0 && finsOk && refuseOk && fautesDetectees ? 0 : 1;
}
