#include <iostream>
#include <string>

int main() {
	int n = 0;
	std::cin >> n;

	bool acquise = false;
	bool enAttente = false;
	int fautes = 0;

	for (int tour = 1; tour <= n; ++tour) {
		std::string appel;
		std::cin >> appel;

		const char *raison = nullptr;

		if (appel == "ACQUIERT") {
			if (acquise) {
				raison = "DEJA ACQUISE";
			} else {
				acquise = true;
			}
		} else if (appel == "DESSINE") {
			if (!acquise) {
				raison = "PAS ACQUISE";
			}
		} else if (appel == "REND") {
			if (!acquise) {
				raison = "RIEN ACQUIS";
			} else {
				acquise = false;
				enAttente = true;
			}
		} else if (appel == "SOUMET") {
			if (!enAttente) {
				raison = "PAS RENDUE";
			} else {
				enAttente = false;
			}
		}

		std::cout << tour << " " << appel << " ";
		if (raison == nullptr) {
			std::cout << "OK\n";
		} else {
			std::cout << raison << "\n";
			++fautes;
		}
	}

	std::cout << "FAUTES " << fautes << "\n";
	return 0;
}