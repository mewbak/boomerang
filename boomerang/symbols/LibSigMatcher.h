#pragma once
#include "SymbolMatcher.h"

class LibSigMatcher :
    public SymbolMatcher
{
public:
    LibSigMatcher(Prog *prog);
    ~LibSigMatcher(void);
};
