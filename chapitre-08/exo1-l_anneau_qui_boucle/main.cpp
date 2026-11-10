#include <iostream>
#include <string>

int main() {
	int images = 0;
	int n = 0;
	std::cin >> images >> n;

	int suivant = 0;
	int emprunte = 0;
	int emprunt = -1;
	int prets = 0;

	for (int tour = 1; tour <= n; ++tour) {
		std::string commande;
		std::cin >> commande;

		if (commande == "EMPRUNTE") {
			if (emprunte == 0) {
				emprunte = 1;
				emprunt = suivant;
				++prets;
				std::cout << tour << " EMPRUNTE " << emprunt << "\n";
			} else {
				std::cout << tour << " EMPRUNTE REFUSE\n";
			}
		} else {
			if (emprunte == 1) {
				emprunte = 0;
				std::cout << tour << " REND " << emprunt << "\n";
				suivant = (suivant + 1) % images;
			} else {
				std::cout << tour << " REND REFUSE\n";
			}
		}
	}

	std::cout << "PRETS " << prets << "\n";
	return 0;
}