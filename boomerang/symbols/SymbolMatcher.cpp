#include "SymbolMatcher.h"
#include "BfdObjMatcher.h"
#include "BfdArchMatcher.h"

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

// just to be exported for using libid as a dynamically linked library
extern "C" EXPORT SymbolMatcher *getInstanceFor(Prog *prog, const char *sSymbolContainer, const char *hint)
{
    if(hint)
        return SymbolMatcherFactory::getInstanceFor(prog, sSymbolContainer, hint);
    else
        return SymbolMatcherFactory::getInstanceFor(prog, sSymbolContainer, hint);

}



SymbolMatcher * SymbolMatcherFactory::getInstanceFor(Prog *prog, const char *sSymbolContainer)
{

    SymbolMatcher *sm = NULL;
    if(BfdObjMatcher::CanHandle(sSymbolContainer))
        sm =  new BfdObjMatcher(prog, sSymbolContainer);

    else if(BfdArchMatcher::CanHandle(sSymbolContainer))
        sm = new BfdArchMatcher(prog, sSymbolContainer);

    // initialize the object
    if(sm &&!sm->Init())
        {
            delete sm;
            sm = NULL;
        }

    return sm;
}

SymbolMatcher * SymbolMatcherFactory::getInstanceFor(Prog *prog, const char *sSymbolContainer, const char *hint)
{

    return new BfdObjMatcher(prog, sSymbolContainer);
}


SymbolMatcher::SymbolMatcher(Prog *prog, const char *sSymbolContainer)
    :m_prog(prog),
     m_sSymbolContainer(sSymbolContainer)
{
}

SymbolMatcher::~SymbolMatcher(void)
{
}

void SymbolMatcher::MatchAll()
{
    while(!Finished())
        {
            Match();
            Next();
        }
}

int SymbolMatcher::Total()
{
    return -1; // not supported
}

bool SymbolMatcher::GetSymbolInfo(SymbolInfo *symInfo)
{
    return false; // not supported
}

int SymbolMatcher::GetTotalSections()
{
    return m_prog->pBF->GetNumSections();

}

PSectionInfo SymbolMatcher::GetSectionInfo(int index)
{
    return m_prog->pBF->GetSectionInfo(index);
}

void SymbolMatcher::AddSymbol(ADDRESS addr, const char *name, Type *type)
{
    printf("Symbol added: %x -> %s\n", addr, name);
    m_prog->pBF->AddSymbol(addr, name);

}
