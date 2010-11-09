/****************************************************************
*
* FILENAME
*
*   \file mipsfrontend.h
*
* PURPOSE 
*
*   Skeleton for MIPS disassembly.
*
* AUTHOR 
*
*   \author Markus Gothe, nietzsche@lysator.liu.se
*
* REVISION 
*
*   $Id: mipsdecoder.h,v 1.1 2007-11-18 16:52:12 thenihilist Exp $
*
*****************************************************************/

#ifndef MIPSDECODER
#define MIPSDECODER

class Prog;
class NJMCDecoder;
struct DecodeResult;

class MIPSDecoder : public NJMCDecoder
{
public:
	/* Default constructor
	 */
	MIPSDecoder(Prog *prog);

	/*
	 * Decodes the machine instruction at pc and returns an RTL instance for
	 * the instruction.
	 */
virtual DecodeResult& decodeInstruction(ADDRESS pc, intptr_t delta);

	/*
	 * Disassembles the machine instruction at pc and returns the number of
	 * bytes disassembled. Assembler output goes to global _assembly
	 */
virtual int decodeAssemblyInstruction(ADDRESS pc, intptr_t delta);


private:
		/*
		 * Various functions to decode the operands of an instruction into an Exp* representation.
		 */
#if 0		
		Exp*		dis_Eaddr(ADDRESS pc, int size = 0);
		Exp*		dis_RegImm(ADDRESS pc);
		Exp*		dis_Reg(unsigned r);
		Exp*		dis_RAmbz(unsigned r);		// Special for rA of certain instructions
#endif
		void		unused(int x);
#if 0
		RTL*		createBranchRtl(ADDRESS pc, std::list<Statement*>* stmts, const char* name);
		bool		isFuncPrologue(ADDRESS hostPC);
		DWord		getDword(ADDRESS lc);
#endif
};

#endif
