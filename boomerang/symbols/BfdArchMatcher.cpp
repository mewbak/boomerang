#include "BfdArchMatcher.h"

int strcmpi(const char* s1, const char* s2);            // See util/util.cpp

BfdArchMatcher::BfdArchMatcher(Prog *prog, const char *sSymbolContainer)
    :BfdObjMatcher(prog, sSymbolContainer),
     m_arch_bfd(NULL)
{
}

BfdArchMatcher::~BfdArchMatcher(void)
{
}

bool BfdArchMatcher::Load()
/**
	loads the BFD object
*/
{
    bfd_init();
    m_arch_bfd = bfd_openr(m_sSymbolContainer.c_str(), NULL);

    if(!m_arch_bfd)
        return false;

    if (!bfd_check_format (m_arch_bfd, bfd_archive))
        {
            return false;
        }

    if(!GetNextBFD())
        return false;


    return true;

}


void BfdArchMatcher::Unload()
/**
	Unloads the bfd object
*/
{
    ObjUnload();

    // don't allow closing of m_bfd
    m_bfd = NULL;
    BfdObjMatcher::Unload();

    // clean up bfd object
    if(m_arch_bfd)
        bfd_close(m_arch_bfd);
    m_arch_bfd = NULL;

}


bool BfdArchMatcher::Next()
{

    if(!BfdObjMatcher::Next())
        {
            ObjUnload();
            return GetNextBFD();
        }
    return true;
}

bool BfdArchMatcher::GetNextBFD()
{
    m_bfd = bfd_openr_next_archived_file(m_arch_bfd, m_bfd);

    if(!m_bfd)
        return false;

    if (!bfd_check_format (m_bfd, bfd_object))
        {
            return false;
        }

    if(!InitSymtab())
        return false;


    // start with the first section
    m_current_section = NULL;
    return Next();
}


// returns true if this class can handle
// this type of sumbol container
bool BfdArchMatcher::CanHandle(const char *sSymbolContainer)
{
    int s = strlen(sSymbolContainer);
    if(s > 4 && !strcmpi(sSymbolContainer + s - 4, ".lib"))
        return true;

    return false;
}

bool BfdArchMatcher::Init()
{
    if(!Load())
        {
            if(m_bfd)
                Unload();
            return false;
        }
    return true;
}

void BfdArchMatcher::ObjUnload()
{
    bfd *old = m_bfd;
    // don't allow closing of m_bfd
    m_bfd = NULL;
    BfdObjMatcher::Unload();

    m_bfd = old;
}
