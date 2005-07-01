
#ifndef SPARCFRONTEND_H
#define SPARCFRONTEND_H

// Class SparcFrontEnd: derived from FrontEnd, with source machine specific
// behaviour

#include <set>
#include "decoder.h"
#include "exp.h"			// Ugh... just for enum OPER
#include "frontend.h"		// In case included bare, e.g. ProcTest.cpp

class FrontEnd;
class SparcDecoder;
struct DecodeResult;
class CallStatement;

class SparcFrontEnd : public FrontEnd
{
public:
	/*
	 * Constructor. Takes some parameters to save passing these around a lot
	 */
	SparcFrontEnd(BinaryFile *pBF, Prog* prog);

	/**
	 * Virtual destructor.
	 */
virtual ~SparcFrontEnd();

virtual platform getFrontEndId() { return PLAT_SPARC; }

	/*
	 * processProc. This is the main function for decoding a procedure.
	 * This overrides the base class processProc to do source machine
	 * specific things (but often calls the base class to do most of the
	 * work. Sparc is an exception)
	 * If frag is true, we are decoding only a fragment of the proc
	 * If spec is true, this is a speculative decode (so give up on any invalid
	 * instruction)
	 * Returns true on a good decode
	 */
virtual bool	processProc(ADDRESS uAddr, UserProc* pProc, std::ofstream &os,
	bool frag = false, bool spec = false);

virtual std::vector<Exp*> &getDefaultParams();
virtual std::vector<Exp*> &getDefaultReturns();

virtual ADDRESS getMainEntryPoint( bool &gotMain );

private:

	void	warnDCTcouple(ADDRESS uAt, ADDRESS uDest);
	bool	optimise_DelayCopy(ADDRESS src, ADDRESS dest, int delta,
			  ADDRESS uUpper);
	BasicBlock* optimise_CallReturn(CallStatement* call, RTL* rtl, RTL* delay, UserProc* pProc);

	void	handleBranch(ADDRESS dest, ADDRESS hiAddress, BasicBlock*& newBB, Cfg* cfg, TargetQueue& tq);
	void	handleCall(UserProc *proc, ADDRESS dest, BasicBlock* callBB, Cfg* cfg, ADDRESS address, int offset = 0);

	void	case_unhandled_stub(ADDRESS addr);

	bool	case_CALL(ADDRESS& address, DecodeResult& inst, DecodeResult& delay_inst, std::list<RTL*>*& BB_rtls,
				UserProc* proc, std::list<CallStatement*>& callList, std::ofstream &os, bool isPattern = false);

	void	case_SD(ADDRESS& address, int delta, ADDRESS hiAddress, DecodeResult& inst, DecodeResult& delay_inst,
				std::list<RTL*>*& BB_rtls, Cfg* cfg, TargetQueue& tq, std::ofstream &os);

	bool	case_DD(ADDRESS& address, int delta, DecodeResult& inst, DecodeResult& delay_inst,
				std::list<RTL*>*& BB_rtls, TargetQueue& tq, UserProc* proc, std::list<CallStatement*>& callList);

	bool	case_SCD(ADDRESS& address, int delta, ADDRESS hiAddress, DecodeResult& inst, DecodeResult& delay_inst,
				std::list<RTL*>*& BB_rtls, Cfg* cfg, TargetQueue& tq);

	bool	case_SCDAN(ADDRESS& address, int delta, ADDRESS hiAddress, DecodeResult& inst, DecodeResult& delay_inst,
				   std::list<RTL*>*& BB_rtls, Cfg* cfg, TargetQueue& tq);

	void	emitNop(std::list<RTL*>* pRtls, ADDRESS uAddr);
	void	emitCopyPC(std::list<RTL*>* pRtls, ADDRESS uAddr);
	unsigned fetch4(unsigned char* ptr);
	void	appendAssignment(Exp* lhs, Exp* rhs, Type* type, ADDRESS addr, std::list<RTL*>* lrtl);
	void	quadOperation(ADDRESS addr, std::list<RTL*>* lrtl, OPER op);

	bool	helperFunc(ADDRESS dest, ADDRESS addr, std::list<RTL*>* lrtl);
	void	gen32op32gives64(OPER op, std::list<RTL*>* lrtl, ADDRESS addr);
	bool	helperFuncLong(ADDRESS dest, ADDRESS addr, std::list<RTL*>* lrtl, std::string& name);
	//void	setReturnLocations(CalleeEpilogue* epilogue, int iReg);

	// This struct represents a single nop instruction. Used as a substitute
	// delay slot instruction
	DecodeResult nop_inst;
	
};

#endif
