#pragma once
#include "BfdObjMatcher.h"

class BfdArchMatcher :
    public BfdObjMatcher
{
public:
    BfdArchMatcher(Prog *prog, const char *sSymbolContainer);
    virtual ~BfdArchMatcher(void);

    // returns true if this class can handle
    // this type of sumbol container
    static bool CanHandle(const char *sSymbolContainer);

    /**
    	moves to the next symbol
    */
    virtual bool Next();

protected:

    /**
    	initializes the object
    */
    virtual bool Init();

    // loads the BFD object
    virtual bool Load();

    virtual void Unload();

    bool GetNextBFD();

    void ObjUnload();

    bfd *m_arch_bfd;
};
