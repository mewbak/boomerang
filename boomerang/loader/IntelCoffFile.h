#ifndef __INTELCOFFFILE_H__
#define __INTELCOFFFILE_H__

#include <stdint.h>
#include "BinaryFile.h"
#include "SymTab.h"
#ifdef _MSC_VER
#define PACKED
#else
#define PACKED __attribute__((packed))
#endif

#ifdef _MSC_VER
#pragma pack(push, 1)
#endif
struct coff_header
  {
    unsigned short  coff_magic;
    unsigned short  coff_sections;
    unsigned long   coff_timestamp;
    unsigned long   coff_symtab_ofs;
    unsigned long   coff_num_syment;
    unsigned short  coff_opthead_size;
    unsigned short  coff_flags;
  }
PACKED;
#ifdef _MSC_VER
#pragma pack(pop)
#endif

class IntelCoffFile : public BinaryFile
  {
  public:
    //
    // Interface
    //
    IntelCoffFile();
    ~IntelCoffFile();

    virtual void UnLoad();

    virtual bool Open(const char *sName);
    virtual bool RealLoad(const char*);
    virtual bool PostLoad(void*);
    virtual void Close();

    virtual LOAD_FMT    GetFormat() const;
    virtual MACHINE     GetMachine() const;
    virtual const char *getFilename() const;
    virtual bool        isLibrary() const;
    virtual std::list<const char *> getDependencyList();
    virtual ADDRESS     getImageBase();
    virtual size_t      getImageSize();
    virtual ADDRESS     GetMainEntryPoint();
    virtual ADDRESS     GetEntryPoint();
    virtual std::list<SectionInfo*>& GetEntryPoints(const char* pEntry = "main");

    virtual const char* SymbolByAddress(ADDRESS uNative);
    // Lookup the name, return the address. If not found, return NO_ADDRESS
    // virtual ADDRESS         GetAddressByName(const char* pName, bool bNoTypeOK = false);
    // virtual void            AddSymbol(ADDRESS uNative, const char *pName) { }
    virtual bool IsDynamicLinkedProc(ADDRESS uNative);
    virtual bool IsRelocationAt(ADDRESS uNative);
    virtual std::map<ADDRESS, std::string> &getSymbols();

    virtual int readNative4(ADDRESS a);
    virtual int readNative2(ADDRESS a);
    virtual int readNative1(ADDRESS a);

  private:
    //
    // Internal stuff
    //
    const char *m_pFilename;
    FILE *m_fd;
    std::list<SectionInfo*> m_EntryPoints;
    std::list<ADDRESS> m_Relocations;
    struct coff_header m_Header;

    PSectionInfo AddSection(SectionInfo*);
    unsigned char* getAddrPtr(ADDRESS a, ADDRESS range);
    int readNative(ADDRESS a, unsigned short n);

    SymTab m_Symbols;
  };

#endif	// !defined(__INTELCOFFFILE_H__)
