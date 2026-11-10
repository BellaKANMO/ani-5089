#include <iostream>
#include <string>
#include <vector>

int main() {
	long long images = 0;
	int n = 0;
	std::cin >> images >> n;

	std::vector<unsigned long long> valeurs(images > 0 ? (size_t)images : 0, 0ULL);
	std::vector<bool> posee(images > 0 ? (size_t)images : 0, false);
	int poses = 0;

	for (int i = 0; i < n; ++i) {
		std::string commande;
		long long index = 0;
		std::cin >> commande >> index;

		const bool dansAnneau = index >= 0 && index < images;

		if (commande == "POSE") {
			unsigned long long valeur = 0;
			std::cin >> valeur;
			if (!dansAnneau) {
				std::cout << "HORS ANNEAU\n";
			} else {
				if (!posee[(size_t)index]) {
					posee[(size_t)index] = true;
					++poses;
				}
				valeurs[(size_t)index] = valeur;
			}
		} else {
			if (dansAnneau && posee[(size_t)index]) {
				std::cout << valeurs[(size_t)index] << "\n";
			} else {
				std::cout << "VIDE\n";
			}
		}
	}

	std::cout << "POSEES " << poses << "\n";
	return 0;
}