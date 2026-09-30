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

    map<string,int> indegree;
    for (const string &mod : needed) {
        indegree[mod] = 0;
    }
    for (const auto &p : deps) {
        const string &mod = p.first;
        if (!needed.count(mod)) continue;
        for (const string &d : p.second) {
            if (needed.count(d)) {
                indegree[d]++;
            }
        }
    }

    vector<string> order;
    while (!indegree.empty()) {
        vector<string> zeros;
        for (auto &p : indegree) {
            if (p.second == 0) zeros.push_back(p.first);
        }
        if (zeros.empty()) {
            cout << "CYCLE\n";
            return 0;
        }
        sort(zeros.begin(), zeros.end());
        string chosen = zeros[0];
        order.push_back(chosen);
        indegree.erase(chosen);

        for (const string &d : deps[chosen]) {
            if (indegree.count(d)) {
                indegree[d]--;
            }
        }
    }

    for (const string &mod : order) {
        cout << mod << "\n";
    }

    return 0;
}
