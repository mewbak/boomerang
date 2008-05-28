#include <cppunit/extensions/HelperMacros.h>

class LoaderTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE( LoaderTest );
  CPPUNIT_TEST( testSparcLoad );
  CPPUNIT_TEST( testPentiumLoad );
  CPPUNIT_TEST( testHppaLoad );
  CPPUNIT_TEST( testPalmLoad );
  CPPUNIT_TEST( testWinLoad );
  CPPUNIT_TEST( testMicroDis1 );
  CPPUNIT_TEST( testMicroDis2 );
  CPPUNIT_TEST( testElfHash );
  CPPUNIT_TEST_SUITE_END();

  public:
    void setUp ();
    void tearDown ();

  protected:
    void testSparcLoad ();
    void testPentiumLoad ();
    void testHppaLoad ();
    void testPalmLoad ();
    void testWinLoad ();

    void testMicroDis1();
    void testMicroDis2();

    void testElfHash();
};

