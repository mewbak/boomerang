#ifndef PENTFRONTEND_H
#define PENTFRONTEND_H

#include "frontend.h"


// Class PentiumFrontEnd: derived from FrontEnd, with source machine specific
// behaviour

class PentiumDecoder;

class PentiumFrontEnd : public FrontEnd
{
private:
	// the decoder
	PentiumDecoder *decoder;

public:
    /*
     * Constructor. Takes some parameters to save passing these around a lot
     */
    PentiumFrontEnd(int delta, ADDRESS uUpper);

virtual ~PentiumFrontEnd();

virtual const char *getFrontEndId() { return "pentium"; }

    /*
     * processProc. This is the main function for decoding a procedure.
     * This overrides the base class processProc to do source machine
     * specific things (but often calls the base class to do most of the
     * work. Sparc is an exception)
     * If spec is true, this is a speculative decode (so give up on any invalid
     * instruction)
     * Returns true on a good decode
     */
virtual bool    processProc(ADDRESS uAddr, UserProc* pProc, std::ofstream &os,
                bool spec = false, PHELPER helperFunc = NULL);

virtual ADDRESS getMainEntryPoint( bool &gotMain );

virtual NJMCDecoder *getDecoder();

private:

    /*
     * Process an F(n)STSW instruction.
     */
	bool 	processStsw(std::list<RTL*>::iterator& rit, std::list<RTL*>* pRtls, PBB pBB, Cfg* pCfg);

    /*
     * Emit a set instruction.
     */
        void 	emitSet(std::list<RTL*>* pRtls, std::list<RTL*>::iterator& itRtl, ADDRESS uAddr,
  			Exp* pLHS, Exp* cond);

    /*
     * Handle the case of being in state 23 and encountering a set instruction.
     */
    	void 	State25(Exp* pLHS, Exp* pRHS, std::list<RTL*>* pRtls, std::list<RTL*>::iterator& rit,
    			ADDRESS uAddr);

	int idPF;              // Parity flag

    /*
     * Process a BB and its successors for floating point code
     */
	void 	processFloatCode(PBB pBB, int& tos, Cfg* pCfg);

    /*
     * Check a HLCall for a helper function, and replace with appropriate
     *  semantics if possible
     */
	bool 	helperFunc(ADDRESS dest, ADDRESS addr, std::list<RTL*>* lrtl);

	bool 	isStoreFsw(Exp* e);
	bool 	isDecAh(RTL* r);
	bool 	isSetX(Exp* e);
	bool 	isAssignFromTern(Exp* e);
	Exp* 	bumpRegisterAll(Exp* e, int min, int max, int delta, int mask);
	unsigned fetch4(unsigned char* ptr);


};

#endif
