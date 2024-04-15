#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <utility>
#include <vector>
#define MAX_THREADS 12

std::default_random_engine engine;

class BacktrackingContext {
  public:
    std::bitset<128> covered;
    std::bitset<128> stat;
    BacktrackingContext() {
        covered.reset();
        stat.reset();
    }
    BacktrackingContext(const BacktrackingContext &other)
        : covered(other.covered), stat(other.stat) {}
};

class Instance {
  public:
    int n;
    std::vector<std::pair<int, int>> edges;
    std::vector<std::vector<int>> g;
    Instance() : n(0), edges({}), g({}) {}
    void load(std::ifstream &inf);
    std::string solve();
};

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

std::bitset<128> shared_global;
std::mutex shared_mutex;

int tcnt;
int mxcnt;
std::mutex tcnt_mutex;
void rec(Instance &inst, BacktrackingContext ctx, std::bitset<128> &out) {
    if (ctx.covered.count() == inst.n) {
        out = ctx.stat;
        return;
    } else if (ctx.stat.count() > shared_global.count()) {
        out = shared_global;
        return;
    }
    std::vector<std::pair<int, int>> cands;
    for (int i = 0; i < inst.n; i++) {
        if (!ctx.covered[i]) {
            int c = 0;
            int jj;
            for (int j : inst.g[i]) {
                if (!ctx.covered[j]) {
                    c++;
                    jj = j;
                }
            }
            if (c == 0) {
                BacktrackingContext ctxcov(ctx);
                ctxcov.covered[i] = true;
                ctxcov.stat[i] = true;
                rec(inst, ctxcov, out);
                return;
            } else if (c == 1) {
                BacktrackingContext ctxcov(ctx);
                ctxcov.covered[jj] = true;
                ctxcov.stat[jj] = true;
                for (int k : inst.g[jj])
                    ctxcov.covered[k] = true;
                rec(inst, ctxcov, out);
                return;
            } else {
                cands.push_back({c, i});
            }
        }
    }
    std::sort(cands.begin(), cands.end());
    while (!cands.empty() && cands.back().first != cands.front().first)
        cands.pop_back();
    std::uniform_int_distribution<int> uid(0, cands.size() - 1);
    int now = cands[uid(engine)].second;
    // Case 1: cover the current vertex
    BacktrackingContext ctx1(ctx);
    ctx1.covered[now] = true;
    ctx1.stat[now] = true;
    for (int nxt : inst.g[now]) {
        ctx1.covered[nxt] = true;
    }
    // Case 2: don't cover the current vertex;
    std::vector<BacktrackingContext> ctxs;
    std::vector<std::bitset<128>> outs;
    for (int nxt : inst.g[now]) {
        BacktrackingContext ctxi(ctx);
        ctxi.covered[nxt] = true;
        ctxi.stat[nxt] = true;
        for (int oth : inst.g[nxt]) {
            ctxi.covered[oth] = true;
        }
        ctxs.push_back(ctxi);
        outs.push_back({});
    }
    std::bitset<128> out1;
    int divisor = ctxs.size();
    std::vector<std::thread> threads;
    int instnum;
    {
        std::lock_guard<std::mutex> guard(tcnt_mutex);
        int curtcnt = tcnt;
        instnum =
            std::max(0, std::min((int)ctxs.size(), MAX_THREADS - 1 - curtcnt));
        for (int i = 0; i < instnum; i++) {
            threads.emplace_back(rec, std::ref(inst), ctxs[i],
                                 std::ref(outs[i]));
            tcnt++;
            mxcnt = std::max(mxcnt, tcnt);
        }
    }
    for (int i = instnum; i < ctxs.size(); i++) {
        rec(inst, ctxs[i], outs[i]);
    }
    rec(inst, ctx1, out1);
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
        std::lock_guard<std::mutex> guard(tcnt_mutex);
        tcnt--;
    }
    out = out1;
    for (int i = 0; i < ctxs.size(); i++) {
        if (out.count() > outs[i].count()) {
            out = outs[i];
        }
    }
    if (shared_global.count() <= out.count())
        return;
    std::lock_guard<std::mutex> guard(shared_mutex);
    if (shared_global.count() > out.count()) {
        shared_global = out;
    }
}

std::string Instance::solve() {
    engine.seed(42); // for consistency during tests
    shared_global.set();
    BacktrackingContext ctx;
    tcnt = 0;
    std::bitset<128> inner;
    rec(*this, ctx, inner);
    std::string out;
    for (int i = 0; i < n; i++) {
        if (inner[i])
            out.push_back('1');
        else
            out.push_back('0');
    }
    return out;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./main <input-filename> <output-filename>"
                  << std::endl;
        return 0;
    }
    std::ifstream inf(argv[1]);
    if (inf.fail()) {
        std::cout << "Error opening input file. Check whether file exists."
                  << std::endl;
        return 0;
    }
    std::ofstream ouf(argv[2]);
    if (ouf.fail()) {
        std::cout << "Error opening output file." << std::endl;
        return 0;
    }
    Instance inst;
    inst.load(inf);
    std::cout << "Loading complete. Solving..." << std::endl;
    ouf << inst.solve() << std::endl;
    return 0;
}