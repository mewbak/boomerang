/*
 * Copyright (C) 2001, Sun Microsystems, Inc
 * Copyright (C) 2001, The University of Queensland
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       rtl.h
 * OVERVIEW:   Definition of the classes that describe an RTL, a low-level
 *             register transfer list. Higher-level RTLs (instance
 *             of class HLJump, HLCall, etc.) represent information about
 *             a control transfer instruction (CTI) in the source program.
 *             analysis code adds information to existing higher-level
 *             RTLs and sometimes creates new higher-level RTLs (e.g., for
 *             switch statements). 
 *============================================================================*/

/*
 * $Revision: 1.1 $
 * 08 Apr 02 - Mike: Mods for boomerang
 * 13 May 02 - Mike: expList is no longer a pointer
 */

#ifndef __RTL_H__
#define __RTL_H__

#include <list>
#include <vector>
#include <map>
#include <set>
#include "exp.h"
#include "type.h"
#include "register.h"

class BasicBlock;
typedef BasicBlock* PBB;

/*==============================================================================
 * Kinds of RTLs, or high-level register transfer lists.
 *============================================================================*/
enum RTL_KIND {
    HL_NONE = 0,
    CALL_RTL,
    RET_RTL,
    JCOND_RTL,
    JUMP_RTL,
    SCOND_RTL,                 // For "setCC" instructions that set destination
                                // to 1 or 0 depending on the condition codes.
    NWAYJUMP_RTL,              // Used to represent switch statements.
};


/*==============================================================================
 * JCOND_TYPE: These values indicate what kind of conditional jump is being
 * performed.
 *============================================================================*/
enum JCOND_TYPE {
    HLJCOND_JE = 0,          // Jump if equals
    HLJCOND_JNE,             // Jump if not equals
    HLJCOND_JSL,             // Jump if signed less
    HLJCOND_JSLE,            // Jump if signed less or equal
    HLJCOND_JSGE,            // Jump if signed greater or equal
    HLJCOND_JSG,             // Jump if signed greater
    HLJCOND_JUL,             // Jump if unsigned less
    HLJCOND_JULE,            // Jump if unsigned less or equal
    HLJCOND_JUGE,            // Jump if unsigned greater or equal
    HLJCOND_JUG,             // Jump if unsigned greater
    HLJCOND_JMI,             // Jump if result is minus
    HLJCOND_JPOS,            // Jump if result is positive
    HLJCOND_JOF,             // Jump if overflow
    HLJCOND_JNOF,            // Jump if no overflow
    HLJCOND_JPAR             // Jump if parity even (Intel only)
};


/*==============================================================================
 * Class RTL: describes low level register transfer lists (actually lists of
 * expressions)
 *============================================================================*/
class RTL {
public:
    RTL();
    RTL(ADDRESS instNativeAddr, std::list<Exp*>* listExp = NULL);
    RTL(const RTL& other);                  // Makes deep copy of "other"
    virtual ~RTL();

    // Return a deep copy, including a deep copy of the list of Exps
    virtual RTL* clone();

    // Assignment copy: set this RTL to a deep copy of "other".
    RTL& operator=(RTL &other);

    // Common enquiry methods
    RTL_KIND getKind() { return kind; };
    ADDRESS getAddress();               // Return RTL's native address
    bool getCommented();                // Return whether to comment in HL C.
    int getSize();                      // Return size in bits of first Exp.
    unsigned getNumBytes()              // Return number of bytes in instr
        {return numNativeBytes;}
    ADDRESS getOutAddr(int idx);        // Return the specified out edge addr
    ADDRESS getFallthrough();           // Return the fall through address
    bool    areFlagsAffected();         // True if flags are affected

    // Expression list enquiry methods
    int getNumExp();                    // Return the number of Exps in RTL.
    Exp* elementAt(unsigned i);         // Return the i'th element in RTL.
    
    // Exp list editing methods
    void appendExp(Exp *e);             // Add e to end of RTL.
    void prependExp(Exp *rt);           // Add e to start of RTL.
    void insertExp(Exp *rt, unsigned i); // Insert rt before Exp at position i
    void updateExp(Exp *rt, unsigned i); // Change Exp at position i.
    void deleteExp(unsigned int);       // Delete Exp at position i.
    void clear();                       // Remove all Exps from this RTL.
    void appendListExp(std::list<Exp*>& le); // Append list of Exps to end.
    void appendRTL(RTL& rtl);           // Append Exps from other RTL to end
    void deepCopyList(std::list<Exp*>& dest);// Make a deep copy of the list of Exp*

     // Print RTL to a stream.
    virtual void print(std::ostream& os = std::cout);

    // Given a map from registers to expressions, replace each use of a reg
    // with the corresponding expression in the map. Then for every
    // definition of such a reg, update the map with the new RHS resulting
    // from the first substitution.
    void subAXP(std::map<Exp*,Exp*>& subMap);


    // Set the RTL's source address
    void updateAddress(ADDRESS addr);

    // Set whether to emit low level C for this RTL as a comment.
    // E.g., in a switch, where a register is loaded from the switch table.
    void setCommented(bool state);

    // Set the number of bytes in the instruction.
    void updateNumBytes(unsigned uNumBytes)
        {numNativeBytes = uNumBytes;}

    // Is this RTL a compare instruction? If so, the passed register and
    // compared value (a semantic string) are set.
    bool isCompare(int& iReg, Exp*& pTerm);

    // Return true if RTL loads the high half of an immediate constant into
    // anything. If so, loads the already shifted high value into the parameter.
    bool isHiImmedLoad(ADDRESS& uHiHalf);

    // As above for low half. Extra parameters are required for SPARC, where
    // bits are potentially transferred from one register to another.
    bool isLoImmedLoad(ADDRESS& uLoHalf, bool& bTrans, int& iSrc);

    // Do a machine dependent, and a standard simplification of the RTL.
    void allSimplify();

    // Perform forward substitutions of temps, if possible. Called from the
    // above
    void forwardSubs();

    // Insert an assignment into this RTL
    //   ssLhs: ptr to Exp to place on LHS
    //   ssRhs: ptr to Exp to place on RHS
    //   prep:  true if prepend (else append)
    //   size:  size of the transfer, or -1 to be the same as the
    //          assign this RTL
    void insertAssign(Exp* ssLhs, Exp* ssRhs, bool prep, int size = -1);

    // Insert an assignment into this RTL, after temps have been defined
    //   ssLhs: ptr to Exp to place on LHS
    //   ssRhs: ptr to Exp to place on RHS
    //   size:  size of the transfer, or -1 to be the same as the
    //          first assign this RTL
    void insertAfterTemps(Exp* ssLhs, Exp* ssRhs, int size = -1);

    // Replace all instances of "search" with "replace".
    virtual void searchAndReplace(Exp* search, Exp* replace,
                                  bool typeSens = false);
    
    // Searches for all instances of "search" and adds them to "result" in
    // reverse nesting order. The search is optionally type sensitive.
    virtual bool searchAll(Exp* search, std::list<Exp*> &result,
                           bool typeSens = false);

protected:
    RTL_KIND kind;
    ADDRESS nativeAddr;         // RTL's source program instruction address
    unsigned numNativeBytes;    // Number of source code bytes from which this
                                // RTL was constructed. Used in coverage
                                // analysis.
    std::list<Exp*> expList;         // List of expressions in this RTL.
    bool isCommented;           // If true, RTL should be emitted as a comment.


#if 0
public:
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    virtual void storeUseDefineStruct(BBBlock& inBlock);
#endif
};


/*=============================================================================
 * HLJump has just one member variable, a semantic string representing the
 * jump's destination (an integer constant for direct jumps; an expression
 * for register jumps). An instance of this class will never represent a
 * return or computed call as these are distinguised by the decoder and are
 * instantiated as HLCalls and HLReturns respecitvely. This class also
 * represents unconditional jumps with a fixed offset (e.g BN, Ba on SPARC).
 *===========================================================================*/
class HLJump: public RTL {
public:
    HLJump(ADDRESS instNativeAddr, std::list<Exp*>* listExp = NULL);
    HLJump(ADDRESS instNativeAddr, ADDRESS jumpDest);
    virtual ~HLJump();

    // Make a deep copy, and make the copy a derived object if needed.
    virtual RTL* clone();

    // Set and return the destination of the jump. The destination is either
    // a Exp, or an ADDRESS that is converted to a Exp.
    void setDest(Exp* pd);
    void setDest(ADDRESS addr);
    virtual Exp* getDest();

    // Return the fixed destination of this CTI. For dynamic CTIs, returns -1.
    ADDRESS getFixedDest();

    // Adjust the fixed destination by a given amount. Invalid for dynamic CTIs.
    void adjustFixedDest(int delta);
    
    // Set and return whether the destination of this CTI is computed.
    // NOTE: These should really be removed, once HLNwayJump and HLNwayCall
    // are implemented properly.
    void setIsComputed();
    bool isComputed();

    virtual void print(std::ostream& os = std::cout);

    // Replace all instances of "search" with "replace".
    void searchAndReplace(Exp* search, Exp* replace,
                          bool typeSens = false);
    
    // Searches for all instances of a given subexpression within this
    // expression and adds them to a given list in reverse nesting order.
    // If typeSens is set, then type is considered during the match
    virtual bool searchAll(Exp* search, std::list<Exp*> &result,
                           bool typeSens = false);

#if 0
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    virtual void storeUseDefineStruct(BBBlock& inBlock);			   
#endif

protected:
    Exp* pDest;              // Destination of a jump or call. This is the
                                // absolute destination for both static and
                                // dynamic CTIs.
    bool m_isComputed;          // True if this is a CTI with a computed
                                // destination address. NOTE: This should be
                                // removed, once HLNwayJump and HLNwayCall
                                // are implemented properly.
};


/*==============================================================================
 * HLJcond has a condition Exp in addition to the destination of the jump.
 *============================================================================*/
class HLJcond: public HLJump {
public:
    HLJcond(ADDRESS instNativeAddr, std::list<Exp*>* listExp = NULL);
    virtual ~HLJcond();

    // Make a deep copy, and make the copy a derived object if needed.
    virtual RTL* clone();

    // Set and return the JCOND_TYPE of this jcond as well as whether the
    // floating point condition codes are used.
    void setCondType(JCOND_TYPE cond, bool usesFloat = false);
    JCOND_TYPE getCond(){ return jtCond; }
    bool isFloat(){ return bFloat; }
    void setFloat(bool b)      { bFloat = b; }

    // Set and return the Exp representing the HL condition
    Exp* getCondExpr();
    void setCondExpr(Exp* pe);
    
    // Probably only used in front386.cc: convert this from an unsigned to a
    // signed conditional branch
    void makeSigned();

    virtual void print(std::ostream& os = std::cout);

    // Replace all instances of "search" with "replace".
    void searchAndReplace(Exp* search, Exp* replace,
                          bool typeSens = false);
    
    // Searches for all instances of a given subexpression within this
    // expression and adds them to a given list in reverse nesting order.
    // If typeSens is set, then type is considered during the match
    virtual bool searchAll(Exp* search, std::list<Exp*> &result,
                           bool typeSens = false);

#if 0
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    void storeUseDefineStruct(BBBlock& inBlock);   
#endif

private:
    JCOND_TYPE jtCond;          // The condition for jumping
    Exp* pCond;              // The Exp representation of the high level
                                // condition: e.g., r[8] == 5
    bool bFloat;                // True if uses floating point CC

};

/*==============================================================================
 * HLNwayJump is derived from HLJump. In addition to the destination of the
 * jump, it has a switch variable Exp.
 *============================================================================*/
typedef struct {
    Exp* pSwitchVar;         // Ptr to Exp repres switch var, e.g. v[7]
    char    chForm;             // Switch form: 'A', 'O', 'R', or 'H'
    int     iLower;             // Lower bound of the switch variable
    int     iUpper;             // Upper bound for the switch variable
    ADDRESS uTable;             // Native address of the table
    int     iNumTable;          // Number of entries in the table (form H only)
    int     iOffset;            // Distance from jump to table (form R only)
    int     delta;              // Host address - Native address
} SWITCH_INFO;

class HLNwayJump: public HLJump {
public:
    HLNwayJump(ADDRESS instNativeAddr, std::list<Exp*>* listExp = NULL);
    virtual ~HLNwayJump();

    // Make a deep copy, and make the copy a derived object if needed.
    virtual RTL* clone();

    // Set and return the Exp representing the switch variable
    SWITCH_INFO* getSwitchInfo(); 
    void setSwitchInfo(SWITCH_INFO* pss);
    
    virtual void print(std::ostream& os = std::cout);

    // Replace all instances of "search" with "replace".
    void searchAndReplace(Exp* search, Exp* replace,
                          bool typeSens = false);
    
    // Searches for all instances of a given subexpression within this
    // expression and adds them to a given list in reverse nesting order.
    // If typeSens is set, then type is considered during the match
    virtual bool searchAll(Exp* search, std::list<Exp*> &result,
                           bool typeSens = false);
    
#if 0
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    void storeUseDefineStruct(BBBlock& inBlock);   
#endif     
     
private:    
    SWITCH_INFO* pSwitchInfo;   // Exp representation of the switch variable:
                                // e.g., r[8]
};

/*==============================================================================
 * HLCall: represents a high level call. Information about parameters and
 * the like are stored here.
 *============================================================================*/
class HLCall: public HLJump {
public:
    HLCall(ADDRESS instNativeAddr, int returnTypeSize = 0,
           std::list<Exp*>* listExp = NULL);
    HLCall::~HLCall();

    // Make a deep copy, and make the copy a derived object if needed.
    virtual RTL* clone();

    // Return true if the called function returns an aggregate: i.e., a
    // struct, union or quad floating point value.
    bool returnsStruct();

    void setBB(PBB BB);                 // Set link from call to enclosing BB
    PBB getBB();                        // Get link from call to enclosing BB

    void setParams(std::list<Exp*>& params); // Set call's parameters
    std::list<Exp*>& getParams();            // Return call's parameters

    void setReturnLoc(Exp* loc);        // Set location used for return value
    Exp* getReturnLoc();                // Get location used for return value

    virtual void print(std::ostream& os = std::cout);

    // Replace all instances of "search" with "replace".
    void searchAndReplace(Exp* search, Exp* replace,
                          bool typeSens = false);
    
    // Searches for all instances of a given subexpression within this
    // expression and adds them to a given list in reverse nesting order.
    // If typeSens is set, then type is considered during the match
    virtual bool searchAll(Exp* search, std::list<Exp*> &result,
                           bool typeSens = false);

    // Set and return whether the call is effectively followed by a return.
    // E.g. on Sparc, whether there is a restore in the delay slot.
    void setReturnAfterCall(bool b);
    bool isReturnAfterCall();

    // Set and return the list of Exps that occur *after* the call (the
    // list of exps in the RTL occur before the call). Useful for odd patterns.
    void setPostCallExpList(std::list<Exp*>* le);
    std::list<Exp*>* getPostCallExpList();

    // Set and return the destination name. Useful for patterns.
    void setDestName(const char* pName);
    const char* getDestName();
    
#if 0
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    void storeUseDefineStruct(BBBlock& inBlock);       
#endif

private:
    int returnTypeSize;         // Size in bytes of the struct, union or quad FP
                                // value returned by the called function.
    PBB basicBlock;             // The call's enclosing basic block.
    Exp* returnLoc;             // Location used to hold value returned by call.
    bool returnAfterCall;       // True if call is effectively followed by
                                // a return.
    
    // The list of locations that are live at this call. This list may be
    // refined at a later stage to match the number of parameters declared
    // for the called procedure.
    std::list<Exp*> params;

    // List of reg transfers that occur *after* the call.
    std::list<Exp*>* postCallExpList;

    // The name of the destination. (Note: this is usually found by looking
    // up the destination using the BinaryFile object, but in this case the
    // name is totally synthetic.) When used, getFixedDest() returns 0
    const char* pDestName;
};


/*==============================================================================
 * HLReturn: represents a high level return.
 *============================================================================*/
class HLReturn: public HLJump {
public:
    HLReturn(ADDRESS instNativeAddr, std::list<Exp*>* listExp = NULL);
    ~HLReturn();

    // Make a deep copy, and make the copy a derived object if needed.
    virtual RTL* clone();

#if 0
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    void storeUseDefineStruct(BBBlock& inBlock);   				    
#endif
};


/*==============================================================================
 * HLScond represents "setCC" type instructions, where some destination is
 * set (to 1 or 0) depending on the condition codes. It has a condition
 * Exp, similar to the HLJcond class.
 * *==========================================================================*/
class HLScond: public RTL {
public:
    HLScond(ADDRESS instNativeAddr, std::list<Exp*>* listExp = NULL);
    virtual ~HLScond();

    // Make a deep copy, and make the copy a derived object if needed.
    virtual RTL* clone();

    // Set and return the JCOND_TYPE of this scond as well as whether the
    // floating point condition codes are used.
    void setCondType(JCOND_TYPE cond, bool usesFloat = false);
    JCOND_TYPE getCond(){return jtCond;}
    bool isFloat(){return bFloat;}
    void setFloat(bool b) { bFloat = b; }

    // Set and return the Exp representing the HL condition
    Exp* getCondExpr();
    void setCondExpr(Exp* pss);

    Exp* getDest();                 // Return the destination of the set
    int getSize();                  // Return the size of the assignment

    void makeSigned();

    virtual void print(std::ostream& os = std::cout);

#if 0
    // Used for type analysis. Stores type information that
    // can be gathered from the RTL instruction inside a
    // data structure within BBBlock inBlock
    void storeUseDefineStruct(BBBlock& inBlock);       
#endif

private:
    JCOND_TYPE jtCond;             // the condition for jumping
    Exp* pCond;                 // Exp representation of the high level
                                   // condition: e.g. r[8] == 5
    bool bFloat;                   // True if condition uses floating point CC
};


/*==============================================================================
 * The TableEntry class represents a single instruction - a string/RTL pair.
 *
 * This class plus ParamEntry and RTLInstDict should be moved to a separate
 * header file...
 *============================================================================*/

class TableEntry {
public:
    TableEntry();
    TableEntry(std::list<std::string>& p, RTL& rtl);

    const TableEntry& operator=(const TableEntry& other);

    void setParam(std::list<std::string>& p);
    void setRTL(RTL& rtl);
    
    // non-zero return indicates failure
    int appendRTL(std::list<std::string>& p, RTL& rtl);
    
public:
    std::list<std::string> params;
    RTL rtl;

#define TEF_NEXTPC 1        
    int flags;                  // aka required capabilities. Init. to 0 
};


/*==============================================================================
 * The ParamEntry class represents the details of a single parameter.
 *============================================================================*/
typedef enum {PARAM_SIMPLE, PARAM_EXPR, PARAM_LAMBDA, PARAM_VARIANT} ParamKind;

class ParamEntry {
 public:
    ParamEntry() {
        exp = NULL;
        kind = PARAM_SIMPLE;
        type = Type();
        regType = Type(TVOID,0,false);
        lhs = false;
        mark = 0;
    }
    
    std::list<std::string> params;        /* PARAM_VARIANT & PARAM_EXPR only */
    std::list<std::string> funcParams;    /* PARAM_LAMBDA - late bound params */
    Exp* exp;                   /* PARAM_EXPR only */
    bool lhs;                   /* True if this param ever appears on the LHS
                                 * of an expression */
    ParamKind kind;
    Type type;
    Type regType;               /* Type of r[this], if any (void otherwise) */
    std::set<int> regIdx;            /* Values this param can take as an r[param] */
    int mark;                   /* Traversal mark. (free temporary use,
                                 * basically) */
};

class PartialType;


/*==============================================================================
 * The RTLInstDict represents a dictionary that maps instruction names to the
 * parameters they take and a template for the Exp list describing their
 * semantics. It handles both the parsing of the SSL file that fills in
 * the dictionary entries as well as instantiation of an Exp list for a given
 * instruction name and list of actual parameters. 
 *============================================================================*/

class RTLInstDict {
public:
    // Parse a file containing a list of instructions definitions in SSL format
    // and build the contents of this dictionary.
    bool readSSLFile(const std::string& SSLFileName, bool bPrint = false);

    // Reset the object to "undo" a readSSLFile()
    void reset();

    // Return the signature of the given instruction.
    std::pair<std::string,unsigned> getSignature(const char* name);

    // Appends an RTL to an idict entry, or Adds it to idict if an
    // entry does not already exist. A non-zero return indicates failure.
    int appendToDict(std::string &n, std::list<std::string>& p, RTL& rtl);
    
    // Given an instruction name and list of actual parameters, return an
    // instantiated RTL for the corresponding instruction entry.
    std::list<Exp*>* instantiateRTL(std::string& name, std::vector<Exp*>& actuals);
    // As above, but takes an RTL & param list directly rather than doing
    // a table lookup by name.
    std::list<Exp*>* instantiateRTL(RTL& rtls, std::list<std::string> &params,
                              std::vector<Exp*>& actuals);

    // Transform the given list into another list which doesn't have
    // post-variables, by either adding temporaries or just removing them
    // where possible. Modifies the list passed, and also returns a pointer
    // to it. Second parameter indicates whether the routine should attempt
    // to optimize the resulting output, ie to minimize the number of
    // temporaries. This is recommended for fully expanded expressions (ie
    // within uqbt), but unsafe otherwise.
    std::list<Exp*>* transformPostVars(std::list <Exp*> *rts, bool optimize);

    void print(std::ostream& os = std::cout);
    
    // Add a new register to the machine
    void addRegister(const char *name, int id, int size, bool flt);

    // If the top level operator of the given expression indicates any kind
    // of type, update ty to match.
    bool partialType(Exp* exp, Type& ty);

    // Scan the assignment given, and attempt to determine the
    // correct overall type. If the type is decideable, updates the expression
    // appropriately and returns true. Other the function return false.
    bool decideType(TypedExp* asgn);

#if 0
    // Type checking and evaluation functions
    std::set<int> evaluateExp(const Exp &ss) const;
    std::set<int> evaluateBitsliceRange(const Exp &lo, const Exp &hi) const;
    int matchRegIdx(const Exp &ss) const;
    std::vector<Type> computeRHSTypes(Exp &ss, int sz) const;
    std::vector<Type> computeLHSTypes(Exp &ss) const;
    std::vector<Type> computeRTAssgnTypes(RTAssgn &rta) const;
#endif
    
    // Go through the params and fixup any lambda functions
    void fixupParams();

public:
    // A map from the symbolic representation of a register (e.g. "%g0")
    // to its index within an array of registers.
    std::map<std::string, int, std::less<std::string> > RegMap;

    // Similar to r_map but stores more information about a register such as its
    // size, its addresss etc (see reg.h).
    std::map<int, Register, std::less<int> > DetRegMap;

    // A map from symbolic representation of a special (non-addressable)
    // register to a Register object
    std::map<std::string, Register, std::less<std::string> > SpecialRegMap;

    // A set of parameter names, to make sure they are declared (?).
    // Was map from string to SemTable index
    std::set<std::string> ParamSet;

    // Parameter (instruction operand, more like addressing mode) details
    // (where given)
    std::map<std::string, ParamEntry> DetParamMap;

    // The maps which summarise the semantics (.ssl) file
    std::map<std::string, Exp*> FlagFuncs;
    std::map<std::string, std::pair<int, void*>*> DefMap;
    std::map<int, Exp*> AliasMap;

    // Map from ordinary instruction to fast pseudo instruction, for use with
    // -f (fast but not as exact) switch
    std::map<std::string, std::string> fastMap;

    bool bigEndian;                // True if this source is big endian

    // The actual dictionary.
    std::map<std::string, TableEntry, std::less<std::string> > idict;

    // An RTL describing the machine's basic fetch-execute cycle
    RTL *fetchExecCycle;

#if 0           // Only used for the type checker?
protected:
    int evaluateExp(const Exp &ss, std::map<int,int> &params,
                       std::map<int,int> &regs) const;
    int evaluateExp(SSCIT &it, SSCIT &end, std::map<int,int> &params,
                       std::map<int,int> &regs) const;
    PartialType computeTypes(Exp &ss, const PartialType &in,
                             std::vector<PartialType> &types) const;
    PartialType computeTypes(Exp &ss, SSIT &it, SSIT &end,
                             const PartialType &in,
                             std::vector<PartialType> &types) const;
    void distributeTypes(const Exp &ss, const PartialType &in,
                         std::vector<PartialType> &types, vector<Type> &out) const;
    void distributeTypes(const Exp &ss, SSCIT &it, SSCIT &end,
                         const PartialType &in,
                         std::vector<PartialType>::iterator &type,
                         std::vector<Type> &out) const;
    std::set<int> evaluateExp(const std::list<Exp> &lss,
                            int (*callback)(std::list<int> &)) const;
    
#endif
    void fixupParamsSub(std::string s, std::list<std::string>& funcParams, bool& haveCount,
                        int mark);
};

#endif /*__RTL_H__*/
