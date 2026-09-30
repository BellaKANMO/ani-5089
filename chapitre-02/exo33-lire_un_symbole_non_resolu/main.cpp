#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <set>
#include <algorithm>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int P;
    cin >> P;
    map<string,string> prefixToModule;
    for (int i = 0; i < P; i++) {
        string prefix, module;
        cin >> prefix >> module;
        prefixToModule[prefix] = module;
    }

    int L;
    cin >> L;
    cin.ignore();

    set<string> modules;
    int unknownCount = 0;

    for (int i = 0; i < L; i++) {
        string line;
        getline(cin, line);
        size_t pos = line.find("undefined reference to '");
        if (pos == string::npos) continue;

        size_t start = line.find("'", pos);
        size_t end = line.find("'", start+1);
        if (start == string::npos || end == string::npos) continue;
        string symbol = line.substr(start+1, end-start-1);

        string bestModule;
        size_t bestLen = 0;
        for (auto &p : prefixToModule) {
            const string &pref = p.first;
            if (symbol.rfind(pref, 0) == 0) {
                if (pref.size() > bestLen) {
                    bestLen = pref.size();
                    bestModule = p.second;
                }
            }
        }

        if (!bestModule.empty()) {
            modules.insert(bestModule);
        } else {
            unknownCount++;
        }
    }

    // Affichage trié
    vector<string> result(modules.begin(), modules.end());
    sort(result.begin(), result.end());
    for (const string &m : result) {
        cout << m << "\n";
    }
    if (unknownCount > 0) {
        cout << "INCONNU " << unknownCount << "\n";
    }

    return 0;
}
