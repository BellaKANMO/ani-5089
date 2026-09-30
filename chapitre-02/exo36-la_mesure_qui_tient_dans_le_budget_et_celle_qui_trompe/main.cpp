#include <iostream>
#include <string>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    long long budget;
    cin >> budget;

    int S;
    cin >> S;

    int trompeCount = 0;

    for (int i = 0; i < S; i++) {
        string nom;
        long long debug, release;
        cin >> nom >> debug >> release;

        long long facteur = (debug + release / 2) / release;

        bool tient = (release <= budget);

        cout << nom << " " << facteur << " " << (tient ? "TIENT" : "DEPASSE") << "\n";

        if (debug > budget && release <= budget) {
            trompeCount++;
        }
    }

    cout << "TROMPE " << trompeCount << "\n";

    return 0;
}
