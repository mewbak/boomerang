/*==============================================================================
 * FILE:       StatementTest.cc
 * OVERVIEW:   Provides the implementation for the StatementTest class, which
 *              tests the dataflow subsystems
 *============================================================================*/
/*
 * $Revision: 1.6 $
 *
 * 14 Jan 03 - Trent: Created
 * 17 Apr 03 - Mike: Added testRecursion to track down a nasty bug
 */

#ifndef BOOMDIR
#error Must define BOOMDIR
#endif

#define HELLO_PENTIUM       BOOMDIR "/test/pentium/hello"
#define FIBO_PENTIUM        BOOMDIR "/test/pentium/fibo-O4"

#include "StatementTest.h"
#include "cfg.h"
#include "rtl.h"
#include "pentiumfrontend.h"
#include "boomerang.h"
#include "exp.h"
#include "managed.h"

#include <sstream>
#include <map>

/*==============================================================================
 * FUNCTION:        StatementTest::registerTests
 * OVERVIEW:        Register the test functions in the given suite
 * PARAMETERS:      Pointer to the test suite
 * RETURNS:         <nothing>
 *============================================================================*/
#define MYTEST(name) \
suite->addTest(new CppUnit::TestCaller<StatementTest> ("Statements", \
    &StatementTest::name, *this))

void StatementTest::registerTests(CppUnit::TestSuite* suite) {

    MYTEST(testLocationSet);
    MYTEST(testWildLocationSet);
#if 0               // Needs to be updated for global dataflow
    MYTEST(testEmpty);
    MYTEST(testFlow);
    MYTEST(testKill);
    MYTEST(testUse);
    MYTEST(testUseOverKill);
    MYTEST(testUseOverBB);
    MYTEST(testUseKill);
    MYTEST(testEndlessLoop);
#endif
    //MYTEST(testRecursion);
    //MYTEST(testExpand);
    MYTEST(testClone);
    MYTEST(testIsAssign);
    MYTEST(testIsFlagAssgn);
}

int StatementTest::countTestCases () const
{ return 2; }   // ? What's this for?

/*==============================================================================
 * FUNCTION:        StatementTest::setUp
 * OVERVIEW:        Set up some expressions for use with all the tests
 * NOTE:            Called before any tests
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void StatementTest::setUp () {
}

/*==============================================================================
 * FUNCTION:        StatementTest::tearDown
 * OVERVIEW:        Delete expressions created in setUp
 * NOTE:            Called after all tests
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void StatementTest::tearDown () {
}

/*==============================================================================
 * FUNCTION:        StatementTest::testEmpty
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testEmpty () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0x123);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    std::list<Statement*>* ls = new std::list<Statement*>;
    ls->push_back(new ReturnStatement);
    pRtls->push_back(new RTL(0x123));
    cfg->newBB(pRtls, RET, 0);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected = "Ret BB: reach in: \n00000123 RET\n"
        "cfg reachExit: \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testFlow
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testFlow () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0x123);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    Assign *a = new Assign(Location::regOf(24),
        new Const(5));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    first->setOutEdge(0, ret);
    ret->addInEdge(first);
    cfg->setEntryBB(first);     // Also sets exitBB; important!
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected =
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: \n"
      "Ret BB: reach in: *32* r[24] := 5, \n"
      "00000123 RET\n"
      "cfg reachExit: *32* r[24] := 5, \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testKill
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testKill () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0x123);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    Assign *e = new Assign(Location::regOf(24),
                     new Const(5));
    e->setProc(proc);
    rtl->appendStmt(e);
    e = new Assign(Location::regOf(24),
                  new Const(6));
    e->setProc(proc);
    rtl->appendStmt(e);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    first->setOutEdge(0, ret);
    ret->addInEdge(first);
    cfg->setEntryBB(first);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected =
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: \n"
      "         *32* r[24] := 6   uses:    used by: \n"
      "Ret BB: reach in: *32* r[24] := 6, \n"
      "00000123 RET\n"
      "cfg reachExit: *32* r[24] := 6, \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testUse
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testUse () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    Assign *a = new Assign(Location::regOf(24),
                     new Const(5));
    a->setProc(proc);
    rtl->appendStmt(a);
    a = new Assign(Location::regOf(28),
                  Location::regOf(24));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    first->setOutEdge(0, ret);
    ret->addInEdge(first);
    cfg->setEntryBB(first);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected =
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: *32* r[28] := r[24], \n"
      "         *32* r[28] := r[24]   uses: *32* r[24] := 5,    used by: \n"
      "Ret BB: reach in: *32* r[24] := 5, *32* r[28] := r[24], \n"
      "00000123 RET\n"
      "cfg reachExit: *32* r[24] := 5, *32* r[28] := r[24], \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testUseOverKill
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testUseOverKill () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    Assign *e = new Assign(Location::regOf(24),
                     new Const(5));
    e->setProc(proc);
    rtl->appendStmt(e);
    e = new Assign(Location::regOf(24),
                     new Const(6));
    e->setProc(proc);
    rtl->appendStmt(e);
    e = new Assign(Location::regOf(28),
                  Location::regOf(24));
    e->setProc(proc);
    rtl->appendStmt(e);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    first->setOutEdge(0, ret);
    ret->addInEdge(first);
    cfg->setEntryBB(first);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected = 
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: \n"
      "         *32* r[24] := 6   uses:    used by: *32* r[28] := r[24], \n"
      "         *32* r[28] := r[24]   uses: *32* r[24] := 6,    used by: \n"
      "Ret BB: reach in: *32* r[24] := 6, *32* r[28] := r[24], \n"
      "00000123 RET\n"
      "cfg reachExit: *32* r[24] := 6, *32* r[28] := r[24], \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testUseOverBB
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testUseOverBB () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    Assign *a = new Assign(Location::regOf(24),
                     new Const(5));
    a->setProc(proc);
    rtl->appendStmt(a);
    a = new Assign(Location::regOf(24),
                     new Const(6));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL();
    a = new Assign(Location::regOf(28),
                  Location::regOf(24));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    first->setOutEdge(0, ret);
    ret->addInEdge(first);
    cfg->setEntryBB(first);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected =
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: \n"
      "         *32* r[24] := 6   uses:    used by: *32* r[28] := r[24], \n"
      "Ret BB: reach in: *32* r[24] := 6, \n"
      "00000000 *32* r[28] := r[24]   uses: *32* r[24] := 6,    used by: \n"
      "00000123 RET\n"
      "cfg reachExit: *32* r[24] := 6, *32* r[28] := r[24], \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testUseKill
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testUseKill () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    Assign *a = new Assign(Location::regOf(24),
                     new Const(5));
    a->setProc(proc);
    rtl->appendStmt(a);
    a = new Assign(Location::regOf(24),
              new Binary(opPlus, Location::regOf(24),
                             new Const(1)));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    first->setOutEdge(0, ret);
    ret->addInEdge(first);
    cfg->setEntryBB(first);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected  = 
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: *32* r[24] := r[24] + 1, \n"
      "         *32* r[24] := r[24] + 1   uses: *32* r[24] := 5,    used by: \n"
      "Ret BB: reach in: *32* r[24] := r[24] + 1, \n"
      "00000123 RET\n"
      "cfg reachExit: *32* r[24] := r[24] + 1, \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testEndlessLoop
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testEndlessLoop () {
    // create Prog
    Prog *prog = new Prog();
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    // r[24] := 5
    Assign *e = new Assign(Location::regOf(24),
                     new Const(5));
    e->setProc(proc);
    rtl->appendStmt(e);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);
    pRtls = new std::list<RTL*>();
    rtl = new RTL();
    // r[24] := r[24] + 1
    e = new Assign(Location::regOf(24),
              new Binary(opPlus, Location::regOf(24),
                             new Const(1)));
    e->setProc(proc);
    rtl->appendStmt(e);
    pRtls->push_back(rtl);
    PBB body = cfg->newBB(pRtls, ONEWAY, 1);
    first->setOutEdge(0, body);
    body->addInEdge(first);
    body->setOutEdge(0, body);
    body->addInEdge(body);
    cfg->setEntryBB(first);
    // compute dataflow
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected =
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: *32* r[24] := r[24] + 1, \n"
      "Oneway BB: reach in: *32* r[24] := 5, *32* r[24] := r[24] + 1, \n"
      "00000000 *32* r[24] := r[24] + 1   uses: *32* r[24] := 5, "
      "*32* r[24] := r[24] + 1,    used by: *32* r[24] := r[24] + 1, \n"
      "cfg reachExit: \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatementTest::testLocationSet
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testLocationSet () {
    Location rof(opRegOf, new Const(12), NULL);
    Const& theReg = *(Const*)rof.getSubExp1();
    LocationSet ls;
    LocationSet::iterator ii;
    ls.insert(rof.clone());
    theReg.setInt(8);
    ls.insert(rof.clone());
    theReg.setInt(31);
    ls.insert(rof.clone());
    theReg.setInt(24);
    ls.insert(rof.clone());
    theReg.setInt(12);
    ls.insert(rof.clone());     // Note: r[12] already inserted
    CPPUNIT_ASSERT_EQUAL(4, ls.size());
    theReg.setInt(8);
    ii = ls.begin();
    CPPUNIT_ASSERT(rof == **ii);
    theReg.setInt(12);
    Exp* e;
    e = *(++ii); CPPUNIT_ASSERT(rof == *e);
    theReg.setInt(24);
    e = *(++ii); CPPUNIT_ASSERT(rof == *e);
    theReg.setInt(31);
    e = *(++ii); CPPUNIT_ASSERT(rof == *e);
    Location mof(opMemOf,
        new Binary(opPlus,
            Location::regOf(14),
            new Const(4)), NULL);
    ls.insert(mof.clone());
    ls.insert(mof.clone());
    CPPUNIT_ASSERT_EQUAL(5, ls.size());
    ii = ls.begin();
    CPPUNIT_ASSERT(mof == **ii);
    LocationSet ls2 = ls;
    Exp* e2 = *ls2.begin();
    CPPUNIT_ASSERT(e2 != *ls.begin());      // Must be cloned
    CPPUNIT_ASSERT_EQUAL(5, ls2.size());
    CPPUNIT_ASSERT(mof == **ls2.begin());
    theReg.setInt(8);
    e = *(++ls2.begin()); CPPUNIT_ASSERT(rof == *e);
}

/*==============================================================================
 * FUNCTION:        StatementTest::testWildLocationSet
 * OVERVIEW:        
 *============================================================================*/
void StatementTest::testWildLocationSet () {
    Location rof12(opRegOf, new Const(12), NULL);
    Location rof13(opRegOf, new Const(13), NULL);
    Assign a10, a20;
    a10.setNumber(10);
    a20.setNumber(20);
    RefExp r12_10(rof12.clone(), &a10);
    RefExp r12_20(rof12.clone(), &a20);
    RefExp r12_0 (rof12.clone(), NULL);
    RefExp r13_10(rof13.clone(), &a10);
    RefExp r13_20(rof13.clone(), &a20);
    RefExp r13_0 (rof13.clone(), NULL);
    RefExp r11_10(Location::regOf(11), &a10);
    RefExp r22_10(Location::regOf(22), &a10);
    LocationSet ls;
    ls.insert(&r12_10);
    ls.insert(&r12_20);
    ls.insert(&r12_0);
    ls.insert(&r13_10);
    ls.insert(&r13_20);
    ls.insert(&r13_0);
    RefExp wildr12(rof12.clone(), (Statement*)-1);
    CPPUNIT_ASSERT(ls.find(&wildr12));
    RefExp wildr13(rof13.clone(), (Statement*)-1);
    CPPUNIT_ASSERT(ls.find(&wildr13));
    RefExp wildr10(Location::regOf(10), (Statement*)-1);
    CPPUNIT_ASSERT(!ls.find(&wildr10));
    // Test findDifferentRef
    Exp* x;
    CPPUNIT_ASSERT( ls.findDifferentRef(&r13_10, x));
    CPPUNIT_ASSERT( ls.findDifferentRef(&r13_20, x));
    CPPUNIT_ASSERT( ls.findDifferentRef(&r13_0 , x));
    CPPUNIT_ASSERT( ls.findDifferentRef(&r12_10, x));
    CPPUNIT_ASSERT( ls.findDifferentRef(&r12_20, x));
    CPPUNIT_ASSERT( ls.findDifferentRef(&r12_0 , x));
    // Next 4 should fail
    CPPUNIT_ASSERT(!ls.findDifferentRef(&r11_10, x));
    CPPUNIT_ASSERT(!ls.findDifferentRef(&r22_10, x));
    ls.insert(&r11_10);
    ls.insert(&r22_10);
    CPPUNIT_ASSERT(!ls.findDifferentRef(&r11_10, x));
    CPPUNIT_ASSERT(!ls.findDifferentRef(&r22_10, x));
}

/*==============================================================================
 * FUNCTION:        StatementTest::testRecursion
 * OVERVIEW:        Test push of argument (X86 style), then call self
 *============================================================================*/
void StatementTest::testRecursion () {
    // create Prog
    BinaryFile *pBF = BinaryFile::Load(HELLO_PENTIUM);  // Don't actually use it
    FrontEnd *pFE = new PentiumFrontEnd(pBF);
    // We need a Prog object with a pBF (for getEarlyParamExp())
    Prog* prog = new Prog(pBF, pFE);
    // create UserProc
    std::string name = "test";
    UserProc *proc = new UserProc(prog, name, 0);
    // create CFG
    Cfg *cfg = proc->getCFG();
    std::list<RTL*>* pRtls = new std::list<RTL*>();
    RTL *rtl = new RTL();
    // push bp
    // r28 := r28 + -4
    Assign *a = new Assign(Location::regOf(28),
        new Binary(opPlus,
            Location::regOf(28),
            new Const(-4)));
    rtl->appendStmt(a);
    // m[r28] := r29
    a = new Assign(
        Location::memOf(
            Location::regOf(28)),
        Location::regOf(29));
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    pRtls = new std::list<RTL*>();
    // push arg+1
    // r28 := r28 + -4
    a = new Assign(Location::regOf(28),
            new Binary(opPlus,
                Location::regOf(28),
                new Const(-4)));
    rtl->appendStmt(a);
    // Reference our parameter. At esp+0 is this arg; at esp+4 is old bp;
    // esp+8 is return address; esp+12 is our arg
    // m[r28] := m[r28+12] + 1
    a = new Assign(Location::memOf(Location::regOf(28)),
                     new Binary(opPlus,
                        Location::memOf(
                            new Binary(opPlus,
                                Location::regOf(28),
                                new Const(12))),
                        new Const(1)));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    PBB first = cfg->newBB(pRtls, FALL, 1);

    // The call BB
    pRtls = new std::list<RTL*>();
    rtl = new RTL(1);
    // r28 := r28 + -4
    a = new Assign(Location::regOf(28),
        new Binary(opPlus, Location::regOf(28), new Const(-4)));
    rtl->appendStmt(a);
    // m[r28] := pc
    a = new Assign(Location::memOf(Location::regOf(28)),
        new Terminal(opPC));
    rtl->appendStmt(a);
    // %pc := (%pc + 5) + 135893848
    a = new Assign(new Terminal(opPC),
        new Binary(opPlus,
            new Binary(opPlus,
                new Terminal(opPC),
                new Const(5)),
            new Const(135893848)));
    a->setProc(proc);
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    CallStatement* c = new CallStatement;
    rtl->appendStmt(c);
#if 0
    // Vector of 1 arg
    std::vector<Exp*> args;
    // m[r[28]+8]
    Exp* a = Location::memOf( new Binary(opPlus,
      Location::regOf(28), new Const(8)));
    args.push_back(a);
    crtl->setArguments(args);
#endif
    c->setDestProc(proc);        // Just call self
    PBB callbb = cfg->newBB(pRtls, CALL, 1);
    first->setOutEdge(0, callbb);
    callbb->addInEdge(first);
    callbb->setOutEdge(0, callbb);
    callbb->addInEdge(callbb);

    pRtls = new std::list<RTL*>();
    rtl = new RTL(0x123);
    rtl->appendStmt(new ReturnStatement);
    // This ReturnStatement requires the following two sets of semantics to pass the
    // tests for standard Pentium calling convention
    // pc = m[r28]
    a = new Assign(new Terminal(opPC),
        Location::memOf(
            Location::regOf(28)));
    rtl->appendStmt(a);
    // r28 = r28 + 4
    a = new Assign(Location::regOf(28),
        new Binary(opPlus,
            Location::regOf(28),
            new Const(4)));
    rtl->appendStmt(a);
    pRtls->push_back(rtl);
    PBB ret = cfg->newBB(pRtls, RET, 0);
    callbb->setOutEdge(0, ret);
    ret->addInEdge(callbb);
    cfg->setEntryBB(first);

// Force "verbose" flag (-v)
    Boomerang* boo = Boomerang::get();
    boo->vFlag = true;
    // decompile the "proc"
    prog->decompile();
    // print cfg to a string
    std::ostringstream st;
    cfg->print(st, true);
    std::string s = st.str();
    // compare it to expected
    std::string expected;
    expected =
      "Fall BB: reach in: \n"
      "00000000 *32* r[24] := 5   uses:    used by: *32* r[24] := r[24] + 1, \n"
      "00000000 *32* r[24] := 5   uses:    used by: *32* r[24] := r[24] + 1, \n"
      "Call BB: reach in: *32* r[24] := 5, *32* r[24] := r[24] + 1, \n"
      "00000001 *32* r[24] := r[24] + 1   uses: *32* r[24] := 5, "
      "*32* r[24] := r[24] + 1,    used by: *32* r[24] := r[24] + 1, \n"
      "cfg reachExit: \n";
    CPPUNIT_ASSERT_EQUAL(expected, s);
    // clean up
    delete prog;
}

/*==============================================================================
 * FUNCTION:        StatamentTest::testClone
 * OVERVIEW:        Test cloning of Assigns (and exps)
 *============================================================================*/
void StatementTest::testClone () {
    Assign* a1 = new Assign(32,
            Location::regOf(8),
            new Binary(opPlus,
                Location::regOf(9),
                new Const(99)));
    Assign* a2 = new Assign(16,
            new Unary(opParam, new Const("x")),
            new Unary(opParam, new Const("y")));
    Statement* c1 = a1->clone();
    Statement* c2 = a2->clone();
    std::ostringstream o1, o2;
    a1->print(o1);
    delete a1;           // And c1 should still stand!
    c1->print(o2);
    a2->print(o1);
    c2->print(o2);
    delete a2;
    std::string expected("   0 *32* r8 := r9 + 99   0 *16* x := y");
    std::string act1(o1.str());
    std::string act2(o2.str());
    CPPUNIT_ASSERT_EQUAL(expected, act1); // Originals
    CPPUNIT_ASSERT_EQUAL(expected, act2); // Clones
    delete c1;
    delete c2;
}
 
/*==============================================================================
 * FUNCTION:        StatementTest::testIsAssign
 * OVERVIEW:        Test assignment test
 *============================================================================*/
void StatementTest::testIsAssign () {
    std::ostringstream ost;
    // r2 := 99
    Assign a(32,
        Location::regOf(2),
        new Const(99));
    a.print(ost);
    std::string expected("   0 *32* r2 := 99");
    std::string actual (ost.str());
    CPPUNIT_ASSERT_EQUAL(expected, actual);
//    CPPUNIT_ASSERT_EQUAL (std::string("*32* r2 := 99"), std::string(ost.str()));
    CPPUNIT_ASSERT(a.isAssign());

    CallStatement* c = new CallStatement;
    CPPUNIT_ASSERT(!c->isAssign());
}

/*==============================================================================
 * FUNCTION:        StatementTest::testIsFlagCall
 * OVERVIEW:        Test the isFlagAssgn function, and opFlagCall
 *============================================================================*/
void StatementTest::testIsFlagAssgn () {
    std::ostringstream ost;
    // FLAG addFlags(r2 , 99)
    Assign fc(
        new Terminal(opFlags),
        new Binary (opFlagCall,
            new Const("addFlags"),
            new Binary(opList,
                Location::regOf(2),
                new Const(99))));
    CallStatement* call = new CallStatement;
    BranchStatement* br = new BranchStatement;
    Assign* as = new Assign(
        Location::regOf(9),
        new Binary(opPlus,
            Location::regOf(10),
            new Const(4)));
    fc.print(ost);
    std::string expected("   0 *32* %flags := addFlags( r2, 99 )");
    std::string actual(ost.str());
    CPPUNIT_ASSERT_EQUAL(expected, actual);
    CPPUNIT_ASSERT (    fc.isFlagAssgn());
    CPPUNIT_ASSERT (!call->isFlagAssgn());
    CPPUNIT_ASSERT (!  br->isFlagAssgn());
    CPPUNIT_ASSERT (!  as->isFlagAssgn());
    delete call; delete br;
}

