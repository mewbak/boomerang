/*==============================================================================
 * FILE:       testAnalysis.cc
 * OVERVIEW:   Command line test of the analysis functions
 *============================================================================*/
/*
 * $Revision: 1.1 $
 * 10 Jul 02 - Mike: Created
 */


#include "cppunit/TextTestResult.h"
#include "cppunit/TestSuite.h"

#include "AnalysisTest.h"
#include "prog.h"
#include <iostream>

// The dreaded Prog global
Prog prog;

int main(int argc, char** argv)
{
    CppUnit::TestSuite suite;

    AnalysisTest at("AnalysisTest");

    at.registerTests(&suite);

    CppUnit::TextTestResult res;

    suite.run( &res );
    std::cout << res << std::endl;

    return 0;
}

