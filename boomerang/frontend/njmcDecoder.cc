/*
 * Copyright (C) 1998-2001, The University of Queensland
 * Copyright (C) 2001, Sun Microsystems, Inc
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       njmcDecoder.cc
 * OVERVIEW:   This file contains the machine independent
 *             decoding functionality.
 *
 * $Revision: 1.1 $
 *============================================================================*/ 
/*
 * 27 Apr 02 - Mike: Mods for boomerang
 */

#include <stdarg.h>         // For varargs
#include "proc.h"
#include "decoder.h"
#include "exp.h"
#include "rtl.h"
#include "prog.h"
#include "BinaryFile.h"

/**********************************
 * NJMCDecoder methods.
 **********************************/   

// Note: can't actually put the constructor here! Gcc notes that all non
// pure (=0) virtual fuctions have to be defined, therefore, it's OK to
// pick one (they choose the first one) and emit the vtable, default constructor
// and constructor, etc, only when they see this first virtual function
// defined. In our case, it's in a dynamic library, so by not defining the
// constructor, we don't have a need for the vtable, and the thing links
// properly.
// Note that the constructor is still defined; jsut that it is defined in the
// library where the first virtual function is also defined.
#if 0
/*==============================================================================
 * FUNCTION:       NJMCDecoder::NJMCDecoder
 * OVERVIEW:       
 * PARAMETERS:     None
 * RETURNS:        N/A
 *============================================================================*/
NJMCDecoder::NJMCDecoder()
{}
#endif

/*==============================================================================
 * FUNCTION:       NJMCDecoder::instantiate
 * OVERVIEW:       Given an instruction name and a variable list of SemStr's
 *                 representing the actual operands of the instruction, use the
 *                 RTL template dictionary to return the instantiated RTL
 *                 representing the semantics of the instruction.
 * PARAMETERS:     name - instruction name
 *                 ... - Semantic String ptrs representing actual operands
 * RETURNS:        an instantiated list of Exps
 *============================================================================*/
std::list<Exp*>* NJMCDecoder::instantiate(ADDRESS pc, const char* name, ...)
{
	// Get the signature of the instruction and extract its parts
	std::pair<std::string,unsigned> sig = prog.RTLDict.getSignature(name);
	std::string opcode = sig.first;
	unsigned numOperands = sig.second;

	// Put the operands into an vector
	std::vector<Exp*> actuals(numOperands);
	va_list args;
	va_start(args,name);
	for (unsigned i = 0; i < numOperands; i++)
		actuals[i] = va_arg(args,Exp*);
	va_end(args);

#ifdef DEBUG_DECODER
	// Display a disassembly of this instruction if necessary
	cout << hex << pc << dec << ": " << name;
	for (std::vector<Exp*>::iterator it = actuals.begin();
		it != actuals.end(); it++)
		cout << " " << **it;
	cout << endl;
#endif

	std::list<Exp*>* instance = prog.RTLDict.instantiateRTL(opcode,actuals);

	// Delete the memory used for the actuals
	for (std::vector<Exp*>::iterator it = actuals.begin();
		it != actuals.end(); it++)
		delete *it;

	return instance;
}

/*==============================================================================
 * FUNCTION:       NJMCDecoder::substituteCallArgs
 * OVERVIEW:       Similarly to the above, given a parameter name
 *                  and a list of Exp*'s representing sub-parameters,
 *                  return a fully substituted Exp for the whole expression
 * NOTE:           Caller must delete result
 * PARAMETERS:     name - parameter name
 *                 ... - Exp* representing actual operands
 * RETURNS:        an instantiated list of Exps
 *============================================================================*/
Exp* NJMCDecoder::instantiateNamedParam(char* name, ...)
{
    if( prog.RTLDict.ParamSet.find(name) == prog.RTLDict.ParamSet.end() ) {
        std::cerr << "No entry for named parameter '" << name << "'\n";
        return 0;
    }
    ParamEntry &ent = prog.RTLDict.DetParamMap[name];
    if (ent.kind != PARAM_EXPR && ent.kind != PARAM_LAMBDA ) {
        std::cerr << "Attempt to instantiate expressionless parameter '" << name
          << "'\n";
        return 0;
    }
    // Start with the RHS
// It may well be opTypedExp, in which case we need another getSubExp1()
assert(ent.exp->getOper() == opAssign);
    Exp* result = ent.exp->getSubExp2()->clone();

    va_list args;
    va_start(args,name);
    for( std::list<std::string>::iterator it = ent.params.begin();
      it != ent.params.end(); it++ ) {
        Exp* formal = new Unary(opParam, new Const((char*)it->c_str()));
        Exp* actual = va_arg(args, Exp*);
        bool change;
        result = result->searchReplaceAll(formal, actual, change);
        delete formal;
    }
    return result;
}

/*==============================================================================
 * FUNCTION:       NJMCDecoder::substituteCallArgs
 * OVERVIEW:       In the event that it's necessary to synthesize the call of
 *                 a named parameter generated with instantiateNamedParam(),
 *                 this substituteCallArgs() will substitute the arguments that
 *                 follow into the expression.
 * NOTE:           Should only be used after instantiateNamedParam(name, ..);
 * NOTE:           exp (the pointer) could be changed
 * PARAMETERS:     name - parameter name
 *                 exp - expression to instantiate into
 *                 ... - Exp* representing actual operands
 * RETURNS:        an instantiated list of Exps
 *============================================================================*/
void NJMCDecoder::substituteCallArgs(char *name, Exp*& exp, ...)
{
    if (prog.RTLDict.ParamSet.find(name) == prog.RTLDict.ParamSet.end()) {
        std::cerr << "No entry for named parameter '" << name << "'\n";
        return;
    }
    ParamEntry &ent = prog.RTLDict.DetParamMap[name];
    /*if (ent.kind != PARAM_EXPR && ent.kind != PARAM_LAMBDA) {
        std::cerr << "Attempt to instantiate expressionless parameter '" << name << "'\n";
        return;
    }*/
    
    va_list args;
    va_start(args, exp);
    for (std::list<std::string>::iterator it = ent.funcParams.begin();
         it != ent.funcParams.end(); it++) {
        Exp* formal = new Unary(opParam, new Const((char*)it->c_str()));
        Exp* actual = va_arg(args, Exp*);
        bool change;
        exp = exp->searchReplaceAll(formal, actual, change);
        delete formal;
    }
}

/*==============================================================================
 * FUNCTION:       DecodeResult::reset
 * OVERVIEW:       Resets the fields of a DecodeResult to their default values.
 * PARAMETERS:     <none>
 * RETURNS:        <nothing>
 *============================================================================*/
void DecodeResult::reset()
{
	numBytes = 0;
	type = NCT;
	valid = true;
	rtl = NULL;
    forceOutEdge = 0;	
}

/*==============================================================================
 * These are functions used to decode instruction operands into
 * Exp*s.
 *============================================================================*/

/*==============================================================================
 * FUNCTION:        NJMCDecoder::dis_Reg
 * OVERVIEW:        Converts a numbered register to a suitable expression.
 * PARAMETERS:      reg - the register number, e.g. 0 for eax
 * RETURNS:         the Exp* for the register NUMBER (e.g. "int 36" for %f4)
 *============================================================================*/
Exp* NJMCDecoder::dis_Reg(int regNum)
{
      Exp* expr = new Unary(opRegOf, new Const(regNum));
      return expr;
}

/*==============================================================================
 * FUNCTION:        NJMCDecoder::dis_Num
 * OVERVIEW:        Converts a number to a Exp* expression.
 * PARAMETERS:      num - a number
 * RETURNS:         the Exp* representation of the given number
 *============================================================================*/
Exp* NJMCDecoder::dis_Num(unsigned num)
{
	Exp* expr = new Const((int)num);
	return expr;
}

/*==============================================================================
 * FUNCTION:        NJMCDecoder::unconditionalJump
 * OVERVIEW:        Process an unconditional jump instruction
 *                  Also check if the destination is a label
 * PARAMETERS:      <none>
 * RETURNS:         the reference to the RTLInstDict object
 *============================================================================*/
void NJMCDecoder::unconditionalJump(const char* name, int size,
  ADDRESS relocd, /*UserProc* proc,*/ int delta, ADDRESS pc, std::list<Exp*>* Exps,
  DecodeResult& result) {
    ADDRESS dest = relocd-delta;
    // Check for a pointer to a label, but not a branch that happens to be
    // to the top of this function (this would a rare function with no
    // prologue).
    const char* fname = prog.pBF->SymbolByAddress(dest);
    // FIXME: The test for a branch to the top of a function (recursive call
    // followed by a return) has to be done somewhere else
    if ((fname == 0) /*|| (proc == 0) || (proc->getNativeAddress() == dest)*/) {
        HLJump* jump = new HLJump(pc, Exps);
        result.rtl = jump;
        result.numBytes = size;
        jump->setDest(dest);
        SHOW_ASM(name<<" "<<relocd)
    } else {
        // The jump is to another function. Handle this as a call/return pair
        HLCall* call = new HLCall(pc, 0, Exps);
        result.rtl = call;
        result.numBytes = size;
        call->setDest(dest);
        call->setReturnAfterCall(true);
        SHOW_ASM(name<<" "<<fname)
    }
}

#if 0       // HACK!
/*==============================================================================
 * FUNCTION:        NJMCDecoder::decodeInstruction
 * OVERVIEW:        Dummy function
 * NOTE:            Must be overridden in a derived class (e.g. in
 *                    ../lib/libsparc.so)
 * PARAMETERS:      Not actually used!
 * RETURNS:         N/A
 *============================================================================*/
DecodeResult dummy;
DecodeResult& decodeInstruction (ADDRESS pc, int delta)
{   assert(0);
    return dummy;
}
#endif
