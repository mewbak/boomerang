/*
 * Copyright (C) 1997-2001, The University of Queensland
 * Copyright (C) 2000-2001, Sun Microsystems, Inc
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       proc.cc
 * OVERVIEW:   Implementation of the Proc hierachy (Proc, UserProc, LibProc).
 *             All aspects of a procedure, apart from the actual code in the
 *             Cfg, are stored here
 *
 * Copyright (C) 1997-2001, The University of Queensland, BT group
 * Copyright (C) 2000-2001, Sun Microsystems, Inc
 *============================================================================*/

/*
 * $Revision: 1.1 $
 *
 * 14 Mar 02 - Mike: Fixed a problem caused with 16-bit pushes in richards2
 * 20 Apr 02 - Mike: Mods for boomerang
 */

/*==============================================================================
 * Dependencies.
 *============================================================================*/

#include <algorithm>        // For find()
#include "exp.h"
#include "cfg.h"
#include "proc.h"
#include "prog.h"


/************************
 * Proc methods.
 ***********************/

Proc::~Proc()
{}

/*==============================================================================
 * FUNCTION:        Proc::Proc
 * OVERVIEW:        Constructor with name, native address.
 * PARAMETERS:      name - Name of procedure
 *                  uNative - Native address of entry point of procedure
 * RETURNS:         <nothing>
 *============================================================================*/
Proc::Proc(std::string& name, ADDRESS uNative)
     : name(name), address(uNative), returnLocn(0)
{
}

/*==============================================================================
 * FUNCTION:        Proc::getName
 * OVERVIEW:        Returns the name of this procedure
 * PARAMETERS:      <none>
 * RETURNS:         the name of this procedure
 *============================================================================*/
const char* Proc::getName()
{
    return name.c_str();
}

/*==============================================================================
 * FUNCTION:        Proc::getNativeAddress
 * OVERVIEW:        Get the native address (entry point).
 * PARAMETERS:      <none>
 * RETURNS:         the native address of this procedure (entry point)
 *============================================================================*/
ADDRESS Proc::getNativeAddress()
{
    return address;
}

/*==============================================================================
 * FUNCTION:        Proc::getNumArgs
 * OVERVIEW:        
 * PARAMETERS:      
 * RETURNS:         
 *============================================================================*/
int Proc::getNumArgs()
{
    return parameters.size();
}

/*==============================================================================
 * FUNCTION:        Proc::getParameter
 * OVERVIEW:        Return the n'th parameter for this procedure.
 * NOTE:            Not efficient for large n. Presently used for n <= 2 (and
 *                    then only for Palm PilotMain)
 * PARAMETERS:      n - an index to a parameter
 * RETURNS:         the parameter at n or NULL if n is out of range
 *============================================================================*/
Exp* Proc::getParameter(unsigned int n)
{
    // Return the indexed arg type.
    if (n >= parameters.size())
        return NULL;
    std::list<TypedExp*>::iterator it = parameters.begin();
    for (unsigned i=0; i < n; i++)
        it++;
    return (*it);
}

/*==============================================================================
 * FUNCTION:      Proc::getParams
 * OVERVIEW:      Get the formal parameters of this procedure.
 * PARAMETERS:    <none>
 * RETURNS:       A reference to the list of parameters
 *============================================================================*/
std::list<TypedExp*>& Proc::getParams()
{
    return parameters;
}

/*==============================================================================
 * FUNCTION:      Proc::printParams
 * OVERVIEW:      Print the formal parameters of this procedure.
 * PARAMETERS:    os - the output stream to use
 * RETURNS:       <nothing>
 *============================================================================*/
void Proc::printParams(std::ostream& os)
{
    for (std::list<TypedExp*>::iterator it = parameters.begin();
      it != parameters.end(); it++) {
        if (it != parameters.begin())
            os << ", ";
        os << *it;
    }
}

#if 0
/*==============================================================================
 * FUNCTION:      Proc::storeParams
 * OVERVIEW:      store the type information of the parameters
 * PARAMETERS:    BBBlock inBlock - the place where type info for params 
 *                are stored
 * RETURNS:       <nothing>
 *============================================================================*/ 
void Proc::storeParams(BBBlock& inBlock)
{
    for (std::list<Exp*>::iterator it = parameters.begin();
      it != parameters.end(); it++) {
        // Always make procedure parameters to be 
        // the define type and with an address of 0

        // This only works for Register parameters. Need
        // a better solution later!
        std::list<int>::iterator semIt = it->indices.begin();
        // Should check bounds;    
        if (*semIt == idRegOf){
            semIt++;
            if (*semIt == idIntConst){
                semIt++;
                regParams.push_back(*semIt);
                semIt--;
            }
            semIt--;
        }
        else {
            // Negative one indicates that it's not
            // a register that we are passing
            regParams.push_back(-1);
        }
    
        typeLex inLex(&(it->indices), NULL, 0, 0);
        typeAnalysis testAnalysis(&inLex, &inBlock, INT_TYPE);
        testAnalysis.yyparse();
       

        // NOTE TO SELF: Should add this to an additional
        // data structure. This would allow matching up the call
        // site datastructure to this data structure and 
        // propagate between the functions
        //
        // Just save the pointers to the chain holding the 
        // register info in this data structure
 
        // (*it).storeUseDefineStruct(inBlock, 0, 0, DEFINE_TYPE);
    }
}
#endif

/*==============================================================================
 * FUNCTION:      Proc::printParamsAsC
 * OVERVIEW:      Print the formal parameters of this procedure, as C would
 *                  want them
 * PARAMETERS:    os - the output stream to use
 * RETURNS:       <nothing>
 *============================================================================*/
void Proc::printParamsAsC(std::ostream& os)
{
    bool first = true;
    for (std::list<TypedExp*>::iterator it = parameters.begin();
      it != parameters.end(); it++) {
        if (!first)
            os << ", ";
        first = false;
        // FIXME: Needs interaction with the particular back end
        os << (*it)->getType().getCtype();  // Type, e.g. "int32"
        os << " ";
        (*it)->printAsHL(os);               // Name, e.g. v2 or r8
    }
}

/*==============================================================================
 * FUNCTION:      Proc::isParam
 * OVERVIEW:      Return true if the given location is a parameter to this
 *                  proc
 * PARAMETERS:    loc: ref to a Exp* representing the location in question
 * RETURNS:       true if given location is a parameter
 *============================================================================*/
bool Proc::isParam(Exp* loc)
{
    std::list<TypedExp*>::iterator it;
    for (it = parameters.begin(); it != parameters.end(); it++)
        if (**it -= *loc)        // Compare, ignore sign
            return true;
    return false;
}

/*==============================================================================
 * FUNCTION:      Proc::printReturnTypeAsC
 * OVERVIEW:      Print the return type of this procedure, as C would want it
 * PARAMETERS:    os - the output stream to use
 * RETURNS:       <nothing>
 *============================================================================*/
void Proc::printReturnTypeAsC(std::ostream& os)
{
    if (returnLocn) {
        assert(returnLocn->getOper() == opTypedExp);
        os << ((TypedExp*)returnLocn)->getType().getCtype();
    }
    else
        os << "void";
}

/*==============================================================================
 * FUNCTION:      Proc::getReturnType
 * OVERVIEW:      Get the return type of this procedure, as a Type
 * PARAMETERS:    <none>
 * RETURNS:       The return type
 *============================================================================*/
Type& Proc::getReturnType()
{
    if (returnLocn) {
        assert(returnLocn->getOper() == opTypedExp);
        return (((TypedExp*)returnLocn)->getType());
    }
    else
        return *new Type(TVOID);
}

/*==============================================================================
 * FUNCTION:      Proc::getReturnLoc
 * OVERVIEW:      Get a pointer to the return location of this procedure,
 *                  as a TypedExp
 * PARAMETERS:    <none>
 * RETURNS:       A pointer to a TypedExp representing the return location
 *                  e.g. v[2], or NULL if none
 *============================================================================*/
TypedExp* Proc::getReturnLoc()
{
    return returnLocn;
}

/*==============================================================================
 * FUNCTION:        operator<<
 * OVERVIEW:        Output operator for a Proc object.
 * PARAMETERS:      os - output stream
 *                  proc -
 * RETURNS:         os
 *============================================================================*/
std::ostream& operator<<(std::ostream& os, Proc& proc)
{
    return proc.put(os);
}

/*==============================================================================
 * FUNCTION:       Proc::matchParams
 * OVERVIEW:       Adjust the given list of potential actual parameter
 *                   locations that are live at a call to this procedure to
 *                   match the formal parameters of this procedure.
 * NOTE:           This was previously a virtual function, implemented
 *                  separately for LibProc and UserProc
 * PARAMETERS:     actuals - an ordered list of locations of actual parameters
 *                 caller - Proc object for calling procedure (for message)
 *                 outgoing - ref to Parameters object which encapsulates the
 *                   PARAMETERS CALLER section of the .pal file
 * RETURNS:        <nothing>, but may add or delete elements from actuals
 *============================================================================*/
bool isInt(const Exp* ss) {
    assert(ss->getOper() == opTypedExp);
    return ((TypedExp*)ss)->getType().getType() == INTEGER;}
bool isFlt(const Exp* ss) {
    assert(ss->getOper() == opTypedExp);
    Type& ty = ((TypedExp*)ss)->getType();
    return (ty.getType() == FLOATP) && (ty.getSize() == 32);}
bool isDbl(const Exp* ss) {
    assert(ss->getOper() == opTypedExp);
    Type& ty = ((TypedExp*)ss)->getType();
    return (ty.getType() == FLOATP) && (ty.getSize() == 64);}

#if 0       // FIXME: Need to think about whether we have a Parameters class
void Proc::matchParams(std::list<Exp*>& actuals, UserProc& caller,
    const Parameters& outgoing) const
{
    int intSize = outgoing.getIntSize();    // Int size for the source machine

    int currSlot = -1;              // Current parameter slot number
    int currSize = 1;               // Size of current parameter, in slots
    int ordParam = 1;               // Param ordinal number (first=1, for msg)
    std::list<Exp*>::const_iterator it = parameters.begin();
    std::list<Exp*>::iterator ita = actuals.begin();
#if 0           // I believe this should be done later - MVE
    if (isAggregateUsed()) {
        // Need to match the aggregate parameter separately, before the main
        // loop
        if (ita == actuals.end())
            insertParams(1, actuals, ita, name, outgoing);
        else ita++;
        assert(it != parameters.end());
        it++;
        ordParam++;
    }
#endif
    // Loop through each formal parameter. There should be no gaps in the formal
    // parameters, because that's the job of missingParamCheck()
    int firstOff;
    for (; it != parameters.end(); it++) {
        // If the current formal is varargs, then leave the remaining actuals
        // as they are
        const Type& ty = it->getType();
        if (ty.getType() == VARARGS) return;

        // Note that we can't call outgoing.getParamSlot here because these are
        // *formal* parameters (could be different locations to outgoing params)
        // (Besides, it could be a library function with no parameter locations)
        currSlot += currSize;
        // Perform alignment, if needed. Note that it's OK to use the outgoing
        // parameters, as we assume that the alignment is the same for incoming
        outgoing.alignSlotNumber(currSlot, ty);
        currSize = ty.getSize() / 8 / intSize;  // Current size in slots
        // Ensure that small types still occupy one slot minimum
        if (currSize == 0) currSize = 1;
//cout << "matchParams: Proc " << name << ": formal " << *it << ", actual "; if (ita != actuals.end()) cout << *ita; cout << std::endl;  // HACK
        // We need to find the subset of actuals with the same slot number
        std::list<Exp*>::iterator itst = ita;      // Remember start of this set
        int numAct = 0;                         // The count of this set
        int actSlot, actSize = 0, nextActSlot;
        if (ita != actuals.end()) {
            actSize = 1;            // Example: int const 0
            nextActSlot = actSlot = outgoing.getParamSlot(*ita, actSize,
                ita == actuals.begin(), firstOff);
            ita++;
            numAct = 1;
        }
        while (ita != actuals.end()) {
            nextActSlot = outgoing.getParamSlot(*ita, actSize, false, firstOff);
            if (actSlot != nextActSlot) break;
            numAct++;
            ita++;
        }
        // if (actSize == 0) this means that we have run out of actual
        // parameters. If (currSlot < actSlot) it means that there is a gap
        // in the actual parameters. Either way, we need to insert one of the
        // dreaded "hidden" (actual)parameters appropriate to the formal
        // parameter (in size and type).
        if ((actSize == 0) || (currSlot < actSlot)) {
            const Exp** newActual = outgoing.getActParamLoc(ty, currSlot);
            actuals.insert(itst, *newActual);
            ita = itst;             // Still need to deal with this actual
            std::ostringstream ost;
            ost << "adding hidden parameter " << *newActual << 
              " to call to " << name;
            warning(str(ost));
            delete newActual;
            continue;               // Move to the next formal parameter
        }
        if (numAct > 1) {
            // This means that there are several actual parameters to choose
            // from, which all have the same slot number. This can happen in
            // architectures like pa-risc, where different registers are used
            // for different types of parameters, and they all could be live

            // The rules depend on the basic type. Integer parameters can
            // overlap (e.g. 68K, often pass one long to cover two shorts).
            // This doesn't happen with floats, because values don't concaten-
            // ate the same way. So the size can be used to choose the right
            // floating point location (e.g. pa-risc)
            LOC_TYPE basicType = it->getType().getType();
            std::list<Exp*>::iterator ch;  // Iterator to chosen item in actuals
            if (basicType != FLOATP)
                // Integer, pointer, etc. For now, assume all the same
                ch = find_if(itst, ita, isInt);
            else if (basicType == FLOATP) {
                int size = it->getType().getSize();
                if (size == 32)
                    ch = find_if(itst, ita, isFlt);
                else if (size == 64)
                    ch = find_if(itst, ita, isDbl);
                else assert(0);
            }
            if (ch == ita) {
                std::ostringstream ost;
                ost << "Parameter " << dec << ordParam << " of proc " << name <<
                  " has no actual parameter of suitable type (slot " <<
                  currSlot << ")";
                error(str(ost));
            } else {
                // Eliminate all entries in actuals from itst up to but not
                // including ita, except the ch one
                // In other words, of all the actual parameter witht the same
                // slot number, keep only ch
                for (; itst != ita; itst++)
                    if (itst != ch)
                        actuals.erase(itst);
            }
        }

        // Check that the sizes at least are compatible
        // For example, sometimes 2 ints are passed for a formal double or long
        if (currSize > actSize) {
            // Check for the 2 int case. itst would point to the first, and
            // ita (if not end) points to the second
            if ((actSize == 1) && (currSize == 2) && (ita != actuals.end()) &&
              (ita->getType().getSize() == itst->getType().getSize())) {
                // Let this through, by just skipping the second int
                // It's up to the back end to cope with this situation
                ita++;
            }
        }

        ordParam++;
    }
    // At this point, any excess actuals can be discarded
    actuals.erase(ita, actuals.end());
}
#endif

#if 0       // FIXME: Again, Parameters object used
/*==============================================================================
 * FUNCTION:        Proc::getParamTypeList
 * OVERVIEW:        Given a list of actual parameters, return a list of
 *                    Type objects representing the types that the actuals
 *                    need to be "cast to"
 * NOTE:            Have to take into account longs overlapping 2 shorts,
 *                    gaps for alignment, etc etc.
 * NOTE:            Caller must delete result
 * PARAMETERS:      actuals: list of actual parameters
 * RETURNS:         Ptr to a list of Types, same size as actuals
 *============================================================================*/
std::list<Type>* Proc::getParamTypeList(const std::list<Exp*>& actuals)
{
    std::list<Type>* result = new std::list<Type>;
    const Parameters& outgoing = prog.csrSrc.getOutgoingParamSpec();
    int intSize = outgoing.getIntSize();    // Int size for the source machine

    int currForSlot = -1;               // Current formal parameter slot number
    int currForSize = 1;                // Size of current formal, in slots
    int ordParam = 1;          // Actual param ordinal number (first=1, for msg)
    std::list<Exp*>::const_iterator it = parameters.begin();
    std::list<Exp*>::const_iterator ita = actuals.begin();
    std::list<Exp*>::const_iterator itaa;
    if (isAggregateUsed()) {
        // The first parameter is a DATA_ADDRESS
        result->push_back(Type(DATA_ADDRESS));
        if (it != parameters.end()) it++;
        if (ita != actuals.end()) ita++;
    }
    int firstOff;
    for (; it != parameters.end(); it++) {
        if (ita == actuals.end())
            // Run out of actual parameters. Can happen with varargs
            break;
        currForSlot += currForSize;
        // Perform alignment, if needed. Note that it's OK to use the outgoing
        // parameters, as we assume that the alignment is the same for incoming
        Type ty = it->getType();
        outgoing.alignSlotNumber(currForSlot, ty);
        currForSize = ty.getSize() / 8 / intSize;  // Current size in slots
        // Ensure that small types still occupy one slot minimum
        if (currForSize == 0) currForSize = 1;
        int actSize = 1;        // Default to 1 (e.g. int consts)
        // Look at the current actual parameter, to get its size
        if (ita->getFirstIdx() == idVar) {
            // Just use the size from the Exp*'s Type
            int bytes = ita->getType().getSize() / 8;
            if (bytes && (bytes < intSize)) {
                std::ostringstream ost;
                ost << "getParamTypelist: one of those evil sub-integer "
                    "parameter passings at call to " << name;
                warning(str(ost));
                actSize = 1;
            }
            else
                actSize = bytes / intSize;
        } else {
            // MVE: not sure that this is the best way to find the size
            outgoing.getParamSlot(*ita, actSize, ita == actuals.begin(),
              firstOff);
        }
        ita++;
        // If the current formal is varargs, that's a special case
        // Similarly, if all the arguments are unknown
        LOC_TYPE lt = ty.getType();
        if ((lt == VARARGS) || (lt == UNKNOWN)) {
            // We want to give all the remaining actuals their own type
            ita--;
            while (ita != actuals.end()) {
                result->push_back(ita->getType());
                ita++;
            }
            break;
        }
        // If the sizes are the same, then we can use the formal's type
        if (currForSize == actSize)
            result->push_back(ty);
        // Else there is an overlap. We get the type of the first formal,
        // and widen it for the number of formals that this actual covers
        else if (actSize > currForSize) {
            Type first = ty;
            int combinedSize = ty.getSize();
            while ((actSize > currForSize) && (it != parameters.end())) {
                currForSlot += currForSize;
                ty = (++it)->getType();
                outgoing.alignSlotNumber(currForSlot, ty);
                currForSize += ty.getSize() / 8 / intSize;
                combinedSize += ty.getSize();
            }
            if (actSize != currForSize) {
                // Something has gone wrong with the matching process
                std::ostringstream ost;
                ost << "getParamTypeList: Actual parameter " << dec << ordParam
                  << " does not match with formals in proc " << name;
                error(str(ost));
            }
            first.setSize(combinedSize);
            result->push_back(first);
        }
        // Could be overlapping parameters, e.g. two ints passed as a
        // double or long. ita points to the second int (unless end)
        else if ((actSize == 1) && (currForSize == 2) && (ita != actuals.end())
          && (itaa = ita, (*--itaa).getType() == ita->getType())) {
            // Let this through, with the type of the formal
            ita++;
            ordParam++;
            result->push_back(ty);
        }
        else {
            assert(actSize > currForSize);
        }
        ordParam++;
    }
    return result;
}
#endif

/**********************
 * LibProc methods.
 *********************/

/*==============================================================================
 * FUNCTION:        LibProc::LibProc
 * OVERVIEW:        Constructor with name, native address.
 * PARAMETERS:      name - Name of procedure
 *                  uNative - Native address of entry point of procedure
 * RETURNS:         <nothing>
 *============================================================================*/
LibProc::LibProc(std::string& name, ADDRESS uNative) : Proc(name,uNative)
{
    // Look up the name in the public map mapLibParam of the prog object
    std::map<std::string, std::list<TypedExp*> >::iterator it;
    it = prog.mapLibParam.find(name);
    if (it == prog.mapLibParam.end()) {
        std::cerr << "Could not find parameters for library function " << name <<
          std::endl;
        // Assume int proc(int) for now. The expression part of the TypedExp
        // will be unused for LibProcs, so just use Const(0).
        TypedExp* parm = new TypedExp(Type(INTEGER), new Const(0));
        returnLocn = (TypedExp*)parm->clone();
        parameters.push_back(parm);
    }
    else {
        // The tail of the parameter list contains the parameters of this
        // procedure. The head element will be removed next.
        parameters = (*it).second;      // The list<TypedExp*>

        // The return type is at the head of the parameter list
        returnLocn = parameters.front();
        parameters.erase(parameters.begin());
    }

/*
    // Debugging
    cout << "signature for library procedure: `";
    printReturnTypeAsC(cout);
    cout << " " << name << "("; 
    printParamsAsC(cout);
    cout << ")'\n";
*/
}


/*==============================================================================
 * FUNCTION:        LibProc::setReturnType
 * OVERVIEW:        Ensure that this library procedure has the
 *                  expected return type.
 * NOTE:            We expect a return value from this library procedure
 * PARAMETERS:      retLoc - the location used to store a value
 *                    returned from this procedure
 *                  retSpec - the spec used to map locations to types
 *                    (which is not an unique mapping)
 * RETURNS:         Always false (see UserProc::setReturnType())
 *============================================================================*/
bool LibProc::setReturnType(TypedExp* retLoc)
{

    // Ensure that the return type of this procedure corresponds to
    // the location used to store the returned value.
    // Use a sign insensitive comparison
    if ((returnLocn == NULL) || !(*returnLocn -= *retLoc)) {
        std::cerr << "caller receives a return value in `" << retLoc;
        std::cerr << "' (size " << retLoc->getType().getSize();
        std::cerr << ") which is incompatible with the `";
        std::cerr << returnLocn->getType().getCtype() << "' returned by `";
        std::cerr << name << "'\n";
    } 
    return false;
}

/*==============================================================================
 * FUNCTION:        LibProc::put
 * OVERVIEW:        Display on os.
 * PARAMETERS:      os -
 * RETURNS:         os
 *============================================================================*/
std::ostream& LibProc::put(std::ostream& os)
{
    os << "library procedure `" << name << "' resides at 0x";
    return os << std::hex << address << std::endl;
}

/**********************
 * UserProc methods.
 *********************/

/*==============================================================================
 * FUNCTION:        UserProc::UserProc
 * OVERVIEW:        Constructor with name, native address.
 * PARAMETERS:      name - Name of procedure
 *                  uNative - Native address of entry point of procedure
 * RETURNS:         <nothing>
 *============================================================================*/
UserProc::UserProc(std::string& name, ADDRESS uNative) :
    Proc(name,uNative), cfg(new Cfg()), decoded(false),
    returnIsSet(false), isSymbolic(false), uniqueID(0)
{
    cfg->setProc(this);              // Initialise cfg.myProc
}

/*==============================================================================
 * FUNCTION:        UserProc::isDecoded
 * OVERVIEW:        
 * PARAMETERS:      
 * RETURNS:         
 *============================================================================*/
bool UserProc::isDecoded()
{
    return decoded;
}

/*==============================================================================
 * FUNCTION:        UserProc::put
 * OVERVIEW:        Display on os.
 * PARAMETERS:      os -
 * RETURNS:         os
 *============================================================================*/
std::ostream& UserProc::put(std::ostream& os)
{
    os << "user procedure `" << name << "' resides at 0x";
    return os << std::hex << address << std::endl;
}


#if 0       // Need toSymbolic
/*==============================================================================
 * FUNCTION:      UserProc::setParams
 * OVERVIEW:      Sets the parameters that have been recovered for this
 *                  procedure through CSR analysis.
 * PARAMETERS:    params - the list of locations used for the parameters
 *                aggUsed - true if the aggregate pointer location is used
 * RETURNS:       <nothing>
 *============================================================================*/
void UserProc::setParams(std::list<TypedExp*>& params, bool aggUsed /* = false */)
{
    parameters.clear();         // Could be called many times
    for (std::list<TypedExp*>::iterator it = params.begin(); it != params.end();it++)
    {
        TypedExp* symbolic;
        toSymbolic(*it, symbolic, false);
        parameters.push_back(symbolic);
        // Under some conditions, parameters are discovered after locals have
        // been created for the same location. That causes a local variable to
        // be delcared in the .c file, shadowing the parameter. So we check for
        // the parameter already being a local variable; if so, the local is
        // deleted
        // Note: can't use find() because we need a special equality for Exp*s
        std::vector<TypedExp*>::iterator ll;
        for (ll = locals.begin(); ll != locals.end(); ll++) {
            if (**ll == *symbolic) {
                delete *ll;
                locals.erase(ll);
                break;
            }
        }
    }
    aggregateUsed = aggUsed;
}
#endif

#if 0       // Was used for some weird CSR purpose
/*==============================================================================
 * FUNCTION:      UserProc::getParamSet
 * OVERVIEW:      Gets the parameters that have been recovered for this
 *                  procedure through CSR analysis, to a set of Semantic Strings
 *                  (not symbolic)
 * PARAMETERS:    <none>
 * RETURNS:       Type insensitive set of Exp*
 *============================================================================*/
setTiExp*& UserProc::getParamSet()
{
    setTiExp*& ret = *new setTiExp*;
    map<Exp*, Exp*>::iterator it;
    for (it = symbolMap.begin(); it != symbolMap.end(); it++) {
        ret.insert(it->first);
    }
    return ret;
}
#endif

#if 0
/*==============================================================================
 * FUNCTION:      UserProc::setPrologue
 * OVERVIEW:      Set the prologue of this procedure.
 * PARAMETERS:    prologue - a caller prologue
 * RETURNS:       <nothing>
 *============================================================================*/
void UserProc::setPrologue(CalleePrologue* prologue)
{
    if (this->prologue != NULL) {
        std::ostringstream ost;
        ost << "attempt to set prologue `" << prologue->getName()
          << "' for proc " << name << "' which already has one: `"
          << this->prologue->getName() << "'";
        if (prologue->getName() == this->prologue->getName()) {
            // This is a warning, not an error, because of the common case of
            // Sparc's same_reg_win, where the epilogue is the same as the
            // prologue, except for the sign of the operand, e.g.
            // add          %sp, -104, %sp      // Prologue
            // ...
            // add          %sp, +104, %sp      // Epilogue
            warning(str(ost));
        } else {
            // Assume that different logue names is always bad
            error(str(ost));
        }
    }
    else
        // Note: don't overwrite prologues. For example, the same_reg_win
        // case as above. If a prologue comes part way through a proc, it is
        // set then (there is no "default" prologue)
        this->prologue = prologue;
}

/*==============================================================================
 * FUNCTION:      UserProc::setEpilogue
 * OVERVIEW:      Set the epilogue of this procedure.
 * PARAMETERS:    epilogue - a caller epilogue
 * RETURNS:       <nothing>
 *============================================================================*/
void UserProc::setEpilogue(CalleeEpilogue* epilogue)
{
    // Only set the given epilogue to be the epilogue of this procedure if it
    // doesn't currently have one of the one it does have comes after the given
    // epilogue in an oredring between epilogues.
    if (this->epilogue == NULL ||
        this->epilogue->getOrder() > epilogue->getOrder())
        this->epilogue = epilogue;
}

/*==============================================================================
 * FUNCTION:      UserProc::getPrologue
 * OVERVIEW:      Get the prologue (if any) of this procedure.
 * PARAMETERS:    <none>
 * RETURNS:       a callee prologue
 *============================================================================*/
CalleePrologue* UserProc::getPrologue()
{
    return this->prologue;
}

/*==============================================================================
 * FUNCTION:      UserProc::getEpilogue
 * OVERVIEW:      Get the epilogue (if any) of this procedure.
 * PARAMETERS:    <none>
 * RETURNS:       a callee epilogue
 *============================================================================*/
CalleeEpilogue* UserProc::getEpilogue()
{
    return this->epilogue;
}
#endif

#if 0           // Needs getLocalsSize
/*==============================================================================
 * FUNCTION:      UserProc::printLocalsAsC
 * OVERVIEW:      Print the locals declaration in C style. This
 *                includes declarations for the block of memory set
 *                aside for local varaiables and the abstract frame
 *                pointer used to index into this block.
 *                Also delcares the symbolic locations (v[0]..v[n-1])
 * NOTE:          FIXME: C specific
 * PARAMETERS:    os - the output stream to use
 * RETURNS:       <nothing>
 *============================================================================*/
void UserProc::printLocalsAsC(std::ostream& os)
{
    // the block of memory for locals
    if (getLocalsSize() != 0) {

        os << "char _locals[" << dec << getLocalsSize() << "];\n";
    }

    // If this is main, and the analysis decided that there were more than
    // two parameters, declare them here so that at least the output will
    // compile
    if ((name == "main") && (parameters.size() > 2)) {
        std::list<TypedExp*>::iterator it = parameters.begin();
        for (it++, it++; it != parameters.end(); it++) {
            os << (*it)->getType().getCtype();      // Type, e.g. int32
            os << " ";
            (*it)->printAsHL(os);                    // Name, e.g. v2 or r8
            os << ";";
        }
        os << "\t/* Dummy parameters */\n";
    }

    // Declare the symbolic locations (v[0] etc)
    for (std::vector<TypedExp*>::iterator it = locals.begin();
      it != locals.end(); it++) {
        os << "\t" << (*it)->getType().getCtype();  // Type, e.g. int32
        os << " ";
        (*it)->printAsHL(os);                        // Name, e.g. v2 or r8
        os << ";\n";
    }
}
#endif

/*==============================================================================
 * FUNCTION:       UserProc::toSymbolic
 * OVERVIEW:       Given a machine dependent location, return a generated
 *                 symbolic representation for it.
 * NOTE:           loc will occasionally be of the forms
 *                  trunc(m[%afp - 20] >> 16, 32, 16) or
 *                  trunc(m[%afp - 20] & 0xFFFF, 16, 32)
 *                  must now cope with these
 * NOTE ALSO:      The fixComplex logic (overlapping parameters) should be
 *                  done in matchParameters, as one case already has
 * PARAMETERS:     loc - a machine dependent location
 *                 result - the symbolic representation for loc
 *                 local: if true, add this symbol to the vector of locals
 *                  result a copy of loc if the mapping isn't there)
 * RETURNS:        <nothing> (but parameter result is set)
 *============================================================================*/
// Simple procedure to effectively substitute result into loc if a complex
// location
// Example: loc = trunc(m[%afp - 20] & 0xFFFF, 32, 16), result = v3; then
// result changed to be trunc(v3 & 0xFFFF, 32, 16)
// Assumes 4 tokens at start, and 2 at end, to be transferred
// FIXME: This is really the most awful hack. Perhaps substitution using
// searchReplace() would be better?
#if 0
void fixComplex(Exp* loc, Exp* result)
{
    for (int i=4-1; i >= 0; i--)
        result.prep(loc.getIndex(i));
    for (int i1=0; i1 < 2; i1++)
        result.push(loc.getIndex(i1));
    // Now a different size; same size as the location we started with
    // Example: passed m[%afp-8]>>16 as 16 bits; result (v1) wass 32 bits;
    // now result (v1>>16) is back to 16 bits again
    result.setTypes(loc);
}

void UserProc::toSymbolic(TypedExp* loc, TypedExp* result,
    bool local /*= true*/)
{
    int idx = loc.getFirstIdx();
    if (idx == idIntConst) {
        // Occasionally pass constants now
        result = loc;      // Symbolic representation is itself
        return;
    }

    // loc2 is the expression to be converted to symbolic form. For simple
    // cases, loc2 == loc. For complex cases, loc2 is the first subexpression
    // of loc, and is converted to symbolic form. The result is effectively
    // substituted into loc
    bool complex = false;
    Exp* loc2 = loc;
    if (!loc.isRegOf() &&
        !loc.isMemOf() &&
        !loc.isVar()) {
            complex = true;
            Exp* tmp = ((Unary*)loc2)->getSubExp1();
            loc2 = tmp->getSubExpr(0, loc2);
            delete tmp;
            // Set the size of the subexpression to double what we are passed.
            // E.g. v1 is twice the size of (v1 >> 16)
            loc2.getType().setSize(loc.getType().getSize() * 2);
    }
            
    
    idx = loc2.getFirstIdx();
    assert(idx == idRegOf || idx == idMemOf || idx == idVar);

    if (idx == idVar) {
        if (find(locals.begin(), locals.end(), loc2) == locals.end()) {
            std::ostringstream ost;
            ost << "`" << loc << "' should already be in the set of locals";
            error(str(ost));
        }
        result = loc2;        // Symbolic representation is itself
        return;
    }

    map<Exp*,Exp*>::iterator it = symbolMap.find(loc2);
    if (it != symbolMap.end()) {
        result = it->second;
        if (complex) fixComplex(loc, result);
//        if (loc.getFirstIdx() == idRegOf) result = loc;   // HACK
        return;
    }
    // Else does not exist in the map
    if (isSymbolic) {
        // Not allowed to add a new (because we have already called
        // propagateSymbolics), and no existing. Return with result = loc
        result = loc;
        return;
    }

    // Don't convert r[] to symbolic (v[]); this would interfere with the
    // overlapping code logic
    if (loc.getFirstIdx() == idRegOf) {
        result = loc;
        return;           // Never add to locals
    }
    else {
        result.clear();
        result.push(idVar);
        result.push(uniqueID);
        if (complex)
            // Overlapping parameters. The var is twice the size of the location
            // that we are passed. E.g. v1 is twice the size of (v1 >> 16)
            result.getType().setSize(loc.getType().getSize() * 2);
        else
            result.setTypes(loc);  // Same size as location it represents
        uniqueID++;
        // Add a new entry
        symbolMap[loc2] = result;
    }

    // Add this to the locals if necessary.
    if (local)
        locals.push_back(result);

    if (complex) fixComplex(loc, result);
    return;
}
#endif

/*==============================================================================
 * FUNCTION:       UserProc::newLocal
 * OVERVIEW:       Return the next available local variable.
 * NOTE:           Caller is responsible for deleting this new Exp*
 * PARAMETERS:     ty: a Type for the local variable
 * RETURNS:        Pointer to the Exp representing the local
 *============================================================================*/
TypedExp* UserProc::newLocal(Type& ty)
{
    TypedExp* result = new TypedExp(ty, new Unary(opVar, new Const(uniqueID)));
    uniqueID++;

    locals.push_back(result);

    return result;
}

/*==============================================================================
 * FUNCTION:       UserProc::setReturnType
 * OVERVIEW:       Given the type of a location that has been
 *                 determined as holding a value returned by this
 *                 procedure, verify this against the type this
 *                 procedure currently thinks it returns. If this is
 *                 the first time this method has been called, then
 *                 the return type of this procedure is set to be the
 *                 given type and the location used for returning that
 *                 type is determined to be the return location of
 *                 this procedure. Otherwise, simply ensure that the
 *                 given type matches the already established return
 *                 type, emitting an error message if it isn't.
 * PARAMETERS:      retLoc - the location used to store a value
 *                    returned from this procedure
 * RETURNS:        true if return location changed
 *============================================================================*/
bool UserProc::setReturnType(TypedExp* retLoc)
{
    if (returnLocn) {

        //Type& retType = returnLocn->getType();
        if (!(*returnLocn -= *retLoc)) {
    
            std::cerr << "caller receives a return value in `";
            std::cerr << retLoc << "' (size ";
            std::cerr << retLoc->getType().getSize();
            std::cerr << ") which is incompatible with the `";
            std::cerr << returnLocn->getType().getCtype() << "' returned by `";
            std::cerr << name << "'\n";
            return false;           // Prevent infinite loop
        } 

        return true;        // FIXME: Is this what we want?
    }

    returnLocn = retLoc;
    return retLoc != NULL;
}

#if 0       // I think we don't need this now
/*==============================================================================
 * FUNCTION:       UserProc::doSetReturnType
 * OVERVIEW:       Given the type of a location that has been
 *                 determined as holding a value returned by this
 *                 procedure, set the return type accordingly
 * PARAMETERS:      retLoc - the location used to store a value
 *                    returned from this procedure
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::doSetReturnType(TypedExp* retLoc)
{
    // We want to know which location is used to return the given
    // type. We get the location used for the return type from the
    // epilogue's return location spec.
    if (epilogue == NULL) {
        // This used to be an assert, but we may not have an epilogue if we
        // have a register jump in this procedure. Also, main or another proc
        // may have an infinite loop (true return through exit() or the like),
        // in whoch case the return type is irrelevant. So now it's an warning
        std::ostringstream ost;
        ost << "cannot set return type for " << name << " since it has no "
            "epilogue";
        warning(str(ost));
        return;
    }
    Type& type = retSpec.typeFor(retLoc);
    Exp* loc = epilogue->getRetSpec()->locationFor(type);
    if (loc) {
        // Convert to symbolic form if necessary, e.g. v99 instead of m[%afp]
        toSymbolic(*loc, returnLocn, true);
    } else {
        std::cerr << "epilogue for user proc " << name;
        std::cerr << " has no return specification\n";
        return;
    }

    // Since we now have a return type, it's possible that the return
    // location is used differently now (e.g. was returning void, now
    // integer, so the integer return location is now used, when it wasn't
    // before). We need to re-analyse this proc as a callee, to take into
    // account the new use of the return location
    // But calling the analysis twice adds the parameters twice. This could
    // be fixed, but it's only Sparc that has this problem, and for sparc,
    // the problem will only happen when there are no parameters.
    // So for now, only do this if no parameters
    if (parameters.size() == 0) {
        // First, reset isSymbolic, so it will create new symbolics if needed
        // (and we will propagate them afterwards)
        isSymbolic = false;
        prog.csrSrc.analyseCallee(this);
        // May be necessary to convert the return location to symbolic...
        if (returnLocn.getFirstIdx() != idVar) {
            // Don't want to declare this as a local variable if it's a
            // parameter. Assume that if it's a parameter, it will be the
            // first one
            bool isParam = false;
            if (parameters.size() && (parameters.front() == returnLocn))
                isParam = true;
            toSymbolic(returnLocn, returnLocn, !isParam);
            // ... and propagate this change throughout the procedure
            propagateSymbolics();
        }
        isSymbolic = true;      // Restore isSymbolic
    }

}
#endif

#if 0       // We will need this or something like it soon
/*==============================================================================
 * FUNCTION:      UserProc::propagateSymbolics
 * OVERVIEW:      Replace each instance of a location in this procedure with its
 *                  symbolic representation if it has one.
 *                  Also handle expressions taking the addresses of these
 * PARAMETERS:    <none>
 * RETURNS:       <nothing>
 *============================================================================*/
void UserProc::propagateSymbolics()
{
    // First, do a pass that checks the sizes of memory references
    checkMemSizes();
    for (map<Exp*,Exp*>::iterator it = symbolMap.begin();
      it != symbolMap.end(); it++) {

        Exp*& search = (Exp*&)it->first;
        Exp*& replace = it->second;

        // Go through the RTLs. For now, we clear the "type sensitive" flag,
        // so that when floating point parameters are passed through memory,
        // they get substituted properly. NOTE: this will break Palm programs,
        // which have stack locations of different sizes needing different
        // symbolic variables! (But casting the parameters may fix this)
// Don't convert expressions with r[] to v[]
//if (search.getFirstIdx() != idRegOf)      // HACK
            cfg->searchAndReplace(search, replace, false);

        // Also remember to fix code taking addresses of these parameters
        // Example: addressparam test. Knock the memOf off the front of the
        // search, and prepend an idAddrOf to the replacement
        if (search.getFirstIdx() == idMemOf) {
            Exp** srch2 = search.getSubExpr(0);
            Exp* repl2(replace);
            repl2.prep(idAddrOf);
            cfg->searchAndReplace(*srch2, repl2, false);
            // Also check if sizeof(search) > sizeof(int).
            // The size of an int varies with the source machine. We use
            // the OFFSET value from the PAL file
            int intSize = prog.csrSrc.getIntSize();
            int size = getVarType(it->second.getSecondIdx()).getSize();
            if (size > intSize*8) {
                //  If so, also do a replacement of m[original+4] by
                // *((int*)&replace+1)
                // i.e. m[ + original int 4   by   m[ + (int*) & replace int 1
                // repl2 is already &replace
//cout << "Search " << search << " and replace " << replace << std::endl;  // HACK
                repl2.push(idIntConst); repl2.push(1);
                repl2.prep(idCastIntStar);
                repl2.prep(idPlus); repl2.prep(idMemOf);
                // *srch2 already has replace without the m[
                srch2->prep(idPlus); srch2->prep(idMemOf);
                srch2->push(idIntConst); srch2->push(4);
                srch2->simplify();
//cout << "Replacing " << *srch2 << " with " << repl2 << std::endl;    // HACK
                cfg->searchAndReplace(*srch2, repl2, false);
            }
            delete srch2;
        }
    }
    // Remember that this has been done
    isSymbolic = true;
}
#endif

/*==============================================================================
 * FUNCTION:        UserProc::getCFG
 * OVERVIEW:        Returns a pointer to the CFG.
 * PARAMETERS:      <none>
 * RETURNS:         a pointer to the CFG
 *============================================================================*/
Cfg* UserProc::getCFG()
{
    return cfg;
}

/*==============================================================================
 * FUNCTION:        UserProc::deleteCFG
 * OVERVIEW:        Deletes the whole CFG for this proc object. Also clears the
 *                  cfg pointer, to prevent strange errors after this is called
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::deleteCFG()
{
    delete cfg;
    cfg = NULL;
}

#if 0           // This should be done by some sort of max stack depth thing
/*==============================================================================
 * FUNCTION:        UserProc::getLocalsSize
 * OVERVIEW:        Sets the number of bytes allocated for locals on
 *                  the stack.
 * PARAMETERS:      <none>
 * RETURNS:         the number of bytes allocated for locals on
 *                  the stack
 *============================================================================*/
int UserProc::getLocalsSize()
{
    if (prologue != NULL)
        return prologue->getLocalsSize();
    else
        return 0;
}
#endif

/*==============================================================================
 * FUNCTION:    Proc::getFirstLocalIndex()
 * OVERVIEW:    Return the index of the first symbolic local declared.
 * PARAMETERS:  None
 * RETURNS:     An integer value of the first symbolic local declared. For e.g
                variable v12, it returns 12. If no locals, returns -1.
 *============================================================================*/
int UserProc::getFirstLocalIndex()
{
    std::vector<TypedExp*>::iterator it = locals.begin();
    if (it == locals.end()) {
        return -1;
    }
    return (*it)->getVarIndex();
}

#if 0       // This will work when all Exp's have types
/*==============================================================================
 * FUNCTION:    Proc::getLastLocalIndex()
 * OVERVIEW:    Return the index of the last symbolic local declared.
 * PARAMETERS:  None
 * RETURNS:     An integer value of the first symbolic local declared. For e.g
                variable v12, it returns 12. If no locals, returns -1.
 *============================================================================*/
int UserProc::getLastLocalIndex()
{
    std::vector<TypedExp*>::iterator it = locals.end(); // just after end
    if (it == locals.begin()) { // must be empty
        return -1;
    }
    it--;			// point to last element
    return it->getSecondIdx();
}
#endif

/*==============================================================================
 * FUNCTION:    UserProc::getSymbolicLocals()
 * OVERVIEW:    Return the list of symbolic locals for the procedure.
 * PARAMETERS:  None
 * RETURNS:     A reference to the list of the procedure's symbolic locals.
 *============================================================================*/
std::vector<TypedExp*>& UserProc::getSymbolicLocals()
{
    return locals;
}

/*==============================================================================
 * FUNCTION:        UserProc::setDecoded
 * OVERVIEW:        
 * PARAMETERS:      
 * RETURNS:         
 *============================================================================*/
void UserProc::setDecoded()
{
    decoded = true;
}

/*==============================================================================
 * FUNCTION:      UserProc::subAXP
 * OVERVIEW:      Given a map from registers to expressions, follow
 *                the control flow of the CFG replacing every use of a
 *                register in this map with the corresponding
 *                expression. Then for every definition of such a
 *                register, update its expression to be the RHS of
 *                the definition after the first type of substitution
 *                has been performed and remove that definition from
 *                the CFG.
 * PARAMETERS:    subMap - a map from register to expressions
 * RETURNS:       <nothing>
 *============================================================================*/
void UserProc::subAXP(std::map<Exp*,Exp*>& subMap)
{
    cfg->subAXP(subMap);
}

/*==============================================================================
 * FUNCTION:    UserProc::getEntryBB
 * OVERVIEW:    Get the BB with the entry point address for this procedure
 * PARAMETERS:  
 * RETURNS:     Pointer to the entry point BB, or NULL if not found
 *============================================================================*/
PBB UserProc::getEntryBB()
{
    return cfg->getEntryBB();
}

/*==============================================================================
 * FUNCTION:        UserProc::setEntryBB
 * OVERVIEW:        Set the entry BB for this procedure
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::setEntryBB()
{
    std::list<PBB>::iterator bbit;
    PBB pBB = cfg->getFirstBB(bbit);        // Get an iterator to the first BB
    // Usually, but not always, this will be the first BB, or at least in the
    // first few
    while (pBB && address != pBB->getLowAddr()) {
        pBB = cfg->getNextBB(bbit);
    }
    cfg->setEntryBB(pBB);
}

/*==============================================================================
 * FUNCTION:        UserProc::getCallees
 * OVERVIEW:        Get the set of callees (procedures called by this proc)
 * PARAMETERS:      <none>
 * RETURNS:         Constant reference to the set
 *============================================================================*/
std::set<Proc*>& UserProc::getCallees()
{
    return calleeSet;
}

/*==============================================================================
 * FUNCTION:        UserProc::setCallee
 * OVERVIEW:        Add this callee to the set of callees for this proc
 * PARAMETERS:      A pointer to the Proc object for the callee
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::setCallee(Proc* callee)
{
    calleeSet.insert(callee);
}

#if 0
/*==============================================================================
 * FUNCTION:        UserProc::addProcCoverage
 * OVERVIEW:        Add this proc's coverage to the program's coverage
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::addProcCoverage()
{
    prog.cover.addRanges(cover);
}

/*==============================================================================
 * FUNCTION:      UserProc::findVarEntry
 * OVERVIEW:      Return a pointer to the given var, i.e. if 2 is passed,
 *                  then on exit, points to the Exp* for v2
 * NOTE:          Private function; only used by get/setVarType
 * PARAMETERS:    idx - index of the variable, e.g. 2 for v2
 * RETURNS:       A pointer to the entry, as above, or NULL if not found
 *============================================================================*/
Exp** UserProc::findVarEntry(int idx)
{
    // The locals are sorted, so we can do a binary search of them
    int min = 0; int max = locals.size()-1;
    int i;
    if (max >= 0) {                  // Do nothing if no locals
        while (min <= max) {
            i = (min + max) >> 1;
            // We are only interested in vars (v[]). Some locals can now be
            // registers (e.g. Pentium function returning in r24)
            if (locals[i].getFirstIdx() != idVar) {
                // Assume that there are only r[] and v[], and that
                // idRegOf < idVar, so we need to move up the map
                min = i+1;
                continue;
            }
            int c = locals[i].getSecondIdx();
            if (c == idx) {
                return &locals[i];
            }
            else if (c > idx)
                max = i-1;
            else /* if (c < idx) */
                min = i+1;
        }
    }

    // Also search the parameters. Assume a linear search is quicker, for
    // the likely very small number of parameters
    std::list<Exp*>::iterator pp;
    for (pp = parameters.begin(); pp != parameters.end(); pp++) {
        if (pp->getFirstIdx() != idVar)
            // Could be a r[] now
            continue;
        if (pp->getSecondIdx() == idx) {
            return &(*pp);
        }
    }

    // Not found in locals or parameters
    std::ostringstream ost;
    ost << "Could not find v" << idx <<
        " in locals or prarameters for procedure " << name;
    error(str(ost));
    return NULL;
}

/*==============================================================================
 * FUNCTION:      UserProc::getVarType
 * OVERVIEW:      Return the type of the given variable
 * PARAMETERS:    idx - index of the variable, e.g. 2 for v2
 * RETURNS:       The Type
 *============================================================================*/
Type UserProc::getVarType(int idx)
{
    Exp* pss = findVarEntry(idx);
    if (pss == NULL)
        return Type();
    return pss->getType();
}

/*==============================================================================
 * FUNCTION:      UserProc::setVarSize
 * OVERVIEW:      Change the size of the given var (i.e. the size it will be
 *                  declared as)
 * PARAMETERS:    idx - the var number (e.g. 2 for v2)
 *                size - the new size
 * RETURNS:       Nothing
 *============================================================================*/
void UserProc::setVarSize(int idx, int size)
{
    Exp** pss = findVarEntry(idx);
    if (pss == NULL) return;
    Type ty = pss->getType();
    ty.setSize(size);
    pss->setType(ty);
}
#endif


#if 0
/*==============================================================================
 * FUNCTION:        UserProc:checkReturnPass
 * OVERVIEW:        Check if this return location is "passed through" this
 *                    function to one of its callees. For example in the
 *                    returncallee test, main uses the return value from add4,
 *                    and this use is "passed on" to add2, since add4 doesn't
 *                    define the return location after the call to add2
 * PARAMETERS:      returnLocBit - the bit for the location used by my caller
 *                  returnLoc - the Location used (as a Exp*)
 *                  retLocations - information about where types are returned
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::checkReturnPass(int returnLocBit, TypedExp* returnLoc)
{
    // We are looking for a path from a call BB to a return BB where no
    // BB (after the call) defines the return location
    std::set<HLCall*>& calls = cfg->getCalls();
    std::set<PBB> seen;          // Set of out edges already checked
    for (std::set<HLCall*>::iterator it = calls.begin(); it != calls.end(); it++) {
        if ((*it)->getReturnLoc() != 0)
            // This call already has a return location - no need to check it
            // or its callee
            continue;
        const PBB pBB = (*it)->getBB();
        checkReturnPassBB(pBB, *it, returnLocBit, returnLoc, seen);
    }
}

/*==============================================================================
 * FUNCTION:        UserProc:checkReturnPassBB
 * OVERVIEW:        Do the main work of checking for return locations being
 *                    "passed through" (see above)
 * PARAMETERS:      pBB - pointer to the current BB which is being checked
 *                  pCall - pointer to the HLCall RTL containing the call
 *                  seen - set of out edges already checked
 *                  others as above
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::checkReturnPassBB(const PBB pBB, HLCall* pCall, int returnLocBit,
    TypedExp* returnLoc, std::set<PBB>& seen)
{
    // We have a call BB. Check all possible out edges (beware of
    // loops!) till a return location is found.
    std::vector<PBB>::const_iterator ito;
    const std::vector<PBB>& outs = pBB->getOutEdges();
    for (ito = outs.begin(); ito != outs.end(); ito++) {
        if (seen.find(*ito) != seen.end())
            // Already checked this edge
            continue;
        seen.insert(*ito);
        if ((*ito)->isDefined(returnLocBit))
            // The return location is defined, killing the path from
            // the call to the return location
            continue;
        if ((*ito)->getType() == RET) {
            // This is what we are looking for! This callee also
            // returns to this location
            ADDRESS uDest = pCall->getFixedDest();
            UserProc* callee = (UserProc*)prog.findProc(uDest);
            if (callee && (int)callee != -1 && !callee->isLib())
                callee->setReturnType(returnLoc, retLocations);
            // We must also set the return location for the call (so that the
            // back end assigns the result of the call).
            if (isSymbolic) {
                // Use the symbolic version if the proc has already called
                // propagateSymbolic()
                Exp* symbolic;
                toSymbolic(returnLoc, symbolic);
                pCall->setReturnLoc(symbolic);
            }
            else pCall->setReturnLoc(returnLoc);
            // No further paths need be investigated
            return;
        }
        else {
            // Recurse through this BB's out edges
            const std::vector<PBB>& grandChildren = (*ito)->getOutEdges();
            std::vector<PBB>::const_iterator itg;
            for (itg = grandChildren.begin(); itg != grandChildren.end();
              itg++) {
                checkReturnPassBB(*itg, pCall, returnLocBit, returnLoc,
                    retLocations, seen);
            }
            
        }
    }
}
#endif

#if 0
/*==============================================================================
 * FUNCTION:        checkMemSize
 * OVERVIEW:        Search this ss, checking for sizes of vars, ensuring
 *                    that the largest size used in the proc is used
 * PARAMETERS:      ss - pointer to Exp* to be checked
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::checkMemSize(Exp** ss)
{
    Exp* memX;            // Wildcard memory
    memX.push(idMemOf); memX.push(-1);
    std::list<Exp**> result;
    if (ss->searchAll(memX, result)) {
        // We have at least one mem; go through the list
        std::list<Exp**>::iterator it;
        for (it=result.begin(); it != result.end(); it++) {
            // Find out what var this would be converted to, if any
            std::map<Exp*, Exp*>::iterator mm;
            mm = symbolMap.find(**it);
            if (mm == symbolMap.end()) continue;
            int vNum = (*mm).second.getSecondIdx();
            // Find out what size this memory is used as
            int size = (*it)->getType().getSize();
            if (size > getVarType(vNum).getSize()) {
                // Change to the larger size
                setVarSize(vNum, size);
                // If there is a mapping for the "other half", then delete it
                // so it will only be satisfied by special logic in
                // propagateSymbolics()
                Exp* memOf(**it);
                // Assume m[ + something int  K ]
                //        0  1               last
                if (memOf.getSecondIdx() == idPlus) {
                    int last = memOf.len() - 1;
                    int K = memOf.getIndex(last);
                    int intSize = prog.csrSrc.getIntSize();
                    memOf.substIndex(last, K + intSize);
                    mm = symbolMap.find(memOf);
                    if (mm != symbolMap.end())
                        symbolMap.erase(mm);
                }
            }
        }
    }
}

/*==============================================================================
 * FUNCTION:        checkMemSizes
 * OVERVIEW:        Loop through all BBs, checking for sizes of memory that
 *                    will soon be converted to vars, ensuring
 *                    that the largest size used in the proc is used for all
 * PARAMETERS:      None
 * RETURNS:         <nothing>
 *============================================================================*/
void UserProc::checkMemSizes()
{
    std::list<PBB>::iterator it;
    PBB pBB = cfg->getFirstBB(it);
    while (pBB) {
        RTLList* pRtls = pBB->getRTLs();
        if (pRtls) {
            RTLList_IT rit;
            for (rit = pRtls->begin(); rit != pRtls->end(); rit++) {
                int n = (*rit)->getNumRT();
                for (int i=0; i<n; i++) {
                    RTAssgn* rt = (RTAssgn*)(*rit)->elementAt(i);
                    if (rt->getKind() != RTASSGN) continue;
                    checkMemSize(rt->getLHS());
                    checkMemSize(rt->getRHS());
                }
            }
        }
        pBB = cfg->getNextBB(it);
    }

}
#endif

