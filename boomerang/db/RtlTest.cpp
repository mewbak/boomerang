/*==============================================================================
 * FILE:       RtlTest.cc
 * OVERVIEW:   Provides the implementation for the RtlTest class, which
 *              tests the RTL and derived classes
 *============================================================================*/
/*
 * $Revision: 1.1 $
 *
 * 13 May 02 - Mike: Created
 */

#include "RtlTest.h"
#include "exp.h"
#include <sstream>

/*==============================================================================
 * FUNCTION:        RtlTest::registerTests
 * OVERVIEW:        Register the test functions in the given suite
 * PARAMETERS:      Pointer to the test suite
 * RETURNS:         <nothing>
 *============================================================================*/
#define MYTEST(name) \
suite->addTest(new CppUnit::TestCaller<RtlTest> ("RtlTest", \
    &RtlTest::name, *this))

void RtlTest::registerTests(CppUnit::TestSuite* suite) {
    MYTEST(testAppend);
    MYTEST(testClone);
}

int RtlTest::countTestCases () const
{ return 2; }   // ? What's this for?

/*==============================================================================
 * FUNCTION:        RtlTest::setUp
 * OVERVIEW:        Set up some expressions for use with all the tests
 * NOTE:            Called before any tests
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void RtlTest::setUp () {
}

/*==============================================================================
 * FUNCTION:        RtlTest::tearDown
 * OVERVIEW:        Delete expressions created in setUp
 * NOTE:            Called after all tests
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void RtlTest::tearDown () {
}

/*==============================================================================
 * FUNCTION:        RtlTest::testAppend
 * OVERVIEW:        Test appendExp and printing of RTLs
 *============================================================================*/
void RtlTest::testAppend () {
    AssignExp* e = new AssignExp(32,
            new Unary(opRegOf, new Const(8)),
            new Binary(opPlus,
                new Unary(opRegOf, new Const(9)),
                new Const(99)));
    RTL r;
    r.appendExp(e);
    std::ostringstream ost;
    r.print(ost);
    std::string actual(ost.str());
    std::string expected("00000000 *32* r[8] := r[9] + 99\n");
    CPPUNIT_ASSERT_EQUAL(expected, actual);
    // No! appendExp does not copy the expression, so deleting the RTL will
    // delete the expression(s) in it.
    // Not sure if that's what we want...
    // delete e;
}

/*==============================================================================
 * FUNCTION:        RtlTest::testClone
 * OVERVIEW:        Test constructor from list of expressions; cloning of RTLs
 *============================================================================*/
void RtlTest::testClone () {
    AssignExp* e1 = new AssignExp(32,
            new Unary(opRegOf, new Const(8)),
            new Binary(opPlus,
                new Unary(opRegOf, new Const(9)),
                new Const(99)));
    AssignExp* e2 = new AssignExp(16,
            new Unary(opParam, new Const("x")),
            new Unary(opParam, new Const("y")));
    std::list<Exp*> le;
    le.push_back(e1);
    le.push_back(e2);
    RTL* r = new RTL(0x1234, &le);
    RTL* r2 = r->clone();
    std::ostringstream o1, o2;
    r->print(o1);
    delete r;           // And r2 should still stand!
    r2->print(o2);
    delete r2;
    std::string expected("00001234 *32* r[8] := r[9] + 99\n         *16* x := y\n");
    std::string a1(o1.str());
    std::string a2(o2.str());
    CPPUNIT_ASSERT_EQUAL(expected, a1);
    CPPUNIT_ASSERT_EQUAL(expected, a2);
}
