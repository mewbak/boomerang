#pragma once
#include "SymbolMatcher.h"
#include "bfd.h"

// if BFD library version is less than 2.15
//#define BFD_2_15

#ifdef BFD_2_15
#define COMDAT_INFO bfd_comdat_info
#else
#define COMDAT_INFO coff_comdat_info
#endif

class BfdObjMatcher :
    public SymbolMatcher
{
public:
    BfdObjMatcher(Prog *prog, const char *sSymbolContainer);
    virtual ~BfdObjMatcher(void);

    // returns true if this class can handle
    // this type of sumbol container
    static bool CanHandle(const char *sSymbolContainer);

    /**
    	returns total number of symbols in this symbol container
    	returns -1 if not supported
    */
    virtual int Total();

    /**
    	returns information about current symbol
    	returns false if not supported, or called
    	when no symbol is loaded
    */
    virtual bool GetSymbolInfo(SymbolInfo *symInfo);

    /**
    	moves to the next symbol
    */
    virtual bool Next();

    /**
    	returns true when there are no symbols
    	remaining, false otherwise
    */
    virtual bool Finished();


    /**
    	Applies the current symbol
    	to the executable
    */
    virtual bool Match();

protected:

    /**
    	initializes the object
    */
    virtual bool Init();

    // loads the BFD object
    virtual bool Load();

    virtual void Unload();

    // returns comdat info from current symbol
    COMDAT_INFO	*get_comdat_info();

    /**
    	Loads the symbol table of the bfd object
    */
    virtual bool InitSymtab();

    /**
    	Demangle the sepecified symbol based
    	on the type of object file
    */
    std::string Demangle(const char *mangled_name);

    bfd *m_bfd;
    asymbol **m_symbol_table;

    // holds current section
    bfd_section * m_current_section;

    int m_nTotalSymbols;

};
