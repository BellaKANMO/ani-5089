#include <iostream>
#include <string>
using namespace std;

bool endsWith(const string &s, const string &suffix) {
    if (s.size() < suffix.size()) return false;
    return s.compare(s.size() - suffix.size(), suffix.size(), suffix) == 0;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string arch;
    cin >> arch;

    int F;
    cin >> F;

    long long total = 0;
    bool signe = false;
    bool abi_ok = false;
    int inutile = 0;

    for (int i = 0; i < F; i++) {
        string path;
        long long size;
        cin >> path >> size;
        total += size;

        if (path.rfind("META-INF/", 0) == 0) {
            if (endsWith(path, ".RSA") || endsWith(path, ".DSA") || endsWith(path, ".EC")) {
                signe = true;
            }
        }

        if (path.rfind("lib/", 0) == 0) {
            string prefix = "lib/" + arch + "/";
            if (path.rfind(prefix, 0) == 0) {
                abi_ok = true;
            } else {
                inutile++;
            }
        }
    }

    cout << total << "\n";
    cout << (signe ? "SIGNE" : "NON SIGNE") << "\n";
    cout << (abi_ok ? "ABI OUI" : "ABI NON") << "\n";
    cout << "INUTILE " << inutile << "\n";

    return 0;
}
