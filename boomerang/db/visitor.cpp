/*
 * Copyright (C) 2004, Mike Van Emmerik and Trent Waddington
 */
/*==============================================================================
 * FILE:       visitor.cpp
 * OVERVIEW:   Provides the implementation for the various visitor and modifier
 *             classes.
 *============================================================================*/
/*
 * $Revision: 1.3 $
 *
 * 14 Jun 04 - Mike: Created, from work started by Trent in 2003
 */

#include "visitor.h"
#include "exp.h"
#include "statement.h"
#include "log.h"
#include "boomerang.h"      // For VERBOSE


// FixProcVisitor class

bool FixProcVisitor::visit(Location* l, bool& override) {
    l->setProc(proc);       // Set the proc, but only for Locations
    override = false;       // Use normal accept logic
    return true;
}

// GetProcVisitor class

bool GetProcVisitor::visit(Location* l, bool& override) {
    proc = l->getProc();
    override = false;
    return proc == NULL;        // Continue recursion only if failed so far
}

// SetConscripts class

bool SetConscripts::visit(Const* c) {
    if (!bInLocalGlobal)
        c->setConscript(++curConscript);
    bInLocalGlobal = false;
    return true;       // Continue recursion
}

bool SetConscripts::visit(Location* l, bool& override) {
    OPER op = l->getOper();
    if (op == opLocal || op == opGlobal || op == opRegOf || op == opParam)
        bInLocalGlobal = true;
    override = false;
    return true;       // Continue recursion
}


bool StmtVisitor::visit(RTL* rtl) {
    // Mostly, don't do anything at the RTL level
    return true;
} 

bool StmtSetConscripts::visit(Assign* stmt) {
    SetConscripts sc(curConscript);
    stmt->getLeft()->accept(&sc);
    stmt->getRight()->accept(&sc);
    curConscript = sc.getLast();
    return true;
}

bool StmtSetConscripts::visit(CallStatement* stmt) {
    SetConscripts sc(curConscript);
    std::vector<Exp*> args;
    args = stmt->getArguments();
    int i, n = args.size();
    for (i=0; i < n; i++)
        args[i]->accept(&sc);
    n = stmt->getNumReturns();
    for (i=0; i < n; i++) {
        Exp* r = stmt->getReturnExp(i);
        r->accept(&sc);
    }
    curConscript = sc.getLast();
    return true;
}

bool StmtSetConscripts::visit(CaseStatement* stmt) {
    SetConscripts sc(curConscript);
    SWITCH_INFO* si = stmt->getSwitchInfo();
    if (si) {
        si->pSwitchVar->accept(&sc);
        curConscript = sc.getLast();
    }
    return true;
}

bool StmtSetConscripts::visit(ReturnStatement* stmt) {
    SetConscripts sc(curConscript);
    int n = stmt->getNumReturns();
    for (int i=0; i < n; i++) {
        Exp* r = stmt->getReturnExp(i);
        r->accept(&sc);
    }
    curConscript = sc.getLast();
    return true;
}

bool StmtSetConscripts::visit(BoolStatement* stmt) {
    SetConscripts sc(curConscript);
    stmt->getCondExpr()->accept(&sc);
    stmt->getDest()->accept(&sc);
    curConscript = sc.getLast();
    return true;
}

void StripPhis::visit(Assign* s) {
    del = s->isPhi();
}

Exp* StripRefs::preVisit(RefExp* e, bool& norecur) {
    norecur = false;
    return e->getSubExp1();     // Do the actual stripping of references!
}

Exp* CallRefsFixer::postVisit(RefExp* r) {
    Exp* ret = r;
    // If child was modified, simplify now
    if (!(unchanged & ~mask)) ret = r->simplify();
    mask >>= 1;
    // Note: r will always == ret here, so the below is safe
    Statement* def = r->getRef();
    CallStatement *call = dynamic_cast<CallStatement*>(def);
    if (call) {
        Exp *e = call->getProven(r->getSubExp1());
        if (e) {
            e = call->substituteParams(e->clone());
            assert(e);
            if (VERBOSE)
                LOG << "fixcall refs replacing " << r << " with " << e
                    << "\n";
            // e = e->simplify();   // No: simplify the parent
            unchanged &= ~mask;
            mod = true;
            return e;
        } else {
            Exp* subExp1 = r->getSubExp1();
            if (call->findReturn(subExp1) == -1) {
                if (VERBOSE && !subExp1->isPC()) {
                    LOG << "nothing proven about " << subExp1 <<
                        " and yet it is referenced by " << r <<
                        ", and not in returns of " << "\n" <<
                        "   " << call << "\n";
                }
            }
        }
    }
    return ret;
}

Exp* CallRefsFixer::postVisit(PhiExp* p) {
    Exp* ret = p;
    // If child was modified, simplify now
    if (!(unchanged & mask)) ret = p->simplify();
    mask >>= 1;

    std::vector<Statement*> remove;
    std::vector<Statement*> insert;
    unsigned n = p->getNumRefs();

    bool oneIsGlobalFunc = false;
    Prog *prog = NULL;
    unsigned int i;
    for (i=0; i < n; i++) {
        Statement* u = p->getAt(i);
        if (u) {
            CallStatement *call = dynamic_cast<CallStatement*>(u);
            if (call)
                prog = call->getProc()->getProg();
        }
    }
    if (prog)
        oneIsGlobalFunc = p->hasGlobalFuncParam(prog);

    for (i=0; i < n; i++) {
        Statement* u = p->getAt(i);
        CallStatement *call = dynamic_cast<CallStatement*>(u);
        if (call) {
            Exp* subExp1 = p->getSubExp1();
            Exp *e = call->getProven(subExp1);
            if (call->isComputed() && oneIsGlobalFunc) {
                e = subExp1->clone();
                if (VERBOSE)
                    LOG << "ignoring ref in phi to computed call with "
                        << "function pointer param " << e << "\n";
            }
            if (e) {
                e = call->substituteParams(e->clone());
                if (e && e->getOper() == opSubscript &&
                    *e->getSubExp1() == *subExp1) {
                    if (VERBOSE)
                        LOG << "fixcall refs replacing param " << i << " in "
                            << p << " with " << e << "\n";
                    p->putAt(i, ((RefExp*)e)->getRef());
                    mod = true;
                } else {
                    if (VERBOSE)
                        LOG << "cant update phi ref to " << e
                                  << "\n";
                }
            } else {
                if (call->findReturn(subExp1) == -1) {
                    if (VERBOSE) {
                        LOG << "nothing proven about " << subExp1 <<
                            " and yet it is referenced by " << p <<
                            ", and not in returns of " << "\n" <<
                            "   " << call << "\n";
                    }
                }
            }
        }
    }
    return ret;
}

Exp* CallRefsFixer::postVisit(Unary *e)    {
    bool isAddrOfMem = e->isAddrOf() && e->getSubExp1()->isMemOf();
    if (isAddrOfMem) return e;
    Exp* ret = e;
    if (!(unchanged & mask)) ret = e->simplify();
    mask >>= 1;
    return ret;
}
Exp* CallRefsFixer::postVisit(Binary *e)    {
    Exp* ret = e;
    if (!(unchanged & mask)) ret = e->simplifyArith()->simplify();
    mask >>= 1;
    return ret;
}
Exp* CallRefsFixer::postVisit(Ternary *e)    {
    Exp* ret = e;
    if (!(unchanged & mask)) ret = e->simplify();
    mask >>= 1;
    return ret;
}
Exp* CallRefsFixer::postVisit(TypedExp *e)    {
    Exp* ret = e;
    if (!(unchanged & mask)) ret = e->simplify();
    mask >>= 1;
    return ret;
}
Exp* CallRefsFixer::postVisit(FlagDef *e)    {
    Exp* ret = e;
    if (!(unchanged & mask)) ret = e->simplify();
    mask >>= 1;
    return ret;
}
Exp* CallRefsFixer::postVisit(Location *e)    {
    Exp* ret = e;
    if (!(unchanged & mask)) ret = e->simplify();
    mask >>= 1;
    return ret;
}
Exp* CallRefsFixer::postVisit(Const *e)    {
    mask >>= 1;
    return e;
}
Exp* CallRefsFixer::postVisit(TypeVal *e)    {
    mask >>= 1;
    return e;
}
Exp* CallRefsFixer::postVisit(Terminal *e)    {
    mask >>= 1;
    return e;
}

// Add used locations finder
bool UsedLocsFinder::visit(Location* e, bool& override) {
    used->insert(e);        // All locations visited are used
    if (e->isMemOf()) {
        // Example: m[r28{10} - 4]  we use r28{10}
        Exp* child = e->getSubExp1();
        child->accept(this);
    }
    override = false;
    return true;
}

bool UsedLocsFinder::visit(Terminal* e) {
    switch (e->getOper()) {
        case opPC:
        case opFlags:
        case opFflags:
        // Fall through
        // The carry flag can be used in some SPARC idioms, etc
        case opDF: case opCF: case opZF: case opNF: case opOF:  // also these
            used->insert(e);
        default:
            break;
    }
    return true;        // Always continue recursion
}

bool UsedLocsFinder::visit(RefExp* e, bool& override) {
    used->insert(e);         // This location is used
    // However, e's subexpression is NOT used ...
    override = true;
    // ... unless that is a m[x], in which case x (not m[x]) is used
    Exp* refd = e->getSubExp1();
    if (refd->isMemOf()) {
        Exp* x = refd->getSubExp1();
        x->accept(this);
    }
    return true;
}

bool UsedLocsFinder::visit(PhiExp* e, bool& override) {
    StatementVec& stmtVec = e->getRefs();
    Exp* subExp1 = e->getSubExp1();
    StatementVec::iterator uu;
    for (uu = stmtVec.begin(); uu != stmtVec.end(); uu++) {
        Exp* temp = new RefExp(subExp1, *uu);
        used->insert(temp);
    }
    override = false;
    return true;
}

bool UsedLocsVisitor::visit(Assign* s, bool& override) {
    Exp* lhs = s->getLeft();
    Exp* rhs = s->getRight();
    if (rhs) rhs->accept(ev);
    // Special logic for the LHS
    if (lhs->isMemOf()) {
        Exp* child = ((Location*)lhs)->getSubExp1();
        child->accept(ev);
    }
    override = true;                // Don't do the usual accept logic
    return true;                    // Continue the recursion
}

bool UsedLocsVisitor::visit(CallStatement* s, bool& override) {
    Exp* pDest = s->getDest();
    if (pDest)
        pDest->accept(ev);
    std::vector<Exp*>::iterator it;
    std::vector<Exp*>& arguments = s->getArguments();
    for (it = arguments.begin(); it != arguments.end(); it++)
        (*it)->accept(ev);
    if (!final) {
        // Ignore the implicit arguments when final
        int n = s->getNumImplicitArguments();
        for (int i=0; i < n; i++)
            s->getImplicitArgumentExp(i)->accept(ev);
    }
    // For the final pass, also only consider the first return
    int n = s->getNumReturns();
    if (final) {
        if (n != 0) {
            Exp* r = s->getReturnExp(0);
            // If of form m[x] then x is used
            if (r->isMemOf()) {
                Exp* x = ((Location*)r)->getSubExp1();
                x->accept(ev);
            }
        }
    } else {
        // Otherwise, consider all returns. If of form m[x] then x is used
        for (int i=0; i < n; i++) {
            Exp* r = s->getReturnExp(i);
            if (r->isMemOf()) {
                Exp* x = ((Location*)r)->getSubExp1();
                x->accept(ev);
            }
        } 
    }
    override = true;            // Don't do the normal accept logic
    return true;                // Continue the recursion
}

bool UsedLocsVisitor::visit(BoolStatement* s, bool& override) {
    Exp* pCond = s->getCondExpr();
    if (pCond)
        pCond->accept(ev);              // Condition is used
    Exp* pDest = s->getDest();
    if (pDest && pDest->isMemOf()) {    // If dest is of form m[x]...
        Exp* x = ((Location*)pDest)->getSubExp1();
        x->accept(ev);                  // ... then x is used
    }
    override = true;            // Don't do the normal accept logic
    return true;                // Continue the recursion
}

//
// Expression subscripter
//
Exp* ExpSubscripter::preVisit(Location* e, bool& norecur) {
    if (*e == *search) {
        norecur = !e->isMemOf();     // Don't double subscript unless m[...]
        return new RefExp(e, def);
    }
    norecur = false;
    return e;
}

Exp* ExpSubscripter::preVisit(Terminal* e) {
    if (*e == *search)
        return new RefExp(e, def);
    return e;
}

Exp* ExpSubscripter::preVisit(RefExp* e, bool& norecur) {
    Exp* base = e->getSubExp1();
    if (*base == *search) {
        norecur = true;     // Don't recurse; would double subscript
        e->setDef(def);
        return e;
    }
    norecur = false;
    return e;
}

