/*==============================================================================
 * FILE:       CTest.cc
 * OVERVIEW:   Provides the implementation for the CTest class, which
 *              tests the c parser
 *============================================================================*/
/*
 * $Revision: 1.2 $
 *
 * 03 Dec 02 - Trent: Created
 */

#include <sstream>
#include "CTest.h"

/*==============================================================================
 * FUNCTION:        CTest::registerTests
 * OVERVIEW:        Register the test functions in the given suite
 * PARAMETERS:      Pointer to the test suite
 * RETURNS:         <nothing>
 *============================================================================*/
#define MYTEST(name) \
suite->addTest(new CppUnit::TestCaller<CTest> ("testC", \
    &CTest::name, *this))

void CTest::registerTests(CppUnit::TestSuite* suite) {

    MYTEST(test1);
}

int CTest::countTestCases () const
{ return 1; }   // ? What's this for?

/*==============================================================================
 * FUNCTION:        CTest::setUp
 * OVERVIEW:        Set up anything needed before all tests
 * NOTE:            Called before any tests
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void CTest::setUp () {
}

/*==============================================================================
 * FUNCTION:        CTest::tearDown
 * OVERVIEW:        Delete objects created in setUp
 * NOTE:            Called after all tests
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void CTest::tearDown () {
}

/*==============================================================================
 * FUNCTION:        CTest::test1
 * OVERVIEW:        Test
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void CTest::test1 () {
    std::istringstream os("int printf(char *fmt);");
    AnsiCParser *p = new AnsiCParser(os);
    p->yyparse();
    CPPUNIT_ASSERT(p->signatures.size() == 1);
    Signature *sig = p->signatures.front();
    CPPUNIT_ASSERT(!strcmp(sig->getName(),"printf"));
    CPPUNIT_ASSERT(*sig->getReturnType() == IntegerType());
}

