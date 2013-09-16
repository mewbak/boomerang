#pragma once
#include <string>
#include "prog.h"


// early declaration
class SymbolMatcher;

/**
	Factory class for symbol matcher
	used to find the compatible symbol matcher
	class for a symbol container file
	*/
class SymbolMatcherFactory
{
public:
    /** Loads the compatible library matching class
        for the specified input file or by hint
        'hint' is used to force specified library matching
        class to be used for the input file

    	*/
    static SymbolMatcher * getInstanceFor(Prog *prog, const char *sSymbolContainer);
    static SymbolMatcher * getInstanceFor(Prog *prog, const char *sSymbolContainer, const char *hint);
};



struct SymbolInfo
{
    std::string name;
    std::string type;
    int size;
};

// default symbol types
#define SYMBOL_TYPE_UNKNOWN		"unknown"
#define SYMBOL_TYPE_FUNCTION	"function"
#define SYMBOL_TYPE_DATA		"data"


/**
	Abstract class for symbol matcher classes
	*/
class SymbolMatcher
{
public:
    SymbolMatcher(Prog *prog, const char *sSymbolContainer);
    virtual		~SymbolMatcher(void);

    /**
    	moves to the next symbol,
    	returns false if no more symbols or
    	on error
    */
    virtual bool Next() = 0;

    /**
    	returns true when there are no symbols
    	remaining, false otherwise
    */
    virtual bool Finished() = 0;

    /**
    	Applies the current symbol
    	to the executable
    */
    virtual bool Match() = 0;


    //////////// From here optional methods

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
    	Applies all symbols in this symbol container
    */
    virtual void MatchAll();


protected:

    /**
    	initializes the object
    */
    virtual bool Init() = 0;

    int GetTotalSections();
    PSectionInfo GetSectionInfo(int index);
    void AddSymbol(ADDRESS addr, const char *name, Type *type);

    Prog *m_prog;
    std::string m_sSymbolContainer;

    // for Init()
    friend class SymbolMatcherFactory;
};


typedef SymbolMatcher *(*SYMMATCH_FACTORY)(Prog *prog, const char *sSymbolContainer, const char *hint);

