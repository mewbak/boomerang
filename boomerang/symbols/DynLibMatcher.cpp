#include "DynLibMatcher.h"

DynLibMatcher::DynLibMatcher(Prog *prog)
    :SymbolMatcher(prog, "X")
{
}

DynLibMatcher::~DynLibMatcher(void)
{
}
