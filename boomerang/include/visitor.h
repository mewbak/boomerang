/*
 * Copyright (C) 2004, Mike Van Emmerik and Trent Waddington
 */
/*==============================================================================
 * FILE:       visitor.h
 * OVERVIEW:   Provides the definition for the various visitor and modifier
 *             classes. These classes sometimes are associated with Statement
 *             and Exp classes, so they are here to avoid #include problems,
 *             and also to make exp.cpp and statement.cpp a little less huge
 *============================================================================*/
/*
 * $Revision: 1.3 $
 *
 * We have Visitor and Modifier classes separate. Visitors are more suited
 *   for searching: they have the capability of stopping the recursion,
 *   but can't change the class of a top level expression. Modifiers always
 *   recurse to the end, and the ExpModifiers' visit function returns an Exp*
 *   so that the top level expression can change class (e.g. RefExp to Binary).
 * The accept() functions (in the target classes) are always the same for all
 *   visitors; they encapsulate where the visitable parts of a Statement or
 *   expression are.
 * The visit() functions contain the logic of the search/modify/whatever.
 *   Often only a few visitor functions have to do anything. Unfortunately,
 *   the visit functions are members of the Visitor (or Modifier) classes, and
 *   so have to use public functions of the target classes.
 *
 * 14 Jun 04 - Mike: Created, from work started by Trent in 2003
 */

/*
 * The ExpVisitor class is used to iterate over all subexpressions in
 * an expression. It contains methods for each kind of subexpression found
 * in an and can be used to eliminate switch statements.
 */

#ifndef __VISITOR_H__
#define __VISITOR_H__

#ifndef NULL
#define NULL 0      // Often defined in stdio.h
#endif

#include "exp.h"

class Statement;
class Assign;
class CaseStatement;
class CallStatement;
class ReturnStatement;
class GotoStatement;
class BoolStatement;
class BranchStatement;

class RTL;
class UserProc;
class BasicBlock;
typedef BasicBlock* PBB;

class LocationSet;

class ExpVisitor {

public:
    ExpVisitor() { }
    virtual ~ExpVisitor() { }

    // visitor functions,
    // return true to continue iterating through the expression
    // Note: you only need to override the ones that "do something"
virtual bool visit(Unary *e,    bool& override) {override = false; return true;}
virtual bool visit(Binary *e,   bool& override) {override = false; return true;}
virtual bool visit(Ternary *e,  bool& override) {override = false; return true;}
virtual bool visit(TypedExp *e, bool& override) {override = false; return true;}
virtual bool visit(FlagDef *e,  bool& override) {override = false; return true;}
virtual bool visit(RefExp *e,   bool& override) {override = false; return true;}
virtual bool visit(PhiExp *e,   bool& override) {override = false; return true;}
virtual bool visit(Location *e, bool& override) {override = false; return true;}
// These three have zero arity, so there is nothing to override
virtual bool visit(Const *e   ) {return true;}
virtual bool visit(Terminal *e) {return true;}
virtual bool visit(TypeVal *e ) {return true;}
};

// This class visits subexpressions, and if a location, sets the UserProc
class FixProcVisitor : public ExpVisitor {
    // the enclosing UserProc (if a Location)
    UserProc* proc;

public:
    void setProc(UserProc* p) { proc = p; }
    virtual bool visit(Location *e, bool& override);
    // All other virtual functions inherit from ExpVisitor, i.e. they just
    // visit their children recursively
};

// This class is more or less the opposite of the above. It finds a proc by
// visiting the whole expression if necessary
class GetProcVisitor : public ExpVisitor {
    UserProc* proc;         // The result (or NULL)

public:
                 GetProcVisitor() {proc = NULL;}    // Constructor
    UserProc*    getProc() {return proc;}
    virtual bool visit(Location *e, bool& override);
    // All others inherit and visit their children
};

// This class visits subexpressions, and if a Const, sets a new conscript
class SetConscripts : public ExpVisitor {
    int     curConscript;
    bool    bInLocalGlobal;     // True when inside a local or global
public:
            SetConscripts(int n) : bInLocalGlobal(false)
                {curConscript = n;}
    int     getLast() {return curConscript;}
    virtual bool visit(Const* e);
    virtual bool visit(Location* e, bool& override);
    // All other virtual functions inherit from ExpVisitor: return true
};

/*
 * The ExpModifier class is used to iterate over all subexpressions in
 * an expression. It contains methods for each kind of subexpression found
 * in an and can be used to eliminate switch statements.
 * It is a little more expensive to use than ExpVisitor, but can make changes
 * to the expression
 */
class ExpModifier {
protected:
    bool    mod;        // Set if there is any change. Don't have to implement
public:
    ExpModifier() {mod = false;}
    virtual ~ExpModifier() { }
    bool    isMod() {return mod;}
    void    clearMod() {mod = false;}

    // visitor functions
    // Most times these won't be needed
    // Note: you only need to override the ones that make a cange.
    // preVisit comes before modifications to the children (if any)
virtual Exp* preVisit(Unary *e,    bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(Binary *e,   bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(Ternary *e,  bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(TypedExp *e, bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(FlagDef *e,  bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(RefExp *e,   bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(PhiExp *e,   bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(Location *e, bool& norecur) {norecur = false; return e;}
virtual Exp* preVisit(Const *e                  ) {                 return e;}
virtual Exp* preVisit(Terminal *e               ) {                 return e;}
virtual Exp* preVisit(TypeVal *e                ) {                 return e;}

    // postVisit comes after modifications to the children (if any)
virtual Exp* postVisit(Unary *e)    {return e;}
virtual Exp* postVisit(Binary *e)   {return e;}
virtual Exp* postVisit(Ternary *e)  {return e;}
virtual Exp* postVisit(TypedExp *e) {return e;}
virtual Exp* postVisit(FlagDef *e)  {return e;}
virtual Exp* postVisit(RefExp *e)   {return e;}
virtual Exp* postVisit(PhiExp *e)   {return e;}
virtual Exp* postVisit(Location *e) {return e;}
virtual Exp* postVisit(Const *e)    {return e;}
virtual Exp* postVisit(Terminal *e) {return e;}
virtual Exp* postVisit(TypeVal *e)  {return e;}
};

/* 
 * The StmtVisitor class is used to iterate over all stmts in a basic 
 * block. It contains methods for each kind of Statement found in an
 * RTL and can be used to eliminate switch statements.
 * It does not visit the expressions in the statement.
 */
class StmtVisitor {
private:
    // the enclosing basic block
    PBB pBB;

public:
    StmtVisitor() { pBB = NULL; }
    virtual ~StmtVisitor() { }

    // allows the container being iteratorated over to identify itself
    PBB getBasicBlock() { return pBB; }
    void setBasicBlock(PBB bb) { pBB = bb; }

    // visitor functions, 
    // returns true to continue iterating the container
    virtual bool visit(RTL *rtl);   // By default, visits all statements
    virtual bool visit(Assign *stmt) = 0;
    virtual bool visit(GotoStatement *stmt) = 0;
    virtual bool visit(BranchStatement *stmt) = 0;
    virtual bool visit(CaseStatement *stmt) = 0;
    virtual bool visit(CallStatement *stmt) = 0;
    virtual bool visit(ReturnStatement *stmt) = 0;
    virtual bool visit(BoolStatement *stmt) = 0;
};

class StmtSetConscripts : public StmtVisitor {
    int     curConscript;
public:
                 StmtSetConscripts(int n) {curConscript = n;}
    int          getLast() {return curConscript;}

    virtual bool visit(Assign *stmt);
    virtual bool visit(GotoStatement *stmt) {return true;}
    virtual bool visit(BranchStatement *stmt) {return true;}
    virtual bool visit(CaseStatement *stmt);
    virtual bool visit(CallStatement *stmt);
    virtual bool visit(ReturnStatement *stmt);
    virtual bool visit(BoolStatement *stmt);
};

// StmtExpVisitor is a visitor of statements, and of expressions within
// those expressions. The visiting of expressions is done by an ExpVisitor.
class StmtExpVisitor {
public:
    ExpVisitor*  ev;
                 StmtExpVisitor(ExpVisitor* v) {
                    ev = v;}
    virtual      ~StmtExpVisitor() {}
    virtual bool visit(Assign *stmt, bool& override)
        {override = false; return true;}
    virtual bool visit(GotoStatement *stmt, bool& override)
        {override = false; return true;}
    virtual bool visit(BranchStatement *stmt, bool& override)
        {override = false; return true;}
    virtual bool visit(CaseStatement *stmt, bool& override)
        {override = false; return true;}
    virtual bool visit(CallStatement *stmt, bool& override)
        {override = false; return true;}
    virtual bool visit(ReturnStatement *stmt, bool& override)
        {override = false; return true;}
    virtual bool visit(BoolStatement *stmt, bool& override)
        {override = false; return true;}
};

// StmtModifier is a class that visits all statements in an RTL, and for
// all expressions in the various types of statements, makes a modification.
// The modification is as a result of an ExpModifier; there is a pointer to
// such an ExpModifier in a StmtModifier
// Classes that derive from StmtModifier inherit the code (in the accept member
// functions) to modify all the expressions in the various types of statement.
// Because there is nothing specialised about a StmtModifier, it is not an
// abstract class (can be instantiated).
class StmtModifier {
public:
    ExpModifier* mod;           // The expression modifier object
                StmtModifier(ExpModifier* em) {mod = em;}   // Constructor
    virtual     ~StmtModifier() {}
    // This class' visitor functions don't return anything. Maybe we'll need
    // return values at a later stage.
    virtual void visit(Assign *stmt)         {};
    virtual void visit(GotoStatement *stmt)  {};
    virtual void visit(BranchStatement *stmt){};
    virtual void visit(CaseStatement *stmt)  {};
    virtual void visit(CallStatement *stmt)  {};
    virtual void visit(ReturnStatement *stmt){};
    virtual void visit(BoolStatement *stmt)  {};
};

class StripPhis : public StmtModifier {
    bool    del;            // Set true if this statment is to be deleted
public:
                 StripPhis(ExpModifier* em) : StmtModifier(em) {del = false;} 
    virtual void visit(Assign* stmt);
    bool    getDelete() {return del;}
};

// This class visits subexpressions, strips references
class StripRefs : public ExpModifier {
public:
            StripRefs() {}
    virtual Exp* preVisit(RefExp* ei, bool& norecur);
    // All other virtual functions inherit and do nothing
};

class CallRefsFixer : public ExpModifier {
    // These two provide 31 bits (or sizeof(int)-1) of information about whether
    // the child is unchanged. If the mask overflows, it goes to zero, and
    // from then on the child is reported as always changing.
    // This is used to avoid calling simplify in most cases where it is not
    // necessary.
    unsigned    mask;
    unsigned    unchanged;
public:
             CallRefsFixer() { mask = 1; unchanged = (unsigned)-1;}
virtual Exp* preVisit(Unary *e,    bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(Binary *e,   bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(Ternary *e,  bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(TypedExp *e, bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(FlagDef *e,  bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(RefExp *e,   bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(PhiExp *e,   bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(Location *e, bool& norecur) {
    norecur = false; mask <<= 1; return e;}
virtual Exp* preVisit(Const *e)     { mask <<= 1; return e;}
virtual Exp* preVisit(Terminal *e)  { mask <<= 1; return e;}
virtual Exp* preVisit(TypeVal *e)   { mask <<= 1; return e;}

virtual Exp* postVisit(Unary *e);
virtual Exp* postVisit(Binary *e);
virtual Exp* postVisit(Ternary *e);
virtual Exp* postVisit(TypedExp *e);
virtual Exp* postVisit(FlagDef *e);
virtual Exp* postVisit(RefExp *e);
virtual Exp* postVisit(PhiExp *e);
virtual Exp* postVisit(Location *e);
virtual Exp* postVisit(Const *e);
virtual Exp* postVisit(Terminal *e);
virtual Exp* postVisit(TypeVal *e);
};

class UsedLocsFinder : public ExpVisitor {
    LocationSet* used;
public:
                 UsedLocsFinder(LocationSet& used) {this->used = &used;}
                 ~UsedLocsFinder() {}

    LocationSet* getLocSet() {return used;}
    virtual bool visit(RefExp *e,   bool& override);
    virtual bool visit(Location *e, bool& override);
    virtual bool visit(Terminal* e);
    virtual bool visit(PhiExp* e,   bool& override);
};

class UsedLocsVisitor : public StmtExpVisitor {
public:
    // The bool final is for ignoring those parts of expressions that are
    // not to be considered for the final pass just before code generation.
    // For example, implicit parameters and returns in CallStatements are
    // ignored (not considered used).
    bool final;
            UsedLocsVisitor(ExpVisitor* v, bool f = false) : StmtExpVisitor(v)
              {final = f;}
    virtual ~UsedLocsVisitor() {}
    // Needs special attention because the lhs of an assignment isn't used
    // (except where it's m[blah], when blah is used)
    virtual bool visit(Assign *stmt, bool& override);
    // Returns aren't used (again, except where m[blah] where blah is used),
    // and there is special logic for when the pass is final
    virtual bool visit(CallStatement *stmt, bool& override);
    // A BoolStatement uses its condition expression, but not its destination
    // (unless it's an m[x], in which case x is used and not m[x])
    virtual bool visit(BoolStatement *stmt, bool& override);
};

class ExpSubscripter : public ExpModifier {
    Exp*        search;
    Statement*  def;
public:
                ExpSubscripter(Exp* s, Statement* d) {
                    search = s; def = d; }
    virtual Exp* preVisit(Location *e, bool& norecur);
    virtual Exp* preVisit(Terminal *e);
    virtual Exp* preVisit(RefExp *e,   bool& norecur);
};

#endif  // #ifndef __VISIOR_H__
