/*==============================================================================
 * FILE:       testFront.cc
 * OVERVIEW:   Command line test of the Frontend and related classes.
 *============================================================================*/
/*
 * $Revision: 1.1 $
 * 08 Apr 02 - Mike: Created
 * 23 May 02 - Mike: Added pentium tests
 */


#include "cppunit/TextTestResult.h"
#include "cppunit/TestSuite.h"

#include "FrontSparcTest.h"
#include "FrontPentTest.h"
//#include "FrontendTest.h"
#include "prog.h"           // For the global prog (ugh)
#include <iostream>

int main(int argc, char** argv)
{
    CppUnit::TestSuite suite;

    FrontSparcTest fst("FrontSparcTest");
//    FrontendTest fet("FrontendTest");
    FrontPentTest fpt("FrontPentTest");

    fst.registerTests(&suite);
    fpt.registerTests(&suite);

    CppUnit::TextTestResult res;

    prog.readLibParams();        // Read library signatures (once!)
    suite.run( &res );
    std::cout << res << std::endl;

    return 0;
}

