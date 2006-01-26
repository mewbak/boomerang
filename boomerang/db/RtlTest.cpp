/*==============================================================================
 * FILE:	   RtlTest.cc
 * OVERVIEW:   Provides the implementation for the RtlTest class, which
 *				tests the RTL and derived classes
 *============================================================================*/
/*
 * $Revision: 1.22 $
 *
 * 13 May 02 - Mike: Created
 */

#include "RtlTest.h"
#include "statement.h"
#include "exp.h"
#include <sstream>
#include "BinaryFile.h"
#include "frontend.h"
#include "sparcfrontend.h"
#include "pentiumfrontend.h"
#include "decoder.h"
#include "proc.h"
#include "prog.h"
#include "visitor.h"

#define SWITCH_SPARC		"test/sparc/switch_cc"
#define SWITCH_PENT			"test/pentium/switch_cc"

/*==============================================================================
 * FUNCTION:		RtlTest::registerTests
 * OVERVIEW:		Register the test functions in the given suite
 * PARAMETERS:		Pointer to the test suite
 * RETURNS:			<nothing>
 *============================================================================*/
#define MYTEST(name) \
suite->addTest(new CppUnit::TestCaller<RtlTest> ("RtlTest", \
	&RtlTest::name, *this))

void RtlTest::registerTests(CppUnit::TestSuite* suite) {
	MYTEST(testAppend);
	MYTEST(testClone);
	MYTEST(testVisitor);
	MYTEST(testIsCompare);
	MYTEST(testSetConscripts);
}

int RtlTest::countTestCases () const
{ return 2; }	// ? What's this for?

/*==============================================================================
 * FUNCTION:		RtlTest::setUp
 * OVERVIEW:		Set up some expressions for use with all the tests
 * NOTE:			Called before any tests
 * PARAMETERS:		<none>
 * RETURNS:			<nothing>
 *============================================================================*/
void RtlTest::setUp () {
}

/*==============================================================================
 * FUNCTION:		RtlTest::tearDown
 * OVERVIEW:		Delete expressions created in setUp
 * NOTE:			Called after all tests
 * PARAMETERS:		<none>
 * RETURNS:			<nothing>
 *============================================================================*/
void RtlTest::tearDown () {
}

/*==============================================================================
 * FUNCTION:		RtlTest::testAppend
 * OVERVIEW:		Test appendExp and printing of RTLs
 *============================================================================*/
void RtlTest::testAppend () {
	Assign* a = new Assign(
			Location::regOf(8),
			new Binary(opPlus,
				Location::regOf(9),
				new Const(99)));
	RTL r;
	r.appendStmt(a);
	std::ostringstream ost;
	r.print(ost);
	std::string actual(ost.str());
	std::string expected("00000000    0 *v* r8 := r9 + 99\n");
	CPPUNIT_ASSERT_EQUAL(expected, actual);
	// No! appendExp does not copy the expression, so deleting the RTL will
	// delete the expression(s) in it.
	// Not sure if that's what we want...
	// delete a;
}

/*==============================================================================
 * FUNCTION:		RtlTest::testClone
 * OVERVIEW:		Test constructor from list of expressions; cloning of RTLs
 *============================================================================*/
void RtlTest::testClone () {
	Assign* a1 = new Assign(
			Location::regOf(8),
			new Binary(opPlus,
				Location::regOf(9),
				new Const(99)));
	Assign* a2 = new Assign(new IntegerType(16),
			new Location(opParam, new Const("x"), NULL),
			new Location(opParam, new Const("y"), NULL));
	std::list<Statement*> ls;
	ls.push_back(a1);
	ls.push_back(a2);
	RTL* r = new RTL(0x1234, &ls);
	RTL* r2 = r->clone();
	std::ostringstream o1, o2;
	r->print(o1);
	delete r;			// And r2 should still stand!
	r2->print(o2);
	delete r2;
	std::string expected("00001234    0 *v* r8 := r9 + 99\n"
						 "            0 *j16* x := y\n");

	std::string act1(o1.str());
	std::string act2(o2.str());
	CPPUNIT_ASSERT_EQUAL(expected, act1);
	CPPUNIT_ASSERT_EQUAL(expected, act2);
}

/*==============================================================================
 * FUNCTION:		RtlTest::testVisitor
 * OVERVIEW:		Test the accept function for correct visiting behaviour.
 * NOTES:			Stub class to test.
 *============================================================================*/

class StmtVisitorStub : public StmtVisitor {
public:
	bool a, b, c, d, e, f, g, h; 

	void clear() { a = b = c = d = e = f = g = h = false; }
	StmtVisitorStub() { clear(); }
	virtual ~StmtVisitorStub() { }
	virtual bool visit(			   RTL *s) { a = true; return false; }
	virtual bool visit(	 GotoStatement *s) { b = true; return false; }
	virtual bool visit(BranchStatement *s) { c = true; return false; }
	virtual bool visit(	 CaseStatement *s) { d = true; return false; }
	virtual bool visit(	 CallStatement *s) { e = true; return false; }
	virtual bool visit(ReturnStatement *s) { f = true; return false; }
	virtual bool visit(	  BoolAssign *s) { g = true; return false; }
	virtual bool visit(			Assign *s) { h = true; return false; }
};

void RtlTest::testVisitor()
{
	StmtVisitorStub* visitor = new StmtVisitorStub();

	/* rtl */
	RTL *rtl = new RTL();
	rtl->accept(visitor);
	CPPUNIT_ASSERT(visitor->a);
	delete rtl;

	/* jump stmt */
	GotoStatement *jump = new GotoStatement;
	jump->accept(visitor);
	CPPUNIT_ASSERT(visitor->b);
	delete jump;

	/* branch stmt */
	BranchStatement *jcond = new BranchStatement;
	jcond->accept(visitor);
	CPPUNIT_ASSERT(visitor->c);
	delete jcond;

	/* nway jump stmt */
	CaseStatement *nwayjump = new CaseStatement;
	nwayjump->accept(visitor);
	CPPUNIT_ASSERT(visitor->d);
	delete nwayjump;

	/* call stmt */
	CallStatement *call = new CallStatement;
	call->accept(visitor);
	CPPUNIT_ASSERT(visitor->e);
	delete call;

	/* return stmt */
	ReturnStatement *ret = new ReturnStatement;
	ret->accept(visitor);
	CPPUNIT_ASSERT(visitor->f);
	delete ret;

	/* "bool" assgn */
	BoolAssign *scond = new BoolAssign(0);
	scond->accept(visitor);
	CPPUNIT_ASSERT(visitor->g);
	delete scond;

	/* assignment stmt */
	Assign *as = new Assign;
	as->accept(visitor);
	CPPUNIT_ASSERT(visitor->h);
	delete as;

	/* polymorphic */
	Statement* s = new CallStatement;
	s->accept(visitor);
	CPPUNIT_ASSERT(visitor->e);
	delete s;

	/* cleanup */
	delete visitor;
}

/*==============================================================================
 * FUNCTION:		RtlTest::testIsCompare
 * OVERVIEW:		Test the isCompare function
 *============================================================================*/
void RtlTest::testIsCompare () {
	BinaryFileFactory bff;
	BinaryFile *pBF = bff.Load(SWITCH_SPARC);
	CPPUNIT_ASSERT(pBF != 0);
	CPPUNIT_ASSERT(pBF->GetMachine() == MACHINE_SPARC);
	Prog* prog = new Prog;
	FrontEnd *pFE = new SparcFrontEnd(pBF, prog, &bff);
	prog->setFrontEnd(pFE);

	// Decode second instruction: "sub		%i0, 2, %o1"
	int iReg;
	Exp* eOperand = NULL;
	DecodeResult inst = pFE->decodeInstruction(0x10910);
	CPPUNIT_ASSERT(inst.rtl != NULL);
	CPPUNIT_ASSERT(inst.rtl->isCompare(iReg, eOperand) == false);
	
	// Decode fifth instruction: "cmp		%o1, 5"
	inst = pFE->decodeInstruction(0x1091c);
	CPPUNIT_ASSERT(inst.rtl != NULL);
	CPPUNIT_ASSERT(inst.rtl->isCompare(iReg, eOperand) == true);
	CPPUNIT_ASSERT_EQUAL(9, iReg);
	std::string expected("5");
	std::ostringstream ost1;
	eOperand->print(ost1);
	std::string actual(ost1.str());
	CPPUNIT_ASSERT_EQUAL(expected, actual);

	pBF->UnLoad();
	delete pBF;
	delete pFE;
	pBF = bff.Load(SWITCH_PENT);
	CPPUNIT_ASSERT(pBF != 0);
	CPPUNIT_ASSERT(pBF->GetMachine() == MACHINE_PENTIUM);
	pFE = new PentiumFrontEnd(pBF, prog, &bff);
	prog->setFrontEnd(pFE);

	// Decode fifth instruction: "cmp	$0x5,%eax"
	inst = pFE->decodeInstruction(0x80488fb);
	CPPUNIT_ASSERT(inst.rtl != NULL);
	CPPUNIT_ASSERT(inst.rtl->isCompare(iReg, eOperand) == true);
	CPPUNIT_ASSERT_EQUAL(24, iReg);
	std::ostringstream ost2;
	eOperand->print(ost2);
	actual = ost2.str();
	CPPUNIT_ASSERT_EQUAL(expected, actual);
	
	// Decode instruction: "add		$0x4,%esp"
	inst = pFE->decodeInstruction(0x804890c);
	CPPUNIT_ASSERT(inst.rtl != NULL);
	CPPUNIT_ASSERT(inst.rtl->isCompare(iReg, eOperand) == false);
	pBF->UnLoad();
	delete pFE;
}

void RtlTest::testSetConscripts() {
	// m[1000] = m[1000] + 1000
	Statement* s1 = new Assign(
		Location::memOf(
			new Const(1000), 0),
		new Binary(opPlus,
		Location::memOf(
			new Const(1000), NULL),
		new Const(1000)));
	
	// "printf("max is %d", (local0 > 0) ? local0 : global1)
	CallStatement* s2 = new CallStatement();
	std::string name("printf");
	Proc* proc = new UserProc(new Prog(), name, 0x2000);	// Making a true LibProc is problematic
	s2->setDestProc(proc);
	s2->setCalleeReturn(new ReturnStatement);		// So it's not a childless call
	Exp* e1 = new Const("max is %d");
	Exp* e2 = new Ternary(opTern,
		new Binary(opGtr,
			Location::local("local0", NULL),
			new Const(0)),
		Location::local("local0", NULL),
		Location::global("global1", NULL));
	StatementList args;
	args.append(new Assign(Location::regOf(8), e1));
	args.append(new Assign(Location::regOf(9), e2));
	s2->setArguments(args);

	std::list<Statement*> list;
	list.push_back(s1);
	list.push_back(s2);
	RTL* rtl = new RTL(0x1000, &list);
	rtl->setConscripts(0, false);
	std::string expected(
		"00001000    0 *v* m[1000\\1\\] := m[1000\\2\\] + 1000\\3\\\n"
		"            0 CALL printf(\n"
		"                *v* r8 := \"max is %d\"\\4\\\n"
		"                *v* r9 := (local0 > 0\\5\\) ? local0 : global1\n"
		"              )\n"
		"              Reaching definitions: \n"
		"              Live variables: \n");

	std::ostringstream ost;
	rtl->print(ost);
	std::string actual = ost.str();
	CPPUNIT_ASSERT_EQUAL(expected, actual);
}
