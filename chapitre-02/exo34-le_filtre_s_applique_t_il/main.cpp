#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int V;
    cin >> V;
    map<string,string> machine;
    for (int i = 0; i < V; i++) {
        string kv;
        cin >> kv;
        size_t pos = kv.find('=');
        string key = kv.substr(0,pos);
        string val = kv.substr(pos+1);
        machine[key] = val;
    }

    int F;
    cin >> F;
    cin.ignore();

    for (int i = 0; i < F; i++) {
        string line;
        getline(cin, line);
        if (line.empty()) { i--; continue; }

        vector<string> terms;
        stringstream ss(line);
        string token;
        while (ss >> token) {
            if (token == "&&") continue;
            terms.push_back(token);
        }

        bool applies = true;
        for (string term : terms) {
            bool neg = false;
            if (term[0] == '!') {
                neg = true;
                term = term.substr(1);
            }
            size_t pos = term.find('=');
            string key = term.substr(0,pos);
            string val = term.substr(pos+1);

            bool truth = false;
            if (machine.count(key) && machine[key] == val) {
                truth = true;
            }
            if (neg) truth = !truth;
            if (!truth) {
                applies = false;
                break;
            }
        }

        cout << (applies ? "OUI" : "NON") << "\n";
    }

    return 0;
}
