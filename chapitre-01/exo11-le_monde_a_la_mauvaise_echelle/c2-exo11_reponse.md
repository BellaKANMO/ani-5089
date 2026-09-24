#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct Boite {
    std::string nom;
    double largeur;   
    double hauteur;   
    double profondeur; 
};

static void afficher(const Boite& b, double facteur) {
    std::cout << std::left << std::setw(16) << b.nom << std::right << std::fixed
              << std::setprecision(2)
              << std::setw(8) << b.largeur * facteur << " x "
              << std::setw(6) << b.hauteur * facteur << " x "
              << std::setw(6) << b.profondeur * facteur << " m\n";
}

int main() {
    double facteur = 1.0;
    std::cout << "Facteur d'echelle : ";
    if (!(std::cin >> facteur) || facteur <= 0.0) {
        std::cerr << "Facteur invalide (nombre strictement positif attendu).\n";
        return 1;
    }

    const Boite salle{"Salle", 5.00, 2.50, 4.00};
    const std::vector<Boite> mobilier = {
        {"Table",    1.20, 0.75, 0.80},
        {"Chaise",   0.45, 0.90, 0.45},
        {"Armoire",  1.00, 2.00, 0.50},
        {"Canape",   2.00, 0.85, 0.90},
        {"Porte",    0.90, 2.05, 0.05},
    };

    std::cout << "\nlargeur x hauteur x profondeur\n\n";
    afficher(salle, facteur);
    for (const auto& m : mobilier) afficher(m, facteur);
    return 0;
}


Personne 1 (facteur 0.5):
    La pièce est minuscule, on dirait un dressing ou une chambre d'enfant. Le plafond est si bas qu'on ne pourrait pas s'y tenir debout, et il faudrait se baisser pour franchir la porte. La table ressemble à une table basse et la chaise semble faite pour un enfant. L'armoire est très peu profonde. On dirait une maison de poupée, ou une pièce pour de très jeunes enfants.

Personne 2 (facteur 1.0):
    C'est un salon ordinaire. La hauteur sous plafond est standard, la table et la porte ont des tailles normales, et le canapé est une confortable deux ou trois places. Rien ne me frappe. C'est un peu grand pour un studio, mais tout à fait correct pour un appartement familial.

Persoone 3 (facteur 1.6):
    La pièce est immense, plutôt un hall ou un loft. Le plafond est plus haut que dans un logement normal, comme dans une église ou une galerie, et la porte serait bizarre dans une maison. La table est si haute qu'il faudrait rester debout, et la chaise fait penser à un tabouret de bar ou à un siège de maître-nageur. L'armoire est si haute qu'il faudrait une échelle. On dirait la chambre d'un géant, ou une salle de réception avec des meubles surdimensionnés.