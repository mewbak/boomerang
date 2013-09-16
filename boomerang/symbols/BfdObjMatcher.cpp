#include "BfdObjMatcher.h"
#include "BytePattern.h"

int strcmpi(const char* s1, const char* s2);            // See util/util.cpp


BfdObjMatcher::BfdObjMatcher(Prog *prog, const char *sSymbolContainer)
    :SymbolMatcher(prog, sSymbolContainer),
     m_bfd(NULL),
     m_symbol_table(NULL),
     m_current_section(0)
{
}

BfdObjMatcher::~BfdObjMatcher(void)
{
    Unload();
}

int BfdObjMatcher::Total()
{
    int total = 0;
    bfd_section *sec = m_bfd->sections;
    while(sec)
        {
            // just count code sections
            if(sec->flags & SEC_CODE)
                total ++;
            sec = sec->next;
        }
    return total;
}

bool BfdObjMatcher::Next()
{

    // find next code section
    do
        {
            if(!m_current_section)
                m_current_section = m_bfd->sections;
            else
                m_current_section = m_current_section->next;
        }
    while(m_current_section && !(m_current_section->flags & SEC_CODE ));

    return !Finished();

}

bool BfdObjMatcher::Finished()
{

    return m_current_section == NULL;
}

bool BfdObjMatcher::GetSymbolInfo(SymbolInfo *symInfo)
{
    if(!m_symbol_table)
        return false;

    symInfo->name = get_comdat_info()->name;
    symInfo->size = bfd_section_size(m_bfd, m_current_section);

    // assume function
    symInfo->type = SYMBOL_TYPE_FUNCTION;

    return true;
}


// Applies the symbol file
// to the executable
bool BfdObjMatcher::Match()
{

    if(!m_bfd)
        return false;

    bool found = false;

    // load function byte codes
    unsigned char *func_data = new unsigned char[bfd_section_size(m_bfd, m_current_section)];
    bool ret = bfd_get_section_contents(m_bfd,
                                        m_current_section,
                                        func_data,
                                        0,
                                        bfd_section_size(m_bfd, m_current_section));

    // now get all wild bytes in the
    // function codes
    long reloc_size = bfd_get_reloc_upper_bound(m_bfd, m_current_section);

    arelent **reloc_data = new arelent *[reloc_size];

    int relocs = bfd_canonicalize_reloc(m_bfd, m_current_section,
                                        reloc_data, m_symbol_table);
    // on success
    if(relocs >= 0)
        {

            // create a byte pattern to be matched
            BytePattern fpat(func_data, bfd_section_size(m_bfd, m_current_section));

            for(int i=0; i<relocs; i++)
                {

                    // find out reloc item size
                    int reloc_item_size = bfd_get_reloc_size(reloc_data[i]->howto);

                    // mark wild bytes
                    fpat.FlagWildBytes(reloc_data[i]->address,
                                       reloc_item_size);

                }

            // now match the pattern against code sections
            int total_sections = GetTotalSections();
            int idx = 0;
            while(idx < total_sections)
                {

                    PSectionInfo sec_info = GetSectionInfo(idx);

                    // if it is a code section
                    if(sec_info->bCode)
                        {
                            int match_pos = fpat.Match((unsigned char *)sec_info->uHostAddr, sec_info->uSectionSize);

                            // if function found
                            if(match_pos >= 0)
                                {

                                    // create the function
                                    if(get_comdat_info())
                                        {

                                            std::string func_proto = Demangle(get_comdat_info()->name);

                                            m_prog->newProc(func_proto.c_str(),
                                                            sec_info->uNativeAddr + match_pos,
                                                            true);


                                            printf("Matched: %s\n", func_proto.c_str());
                                        }
                                    // add any symbols referenced by this
                                    // function
                                    for(int i=0; i<relocs; i++)
                                        {

                                            // find out reloc item size
                                            int reloc_item_size = bfd_get_reloc_size(reloc_data[i]->howto);

                                            std::string sym_proto = Demangle(reloc_data[i]->sym_ptr_ptr[0]->name);

                                            // add the symbol
                                            AddSymbol((reloc_data[i]->howto->pc_relative ? sec_info->uNativeAddr + match_pos + reloc_data[i]->address : 0) +
                                                      *(unsigned long *)((char *)sec_info->uHostAddr + match_pos + reloc_data[i]->address) -
                                                      *(unsigned long *)(func_data + reloc_data[i]->address),
                                                      sym_proto.c_str(),
                                                      NULL);
                                            printf("Symbol Reference: %s\n", sym_proto.c_str());




                                        }

                                }
                        }

                    idx ++;
                }

        }
    delete [] reloc_data;

    if(!ret)
        goto cleanup;


    //search for a match

cleanup:
    delete [] func_data;

    return found;


}

bool BfdObjMatcher::Load()
/**
	loads the BFD object
*/
{
    bfd_init();
    m_bfd = bfd_openr(m_sSymbolContainer.c_str(), NULL);

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
    Next();

    return true;

}


void BfdObjMatcher::Unload()
/**
	Unloads the bfd object
*/
{
    // clean up symbol table
    if(m_symbol_table)
        delete []m_symbol_table;
    m_symbol_table = NULL;


    // clean up bfd object
    if(m_bfd)
        bfd_close(m_bfd);
    m_bfd = NULL;
}

bool BfdObjMatcher::InitSymtab()
/**
	Loads the symbol table of the bfd object
*/
{
    int storage_needed = bfd_get_symtab_upper_bound (m_bfd);
    if (storage_needed <= 0)
        return false;

    m_symbol_table = new asymbol * [storage_needed / sizeof(asymbol *)];
    m_nTotalSymbols =
        bfd_canonicalize_symtab (m_bfd, m_symbol_table);

    if (m_nTotalSymbols < 0)
        return false;

    return true;
}

// returns true if this class can handle
// this type of sumbol container
bool BfdObjMatcher::CanHandle(const char *sSymbolContainer)
{
    int s = strlen(sSymbolContainer);
    if(s > 4 && !strcmpi(sSymbolContainer + s - 4, ".obj"))
        return true;

    return false;
}

bool BfdObjMatcher::Init()
{
    if(!Load())
        {
            if(m_bfd)
                Unload();
            return false;
        }
    return true;
}

std::string BfdObjMatcher::Demangle(const char *mangled_name)
/**
	Demangle the sepecified symbol based
	on the type of object file
*/
{

    return mangled_name + 1;
//	return do_symbol_demangle(m_bfd->arch_info,
//								m_
}

COMDAT_INFO *BfdObjMatcher::get_comdat_info()
{
#ifdef BFD_2_15
    return m_current_section->comdat;
#else
    return bfd_coff_get_comdat_section(m_bfd, m_current_section);
#endif
}
