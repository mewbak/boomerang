#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>

#include "exp.h"

class ExpTest : public CppUnit::TestCase {
  protected:
    Const*  m_99;
    Unary*  m_rof2;

  public:
    ExpTest(std::string name) : CppUnit::TestCase (name)
    {}

    virtual void registerTests(CppUnit::TestSuite* suite);

    int countTestCases () const;

    void setUp ();
    void tearDown ();

    void test99 ();
    void testFlt ();
    void testRegOf2 ();

    void testPlus ();
    void testMinus ();
    void testMult ();
    void testDiv ();
    void testMults ();
    void testDivs ();
    void testMod ();
    void testMods ();

    void testIsAssign();
    void testIsAfpTerm();
    void testIsFlagCall();

    void testCompare1();
    void testCompare2();
    void testCompare3();
    void testCompare4();
    void testCompare5();
    void testCompare6();

    void testSearchReplace1();
    void testSearchReplace2();
    void testSearchReplace3();
    void testSearchReplace4();

    void testSearch1();
    void testSearch2();
    void testSearch3();
    void testSearchAll();

    void testPartitionTerms();
    void testAccumulate();
    void testSimplifyArith();
    void testSimplifyUnary();
    void testSimplifyBinary();

    void testBecome();
    void testLess();
    void testMapOfExp();

    void testDecideType();
    void testList();
    void testClone();
    void testParen();
};

