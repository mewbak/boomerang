/*
 * Experimental Dominator frontier code largely as per Appel
 */

#include "dom.h"

void Cfg::DFS(DOM* d, int p, int n) {
    if (d->dfnum[n] == 0) {
        d->dfnum[n] = d->N; d->vertex[d->N] = n; d->parent[n] = p;
        d->N++;
        // For each successor w of n
        PBB bb = d->BBs[n];
        std::vector<PBB>::iterator oo;
        for (oo = bb->m_OutEdges.begin(); oo != bb->m_OutEdges.end(); oo++) {
            PBB succ = *oo;
            int w;
            if (d->indices.find(succ) == d->indices.end()) {
                w = d->next++;
                d->indices[succ] = w;
                d->BBs[w] = succ;
            }
            else
                w = d->indices[succ];
            DFS(d, n, w);
        }
    }
}

void Cfg::dominators(DOM* d) {
    PBB r = getEntryBB();
    int numBB = m_listBB.size();
    d->BBs.resize(numBB, (PBB)-1);
    d->N = 0; d->next = 1; d->BBs[0] = r; d->indices[r] = 0;
    // Initialise to "none"
    d->dfnum.resize(numBB, 0);
    d->semi.resize(numBB, -1);
    d->ancestor.resize(numBB, -1);
    d->idom.resize(numBB, -1);
    d->samedom.resize(numBB, -1);
    d->vertex.resize(numBB, -1);
    d->parent.resize(numBB, -1);
    d->best.resize(numBB, -1);
    d->bucket.resize(numBB);
    d->DF.resize(numBB);
    DFS(d, -1, 0);
    for (int i=d->N-1; i >= 1; i--) {
        int n = d->vertex[i]; int p = d->parent[n]; int s = p;
        /* These lines calculate the semi-dominator of n, based on the
            Semidominator Theorem */
        // for each predecessor v of n
        PBB bb = d->BBs[n];
        std::vector<PBB>::iterator it;
        for (it = bb->m_InEdges.begin(); it != bb->m_InEdges.end(); it++) {
            int v = d->indices[*it];
            int sdash;
            if (d->dfnum[v] <= d->dfnum[n])
                sdash = v;
            else sdash = d->semi[ancestorWithLowestSemi(d, v)];
            if (d->dfnum[sdash] < d->dfnum[s])
                s = sdash;
        }
        d->semi[n] = s;
        /* Calculation of n'd dominator is deferred until the path from s to n
            has been linked into the forest */
        d->bucket[s].insert(n);
        Link(d, p, n);
        // for each v in bucket[p]
        std::set<int>::iterator jj;
        for (jj=d->bucket[p].begin(); jj != d->bucket[p].end(); jj++) {
            int v = *jj;
            /* Now that the path from p to v has been linked into the spanning
                forest, these lines calculate the dominator of v, based on the
                first clause of the Dominator Theorem, or else defer the calc-
                ulation until y's dominator is known. */
            int y = ancestorWithLowestSemi(d, v);
            if (d->semi[y] == d->semi[v])
                d->idom[v] = p;         // Success!
            else d->samedom[v] = y;     // Defer
        }
        d->bucket[p].clear();
    }
    for (int i=1; i < d->N-1; i++) {
        /* Now all the deferred dominator calculations, based on the second
            clause of the Dominator Theorem, are performed. */
        int n = d->vertex[i];
        if (d->samedom[n] != -1) {
            d->idom[n] = d->idom[d->samedom[n]];    // Deferred success!
        }
    }
    computeDF(d, 0);            // Finally, compute the dominance frontiers
}

int Cfg::ancestorWithLowestSemi(DOM* d, int v) {
    int a = d->ancestor[v];
    if (d->ancestor[a] != -1) {
        int b = ancestorWithLowestSemi(d, a);
        d->ancestor[v] = d->ancestor[a];
        if (d->dfnum[d->semi[b]] < d->dfnum[d->semi[d->best[v]]])
            d->best[v] = b;
    }
    return d->best[v];
}

void Cfg::Link(DOM* d, int p, int n) {
    d->ancestor[n] = p; d->best[n] = n;
}

// Return true if n dominates w
bool dominate(DOM* d, int n, int w) {
    while (d->idom[w] != -1) {
        if (d->idom[w] == n)
            return true;
        w = d->idom[w];     // Move up the dominator tree
    }
    return false;
}

void Cfg::computeDF(DOM* d, int n) {
    std::set<int> S;
    /* THis loop computes DF_local[n] */
    // for each node y in succ(n)
    PBB bb = d->BBs[n];
    std::vector<PBB>::iterator it;
    for (it = bb->m_OutEdges.begin(); it != bb->m_OutEdges.end(); it++) {
        int y = d->indices[*it];
        if (d->idom[y] != n)
            S.insert(y);
    }
    // for each child c of n in the dominator tree
    // Note: this is a linear search!
    int sz = d->ancestor.size();
    for (int c = 0; c < sz; c++) {
        if (d->idom[c] != n) continue;
        computeDF(d, c);
        /* This loop computes DF_up[c] */
        // for each element w of DF[c]
        std::set<int>& s = d->DF[c];
        std::set<int>::iterator ww;
        for (ww = s.begin(); ww != s.end(); ww++) {
            int w = *ww;
            // if n does not dominate w, or if n = w
            if (n == w || !dominate(d, n, w)) {
                S.insert(w);
            }
        }
    }
    d->DF[n] = S;
}

void Cfg::placePhiFunctions(DOM* d, int memDepth) {
    // First free some memory no longer needed
    d->dfnum.resize(0);
    d->semi.resize(0);
    d->ancestor.resize(0);
    d->idom.resize(0);
    d->samedom.resize(0);
    d->vertex.resize(0);
    d->parent.resize(0);
    d->best.resize(0);
    d->bucket.resize(0);

    // Set the sizes of needed vectors
    int numBB = d->indices.size();
    d->A_orig.resize(numBB);

    // We need to create A_orig for the current memory depth
    for (int n=0; n < numBB; n++) {
        BasicBlock::rtlit rit; BasicBlock::elit ii, cii;
        PBB bb = d->BBs[n];
        for (Statement* s = bb->getFirstStmt(rit, ii, cii); s;
          s = bb->getNextStmt(rit, ii, cii)) {
            AssignExp* ae = dynamic_cast<AssignExp*>(s);
            if (ae) {
                Exp* lhs = ae->getLeft();
                if (lhs->getMemDepth() == memDepth)
                    d->A_orig[n].insert(ae->getLeft());
            }
        }
    }

    // For each node n
    for (int n=0; n < numBB; n++) {
        // For each variable a in A_orig[n]
        std::set<Exp*, lessExpStar>& s = d->A_orig[n];
        std::set<Exp*, lessExpStar>::iterator aa;
        for (aa = s.begin(); aa != s.end(); aa++) {
            Exp* a = *aa;
            d->defsites[a].insert(n);
        }
    }

    // For each variable a (in defsites, I presume)
    std::map<Exp*, std::set<int>, lessExpStar>::iterator mm;
    for (mm = d->defsites.begin(); mm != d->defsites.end(); mm++) {
        Exp* a = (*mm).first;               // *mm is pair<Exp*, set<int>>
        std::set<int> W = d->defsites[a];   // set copy
        // While W not empty
        while (W.size()) {
            // Remove some node n from W
            int n = *W.begin();             // Copy first element
            W.erase(W.begin());             // Remove first element
            // for each y in DF[n]
            std::set<int>::iterator yy;
            std::set<int>& DFn = d->DF[n];
            for (yy = DFn.begin(); yy != DFn.end(); yy++) {
                int y = *yy;
                // if y not element of A_phi[a]
                std::set<int>& s = d->A_phi[a];
                if (s.find(y) == s.end()) {
                    // Supposed to insert trivial phi function for a at top of
                    // block y
//std::cerr << "Trivial phi function for " << a << " at top of block " << std::dec << y << " (0x" << std::hex << (unsigned)d->BBs[y]->getLowAddr() << ")\n";
                    // A_phi[a] <- A_phi[a] U {y}
                    s.insert(y);
                    // if a !elementof A_orig[y]
                    if (d->A_orig[y].find(a) == d->A_orig[y].end())
                        // W <- W U {y}
                        W.insert(y);
                }
            }
        }
    }
}
