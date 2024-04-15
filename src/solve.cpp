#include "util.h"
#include <algorithm>
#include <mutex>
#include <random>
#include <thread>

std::default_random_engine engine;

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