#include <cppunit/TestCaller.h>
#include <cppunit/TestCase.h>
#include <cppunit/TestSuite.h>
#include "BinaryFile.h"

class FrontEnd;
class NJMCDecoder;

class FrontPentTest : public CppUnit::TestCase {
  protected:
    // The FrontEndSrc object
    FrontEnd* fe;
    // The decoder object
    NJMCDecoder* decoder;
    // Handle set by getInstanceFor()
    void* dlHandle;

  public:
    FrontPentTest(std::string name) : CppUnit::TestCase (name)
    {}

    virtual void registerTests(CppUnit::TestSuite* suite);

    int countTestCases () const;

    void setUp ();
    void tearDown ();

    void test1 ();
    void test2 ();
    void test3 ();
    void testBranch();
};

