/*
 * Copyright (C) 2002, Trent Waddington
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       dataflow.cpp
 * OVERVIEW:   Implementation of the dataflow classes.
 *============================================================================*/

/*
 * $Revision: 1.41 $
 * 03 Jul 02 - Trent: Created
 * 09 Jan 03 - Mike: Untabbed, reformatted
 * 03 Feb 03 - Mike: cached dataflow (uses and usedBy)
 */

/*==============================================================================
 * Dependencies.
 *============================================================================*/

#include <assert.h>
#if defined(_MSC_VER) && _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif 

#include "dataflow.h"
#include "exp.h"
#include "cfg.h"
#include "proc.h"
#include "boomerang.h"
#include "rtl.h"            // For debugging code
#include <sstream>

#define VERBOSE Boomerang::get()->vFlag



#if 0
// Finds a definition for a given location
// MVE: Is this useful?
Statement *Statement::findDef(Exp *e) {
    StmtSetIter it;
    for (Statement* s = uses.getFirst(it); s; s = uses.getNext(it)) {
        if (s->getLeft() && *s->getLeft() == *e)
            return s;
    }
    return NULL;
}

// From the set of reaching statements, find those which assign to a location
// that I use (i.e. are in my RHS, or in a m[] on my LHS), parameter, etc
// This is the set of statements that this statement uses (relies on)
// Also calculates usedBy
void Statement::calcUses(StatementSet &uses) {
    StatementSet reachIn;
    // Assume (for now) that calculation of uses is interprocedural, and
    // everything is still set up for phase 2
    getReachIn(reachIn, 2);
    StmtSetIter it;
    for (Statement* s = reachIn.getFirst(it); s; s = reachIn.getNext(it)) {
        assert(s);
        Exp *left = s->getLeft();
        if (left == NULL) continue;     // E.g. HLCall with no return value
        if (usesExp(left)) {
            uses.insert(s);             // This statement uses s
            s->usedBy.insert(this);     // s is usedBy this Statement
        }
    }
}

// From all statements in this proc, find those which use my LHS
// These statements rely on my assignment; this statement is usedBy these
void Statement::calcUsedBy(StatementSet &usedBy) {
#if 0       // Done in calcUses now
    if (getLeft() == NULL) return;
    StatementList stmts;
    proc->getStatements(stmts);
    StmtListIter it;
    for (Statement* s = stmts.getFirst(it); s; s = stmts.getNext(it)) {
        if (s->findDef(getLeft()) == this)
            usedBy.insert(s);
    }
#endif
}

/* Goes through the definitions which reach this expression and creates a
   link from any definition that is used by this expression to this 
   expression.
 */
void Statement::calcUseLinks() {
    calcUses(uses);             // Does both uses and usedBy now
}
#endif

// replace a use in this statement
void Statement::replaceRef(Statement *def) {
    Exp* lhs = def->getLeft();
    Exp* rhs = def->getRight();
    assert(lhs);
    assert(rhs);
    // "Wrap" the LHS in a single ref RefsExp
    // This is so that it doesn't "short circuit" to unsubscripted variables
    // Example: 42:r28 := r28{14}-4 into m[r28-24] := m[r28{42}] + ...
    // The bare r28 on the left "short circuits" to the bare r28 in this LHS
    Unary* re;
    if (Boomerang::get()->impSSA)
        re = new RefsExp(lhs, def);
    else
        re = new RefExp(lhs, def);

    // do the replacement
    doReplaceRef(re, rhs);

    // Careful: don't delete re while lhs is still a part of it!
    // Else, will delete lhs, which is still a part of def!
    re->setSubExp1ND(NULL);
    delete re;
}

// special replace a use in this statement (where this statement has a
// component with two refs)
void Statement::specialReplaceRef(Statement *def) {
    Exp* lhs = def->getLeft();
    Exp* rhs = def->getRight();
    assert(lhs);
    assert(rhs);
    // "Wrap" the LHS in a double ref RefsExp
    // Example: this == 119 *32* r[29] := m[r[28]{85 119}],
    //           def ==  85 *32* r[29] := r[28]{83}
    // In order to substitute into the double ref component, we have to wrap
    // lhs in a double ref RefsExp
    RefsExp re(lhs, def);
    re.addSubscript(this);

    // do the replacement
    doReplaceRef(&re, rhs);

    // Careful: don't allow re to destruct while lhs is still a part of it!
    // Else, will delete lhs, which is still a part of def!
    re.setSubExp1ND(NULL);
}

/* Get everything that reaches this assignment.
   To get the reachout, use getReachIn(reachset), calcReachOut(reachset).
 */
void Statement::getReachIn(StatementSet &reachin, int phase) {
    assert(pbb);
    pbb->getReachInAt(this, reachin, phase);
}

void Statement::getAvailIn(StatementSet &availin, int phase) {
    assert(pbb);
    pbb->getAvailInAt(this, availin, phase);
}

//void Statement::getLiveOut(LocationSet &liveout, int phase) {
//    assert(pbb);
//    pbb->getLiveOutAt(this, liveout, phase);
//}

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

void Statement::checkLiveIn(LocationSet& liveout, igraph& ig) {
    // Note: this is an O(N**2) operation!
    LocSetIter aa, bb;
    for (Exp* a = liveout.getFirst(aa); a; a = liveout.getNext(aa)) {
        bb = aa;
        Exp* b = liveout.getNext(bb);
        while (b) {
            if (*a *= *b) {         // Compare, ignoring subscripts
                if (*a < *b)
                    insertInterference(ig, a);
                else
                    insertInterference(ig, b);
            }
            b = liveout.getNext(bb);
        }
    }
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

/* calculates the definitions that are not killed by this assignment.
   If the reach set is empty, it will contain anything this assignment defines.
   If the reach set is not empty, then it will not contain anything this
      assignment kills.
 */
void Statement::calcReachOut(StatementSet &reach) {
    // calculate kills
    killDef(reach);
    // add this def
    if (getLeft() != NULL)
        reach.insert(this);
}

/* calculates the definitions that are not killed by this statement (along
    any path).
   If the available set is empty, it will contain anything this assignment
   defines. If the available set is not empty, then it will not contain
   anything this assignment kills.
   Note: now identical to the above
 */
void Statement::calcAvailOut(StatementSet &avail) {
    // calculate kills
    killDef(avail);         // Reaching and available defs both killed by defs
    // add this def
    if (getLeft() != NULL)
        avail.insert(this);
}

/* calculates the definitions containing live variables that are not killed by
   this statement.
   If the live set is empty, it will contain anything this assignment defines.
   If the reach set is not empty, then it will not contain anything this
      assignment kills.
 */
void Statement::calcLiveIn(LocationSet &live) {
    // Even though this is a backwards flow problem, we need to do the kills
    // first. Consider
    // eax := eax + 5
    // where eax is in fact live (used before being defined) after this stmt.
    // It is clearly still live before the statement, so we do the kill (which
    // removes eax from the liveness set) then insert all our uses (which in
    // this case is eax again). Weird.
    // calculate kills
    killLive(live);
    // add all locations that this statement uses (register or memory)
    addUsedLocs(live);
#if 0
    // Now substitute. If any of the locations in the live set use this
    // statement's left hand side, do the substitution
    // (Since live is a set of locations, this will only happend for
    // memofs)
    live.substitute(*this);
#endif
}

void Statement::calcDeadIn(LocationSet &dead) {
    // calculate kills
    killDead(dead);
    // add the location that this statement defines (register or memory)
    Exp* left = getLeft();
    if (left) dead.insert(left);
//    dead.substitute(*this);
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

// Update the dataflow for this stmt. This stmt is about to be deleted.
// Don't assume the statement being erased has no dataflow; it could be
// of the form x := x
// 
//   Before           After
//     (1)             (1)
//     ^ |usedBy       ^ |
// uses| v             | |
//     (2) = this      | |
//     ^ |usedBy       | |
// uses| v             | v
//     (3)             (3)
//
#if 0
void Statement::updateDfForErase() {
    // First fix the down arrows (usedBy)
    StmtSetIter it, uu;
    for (Statement* ss = uses.getFirst(it); ss; ss = uses.getNext(it)) {
        // it is iterating through the (1) set
        // This is the usedBy entry from this (1) to (2)
        // Erase this use of my definition, since I'm about to be deleted
        ss->usedBy.remove(this);
        // The use from this (1) to each (3) comes next
        for (Statement* su = usedBy.getFirst(uu); su;
          su = usedBy.getNext(uu))
            ss->usedBy.insert(su);        // This (3) usedby this (1)
    }
    // Next, fix the up arrows (uses)
    for (Statement* ss = usedBy.getFirst(it); ss; ss = usedBy.getNext(it)) {
        // it is iterating through the (3) set
        // This is the uses entry from this (3) to (2)
        // Erase this def of my rhs, since I'm about to be deleted
        ss->uses.remove(this);
        // The uses from this (3) to each (1) comes next
        for (Statement* suu = uses.getFirst(uu); suu; suu = uses.getNext(uu))
            ss->uses.insert(suu);        // This (3) uses this (1)
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
    os << s->getNumber() << ":";
    s->print(os, true);
    return os;
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

// Killing difference. Kill any element of this where there is an element of
// other that defines the same location
void StatementSet::makeKillDiff(StatementSet& other) {
    StmtSetIter it;
    for (it = other.sset.begin(); it != other.sset.end(); it++)
        (*it)->killDef(*this);
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

// Print to cerr, for debugging
void StatementSet::prints() {
    StmtSetIter it;
    for (it = sset.begin(); it != sset.end(); it++)
        std::cerr << *it << ",\t";
    std::cerr << "\n";
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

void LocationSet::prints() {
    LocSetIter it;
    for (it = sset.begin(); it != sset.end(); it++)
        std::cerr << *it << ",\t";
    std::cerr << "\n";
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

void StatementList::prints() {
    StmtListIter it;
    for (it = slist.begin(); it != slist.end(); it++) {
        std::cerr << *it << ",\t";
    }
}

static char debug_buffer[200];
char* Statement::prints() {
      std::ostringstream ost;
      print(ost, true);
      strncpy(debug_buffer, ost.str().c_str(), 199);
      debug_buffer[199] = '\0';
      return debug_buffer;
}

void Statement::getDefinitions(LocationSet &def) {
    assert(false);
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
            if (e->getNumRefs() == 2) {
                StmtSetIter dummy;
                Statement* d1 = ((RefsExp*)e)->getFirstRef(dummy);
                Statement* d2 = ((RefsExp*)e)->getNextRef(dummy);
                // Don't propagate if one of the defs is on the exclude list
                if (exclude.exists(d1)) continue;
                if (exclude.exists(d2)) continue;
                // Warning! This also tries to propagate into loops!
                // Not safe in general! (This is "Mike's hack")
                if (Boomerang::get()->recursionBust &&
                  (d1 == this || d2 == this) &&
                  (*getLeft() == *((RefsExp*)e)->getSubExp1())) {
                    // Experimental: get rid of defs caught up in recursion
                    // Get the two definitions we reference
                    // This is the special case where we have something like
                    // 119 *32* r[29] := m[r[29]{85 119}]
                    // Mike believes we can ignore the 119 part!
                    if (d1 == this)
                        change = doPropagateTo(memDepth, d2, true);
                    else
                        change = doPropagateTo(memDepth, d1, true);
                    continue;
                } else if (!Boomerang::get()->noPropMult && *d1 == *d2) {
                    // Different definitions, but they are the same
                    change = doPropagateTo(memDepth, d1, true);
                    continue;
                } else continue;
            } 
            // FIXME: Could find rare cases with 3 or more definitions, all
            // the same; could propagate to these if the -npm flag not set
            if (e->getNumRefs() != 1) continue;
            // Can propagate TO this (if memory depths are suitable)
            StmtSetIter dummy;
            Statement* def;
            if (Boomerang::get()->impSSA)
                def = ((RefsExp*)e)->getFirstRef(dummy);
            else
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
            AssignExp* ae = dynamic_cast<AssignExp*>(def);
            if (ae && ae->isPhi())
                // Don't propagate phi statements!
                continue;
            change = doPropagateTo(memDepth, def, false);
        }
    } while (change && ++changes < 20);
}

bool Statement::doPropagateTo(int memDepth, Statement* def, bool twoRefs) {
    // Check the depth of the definition (an assignment)
    // This checks the depth for the left and right sides, and
    // gives the max for both. Example: can't propagate
    // tmp := m[x] to foo := tmp if memDepth == 0
    int depth = (dynamic_cast<AssignExp*>(def))->getMemDepth();
    if (depth > memDepth)
        return false;

    // Respect the -p N switch
    if (Boomerang::get()->numToPropagate >= 0) {
        if (Boomerang::get()->numToPropagate == 0) return false;
            Boomerang::get()->numToPropagate--;
    }

    if (twoRefs)
        // A special version of replaceRef is needed, which wraps the LHS
        // with two refs
        specialReplaceRef(def);
    else
        replaceRef(def);
    if (VERBOSE) {
        if (twoRefs) std::cerr << "Special: ";
        std::cerr << "Propagating " << std::dec << def->getNumber() <<
          " into " << getNumber() <<
          ", result is " << this << "\n";
    }
    return true;
}

bool Statement::operator==(Statement& other) {
    AssignExp* ae1 = dynamic_cast<AssignExp*>(this);
    AssignExp* ae2 = dynamic_cast<AssignExp*>(&other);
    assert(ae1);
    assert(ae2);
    return *ae1 == *ae2;
}

bool Statement::isNullStatement() {
    AssignExp *e = dynamic_cast<AssignExp*>(this);
    if (e == NULL) return false;
    Exp* sub2 = e->getSubExp2();
    if (sub2->isSubscript()) {
        if (Boomerang::get()->impSSA) {
            RefsExp* re = (RefsExp*)sub2;
            if (re->getNumRefs() != 1)
                // Can't be null
                return false;
            StmtSetIter dummy;
            // Has only 1 reference; has to be equal to self to be a null statement
            // Not even necessary to compare the LHS and RHS; just the statement
            // numbers
            return re->getFirstRef(dummy)->number == number;
        }
        else
            // Must refer to self to be null
            return this == ((RefExp*)sub2)->getRef();
    }
    else
        // Null if left == right
        return *e->getSubExp1() == *sub2;
}

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
        Statement* c = orig->cloneStmt();
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
