/*
 * Copyright (C) 2002, Trent Waddington
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       statement.cpp
 * OVERVIEW:   Implementation of the Statement and related classes.
 *             (Was dataflow.cpp)
 *============================================================================*/

/*
 * $Revision: 1.23 $
 * 03 Jul 02 - Trent: Created
 * 09 Jan 03 - Mike: Untabbed, reformatted
 * 03 Feb 03 - Mike: cached dataflow (uses and usedBy) (since reversed)
 * 25 Jul 03 - Mike: dataflow.cpp, hrtl.cpp -> statement.cpp
 */

/*==============================================================================
 * Dependencies.
 *============================================================================*/

#include <assert.h>
#include <iomanip>          // For setfill etc
#if defined(_MSC_VER) && _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif 

#include "statement.h"
#include "exp.h"
#include "cfg.h"
#include "proc.h"
#include "prog.h"
#include "boomerang.h"
#include "rtl.h"            // For debugging code
#include "util.h"
#include "signature.h"
#include <sstream>

#define VERBOSE Boomerang::get()->vFlag

static char debug_buffer[200];      // For prints functions

// replace a use in this statement
void Statement::replaceRef(Statement *def) {
    Exp* lhs = def->getLeft();
    Exp* rhs = def->getRight();
    assert(lhs);
    assert(rhs);
    // "Wrap" the LHS in a RefExp
    // This was so that it matches with the thing it is replacing.
    // Example: 42: r28 := r28{14}-4 into m[r28-24] := m[r28{42}] + ...
    // The r28 needs to be subscripted with {42} to match the thing on
    // the RHS that is being substituted into. (It also makes sure it
    // never matches the other r28, which should really be r28{0}).
    Unary* re;
    re = new RefExp(lhs, def);

    // do the replacement
    doReplaceRef(re, rhs);

    // Careful: don't delete re while lhs is still a part of it!
    // Else, will delete lhs, which is still a part of def!
    re->setSubExp1ND(NULL);
    delete re;
}

// Check the liveout set for interferences
// Examples:  r[24]{3} and r[24]{5} both live at same time,
// or m[r[28]{3}] and m[r[28]{3}]{2}
static int nextVarNum = 0;
void insertInterference(igraph& ig, Exp* e) {
    igraph::iterator it = ig.find(e);
    if (it == ig.end())
        // We will be inserting a new element
        ig.insert(std::pair<Exp*, int>(e, ++nextVarNum));
    // else it is already in the map: no need to do anything
}

bool Statement::mayAlias(Exp *e1, Exp *e2, int size) { 
    if (*e1 == *e2) return true;
    // Pass the expressions both ways. Saves checking things like
    // m[exp] vs m[exp+K] and m[exp+K] vs m[exp] explicitly (only need to
    // check one of these cases)
    bool b =  (calcMayAlias(e1, e2, size) && calcMayAlias(e2, e1, size)); 
    if (b && VERBOSE) {
        std::cerr << "May alias: " << e1 << " and " << e2 << " size " << size
          << "\n";
    }
    return b;
}

// returns true if e1 may alias e2
bool Statement::calcMayAlias(Exp *e1, Exp *e2, int size) {
    // currently only considers memory aliasing..
    if (!e1->isMemOf() || !e2->isMemOf()) {
        return false;
    }
    Exp *e1a = e1->getSubExp1();
    Exp *e2a = e2->getSubExp1();
    // constant memory accesses
    if (e1a->isIntConst() && 
        e2a->isIntConst()) {
        ADDRESS a1 = ((Const*)e1a)->getAddr();
        ADDRESS a2 = ((Const*)e2a)->getAddr();
        int diff = a1 - a2;
        if (diff < 0) diff = -diff;
        if (diff*8 >= size) return false;
    }
    // same left op constant memory accesses
    if (
      e1a->getArity() == 2 &&
      e1a->getOper() == e2a->getOper() &&
      e1a->getSubExp2()->isIntConst() &&
      e2a->getSubExp2()->isIntConst() &&
      *e1a->getSubExp1() == *e2a->getSubExp1()) {
        int i1 = ((Const*)e1a->getSubExp2())->getInt();
        int i2 = ((Const*)e2a->getSubExp2())->getInt();
        int diff = i1 - i2;
        if (diff < 0) diff = -diff;
        if (diff*8 >= size) return false;
    }
    // [left] vs [left +/- constant] memory accesses
    if (
      (e2a->getOper() == opPlus || e2a->getOper() == opMinus) &&
      *e1a == *e2a->getSubExp1() &&
      e2a->getSubExp2()->isIntConst()) {
        int i1 = 0;
        int i2 = ((Const*)e2a->getSubExp2())->getInt();
        int diff = i1 - i2;
        if (diff < 0) diff = -diff;
        if (diff*8 >= size) return false;
    }
    // Don't need [left +/- constant ] vs [left] because called twice with
    // args reversed
    return true;
}


/* 
 * Returns true if the statement can be propagated to all uses (and
 * therefore can be removed).
 * Returns false otherwise.
 *
 * To completely propagate a statement which does not kill any of its
 * own uses it is sufficient to show that:
 * of all the definitions reaching each target, those that define locations
 * that the source statement uses, should also reach the source statement.
 * Reaching the source statement is most easily accomplished by searching
 * the set of stataments that the source statement uses (its uses set).
 * (the above is for condition 2 of the Dragon book, p636).
 *
 * A statement that kills one or more of its own uses is slightly more 
 * complicated. 
 All the uses that are not killed must still have their
 * definitions reach the expression to be propagated to, but the
 * uses that were killed must have their definitions available at the
 * expression to be propagated to after the statement is 
 * removed.  This is clearly the case if the only use killed by a 
 * statement is the same as the left hand side, however, if multiple uses
 * are killed a search must be conducted to ensure that no statement between
 * the source and the destination kills the other uses. 
 * Example: *32* m[2] := m[0] + m[4]
 * This is considered too complex a task and is therefore defered for
 * later experimentation.
 */
#if 0
bool Statement::canPropagateToAll() {
    StatementSet defs;     // Set of locations used, except for (max 1) killed
    defs = uses;
    int nold = uses.size();     // Number of statements I use
    killDef(defs);            // Number used less those killed this stmt
    if (nold - defs.size() > 1) {
        // See comment above.
        if (VERBOSE) {
            std::cerr << "too hard failure in canPropagateToAll: ";
            printWithUses(std::cerr);
            std::cerr << std::endl;
        }
        return false;
    }

    if (usedBy.size() == 0) {
        return false;
    }

    Exp* thisLhs = getLeft();
    StmtSetIter it;
    // We would like to propagate to each dest
    // sdest iterates through the destinations
    for (Statement* sdest = usedBy.getFirst(it); sdest;
         sdest = usedBy.getNext(it)) {
        // all locations used by this (the source statement) must not be
        // defined on any path from this statement to the destination
        // This is the condition 2 in the Dragon book, p636
        if (sdest == this) 
            return false; // can't propagate to self
        StatementSet destIn;
        // Note: this all needs changing. Can propagate anything with SSA!
        sdest->getReachIn(destIn, 2);
        StmtSetIter dd;
        for (Statement* reachDest = destIn.getFirst(dd); reachDest;
          reachDest = destIn.getNext(dd)) {
            if (reachDest == this) {
                // That means that the source defined one of its uses, e.g.
                // it was r[28] := r[28] - 4
                // this is fine
                continue;
            }
            // Does this reaching definition define a location used by the
            // source statement?
            Exp* lhsReachDest = reachDest->getLeft();
            if (lhsReachDest == NULL) continue;
            if (usesExp(lhsReachDest)) {
                // Yes, it is such a definition. Does this definition also reach
                // the source statement? i.e. reachDest in uses?
                if (!uses.exists(reachDest)) {
                    // No... condition 2 does not hold
#if 0
  std::cerr << "Can't propagate " << this << " because destination " << sdest << " has a reaching definition " << reachDest << " which is not in my uses set: ";
  uses.print();
#endif
                    return false;
                }
            }
        }
        // Mike's idea: reject if more than 1 def reaches the dest
        // Must be only one definition (this statement) of thisLhs that reaches
        // each destination (Dragon book p636 condition 1)
        // sdest->uses is a set of statements defining various things that
        // sdest uses (not all of them define thisLhs, e.g. if sdest is 
        // foo := thisLhs + z, some of them define z)
        int defThisLhs = 0;
        StmtSetIter dui;
        for (Statement* du = sdest->uses.getFirst(dui); du;
          du = sdest->uses.getNext(dui)) {
            Exp* lhs = du->getLeft();
            if (*lhs == *thisLhs) defThisLhs++;
        }
        assert(defThisLhs);         // Should at least find one (this)
        if (defThisLhs > 1) {
#if 0
  std::cerr << "Can't propagate " << this << " because there are " << defThisLhs
    << " uses for destination " << sdest << "; they include: ";
  StmtSetIter xx;
  for (Statement* ss = sdest->uses.getFirst(xx); ss;
    ss = sdest->uses.getNext(xx))
      std::cerr << ss << ", "; std::cerr << "\n";
#endif
            return false;
        }
    }
    return true;
}

// assumes canPropagateToAll has returned true
// assumes this statement will be removed by the caller
void Statement::propagateToAll() {
    StmtSetIter it;
    for (Statement* s = usedBy.getFirst(it); s; s = usedBy.getNext(it)) {
        s->replaceRef(this);
    }
}
#endif

/*==============================================================================
 * FUNCTION:        operator<<
 * OVERVIEW:        Output operator for Statement*
 *                  Just makes it easier to use e.g. std::cerr << myStmtStar
 * PARAMETERS:      os: output stream to send to
 *                  p: ptr to Statement to print to the stream
 * RETURNS:         copy of os (for concatenation)
 *============================================================================*/
std::ostream& operator<<(std::ostream& os, Statement* s) {
    if (s == NULL) {os << "NULL "; return os;}
    s->print(os, true);
    return os;
}

bool Statement::isOrdinaryAssign() {
    if (kind != STMT_ASSIGN)
        return false;
    OPER op = ((Assign*)this)->getRight()->getOper();
    if (op == opFlagCall) return false;
    if (op == opPhi) return false;
    return true;
}

bool Statement::isFlagAssgn() {
    if (kind != STMT_ASSIGN)
        return false;
    OPER op = ((Assign*)this)->getRight()->getOper();
    return (op == opFlagCall);
}

    
bool Statement::isPhi() {
    if (kind != STMT_ASSIGN)
        return false;
    OPER op = ((Assign*)this)->getRight()->getOper();
    return (op == opPhi);
}
    

//
// StatementSet methods
//

// Make this set the union of itself and other
void StatementSet::makeUnion(StatementSet& other) {
    StmtSetIter it;
    for (it = other.sset.begin(); it != other.sset.end(); it++) {
        sset.insert(*it);
    }
}

// Make this set the difference of itself and other
void StatementSet::makeDiff(StatementSet& other) {
    StmtSetIter it;
    for (it = other.sset.begin(); it != other.sset.end(); it++) {
        sset.erase(*it);
    }
}


// Make this set the intersection of itself and other
void StatementSet::makeIsect(StatementSet& other) {
    StmtSetIter it, ff;
    for (it = sset.begin(); it != sset.end(); it++) {
        ff = other.sset.find(*it);
        if (ff == other.sset.end())
            // Not in both sets
            sset.erase(it);
    }
}

// Check for the subset relation, i.e. are all my elements also in the set
// other. Effectively (this intersect other) == this
bool StatementSet::isSubSetOf(StatementSet& other) {
    StmtSetIter it, ff;
    for (it = sset.begin(); it != sset.end(); it++) {
        ff = other.sset.find(*it);
        if (ff == other.sset.end())
            return false;
    }
    return true;
}

Statement* StatementSet::getFirst(StmtSetIter& it) {
    it = sset.begin();
    if (it == sset.end())
        // No elements
        return NULL;
    return *it;         // Else return the first element
}

Statement* StatementSet::getNext(StmtSetIter& it) {
    if (++it == sset.end())
        // No more elements
        return NULL;
    return *it;         // Else return the next element
}

// Remove this Statement. Return false if it was not found
bool StatementSet::remove(Statement* s) {
    if (sset.find(s) != sset.end()) {
        sset.erase(s);
        return true;
    }
    return false;
}

// Find s in this Statement set. Return true if found
bool StatementSet::exists(Statement* s) {
    StmtSetIter it = sset.find(s);
    return (it != sset.end());
}

// Find a definition for loc in this Statement set. Return true if found
bool StatementSet::defines(Exp* loc) {
    StmtSetIter it;
    for (it = sset.begin(); it != sset.end(); it++) {
        Exp* lhs = (*it)->getLeft();
        if (lhs && (*lhs == *loc))
            return true;
    }
    return false;
}

// Remove if defines the given expression
bool StatementSet::removeIfDefines(Exp* given) {
    bool found = false;
    std::set<Statement*>::iterator it;
    for (it = sset.begin(); it != sset.end(); it++) {
        Exp* left = (*it)->getLeft();
        if (left && *left == *given) {
            // Erase this Statement
            sset.erase(it);
            found = true;
        }
    }
    return found;
}

// As above, but given a whole statement set
bool StatementSet::removeIfDefines(StatementSet& given) {
    StmtSetIter it;
    bool found = false;
    for (Statement* s = given.getFirst(it); s; s = given.getNext(it)) {
        Exp* givenLeft = s->getLeft();
        if (givenLeft)
            found |= removeIfDefines(givenLeft);
    }
    return found;
}

// Print to a string, for debugging
char* StatementSet::prints() {
    std::ostringstream ost;
    StmtSetIter it;
    for (it = sset.begin(); it != sset.end(); it++)
        ost << *it << ",\t";
    ost << "\n";
    strncpy(debug_buffer, ost.str().c_str(), 199);
    debug_buffer[199] = '\0';
    return debug_buffer;
}

// Print just the numbers to stream os
void StatementSet::printNums(std::ostream& os) {
    StmtSetIter it;
    os << std::dec;
    for (it = sset.begin(); it != sset.end(); ) {
        if (*it)
            (*it)->printNum(os);
        else
            os << "0";              // Special case for no definition
        if (++it != sset.end())
            os << " ";
    }
}

bool StatementSet::operator<(const StatementSet& o) const {
    if (sset.size() < o.sset.size()) return true;
    if (sset.size() > o.sset.size()) return false;
    std::set<Statement*, lessExpStar>::iterator it1;
    std::set<Statement*, lessExpStar>::const_iterator it2;
    for (it1 = sset.begin(), it2 = o.sset.begin(); it1 != sset.end();
      it1++, it2++) {
        if (*it1 < *it2) return true;
        if (*it1 > *it2) return false;
    }
    return false;
}

bool StatementSet::isLast(StmtSetIter& it) {
    return it == sset.end();
}
 
//
// LocationSet methods
//

// Assignment operator
LocationSet& LocationSet::operator=(const LocationSet& o) {
    sset.clear();
    std::set<Exp*, lessExpStar>::const_iterator it;
    for (it = o.sset.begin(); it != o.sset.end(); it++) {
        sset.insert((*it)->clone());
    }
    return *this;
}

// Copy constructor
LocationSet::LocationSet(const LocationSet& o) {
    std::set<Exp*, lessExpStar>::const_iterator it;
    for (it = o.sset.begin(); it != o.sset.end(); it++)
        sset.insert((*it)->clone());
}

char* LocationSet::prints() {
    std::ostringstream ost;
    LocSetIter it;
    for (it = sset.begin(); it != sset.end(); it++)
        ost << *it << ",\t";
    ost << "\n";
    strncpy(debug_buffer, ost.str().c_str(), 199);
    debug_buffer[199] = '\0';
    return debug_buffer;
}

void LocationSet::remove(Exp* given) {
    std::set<Exp*, lessExpStar>::iterator it = sset.find(given);
    if (it == sset.end()) return;
//std::cerr << "LocationSet::remove at " << std::hex << (unsigned)this << " of " << *it << "\n";
//std::cerr << "before: "; print();
    // NOTE: if the below uncommented, things go crazy. Valgrind says that
    // the deleted value gets used next in LocationSet::operator== ?!
    //delete *it;         // These expressions were cloned when created
    sset.erase(it);
//std::cerr << "after : "; print();
}

void LocationSet::remove(LocSetIter ll) {
    //delete *ll;       // Don't trust this either
    sset.erase(ll);
}

// Remove locations defined by any of the given set of statements
// Used for killing in liveness sets
void LocationSet::removeIfDefines(StatementSet& given) {
    StmtSetIter it;
    for (Statement* s = given.getFirst(it); s; s = given.getNext(it)) {
        Exp* givenLeft = s->getLeft();
        if (givenLeft)
            sset.erase(givenLeft);
    }
}

// Make this set the union of itself and other
void LocationSet::makeUnion(LocationSet& other) {
    LocSetIter it;
    for (it = other.sset.begin(); it != other.sset.end(); it++) {
        sset.insert(*it);
    }
}

// Make this set the set difference of itself and other
void LocationSet::makeDiff(LocationSet& other) {
    LocSetIter it;
    for (it = other.sset.begin(); it != other.sset.end(); it++) {
        sset.erase(*it);
    }
}

Exp* LocationSet::getFirst(LocSetIter& it) {
    it = sset.begin();
    if (it == sset.end())
        // No elements
        return NULL;
    return *it;         // Else return the first element
}

Exp* LocationSet::getNext(LocSetIter& it) {
    if (++it == sset.end())
        // No more elements
        return NULL;
    return *it;         // Else return the next element
}

bool LocationSet::operator==(const LocationSet& o) const {
    // We want to compare the strings, not the pointers
    if (size() != o.size()) return false;
    std::set<Exp*, lessExpStar>::const_iterator it1, it2;
    for (it1 = sset.begin(), it2 = o.sset.begin(); it1 != sset.end();
      it1++, it2++) {
        if (!(**it1 == **it2)) return false;
    }
    return true;
}

bool LocationSet::find(Exp* e) {
    return sset.find(e) != sset.end();
}

bool LocationSet::findDifferentRef(RefExp* e) {
    RefExp search(e->getSubExp1()->clone(), (Statement*)-1);
    LocSetIter pos = sset.find(&search);
    if (pos == sset.end()) return false;
    while (pos != sset.end()) {
        // Exit if we've gone to a new base expression
        // E.g. searching for r13{10} and **pos is r14{0}
        if (!(**pos *= *e)) break;      // *= is ref-insensitive compare
        // Bases are the same; return true if only different ref
        if (!(**pos == *e)) return true;
        ++pos;
    }
    return false;
}

// Add a subscript (to definition d) to each element
void LocationSet::addSubscript(Statement* d) {
    LocSetIter it;
    std::set<Exp*, lessExpStar> newSet;
    for (it = sset.begin(); it != sset.end(); it++)
        newSet.insert((*it)->expSubscriptVar(*it, d));
    sset = newSet;          // Replace the old set!
    // Note: don't delete the old exps; they are copied in the new set
}

// Substitute s into all members of the set
void LocationSet::substitute(Statement& s) {
    Exp* lhs = s.getLeft();
    if (lhs == NULL) return;
    Exp* rhs = s.getRight();
    if (rhs == NULL) return;        // ? Will this ever happen?
    LocSetIter it;
    // Note: it's important not to change the pointer in the set of pointers
    // to expressions, without removing and inserting again. Otherwise, the
    // set becomes out of order, and operations such as set comparison fail!
    // To avoid any funny behaviour when iterating the loop, we use the follow-
    // ing two sets
    LocationSet removeSet;          // These will be removed after the loop
    LocationSet removeAndDelete;    // These will be removed then deleted
    LocationSet insertSet;          // These will be inserted after the loop
    bool change;
    for (it = sset.begin(); it != sset.end(); it++) {
        Exp* loc = *it;
        Exp* replace;
        if (loc->search(lhs, replace)) {
            if (rhs->isTerminal()) {
                // This is no longer a location of interest (e.g. %pc)
                removeSet.insert(loc);
                continue;
            }
            loc = loc->clone()->searchReplaceAll(lhs, rhs, change);
            if (change) {
                loc = loc->simplifyArith();
                loc = loc->simplify();
                // If the result is no longer a register or memory (e.g.
                // r[28]-4), then delete this expression and insert any
                // components it uses (in the example, just r[28])
                if (!loc->isRegOf() && !loc->isMemOf()) {
                    // Note: can't delete the expression yet, because the
                    // act of insertion into the remove set requires silent
                    // calls to the compare function
                    removeAndDelete.insert(*it);
                    loc->addUsedLocs(insertSet);
                    continue;
                }
                // Else we just want to replace it
                // Regardless of whether the top level expression pointer has
                // changed, remove and insert it from the set of pointers
                removeSet.insert(*it);      // Note: remove the unmodified ptr
                insertSet.insert(loc);
            }
        }
    }
    makeDiff(removeSet);       // Remove the items to be removed
    makeDiff(removeAndDelete); // These are to be removed as well
    makeUnion(insertSet);      // Insert the items to be added
    // Now delete the expressions that are no longer needed
    LocSetIter dd;
    for (Exp* e = removeAndDelete.getFirst(dd); e;
      e = removeAndDelete.getNext(dd))
        delete e;               // Plug that memory leak
}

//
// StatementList methods
//

bool StatementList::remove(Statement* s) {
    for (StmtListIter it = slist.begin(); it != slist.end(); it++) {
        if (*it == s) {
            slist.erase(it);
            return true;
        }
    }
    return false;
}

Statement* StatementList::remove(StmtListIter& it) {
    it = slist.erase(it);
    if (it == slist.end())
        return NULL;
    return *it;
}

void StatementList::append(StatementList& sl) {
    for (StmtListIter it = sl.slist.begin(); it != sl.slist.end(); it++) {
        slist.push_back(*it);
    }
}

void StatementList::append(StatementSet& ss) {
    StmtSetIter it;
    for (Statement* s  = ss.getFirst(it); s; s = ss.getNext(it)) {
        slist.push_back(s);
    }
}

Statement* StatementList::getFirst(StmtListIter& it) {
    it = slist.begin();
    if (it == slist.end())
        // No elements
        return NULL;
    return *it;         // Else return the first element
}

Statement* StatementList::getNext(StmtListIter& it) {
    if (++it == slist.end())
        // No more elements
        return NULL;
    return *it;         // Else return the next element
}

Statement* StatementList::getLast(StmtListRevIter& it) {
    it = slist.rbegin();
    if (it == slist.rend())
        // No elements
        return NULL;
    return *it;         // Else return the last element
}

Statement* StatementList::getPrev(StmtListRevIter& it) {
    if (++it == slist.rend())
        // No more elements
        return NULL;
    return *it;         // Else return the previous element
}

char* StatementList::prints() {
    std::ostringstream ost;
    StmtListIter it;
    for (it = slist.begin(); it != slist.end(); it++) {
        ost << *it << ",\t";
    }
    strncpy(debug_buffer, ost.str().c_str(), 199);
    debug_buffer[199] = '\0';
    return debug_buffer;
}

//
// StatementVec methods
//

void StatementVec::putAt(int idx, Statement* s) {
    if (idx >= (int)svec.size())
        svec.resize(idx+1, NULL);
    svec[idx] = s;
}

Statement* StatementVec::getFirst(StmtVecIter& it) {
    it = svec.begin();
    if (it == svec.end())
        // No elements
        return NULL;
    return *it;         // Else return the first element
}

Statement* StatementVec::getNext(StmtVecIter& it) {
    if (++it == svec.end())
        // No more elements
        return NULL;
    return *it;         // Else return the next element
}

Statement* StatementVec::getLast(StmtVecRevIter& it) {
    it = svec.rbegin();
    if (it == svec.rend())
        // No elements
        return NULL;
    return *it;         // Else return the last element
}

Statement* StatementVec::getPrev(StmtVecRevIter& it) {
    if (++it == svec.rend())
        // No more elements
        return NULL;
    return *it;         // Else return the previous element
}

char* StatementVec::prints() {
    std::ostringstream ost;
    StmtVecIter it;
    for (it = svec.begin(); it != svec.end(); it++) {
        ost << *it << ",\t";
    }
    strncpy(debug_buffer, ost.str().c_str(), 199);
    debug_buffer[199] = '\0';
    return debug_buffer;
}

// Print just the numbers to stream os
void StatementVec::printNums(std::ostream& os) {
    StmtVecIter it;
    os << std::dec;
    for (it = svec.begin(); it != svec.end(); ) {
        if (*it)
            (*it)->printNum(os);
        else
            os << "0";              // Special case for no definition
        if (++it != svec.end())
            os << " ";
    }
}




char* Statement::prints() {
    std::ostringstream ost;
    print(ost, true);
    strncpy(debug_buffer, ost.str().c_str(), 199);
    debug_buffer[199] = '\0';
    return debug_buffer;
}

// exclude: a set of statements not to propagate from
void Statement::propagateTo(int memDepth, StatementSet& exclude) {
    bool change;
    int changes = 0;
    // Repeat substituting into s while there is a single reference
    // component in this statement
    do {
        LocationSet exps;
        addUsedLocs(exps);
        LocSetIter ll;
        change = false;
        for (Exp* e = exps.getFirst(ll); e; e = exps.getNext(ll)) {
            if (!e->getNumRefs() == 1) continue;
            // Can propagate TO this (if memory depths are suitable)
            Statement* def;
            def = ((RefExp*)e)->getRef();
            if (def == NULL)
                // Can't propagate statement "0"
                continue;
            if (def == this)
                // Don't propagate to self! Can happen with %pc's
                continue;
            if (def->isNullStatement())
                // Don't propagate a null statement! Can happen with %pc's
                // (this would have no effect, and would infinitely loop)
                continue;
            // Don't propagate from statements in the exclude list
            if (exclude.exists(def)) continue;
            if (def->isPhi())
                // Don't propagate phi statements!
                continue;
            if (def->isCall())
                continue;
            if (def->isBool())
                continue;
            change = doPropagateTo(memDepth, def);
        }
    } while (change && ++changes < 20);
}

bool Statement::doPropagateTo(int memDepth, Statement* def) {
    // Check the depth of the definition (an assignment)
    // This checks the depth for the left and right sides, and
    // gives the max for both. Example: can't propagate
    // tmp := m[x] to foo := tmp if memDepth == 0
    Assign* assignDef = dynamic_cast<Assign*>(def);
    if (assignDef) {
        int depth = assignDef->getMemDepth();
        if (depth > memDepth)
            return false;
    }

    // Respect the -p N switch
    if (Boomerang::get()->numToPropagate >= 0) {
        if (Boomerang::get()->numToPropagate == 0) return false;
            Boomerang::get()->numToPropagate--;
    }

    replaceRef(def);
    if (VERBOSE) {
        std::cerr << "Propagating " << std::dec << def->getNumber() <<
          " into " << getNumber() <<
          ", result is " << this << "\n";
    }
    return true;
}

bool Statement::operator==(Statement& other) {
    // When do we need this?
    assert(0);
#if 0
    Assign* ae1 = dynamic_cast<Assign*>(this);
    Assign* ae2 = dynamic_cast<Assign*>(&other);
    assert(ae1);
    assert(ae2);
    return *ae1 == *ae2;
#else
    return false;
#endif
}

bool Statement::isNullStatement() {
    if (kind != STMT_ASSIGN) return false;
    Exp* right = ((Assign*)this)->getRight();
    if (right->isSubscript()) {
        // Must refer to self to be null
        return this == ((RefExp*)right)->getRef();
    }
    else
        // Null if left == right
        return *((Assign*)this)->getLeft() == *right;
}

bool Statement::deserialize_fid(std::istream&, int) {
    // TODO: complete!
    return false;
}

bool Statement::isFpush() {
    if (kind != STMT_ASSIGN) return false;
    return ((Assign*)this)->getRight()->getOper() == opFpush;
}
bool Statement::isFpop() {
    if (kind != STMT_ASSIGN) return false;
    return ((Assign*)this)->getRight()->getOper() == opFpop;
}


#if 0
/*
 *  *   *   *   *   *
 *   Class Expand   *
 *  *   *   *   *   *
 */
void Expand::process(Statement* orig, std::string parentString,
  StatementSet& seenSet)  {
    this->orig = orig;
    this->parentString = parentString;
    seen = seenSet;
    seen.insert(orig);         // We've seen self
    LocationSet defs;
    orig->addUsedLocs(defs);
    // Need to find those coponents that are memofs
    LocSetIter ll;
    // First find all the memory components
    bool found = false;
    RefsExp* re;
    Exp* d;
    for (d = defs.getFirst(ll); d; d = defs.getNext(ll)) {
        if (d->isSubscript())
            d = ((RefsExp*)d)->getSubExp1();
        if (d->isMemOf()) {
            Exp* sub = ((Unary*)d)->getSubExp1();
            LocationSet mrefs;
            sub->addUsedLocs(mrefs);
            LocSetIter mri;
            for (Exp* mr = mrefs.getFirst(mri); mr; mr = mrefs.getNext(mri)) {
                if (mr->getNumRefs() > 1) {
                    found = true;
                    re = (RefsExp*)mr;
                    break;
                }
            }
            break;
        }
    }
    if (!found) return;
    StmtSetIter rr;
    for (Statement* s = re->getFirstRef(rr); s; s = re->getNextRef(rr)) {
        // Replace the multiple ref with just one
        Statement* c = orig->clone();
        RefsExp* to = new RefsExp(re->getSubExp1(), s);
        c->searchAndReplace(re, to);
        stmts.append(c);
    }
    // Now substitute where possible
    StmtListIter ss;
    for (Statement* s = stmts.getFirst(ss); s; s = stmts.getNext(ss)) {
        s->propagateTo(0, seen);
    }
    
}

Expand::~Expand() {
    StmtListIter ll;
    for (Statement* s = stmts.getFirst(ll); s; s = stmts.getNext(ll)) {
        delete s;
    }
}

void Expand::print(std::ostream& ost) {
    char c = 'a';
    StmtListIter ll;
    for (Statement* s = stmts.getFirst(ll); s; s = stmts.getNext(ll), c++) {
        ost << orig->getNumber() << parentString << c << " " << s << "\n";
    }
}
#endif




/*
 * This code was in hrtl.cpp
 * Classes derived from Statement
 */


#if defined(_MSC_VER) && _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif 

/******************************************************************************
 * GotoStatement methods
 *****************************************************************************/

/*==============================================================================
 * FUNCTION:        GotoStatement::GotoStatement
 * OVERVIEW:        Constructor.
 * PARAMETERS:      listStmt: a list of Statements (not the same as an RTL)
 *                    to serve as the initial list of statements
 * RETURNS:         N/a
 *============================================================================*/
GotoStatement::GotoStatement()
  : pDest(NULL), m_isComputed(false) {
    kind = STMT_GOTO;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::GotoStatement
 * OVERVIEW:        Construct a jump to a fixed address
 * PARAMETERS:      uDest: native address of destination
 * RETURNS:         N/a
 *============================================================================*/
GotoStatement::GotoStatement(ADDRESS uDest) : m_isComputed(false) {
    kind = STMT_GOTO;
    pDest = new Const(uDest);
}

/*==============================================================================
 * FUNCTION:        GotoStatement::~GotoStatement
 * OVERVIEW:        Destructor
 * PARAMETERS:      None
 * RETURNS:         N/a
 *============================================================================*/
GotoStatement::~GotoStatement() {
    if (pDest) delete pDest;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::getFixedDest
 * OVERVIEW:        Get the fixed destination of this CTI. Assumes destination
 *                  simplication has already been done so that a fixed dest will
 *                  be of the Exp form:
 *                     opIntConst dest
 * PARAMETERS:      <none>
 * RETURNS:         Fixed dest or NO_ADDRESS if there isn't one
 *============================================================================*/
ADDRESS GotoStatement::getFixedDest() {
    if (pDest->getOper() != opIntConst) return NO_ADDRESS;
    return ((Const*)pDest)->getAddr();
}

/*==============================================================================
 * FUNCTION:        GotoStatement::setDest
 * OVERVIEW:        Set the destination of this jump to be a given expression.
 * PARAMETERS:      addr - the new fixed address
 * RETURNS:         Nothing
 *============================================================================*/
void GotoStatement::setDest(Exp* pd) {
    if (pDest != NULL)
        delete pDest;
    pDest = pd;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::setDest
 * OVERVIEW:        Set the destination of this jump to be a given fixed address.
 * PARAMETERS:      addr - the new fixed address
 * RETURNS:         <nothing>
 *============================================================================*/
void GotoStatement::setDest(ADDRESS addr) {
// This fails in FrontSparcTest, do you really want it to Mike? -trent
//  assert(addr >= prog.limitTextLow && addr < prog.limitTextHigh);
    // Delete the old destination if there is one
    if (pDest != NULL)
        delete pDest;

    pDest = new Const(addr);
}

/*==============================================================================
 * FUNCTION:        GotoStatement::getDest
 * OVERVIEW:        Returns the destination of this CTI.
 * PARAMETERS:      None
 * RETURNS:         Pointer to the SS representing the dest of this jump
 *============================================================================*/
Exp* GotoStatement::getDest() {
    return pDest;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::adjustFixedDest
 * OVERVIEW:        Adjust the destination of this CTI by a given amount. Causes
 *                  an error is this destination is not a fixed destination
 *                  (i.e. a constant offset).
 * PARAMETERS:      delta - the amount to add to the destination (can be
 *                  negative)
 * RETURNS:         <nothing>
 *============================================================================*/
void GotoStatement::adjustFixedDest(int delta) {
    // Ensure that the destination is fixed.
    if (pDest == 0 || pDest->getOper() != opIntConst)
        std::cerr << "Can't adjust destination of non-static CTI\n";

    ADDRESS dest = ((Const*)pDest)->getAddr();
    ((Const*)pDest)->setAddr(dest + delta);
}

/*==============================================================================
 * FUNCTION:        GotoStatement::searchAndReplace
 * OVERVIEW:        Replace all instances of search with replace.
 * PARAMETERS:      search - a location to search for
 *                  replace - the expression with which to replace it
 * RETURNS:         True if any change
 *============================================================================*/
bool GotoStatement::searchAndReplace(Exp* search, Exp* replace) {
    bool change = false;
    if (pDest) {
        pDest->searchReplaceAll(search, replace, change);
    }
    return change;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::searchAll
 * OVERVIEW:        Find all instances of the search expression
 * PARAMETERS:      search - a location to search for
 *                  result - a list which will have any matching exprs
 *                           appended to it
 * RETURNS:         true if there were any matches
 *============================================================================*/
bool GotoStatement::searchAll(Exp* search, std::list<Exp*> &result) {
    if (pDest)  return pDest->searchAll(search, result);
    return false;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::print
 * OVERVIEW:        Display a text reprentation of this RTL to the given stream
 * NOTE:            Usually called from RTL::print, in which case the first 9
 *                    chars of the print have already been output to os
 * PARAMETERS:      os: stream to write to
 * RETURNS:         Nothing
 *============================================================================*/
void GotoStatement::print(std::ostream& os /*= cout*/, bool withDF) {
    os << std::setw(4) << std::dec << number << " ";
    os << "GOTO ";
    if (pDest == NULL)
        os << "*no dest*";
    else if (pDest->getOper() != opIntConst)
         pDest->print(os);
    else
        os << "0x" << std::hex << getFixedDest();
}

/*==============================================================================
 * FUNCTION:      GotoStatement::setIsComputed
 * OVERVIEW:      Sets the fact that this call is computed.
 * NOTE:          This should really be removed, once CaseStatement and
 *                  HLNwayCall are implemented properly
 * PARAMETERS:    <none>
 * RETURNS:       <nothing>
 *============================================================================*/
void GotoStatement::setIsComputed(bool b) {
    m_isComputed = b;
}

/*==============================================================================
 * FUNCTION:      GotoStatement::isComputed
 * OVERVIEW:      Returns whether or not this call is computed.
 * NOTE:          This should really be removed, once CaseStatement and HLNwayCall
 *                  are implemented properly
 * PARAMETERS:    <none>
 * RETURNS:       this call is computed
 *============================================================================*/
bool GotoStatement::isComputed() {
    return m_isComputed;
}

/*==============================================================================
 * FUNCTION:        GotoStatement::clone
 * OVERVIEW:        Deep copy clone
 * PARAMETERS:      <none>
 * RETURNS:         Pointer to a new Statement, a clone of this GotoStatement
 *============================================================================*/
Statement* GotoStatement::clone() {
    GotoStatement* ret = new GotoStatement();
    ret->pDest = pDest->clone();
    ret->m_isComputed = m_isComputed;
    return ret;
}

// visit this Statement in the RTL
bool GotoStatement::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

// serialize this rtl
bool GotoStatement::serialize_rest(std::ostream &ouf) {
    if (pDest && pDest->getOper() == opIntConst) {
        saveFID(ouf, FID_RTL_FIXDEST);
        saveValue(ouf, ((Const *)pDest)->getAddr());
    } else if (pDest) {
        saveFID(ouf, FID_RTL_JDEST);
        int l;
        pDest->serialize(ouf, l);
    }

    return true;
}

// deserialize an rtl
bool GotoStatement::deserialize_fid(std::istream &inf, int fid) {
    switch (fid) {
        case FID_RTL_FIXDEST:
            {
                ADDRESS a;
                loadValue(inf, a);
                pDest = new Const(a);
            }
            break;
        case FID_RTL_JDEST:
            {
                pDest = Exp::deserialize(inf);
                if (pDest->getOper() != opIntConst)
                    m_isComputed = true;
                else
                    m_isComputed = false;
            }
            break;
        default:
            return Statement::deserialize_fid(inf, fid);
    }

    return true;
}

void GotoStatement::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
    // dont generate any code for jumps, they will be handled by the BB
}

void GotoStatement::simplify() {
    if (isComputed()) {
        pDest = pDest->simplifyArith();
        pDest = pDest->simplify();
    }
}

/**********************************
 * BranchStatement methods
 **********************************/

/*==============================================================================
 * FUNCTION:        BranchStatement::BranchStatement
 * OVERVIEW:        Constructor.
 * PARAMETERS:      None
 * RETURNS:         N/a
 *============================================================================*/
BranchStatement::BranchStatement() : jtCond((BRANCH_TYPE)0), pCond(NULL),
  bFloat(false) {
    kind = STMT_BRANCH;
}

/*==============================================================================
 * FUNCTION:        BranchStatement::~BranchStatement
 * OVERVIEW:        Destructor
 * PARAMETERS:      None
 * RETURNS:         N/a
 *============================================================================*/
BranchStatement::~BranchStatement() {
    if (pCond)
        delete pCond;
}

/*==============================================================================
 * FUNCTION:        BranchStatement::setCondType
 * OVERVIEW:        Sets the BRANCH_TYPE of this jcond as well as the flag
 *                  indicating whether or not the floating point condition codes
 *                  are used.
 * PARAMETERS:      cond - the BRANCH_TYPE
 *                  usesFloat - this condional jump checks the floating point
 *                    condition codes
 * RETURNS:         a semantic string
 *============================================================================*/
void BranchStatement::setCondType(BRANCH_TYPE cond, bool usesFloat /*= false*/) {
    jtCond = cond;
    bFloat = usesFloat;

    if (bFloat) return;

    // set pCond to a high level representation of this type
    Exp* p = NULL;
#if 0
    switch(cond) {
        case BRANCH_JE:
            p = new Terminal(opZF);
            break;
        case BRANCH_JNE:
            p = new Unary(opNot, new Terminal(opZF));
            break;
        case BRANCH_JSL:
            // N xor V
            p = new Binary(opNotEqual, new Terminal(opNF), new Terminal(opOF));
            break;
        case BRANCH_JSLE:
            // Z or (N xor V)
            p = new Binary(opOr,
                new Terminal(opZF),
                new Binary(opNotEqual, new Terminal(opNF), new Terminal(opOF)));
            break;
        case BRANCH_JSGE:
            // not (N xor V) same as (N == V)
            p = new Binary(opEquals, new Terminal(opNF), new Terminal(opOF));
            break;
        case BRANCH_JSG:
            // not (Z or (N xor V))
            p = new Unary(opNot,
                new Binary(opOr,
                    new Terminal(opZF),
                    new Binary(opNotEqual,
                        new Terminal(opNF), new Terminal(opOF))));
            break;
        case BRANCH_JUL:
            // C
            p = new Terminal(opCF);
            break;
        case BRANCH_JULE:
            // C or Z
            p = new Binary(opOr, new Terminal(opCF), 
                                 new Terminal(opZF));
            break;
        case BRANCH_JUGE:
            // not C
            p = new Unary(opNot, new Terminal(opCF));
            break;
        case BRANCH_JUG:
            // not (C or Z)
            p = new Unary(opNot,
                new Binary(opOr,
                    new Terminal(opCF),
                    new Terminal(opZF)));
            break;
        case BRANCH_JMI:
            // N
            p = new Terminal(opNF);
            break;
        case BRANCH_JPOS:
            // not N
            p = new Unary(opNot, new Terminal(opNF));
            break;
        case BRANCH_JOF:
            // V
            p = new Terminal(opOF);
            break;
        case BRANCH_JNOF:
            // not V
            p = new Unary(opNot, new Terminal(opOF));
            break;
        case BRANCH_JPAR:
            // Can't handle (could happen as a result of a failure of Pentium
            // floating point analysis)
            assert(false);
            break;
    }
#else
    p = new Terminal(opFlags);
#endif
    assert(p);
    setCondExpr(p);
}

/*==============================================================================
 * FUNCTION:        BranchStatement::makeSigned
 * OVERVIEW:        Change this from an unsigned to a signed branch
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void BranchStatement::makeSigned() {
    // Make this into a signed branch
    switch (jtCond)
    {
        case BRANCH_JUL : jtCond = BRANCH_JSL;  break;
        case BRANCH_JULE: jtCond = BRANCH_JSLE; break;
        case BRANCH_JUGE: jtCond = BRANCH_JSGE; break;
        case BRANCH_JUG : jtCond = BRANCH_JSG;  break;
        default:
            // Do nothing for other cases
            break;
    }
}

/*==============================================================================
 * FUNCTION:        BranchStatement::getCondExpr
 * OVERVIEW:        Return the SemStr expression containing the HL condition.
 * PARAMETERS:      <none>
 * RETURNS:         ptr to an expression
 *============================================================================*/
Exp* BranchStatement::getCondExpr() {
    return pCond;
}

/*==============================================================================
 * FUNCTION:        BranchStatement::setCondExpr
 * OVERVIEW:        Set the SemStr expression containing the HL condition.
 * PARAMETERS:      Pointer to Exp to set
 * RETURNS:         <nothing>
 *============================================================================*/
void BranchStatement::setCondExpr(Exp* e) {
    if (pCond) delete pCond;
    pCond = e;
}

bool BranchStatement::search(Exp* search, Exp*& result) {
    if (pCond) return pCond->search(search, result);
    result = NULL;
    return false;
}

/*==============================================================================
 * FUNCTION:        BranchStatement::searchAndReplace
 * OVERVIEW:        Replace all instances of search with replace.
 * PARAMETERS:      search - a location to search for
 *                  replace - the expression with which to replace it
 * RETURNS:         True if any change
 *============================================================================*/
bool BranchStatement::searchAndReplace(Exp* search, Exp* replace) {
    GotoStatement::searchAndReplace(search, replace);
    bool change = false;
    if (pCond)
        pCond = pCond->searchReplaceAll(search, replace, change);
    return change;
}

// update type for expression
Type *BranchStatement::updateType(Exp *e, Type *curType) {
    if (jtCond == BRANCH_JUGE || jtCond == BRANCH_JULE ||
        jtCond == BRANCH_JUG || jtCond == BRANCH_JUL && 
        curType->isInteger()) {
        ((IntegerType*)curType)->setSigned(false);
    }
    return curType;
}

// Convert from SSA form
void BranchStatement::fromSSAform(igraph& ig) {
   pCond = pCond->fromSSA(ig); 
}

/*==============================================================================
 * FUNCTION:        BranchStatement::searchAll
 * OVERVIEW:        Find all instances of the search expression
 * PARAMETERS:      search - a location to search for
 *                  result - a list which will have any matching exprs
 *                           appended to it
 * RETURNS:         true if there were any matches
 *============================================================================*/
bool BranchStatement::searchAll(Exp* search, std::list<Exp*> &result) {
    if (pCond) return pCond->searchAll(search, result);
    return false;
}


/*==============================================================================
 * FUNCTION:        BranchStatement::print
 * OVERVIEW:        Write a text representation to the given stream
 * PARAMETERS:      os: stream
 * RETURNS:         Nothing
 *============================================================================*/
void BranchStatement::print(std::ostream& os /*= cout*/, bool withDF) {
    os << std::setw(4) << std::dec << number << " ";
    os << "BRANCH ";
    if (pDest == NULL)
        os << "*no dest*";
    else if (!pDest->isIntConst())
        os << pDest;
    else {
        // Really we'd like to display the destination label here...
        os << "0x" << std::hex << getFixedDest();
    }
    os << ", condition ";
    switch (jtCond) {
        case BRANCH_JE:    os << "equals"; break;
        case BRANCH_JNE:   os << "not equals"; break;
        case BRANCH_JSL:   os << "signed less"; break;
        case BRANCH_JSLE:  os << "signed less or equals"; break;
        case BRANCH_JSGE:  os << "signed greater or equals"; break;
        case BRANCH_JSG:   os << "signed greater"; break;
        case BRANCH_JUL:   os << "unsigned less"; break;
        case BRANCH_JULE:  os << "unsigned less or equals"; break;
        case BRANCH_JUGE:  os << "unsigned greater or equals"; break;
        case BRANCH_JUG:   os << "unsigned greater"; break;
        case BRANCH_JMI:   os << "minus"; break;
        case BRANCH_JPOS:  os << "plus"; break;
        case BRANCH_JOF:   os << "overflow"; break;
        case BRANCH_JNOF:  os << "no overflow"; break;
        case BRANCH_JPAR:  os << "parity"; break;
    }
    if (bFloat) os << " float";
    os << std::endl;
    if (pCond) {
        os << "High level: " << pCond;
    }
}

/*==============================================================================
 * FUNCTION:        BranchStatement::clone
 * OVERVIEW:        Deep copy clone
 * PARAMETERS:      <none>
 * RETURNS:         Pointer to a new Statement, a clone of this BranchStatement
 *============================================================================*/
Statement* BranchStatement::clone() {
    BranchStatement* ret = new BranchStatement();
    ret->pDest = pDest->clone();
    ret->m_isComputed = m_isComputed;
    ret->jtCond = jtCond;
    if (pCond) ret->pCond = pCond->clone();
    else ret->pCond = NULL;
    ret->m_isComputed = m_isComputed;
    ret->bFloat = bFloat;
    // Statement members
    ret->pbb = pbb;
    ret->proc = proc;
    ret->number = number;
    return ret;
}

// visit this rtl
bool BranchStatement::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

// serialize this rtl
bool BranchStatement::serialize_rest(std::ostream &ouf) {
    GotoStatement::serialize_rest(ouf);

    saveFID(ouf, FID_RTL_JCONDTYPE);
    saveValue(ouf, (char)jtCond);

    saveFID(ouf, FID_RTL_USESFLOATCC);
    saveValue(ouf, bFloat);

    if (pCond) {
        saveFID(ouf, FID_RTL_JCOND);
        int l;
        pCond->serialize(ouf, l);
    }

    return true;
}

// deserialize an rtl
bool BranchStatement::deserialize_fid(std::istream &inf, int fid) {
    char ch;

    switch (fid) {
        case FID_RTL_JCONDTYPE:             
            loadValue(inf, ch);
            jtCond = (BRANCH_TYPE)ch;
            break;
        case FID_RTL_USESFLOATCC:
            loadValue(inf, bFloat);
            break;
        case FID_RTL_JCOND:
            pCond = Exp::deserialize(inf);
            break;
        default:
            return GotoStatement::deserialize_fid(inf, fid);
    }

    return true;
}

void BranchStatement::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
    // dont generate any code for jconds, they will be handled by the bb
}

bool BranchStatement::usesExp(Exp *e) {
    Exp *tmp;
    return pCond && pCond->search(e, tmp);
}

// process any constants in the statement
void BranchStatement::processConstants(Prog *prog) {
}

void BranchStatement::doReplaceRef(Exp* from, Exp* to) {
    bool change;
    assert(pCond);
    pCond = pCond->searchReplaceAll(from, to, change);
    simplify();
}

// Common to BranchStatement and BoolStatement
void condToRelational(Exp*& pCond, BRANCH_TYPE jtCond) {
    pCond = pCond->simplifyArith()->simplify();

    std::stringstream os;
    pCond->print(os);
    std::string s = os.str();

    if (pCond->getOper() == opFlagCall && 
        !strncmp(((Const*)pCond->getSubExp1())->getStr(), 
                "SUBFLAGS", 8)) {
        Exp *e = pCond;
        OPER op = opWild;
        switch (jtCond) {
            case BRANCH_JE:    op = opEquals; break;
            case BRANCH_JNE:   op = opNotEqual; break;
            case BRANCH_JSL:   op = opLess; break;
            case BRANCH_JSLE:  op = opLessEq; break;
            case BRANCH_JSGE:  op = opGtrEq; break;
            case BRANCH_JSG:   op = opGtr; break;
            case BRANCH_JUL:   op = opLessUns; break;
            case BRANCH_JULE:  op = opLessEqUns; break;
            case BRANCH_JUGE:  op = opGtrEqUns; break;
            case BRANCH_JUG:   op = opGtrUns; break;
            case BRANCH_JMI:
                pCond = new Binary(opLess,
                    pCond->getSubExp2()->getSubExp2()->getSubExp2()
                        ->getSubExp1()->clone(), new Const(0));
                delete e;
                break;
            case BRANCH_JPOS:
                pCond = new Binary(opGtrEq,
                    pCond->getSubExp2()->getSubExp2()->getSubExp2()
                        ->getSubExp1()->clone(), new Const(0));
                delete e;
                break;
            case BRANCH_JOF:
            case BRANCH_JNOF:
            case BRANCH_JPAR:
                break;
        }
        if (op != opWild) {
            pCond = new Binary(op,
                pCond->getSubExp2()->getSubExp1()->clone(), 
                pCond->getSubExp2()->getSubExp2()->getSubExp1()
                    ->clone());
            delete e;
        }
    }
    if (pCond->getOper() == opFlagCall && 
        !strncmp(((Const*)pCond->getSubExp1())->getStr(), 
                "LOGICALFLAGS", 12)) {
        Exp *e = pCond;
        switch (jtCond) {
            case BRANCH_JE:
                pCond = new Binary(opEquals,
                    pCond->getSubExp2()->getSubExp1()->clone(), 
                    new Const(0));
                break;
            case BRANCH_JNE:
                pCond = new Binary(opNotEqual,
                    pCond->getSubExp2()->getSubExp1()->clone(), 
                    new Const(0));
                break;
            case BRANCH_JMI:
                pCond = new Binary(opLess,
                    pCond->getSubExp2()->getSubExp1()->clone(), 
                    new Const(0));
                delete e;
                break;
            case BRANCH_JPOS:
                pCond = new Binary(opGtrEq,
                    pCond->getSubExp2()->getSubExp1()->clone(), 
                    new Const(0));
                delete e;
                break;
            default:
                break;
        }
    }
}


void BranchStatement::simplify() {
    if (pCond)
        condToRelational(pCond, jtCond);
}

void BranchStatement::addUsedLocs(LocationSet& used) {
    if (pCond)
        pCond->addUsedLocs(used);
}

void BranchStatement::fixCallRefs() {
    if (pCond)
        pCond = pCond->fixCallRefs();
}

void BranchStatement::subscriptVar(Exp* e, Statement* def) {
    if (pCond)
        pCond = pCond->expSubscriptVar(e, def);
}

/**********************************
 * CaseStatement methods
 **********************************/
/*==============================================================================
 * FUNCTION:        CaseStatement::CaseStatement
 * OVERVIEW:        Constructor.
 * PARAMETERS:      None
 * RETURNS:         N/a
 *============================================================================*/
CaseStatement::CaseStatement() :
    pSwitchInfo(NULL) {
    kind = STMT_CASE;
}

/*==============================================================================
 * FUNCTION:        CaseStatement::~CaseStatement
 * OVERVIEW:        Destructor
 * NOTE:            Don't delete the pSwitchVar; it's always a copy of something
 *                  else (so don't delete twice)
 * PARAMETERS:      None
 * RETURNS:         N/a
 *============================================================================*/
CaseStatement::~CaseStatement() {
    if (pSwitchInfo)
        delete pSwitchInfo;
}

/*==============================================================================
 * FUNCTION:        CaseStatement::getSwitchInfo
 * OVERVIEW:        Return a pointer to a struct with switch information in it
 * PARAMETERS:      <none>
 * RETURNS:         a semantic string
 *============================================================================*/
SWITCH_INFO* CaseStatement::getSwitchInfo() {
    return pSwitchInfo;
}

/*==============================================================================
 * FUNCTION:        CaseStatement::setSwitchInfo
 * OVERVIEW:        Set a pointer to a SWITCH_INFO struct
 * PARAMETERS:      Pointer to SWITCH_INFO struct
 * RETURNS:         <nothing>
 *============================================================================*/
void CaseStatement::setSwitchInfo(SWITCH_INFO* psi) {
    pSwitchInfo = psi;
}

/*==============================================================================
 * FUNCTION:        CaseStatement::searchAndReplace
 * OVERVIEW:        Replace all instances of search with replace.
 * PARAMETERS:      search - a location to search for
 *                  replace - the expression with which to replace it
 * RETURNS:         True if any change
 *============================================================================*/
bool CaseStatement::searchAndReplace(Exp* search, Exp* replace) {
    GotoStatement::searchAndReplace(search, replace);
    bool ch = false;
    if (pSwitchInfo && pSwitchInfo->pSwitchVar)
        pSwitchInfo->pSwitchVar->searchReplaceAll(search, replace, ch);
    return ch;
}

/*==============================================================================
 * FUNCTION:        CaseStatement::searchAll
 * OVERVIEW:        Find all instances of the search expression
 * PARAMETERS:      search - a location to search for
 *                  result - a list which will have any matching exprs
 *                           appended to it
 * NOTES:           search can't easily be made const
 * RETURNS:         true if there were any matches
 *============================================================================*/
bool CaseStatement::searchAll(Exp* search, std::list<Exp*> &result) {
    return GotoStatement::searchAll(search, result) ||
        ( pSwitchInfo && pSwitchInfo->pSwitchVar &&
          pSwitchInfo->pSwitchVar->searchAll(search, result) );
}

/*==============================================================================
 * FUNCTION:        CaseStatement::print
 * OVERVIEW:        Write a text representation to the given stream
 * PARAMETERS:      os: stream
 *                  indent: number of columns to skip
 * RETURNS:         Nothing
 *============================================================================*/
void CaseStatement::print(std::ostream& os /*= cout*/, bool withDF) {
    os << std::setw(4) << std::dec << number << " ";
    os << "CASE [";
    if (pDest == NULL)
        os << "*no dest*";
    else os << pDest;
    os << "] ";
    if (pSwitchInfo)
        os << "Switch variable: " << pSwitchInfo->pSwitchVar << std::endl;
}


/*==============================================================================
 * FUNCTION:        CaseStatement::clone
 * OVERVIEW:        Deep copy clone
 * PARAMETERS:      <none>
 * RETURNS:         Pointer to a new Statement that is a clone of this one
 *============================================================================*/
Statement* CaseStatement::clone() {
    CaseStatement* ret = new CaseStatement();
    ret->pDest = pDest->clone();
    ret->m_isComputed = m_isComputed;
    ret->pSwitchInfo = new SWITCH_INFO;
    *ret->pSwitchInfo = *pSwitchInfo;
    ret->pSwitchInfo->pSwitchVar = pSwitchInfo->pSwitchVar->clone();
    return ret;
}

// visit this rtl
bool CaseStatement::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

// serialize this rtl
bool CaseStatement::serialize_rest(std::ostream &ouf) {
    return true;
}

// deserialize an rtl
bool CaseStatement::deserialize_fid(std::istream &inf, int fid) {
    switch (fid) {
        default:
            return Statement::deserialize_fid(inf, fid);
    }

    return true;
}

void CaseStatement::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
    // dont generate any code for switches, they will be handled by the bb
}

void CaseStatement::simplify() {
    // TODO
}

/**********************************
 *      CallStatement methods
 **********************************/

/*============================================================================== * FUNCTION:         CallStatement::CallStatement
 * OVERVIEW:         Constructor for a call that we have extra information
 *                   for and is part of a prologue.
 * PARAMETERS:       returnTypeSize - the size of a return union, struct or quad
 *                     floating point value
 * RETURNS:          <nothing>
 *============================================================================*/
CallStatement::CallStatement(int returnTypeSize /*= 0*/):  
      returnTypeSize(returnTypeSize), returnAfterCall(false) {
    kind = STMT_CALL;
    procDest = NULL;
}

/*==============================================================================
 * FUNCTION:      CallStatement::~CallStatement
 * OVERVIEW:      Destructor
 * PARAMETERS:    BB - the enclosing basic block of this call
 * RETURNS:       <nothing>
 *============================================================================*/
CallStatement::~CallStatement() {
    for (unsigned i = 0; i < arguments.size(); i++)
        delete arguments[i];
    for (unsigned i = 0; i < returns.size(); i++)
        delete returns[i];
}

/*==============================================================================
 * FUNCTION:      CallStatement::getArguments
 * OVERVIEW:      Return a copy of the locations that have been determined
 *                as the actual arguments for this call.
 * PARAMETERS:    <none>
 * RETURNS:       A reference to the list of arguments
 *============================================================================*/
std::vector<Exp*>& CallStatement::getArguments() {
    return arguments;
}

int CallStatement::getNumReturns() {
    return returns.size();
}

Exp *CallStatement::getReturnExp(int i) {
    return returns[i];
}

int CallStatement::findReturn(Exp *e) {
    for (unsigned i = 0; i < returns.size(); i++)
        if (*returns[i] == *e)
            return i;
    return -1;
}

void CallStatement::removeReturn(Exp *e)
{
    int i = findReturn(e);
    if (i != -1) {
        for (unsigned j = i+1; j < returns.size(); j++)
            returns[j-1] = returns[j];
        returns.resize(returns.size()-1);
    }
}

Exp *CallStatement::getProven(Exp *e) {
    assert(procDest);
    return procDest->getProven(e);
}

Exp *CallStatement::substituteParams(Exp *e)
{
    e = e->clone();
    LocationSet locs;
    e->addUsedLocs(locs);
    LocSetIter xx;
    for (Exp* x = locs.getFirst(xx); x; x = locs.getNext(xx)) {
        Exp *r = findArgument(x);
        if (r == NULL) continue;
        bool change;
        e = e->searchReplaceAll(x, r->clone(), change);
    }
    return e->simplify();
}

Exp *CallStatement::findArgument(Exp *e) {
    assert(procDest);
    int n = procDest->getSignature()->findParam(e);
    if (n == -1) return NULL;
    return arguments[n];
}

void CallStatement::addArgument(Exp *e)
{
    e = substituteParams(e);
    arguments.push_back(e);
}

Type *CallStatement::getArgumentType(int i) {
    assert(i < (int)arguments.size());
    assert(procDest);
    return procDest->getSignature()->getParamType(i);
}

/*==============================================================================
 * FUNCTION:      CallStatement::setArguments
 * OVERVIEW:      Set the arguments of this call.
 * PARAMETERS:    arguments - the list of locations that reach this call
 * RETURNS:       <nothing>
 *============================================================================*/
void CallStatement::setArguments(std::vector<Exp*>& arguments) {
    this->arguments = arguments;
}

/*==============================================================================
 * FUNCTION:      CallStatement::setSigArguments
 * OVERVIEW:      Set the arguments of this call based on signature info
 * PARAMETERS:    None
 * RETURNS:       <nothing>
 *============================================================================*/
void CallStatement::setSigArguments() {
    if (procDest == NULL) return;
    int n = procDest->getSignature()->getNumParams();
    arguments.resize(n);
    for (int i = 0; i < n; i++) {
        Exp *e = procDest->getSignature()->getArgumentExp(i);
        assert(e);
        arguments[i] = e->clone();
    }
    if (procDest->getSignature()->hasEllipsis()) {
        // Just guess 4 parameters for now
        for (int i = 0; i < 4; i++)
            arguments.push_back(procDest->getSignature()->
                            getArgumentExp(arguments.size())->clone());
    }
    UserProc *u = dynamic_cast<UserProc*>(procDest);
    if (u)
        u->addCaller(this);

    // initialize returns
    for (int i = 0; i < procDest->getSignature()->getNumReturns(); i++)
        returns.push_back(procDest->getSignature()->getReturnExp(i)->clone());
}

#if 0
/*==============================================================================
 * FUNCTION:      CallStatement::getReturnLoc
 * OVERVIEW:      Return the location that will be used to hold the value
 *                  returned by this call.
 * PARAMETERS:    <none>
 * RETURNS:       ptr to the location that will be used to hold the return value
 *============================================================================*/
Exp* CallStatement::getReturnLoc() {
    if (returns.size() == 1)
        return returns[0];
    return NULL;
}

Type* CallStatement::getLeftType() {
    if (procDest == NULL)
        return new VoidType();
    return procDest->getSignature()->getReturnType();
}
#endif

/*==============================================================================
 * FUNCTION:         CallStatement::returnsStruct
 * OVERVIEW:         Returns true if the function called by this call site
 *                   returns an aggregate value (i.e a struct, union or quad
 *                   floating point value).
 * PARAMETERS:       <none>
 * RETURNS:          the called function returns an aggregate value
 *============================================================================*/
bool CallStatement::returnsStruct() {
    return (returnTypeSize != 0);
}

bool CallStatement::search(Exp* search, Exp*& result) {
    result = NULL;
    for (unsigned i = 0; i < returns.size(); i++) {
        if (*returns[i] == *search) {
            result = returns[i];
            return true;
        }
        if (returns[i]->search(search, result)) return true;
    }
    for (unsigned i = 0; i < arguments.size(); i++) {
        if (*arguments[i] == *search) {
            result = arguments[i];
            return true;
        }
        if (arguments[i]->search(search, result)) return true;
    }
    return false;
}

/*==============================================================================
 * FUNCTION:        CallStatement::searchAndReplace
 * OVERVIEW:        Replace all instances of search with replace.
 * PARAMETERS:      search - a location to search for
 *                  replace - the expression with which to replace it
 * RETURNS:         True if any change
 *============================================================================*/
bool CallStatement::searchAndReplace(Exp* search, Exp* replace) {
    bool change = GotoStatement::searchAndReplace(search, replace);
    for (int i = 0; i < (int)returns.size(); i++) {
        bool ch;
        returns[i] = returns[i]->searchReplaceAll(search, replace, ch);
        change |= ch;
    }
    for (unsigned i = 0; i < arguments.size(); i++) {
        bool ch;
        arguments[i] = arguments[i]->searchReplaceAll(search, replace, ch);
        change |= ch;
    }
    return change;
}

/*==============================================================================
 * FUNCTION:        CallStatement::searchAll
 * OVERVIEW:        Find all instances of the search expression
 * PARAMETERS:      search - a location to search for
 *                  result - a list which will have any matching exprs
 *                           appended to it
 * RETURNS:         true if there were any matches
 *============================================================================*/
bool CallStatement::searchAll(Exp* search, std::list<Exp *>& result) {
    bool found = false;
    for (unsigned i = 0; i < arguments.size(); i++)
        if (arguments[i]->searchAll(search, result))
            found = true;
    for (unsigned i = 0; i < returns.size(); i++)
        if (returns[i]->searchAll(search, result))
            found = true;
    return found;
}

/*==============================================================================
 * FUNCTION:        CallStatement::print
 * OVERVIEW:        Write a text representation of this RTL to the given stream
 * PARAMETERS:      os: stream to write to
 * RETURNS:         Nothing
 *============================================================================*/
void CallStatement::print(std::ostream& os /*= cout*/, bool withDF) {
    os << std::setw(4) << std::dec << number << " ";
 
    os << "CALL ";
    if (procDest)
        os << procDest->getName();
    else if (pDest == NULL)
            os << "*no dest*";
    else {
        if (pDest->isIntConst())
            os << "0x" << std::hex << ((Const*)pDest)->getInt();
        else
            pDest->print(os);       // Could still be an expression
    }

    // Print the actual arguments of the call
    os << "(";    
    for (unsigned i = 0; i < arguments.size(); i++) {
        if (i != 0)
            os << ", ";
        os << arguments[i];
    }
    os << ")";

    // And the return locations 
    if (getNumReturns()) {
        os << " { ";
        for (int i = 0; i < getNumReturns(); i++) {
            if (i != 0)
                os << ", ";
            os << getReturnExp(i);
        }
        os << " }";
    }
}

/*==============================================================================
 * FUNCTION:         CallStatement::setReturnAfterCall
 * OVERVIEW:         Sets a bit that says that this call is effectively followed
 *                      by a return. This happens e.g. on Sparc when there is a
 *                      restore in the delay slot of the call
 * PARAMETERS:       b: true if this is to be set; false to clear the bit
 * RETURNS:          <nothing>
 *============================================================================*/
void CallStatement::setReturnAfterCall(bool b) {
    returnAfterCall = b;
}

/*==============================================================================
 * FUNCTION:         CallStatement::isReturnAfterCall
 * OVERVIEW:         Tests a bit that says that this call is effectively
 *                      followed by a return. This happens e.g. on Sparc when
 *                      there is a restore in the delay slot of the call
 * PARAMETERS:       none
 * RETURNS:          True if this call is effectively followed by a return
 *============================================================================*/
bool CallStatement::isReturnAfterCall() {
    return returnAfterCall;
}

/*==============================================================================
 * FUNCTION:        CallStatement::clone
 * OVERVIEW:        Deep copy clone
 * PARAMETERS:      <none>
 * RETURNS:         Pointer to a new Statement, a clone of this CallStatement
 *============================================================================*/
Statement* CallStatement::clone() {
    CallStatement* ret = new CallStatement();
    ret->pDest = pDest->clone();
    ret->m_isComputed = m_isComputed;
    int n = arguments.size();
    for (int i=0; i < n; i++)
        ret->arguments.push_back(arguments[i]->clone());
    // Statement members
    ret->pbb = pbb;
    ret->proc = proc;
    ret->number = number;
    return ret;
}

// visit this rtl
bool CallStatement::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

// serialize this rtl
bool CallStatement::serialize_rest(std::ostream &ouf) {
    GotoStatement::serialize_rest(ouf);

    if (procDest) {
        saveFID(ouf, FID_RTL_CALLDESTSTR);
        saveString(ouf, std::string(procDest->getName()));
    }

    return true;
}

// deserialize an rtl
bool CallStatement::deserialize_fid(std::istream &inf, int fid) {
    switch (fid) {
        case FID_RTL_CALLDESTSTR:
            loadString(inf, destStr);           
            break;
        default:
            return GotoStatement::deserialize_fid(inf, fid);
    }

    return true;
}

Proc* CallStatement::getDestProc() {
    return procDest; 
}

void CallStatement::setDestProc(Proc* dest) { 
    assert(dest);
    assert(procDest == NULL);
    procDest = dest;
    destStr = procDest->getName();
}

void CallStatement::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
    LocationSet defs;
    getDefinitions(defs);

    Proc *p = getDestProc();

    if (p == NULL && isComputed()) {
        hll->AddIndCallStatement(indLevel, pDest, arguments);
        return;
    }

#if 0
    std::cerr << "call: ";
    print(std::cerr, false);
    std::cerr << "in proc " << proc->getName() << std::endl;
#endif
    assert(p);
    hll->AddCallStatement(indLevel, p, arguments, defs);
}

void CallStatement::simplify() {
    GotoStatement::simplify();
    for (unsigned i = 0; i < arguments.size(); i++) {
        arguments[i] = arguments[i]->simplifyArith()->simplify();
    }
}

#if 0
void CallStatement::decompile() {
    if (procDest) { 
        UserProc *p = dynamic_cast<UserProc*>(procDest);
        if (p != NULL)
            p->decompile();

        // FIXME: Likely there is a much better place to do this
        // init return location
        setIgnoreReturnLoc(false);
    } else {
        // TODO: indirect call
    }
}
#endif

// update type for expression
Type *CallStatement::updateType(Exp *e, Type *curType) {
    return curType;
}

bool CallStatement::usesExp(Exp *e) {
    Exp *where;
    for (unsigned i = 0; i < arguments.size(); i++) {
        if (arguments[i]->search(e, where)) {
            return true;
        }
    }
    for (unsigned int i = 0; i < returns.size(); i++) {
        if (returns[i]->isMemOf() && 
                returns[i]->getSubExp1()->search(e, where))
            return true;
    }
    if (procDest == NULL)
        // No destination (e.g. indirect call)
        // For now, just return true (overstating uses is safe)
        return true;
#if 0   // Huh? This was wrong anyway!
    if (!procDest->isLib()) {
        // Get the info that was summarised on the way down
        if (liveEntry.find(e)) return true;
    }
#endif
    return false;
}

// Add all locations that this call uses
void CallStatement::addUsedLocs(LocationSet& used) {
    for (unsigned i = 0; i < arguments.size(); i++)
        arguments[i]->addUsedLocs(used);
    
    for (unsigned i = 0; i < returns.size(); i++)
        if (returns[i]->isMemOf())
            returns[i]->getSubExp1()->addUsedLocs(used);
}

void CallStatement::fixCallRefs() {
    for (unsigned i = 0; i < arguments.size(); i++)
        arguments[i] = arguments[i]->fixCallRefs();

    for (unsigned i = 0; i < returns.size(); i++)
        if (returns[i]->isMemOf())
            returns[i]->getSubExp1()->fixCallRefs();
}

bool CallStatement::isDefinition() 
{
    LocationSet defs;
    getDefinitions(defs);
    return defs.size() != 0;
}

void CallStatement::getDefinitions(LocationSet &defs) {
    if (procDest) {
        for (int i = 0; i < getNumReturns(); i++)
            defs.insert(getReturnExp(i));
    } else {
        // TODO: computed call
    }
}

void CallStatement::subscriptVar(Exp* e, Statement* def) {
    for (unsigned i = 0; i < arguments.size(); i++) {
        arguments[i] = arguments[i]->expSubscriptVar(e, def);
    }
}

void CallStatement::doReplaceRef(Exp* from, Exp* to) {
    bool change = false;
    for (unsigned i = 0; i < arguments.size(); i++) {
        arguments[i] = arguments[i]->searchReplaceAll(from, to,
          change);
        arguments[i] = arguments[i]->simplifyArith()->simplify();
    }
#if 0       // Call later
    processConstants(proc->getProg());
    if (getDestProc() && getDestProc()->getSignature()->hasEllipsis()) {
        // functions like printf almost always have too many args
        std::string name(getDestProc()->getName());
        if ((name == "printf" || name == "scanf") &&
          getArgumentExp(0)->isStrConst()) {
            char *str = ((Const*)getArgumentExp(0))->getStr();
            // actually have to parse it
            int n = 1;      // Number of %s plus 1 = number of args
            char *p = str;
            while ((p = strchr(p, '%'))) {
                // special hack for scanf
                if (name == "scanf") {
                    setArgumentExp(n, new Unary(opAddrOf, 
                        new Unary(opMemOf, getArgumentExp(n))));
                }
                p++;
                switch(*p) {
                    case '%':
                        break;
                        // TODO: there's type information here
                    default: 
                        n++;
                }
                p++;
            }
            setNumArguments(n);
        }
    }
#endif
}

void CallStatement::setNumArguments(int n) {
    int oldSize = arguments.size();
    arguments.resize(n);
    // printf, scanf start with just 2 arguments
    for (int i = oldSize; i < n; i++) {
        arguments[i] = procDest->getSignature()->getArgumentExp(i)->clone();
    }
}

void CallStatement::removeArgument(int i)
{
    for (unsigned j = i+1; j < arguments.size(); j++)
        arguments[j-1] = arguments[j];
    arguments.resize(arguments.size()-1);
}

// Convert from SSA form
void CallStatement::fromSSAform(igraph& ig) {
    int n = arguments.size();
    for (int i=0; i < n; i++) {
        arguments[i] = arguments[i]->fromSSA(ig);
    }
    n = returns.size();
    for (int i=0; i < n; i++) {
        returns[i] = returns[i]->fromSSAleft(ig, this);
    }
}


// Insert actual arguments to match the formal parameters
// This is called very late, after all propagation
void CallStatement::insertArguments(StatementSet& rs) {
    // FIXME: Fix this, or delete the whole function
#if 0
    if (procDest == NULL) return;
    if (procDest->isLib()) return;
    Signature* sig = procDest->getSignature();
    int num = sig->getNumParams();
    //arguments.resize(num);
    // Get the set of definitions that reach this call
    StatementSet rd;
    getBB()->getReachInAt(this, rd, 2);
    StatementSet empty;
    for (int i=0; i<num; i++) {
        Exp* loc = sig->getArgumentExp(i)->clone();
        // Needs to be subscripted with everything that reaches the parameters
        // FIXME: need to be sensible about memory depths
        loc->updateRefs(rd, 0, rs);
        propagateTo(0, empty);
        loc->updateRefs(rd, 1, rs);
        propagateTo(1, empty);
        arguments.push_back(loc);
    }
#endif
}

void CallStatement::processConstants(Prog *prog) {
    for (unsigned i = 0; i < arguments.size(); i++) {
        Type *t = getArgumentType(i);
        if (t == NULL) continue;
        // char* and a constant
        if (arguments[i]->isIntConst()) {
            if (t->isPointer()) {
                PointerType *pt = (PointerType*)t;
                if (pt->getPointsTo()->isChar()) {
                    char *str = 
                        prog->getStringConstant(((Const*)arguments[i])->getAddr());
                    if (str) {
                        std::string s(str);
                        while (s.find('\n') != (unsigned)-1)
                            s.replace(s.find('\n'), 1, "\\n");
                        delete arguments[i];
                        arguments[i] = new Const(strdup(s.c_str()));
                    }
                }
                if (pt->getPointsTo()->isFunc()) {
                    ADDRESS a = ((Const*)arguments[i])->getAddr();
                    prog->decode(a);
                }
            } else if (t->isFloat()) {
                arguments[i]->setOper(opFltConst);
            }
        }
#if 0
        if (t->isPointer() && arguments[i]->getOper() != opAddrOf) {
            arguments[i] = new Unary(opAddrOf, 
                                     new Unary(opMemOf, arguments[i]));
        }
#endif
    }

    // This code was in CallStatement:doReplaceRef()
    if (getDestProc() && getDestProc()->getSignature()->hasEllipsis()) {
        // functions like printf almost always have too many args
        std::string name(getDestProc()->getName());
        if ((name == "printf" || name == "scanf") &&
          getArgumentExp(0)->isStrConst()) {
            char *str = ((Const*)getArgumentExp(0))->getStr();
            // actually have to parse it
            int n = 1;      // Number of %s plus 1 = number of args
            char *p = str;
            while ((p = strchr(p, '%'))) {
                // special hack for scanf
                if (name == "scanf") {
                    setArgumentExp(n, new Unary(opAddrOf,
                        new Unary(opMemOf, getArgumentExp(n))));
                }
                p++;
                switch(*p) {
                    case '%':
                        break;
                        // TODO: there's type information here
                    default:
                        n++;
                }
                p++;
            }
            setNumArguments(n);
        }
    }

}

/**********************************
 * ReturnStatement methods
 **********************************/

/*==============================================================================
 * FUNCTION:         ReturnStatement::ReturnStatement
 * OVERVIEW:         Constructor.
 * PARAMETERS:       None
 * RETURNS:          <nothing>
 *============================================================================*/
ReturnStatement::ReturnStatement() : nBytesPopped(0), returnVal(NULL) {
    kind = STMT_RET;
}

/*==============================================================================
 * FUNCTION:         ReturnStatement::~ReturnStatement
 * OVERVIEW:         Destructor.
 * PARAMETERS:       <none>
 * RETURNS:          <nothing>
 *============================================================================*/
ReturnStatement::~ReturnStatement() {
    if (returnVal)
        delete returnVal;
}

/*==============================================================================
 * FUNCTION:        ReturnStatement::clone
 * OVERVIEW:        Deep copy clone
 * PARAMETERS:      <none>
 * RETURNS:         Pointer to a new Statement, a clone of this ReturnStatement
 *============================================================================*/
Statement* ReturnStatement::clone() {
    ReturnStatement* ret = new ReturnStatement();
    ret->pDest = NULL;                      // pDest should be null
    ret->m_isComputed = m_isComputed;
    return ret;
}

// visit this rtl
bool ReturnStatement::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

// serialize this rtl
bool ReturnStatement::serialize_rest(std::ostream &ouf) {
    GotoStatement::serialize_rest(ouf);

    return true;
}

// deserialize an rtl
bool ReturnStatement::deserialize_fid(std::istream &inf, int fid) {
    switch (fid) {
        default:
            return GotoStatement::deserialize_fid(inf, fid);
    }

    return true;
}

void ReturnStatement::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
}

void ReturnStatement::simplify() {
    if (returnVal)
        returnVal = returnVal->simplify();
}

void ReturnStatement::setReturnValue(Exp *e) { 
    if (returnVal) delete returnVal;
    returnVal = e; 
}

void ReturnStatement::print(std::ostream& os /*= cout*/, bool withDF) {
    os << std::setw(4) << std::dec << number << " ";
    os << "RET";
}

/**********************************************************************
 * BoolStatement methods
 * These are for statements that set a destination (usually to 1 or 0)
 * depending in a condition code (e.g. Pentium)
 **********************************************************************/

/*==============================================================================
 * FUNCTION:         BoolStatement::BoolStatement
 * OVERVIEW:         Constructor.
 * PARAMETERS:       sz: size of the assignment
 * RETURNS:          <N/a>
 *============================================================================*/
BoolStatement::BoolStatement(int sz): jtCond((BRANCH_TYPE)0), pCond(NULL),
  pDest(NULL), size(sz) {
    kind = STMT_BOOL;
}

/*==============================================================================
 * FUNCTION:        BoolStatement::~BoolStatement
 * OVERVIEW:        Destructor
 * PARAMETERS:      None
 * RETURNS:         N/a
 *============================================================================*/
BoolStatement::~BoolStatement() {
    if (pCond)
        delete pCond;
}

/*==============================================================================
 * FUNCTION:        BoolStatement::setCondType
 * OVERVIEW:        Sets the BRANCH_TYPE of this jcond as well as the flag
 *                  indicating whether or not the floating point condition codes
 *                  are used.
 * PARAMETERS:      cond - the BRANCH_TYPE
 *                  usesFloat - this condional jump checks the floating point
 *                    condition codes
 * RETURNS:         a semantic string
 *============================================================================*/
void BoolStatement::setCondType(BRANCH_TYPE cond, bool usesFloat /*= false*/) {
    jtCond = cond;
    bFloat = usesFloat;
    setCondExpr(new Terminal(opFlags));
    getDest();
}

/*==============================================================================
 * FUNCTION:        BoolStatement::makeSigned
 * OVERVIEW:        Change this from an unsigned to a signed branch
 * NOTE:            Not sure if this is ever going to be used
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void BoolStatement::makeSigned() {
    // Make this into a signed branch
    switch (jtCond)
    {
        case BRANCH_JUL : jtCond = BRANCH_JSL;  break;
        case BRANCH_JULE: jtCond = BRANCH_JSLE; break;
        case BRANCH_JUGE: jtCond = BRANCH_JSGE; break;
        case BRANCH_JUG : jtCond = BRANCH_JSG;  break;
        default:
            // Do nothing for other cases
            break;
    }
}

/*==============================================================================
 * FUNCTION:        BoolStatement::getCondExpr
 * OVERVIEW:        Return the Exp expression containing the HL condition.
 * PARAMETERS:      <none>
 * RETURNS:         a semantic string
 *============================================================================*/
Exp* BoolStatement::getCondExpr() {
    return pCond;
}

/*==============================================================================
 * FUNCTION:        BoolStatement::setCondExpr
 * OVERVIEW:        Set the Exp expression containing the HL condition.
 * PARAMETERS:      Pointer to semantic string to set
 * RETURNS:         <nothing>
 *============================================================================*/
void BoolStatement::setCondExpr(Exp* pss) {
    if (pCond) delete pCond;
    pCond = pss;
}

/*==============================================================================
 * FUNCTION:        BoolStatement::print
 * OVERVIEW:        Write a text representation to the given stream
 * PARAMETERS:      os: stream
 * RETURNS:         <Nothing>
 *============================================================================*/
void BoolStatement::print(std::ostream& os /*= cout*/, bool withDF) {
    os << std::setw(4) << std::dec << number << " ";
    os << "BOOL ";
    pDest->print(os);
    os << " := CC(";
    switch (jtCond)
    {
        case BRANCH_JE:    os << "equals"; break;
        case BRANCH_JNE:   os << "not equals"; break;
        case BRANCH_JSL:   os << "signed less"; break;
        case BRANCH_JSLE:  os << "signed less or equals"; break;
        case BRANCH_JSGE:  os << "signed greater or equals"; break;
        case BRANCH_JSG:   os << "signed greater"; break;
        case BRANCH_JUL:   os << "unsigned less"; break;
        case BRANCH_JULE:  os << "unsigned less or equals"; break;
        case BRANCH_JUGE:  os << "unsigned greater or equals"; break;
        case BRANCH_JUG:   os << "unsigned greater"; break;
        case BRANCH_JMI:   os << "minus"; break;
        case BRANCH_JPOS:  os << "plus"; break;
        case BRANCH_JOF:   os << "overflow"; break;
        case BRANCH_JNOF:  os << "no overflow"; break;
        case BRANCH_JPAR:  os << "parity"; break;
    }
    os << ")";
    if (bFloat) os << ", float";
    os << std::endl;
    if (pCond)
        os << "High level: " << pCond << "\n";
}

/*==============================================================================
 * FUNCTION:        BoolStatement::clone
 * OVERVIEW:        Deep copy clone
 * PARAMETERS:      <none>
 * RETURNS:         Pointer to a new Statement, a clone of this BoolStatement
 *============================================================================*/
Statement* BoolStatement::clone() {
    BoolStatement* ret = new BoolStatement(size);
    ret->jtCond = jtCond;
    if (pCond) ret->pCond = pCond->clone();
    else ret->pCond = NULL;
    ret->bFloat = bFloat;
    ret->size = size;
    // Statement members
    ret->pbb = pbb;
    ret->proc = proc;
    ret->number = number;
    return ret;
}

// visit this Statement
bool BoolStatement::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

// serialize this rtl
bool BoolStatement::serialize_rest(std::ostream &ouf) {
    return true;
}

// deserialize an rtl
bool BoolStatement::deserialize_fid(std::istream &inf, int fid) {
    switch (fid) {
        default:
            return Statement::deserialize_fid(inf, fid);
    }

    return true;
}

void BoolStatement::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
    assert(pDest);
    assert(pCond);
    // pDest := (pCond) ? 1 : 0
    Assign as(pDest->clone(), new Ternary(opTern, pCond->clone(),
      new Const(1), new Const(0)));
    hll->AddAssignmentStatement(indLevel, &as);
}

void BoolStatement::simplify() {
    if (pCond)
        condToRelational(pCond, jtCond);
}

void BoolStatement::getDefinitions(LocationSet &defs) 
{
    defs.insert(getLeft());
}

Type* BoolStatement::getLeftType()
{
    return new BooleanType();
}

bool BoolStatement::usesExp(Exp *e)
{
    assert(pDest && pCond);
    Exp *where = 0;
    return (pCond->search(e, where) || (pDest->isMemOf() && 
        ((Unary*)pDest)->getSubExp1()->search(e, where)));
}

void BoolStatement::processConstants(Prog *prog)
{
}

bool BoolStatement::search(Exp *search, Exp *&result)
{
    assert(pDest);
    if (pDest->search(search, result)) return true;
    assert(pCond);
    return pCond->search(search, result);
}

bool BoolStatement::searchAndReplace(Exp *search, Exp *replace) {
    bool change = false;
    assert(pCond);
    assert(pDest);
    pCond = pCond->searchReplaceAll(search, replace, change);
    pDest = pDest->searchReplaceAll(search, replace, change);
    return change;
}

Type* BoolStatement::updateType(Exp *e, Type *curType) {
    delete curType;
    return new BooleanType();
}

// Convert from SSA form
void BoolStatement::fromSSAform(igraph& ig) {
    pCond = pCond->fromSSA(ig); 
    pDest = pDest->fromSSAleft(ig, this);
}

void BoolStatement::doReplaceRef(Exp* from, Exp* to) {
    searchAndReplace(from, to);
    simplify();
}

void BoolStatement::addUsedLocs(LocationSet& used) {
    if (pCond)
        pCond->addUsedLocs(used);
}

void BoolStatement::fixCallRefs() {
    if (pCond)
        pCond = pCond->fixCallRefs();
}

void BoolStatement::subscriptVar(Exp* e, Statement* def) {
    if (pCond) pCond = pCond->expSubscriptVar(e, def);
    if (pDest) pDest = pDest->expSubscriptVar(e, def);
}


void BoolStatement::setDest(std::list<Statement*>* stmts) {
    assert(stmts->size() == 1);
    Assign* first = (Assign*)stmts->front();
    assert(first->getKind() == STMT_ASSIGN);
    pDest = first->getLeft();
}

//  //  //  //
// Assign //
//  //  //  //

Assign::Assign() : size(32) {
    setKind(STMT_ASSIGN);
}
Assign::Assign(Exp* lhs, Exp* rhs) : lhs(lhs), rhs(rhs), size(32), guard(NULL)
{
    setKind(STMT_ASSIGN);
    if (lhs->getOper() == opTypedExp) { 
        size = ((TypedExp*)lhs)->getType()->getSize(); 
    } 
}
Assign::Assign(int sz, Exp* lhs, Exp* rhs) : lhs(lhs), rhs(rhs), size(sz),
  guard(NULL) {
    setKind(STMT_ASSIGN);
}
Assign::Assign(Assign& o) : size(o.size) {
    setKind(STMT_ASSIGN);
    lhs = o.lhs->clone();
    rhs = o.rhs->clone();
    if (o.guard)
        guard = o.guard->clone();
    else
        guard = NULL;
}

int Assign::getSize() {
    return size;
}
void Assign::setSize(int sz) {
    size = sz;
}

Statement* Assign::clone() {
    Assign* a = new Assign(size, lhs->clone(), rhs->clone());
    // Statement members
    a->pbb = pbb;
    a->proc = proc;
    a->number = number;
    return a;
}

// visit this Statement
bool Assign::accept(StmtVisitor* visitor) {
    return visitor->visit(this);
}

void Assign::simplify() {
    // simplify arithmetic of assignment
    if (Boomerang::get()->noBranchSimplify) {
        if (lhs->getOper() == opZF ||
            lhs->getOper() == opCF ||
            lhs->getOper() == opOF ||
            lhs->getOper() == opNF)
            return;
    }
    lhs = lhs->simplifyArith();
    rhs = rhs->simplifyArith();
    // simplify the resultant expression
    lhs = lhs->simplify();
    rhs = rhs->simplify();
}

void Assign::simplifyAddr() {
    lhs = lhs->simplifyAddr();
    rhs = rhs->simplifyAddr();
}

void Assign::fixSuccessor() {
    lhs = lhs->fixSuccessor();
    rhs = rhs->fixSuccessor();
}

bool Assign::searchAndReplace(Exp* search, Exp* replace) {
    bool change = false;
    lhs = lhs->searchReplaceAll(search, replace, change);
    rhs = rhs->searchReplaceAll(search, replace, change);
    return change;
}

void Assign::print(std::ostream& os, bool withUses) {
    os << std::setw(4) << std::dec << number << " ";
    os << "*" << std::dec << size << "* ";
    if (lhs) lhs->print(os, withUses);
    os << " := ";
    if (rhs) rhs->print(os, withUses);
}

void Assign::getDefinitions(LocationSet &defs) {
    defs.insert(lhs);
}

bool Assign::search(Exp* search, Exp*& result) {
    if (lhs->search(search, result))
        return true;
    return rhs->search(search, result);
}

bool Assign::searchAll(Exp* search, std::list<Exp*>& result) {
    bool res;
    std::list<Exp*> leftResult;
    std::list<Exp*>::iterator it;
    res = lhs->searchAll(search, leftResult);
    // Ugh: searchAll clears the list!
    res |= rhs->searchAll(search, result);
    for (it = leftResult.begin(); it != leftResult.end(); it++)
        result.push_back(*it);
    return res;
}

void Assign::generateCode(HLLCode *hll, BasicBlock *pbb, int indLevel) {
    hll->AddAssignmentStatement(indLevel, this);
}


int Assign::getMemDepth() {
    int d1 = lhs->getMemDepth();
    int d2 = rhs->getMemDepth();
    return std::max(d1, d2);
}

void Assign::fromSSAform(igraph& ig) {
    lhs = lhs->fromSSAleft(ig, this);
    rhs = rhs->fromSSA(ig);
}

void Assign::setRight(Exp* e) {
    if (rhs) delete rhs;
    rhs = e;
}

bool Assign::serialize(std::ostream &ouf, int &len) {
    std::streampos st = ouf.tellp();

    char ch = 'A';
    saveValue(ouf, ch, false);  // ???

    int l;
    lhs->serialize(ouf, l);
    rhs->serialize(ouf, l);
    saveValue(ouf, size, false);

    saveFID(ouf, FID_EXP_END);
    saveLen(ouf, 0);

    len = ouf.tellp() - st;
    return true;
}

// update type for expression
Type *Assign::updateType(Exp *e, Type *curType) {
    return curType;
}

bool Assign::usesExp(Exp *e) {
    Exp *where = 0;
    return (rhs->search(e, where) || (lhs->isMemOf() && 
        ((Unary*)lhs)->getSubExp1()->search(e, where)));
}

void Assign::doReplaceRef(Exp* from, Exp* to) {
    bool changeright = false;
    rhs = rhs->searchReplaceAll(from, to, changeright);
    bool changeleft = false;
    // If LHS is a memof, substitute its subexpression as well
    if (lhs->isMemOf()) {
        Exp* subsub1 = ((Unary*)lhs)->getSubExp1();
        ((Unary*)lhs)->setSubExp1ND(
          subsub1->searchReplaceAll(from, to, changeleft));
    }
    //assert(changeright || changeleft);    // HACK!
    if (!changeright && !changeleft)
        std::cerr << "Exp::doReplaceRef: could not change " << from << " to " <<
          to << " in " << std::dec <<
          dynamic_cast<Statement*>(this)->getNumber() << (Exp*)this << " !!\n";
    // simplify the expression
    rhs = rhs->simplifyArith();
    lhs = lhs->simplifyArith();
    rhs = rhs->simplify();
    lhs = lhs->simplify();
}

void Assign::subscriptVar(Exp* e, Statement* def) {
    // Replace all e with e{def} (on the RHS or in memofs in the LHS)
    // NOTE: don't use searchReplace. It deletes the original, which could
    // already be used as a key in a map!
    rhs = rhs->expSubscriptVar(e, def);
    if (lhs->isMemOf()) {
        Exp* subLeft = ((Unary*)lhs)->getSubExp1();
        Exp* temp = subLeft->expSubscriptVar(e, def);
        if (subLeft != temp)
            ((Unary*)lhs)->setSubExp1ND(temp);
    }
}

/*==============================================================================
 * FUNCTION:        Assign::addUsedLocs
 * OVERVIEW:        Add all locations (registers or memory) used by this
 *                    assignment
 * PARAMETERS:      used: ref to a LocationSet to insert the used locations into
 * RETURNS:         nothing
 *============================================================================*/
void Assign::addUsedLocs(LocationSet& used) {
    rhs->addUsedLocs(used);
    if (lhs->isMemOf()) {
        // We also use any expr like m[exp] on the LHS (but not the outer m[])
        Exp* leftChild = ((Unary*)lhs)->getSubExp1();
        leftChild->addUsedLocs(used);
    }
}

void Assign::fixCallRefs() {
    rhs = rhs->fixCallRefs();
    if (lhs->isMemOf()) {
        ((Unary*)lhs)->refSubExp1() =
          ((Unary*)lhs)->getSubExp1()->fixCallRefs();
    }
}

#if 0
Exp* Assign::updateRefs(StatementSet& defs, int memDepth, StatementSet& rs) {
    // No need to test for equality to left
    // However, need to update aa iff LHS is m[aa]
    if (lhs->isMemOf())
        // Beware. Consider left == m[r[28]]; subExp1 is the same.
        // If we call subExp1->updateRefs, we will double subscript our
        // LHS (violating a basic property of SSA form)
        // If we call left->updateRefs, we would get a
        // subscript of a subscript, also not what we want!
        // Don't call setSubExp1 either, since it deletes the old
        // expression (old expression is always needed)
        ((Unary*)lhs)->setSubExp1ND(lhs->getSubExp1()->
          updateRefs(defs, memDepth, rs));
    rhs = rhs->updateRefs(defs, memDepth, rs);
    return this;
}
#endif


// Not sure if anything needed here
void Assign::processConstants(Prog* prog) {
}
