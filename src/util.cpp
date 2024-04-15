#include "util.h"
#include <fstream>

BacktrackingContext::BacktrackingContext() {
    covered.reset();
    stat.reset();
}

BacktrackingContext::BacktrackingContext(const BacktrackingContext &other)
    : covered(other.covered), stat(other.stat) {}

Instance::Instance() : n(0), edges({}), g({}) {}

void Instance::load(std::ifstream &inf) {
    inf >> n;
    g.resize(n);
    int m;
    inf >> m;
    edges.reserve(m);
    for (int i = 0; i < m; i++) {
        int u, v;
        inf >> u >> v;
        edges.push_back({u, v});
        g[u].push_back(v);
        g[v].push_back(u);
    }
}