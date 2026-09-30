#include <iostream>
#include <sstream>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <algorithm>

using namespace std;

map<string, vector<string>> deps;
set<string> needed;

void dfs(const string &module) {
    if (needed.count(module)) return;
    needed.insert(module);
    if (deps.count(module)) {
        for (const string &d : deps[module]) {
            dfs(d);
        }
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N;
    cin >> N;
    cin.ignore();

    for (int i = 0; i < N; i++) {
        string line;
        getline(cin, line);
        if (line.empty()) { i--; continue; }
        stringstream ss(line);
        string module;
        ss >> module;
        vector<string> list;
        string dep;
        while (ss >> dep) {
            list.push_back(dep);
        }
        deps[module] = list;
    }

    int M;
    cin >> M;
    vector<string> direct(M);
    for (int i = 0; i < M; i++) {
        cin >> direct[i];
    }

    for (const string &mod : direct) {
        dfs(mod);
    }

    vector<string> result(needed.begin(), needed.end());
    sort(result.begin(), result.end());

    for (const string &mod : result) {
        cout << mod << "\n";
    }

    return 0;
}
