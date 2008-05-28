#include <cppunit/extensions/HelperMacros.h>

class UtilTest : public CPPUNIT_NS::TestFixture {
  CPPUNIT_TEST_SUITE( UtilTest );
  CPPUNIT_TEST( testTypeLong );
  CPPUNIT_TEST( testNotEqual );
  CPPUNIT_TEST_SUITE_END();

  public:
    void setUp ();
    void tearDown ();

  protected:
    void testTypeLong ();
    void testNotEqual ();
};

