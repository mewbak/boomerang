#pragma once
#include "SymbolMatcher.h"

class DynLibMatcher :
    public SymbolMatcher
{
public:
    DynLibMatcher(Prog *prog);
    ~DynLibMatcher(void);
};
