/*==============================================================================
 * FILE:       testAll.cc
 * OVERVIEW:   Command line test of all of Boomerang
 *============================================================================*/
/*
 * $Revision: 1.7 $
 * 15 Jul 02 - Mike: Created from testDbase
*/


#include "cppunit/TextTestResult.h"
#include "cppunit/TestSuite.h"

#include "exp.h"

#include "ExpTest.h"
#include "ProgTest.h"
#include "ProcTest.h"
#include "StatementTest.h"
#include "RtlTest.h"
#include "ParserTest.h"
#include "TypeTest.h"
#include "FrontSparcTest.h"
#include "FrontPentTest.h"
#include "AnalysisTest.h"
#include "CTest.h"
#include "CfgTest.h"

#include "prog.h"

#include <sstream>
#include <iostream>

int main(int argc, char** argv)
{
//std::cerr << "Prog at " << std::hex << &prog << std::endl;
    CppUnit::TestSuite suite;

    ExpTest  expt("ExpTest");
    ProgTest progt("ProgTest");
    ProcTest proct("ProcTest");
    RtlTest rtlt("RtlTest");
    ParserTest parsert("ParserTest");
    TypeTest typet("TypeTest");
    FrontSparcTest fst("FrontSparcTest");
//    FrontendTest fet("FrontendTest");
    FrontPentTest fpt("FrontPentTest");
    AnalysisTest ant("AnalysisTest");
    CTest c("CTest");
    StatementTest dft("StatementTest");
    CfgTest cfgt("CfgTest");

    expt.registerTests(&suite);
    progt.registerTests(&suite);
    proct.registerTests(&suite);
    rtlt.registerTests(&suite);
    parsert.registerTests(&suite);
    typet.registerTests(&suite);
    fst.registerTests(&suite);
    fpt.registerTests(&suite);
    ant.registerTests(&suite);
    c.registerTests(&suite);
    dft.registerTests(&suite);
    cfgt.registerTests(&suite);

    CppUnit::TextTestResult res;

    suite.run( &res );
    std::cout << res << std::endl;

    return 0;
}
