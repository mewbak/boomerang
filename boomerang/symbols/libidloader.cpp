

#ifndef _WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include "SymbolMatcher.h"
#include "config.h"				// For UNDERSCORE_NEEDED etc

#include <iostream>


#define FACTORY_PROC "getInstanceFor"

SymbolMatcher * SymbolMatcherFactory_getInstanceFor(Prog *prog, const char *sSymbolContainer, const char *hint)
{
    std::string libName = "libid";
    SymbolMatcher *res;


// Load the specific loader library
#ifndef _WIN32		// Cygwin, Unix/Linux
    libName = std::string("lib/lib") + libName;
#ifdef	__CYGWIN__
    libName += ".dll";		// Cygwin wants .dll, but is otherwise like Unix
#else
#if HOST_OSX
    libName += ".dylib";
#else
    libName += ".so";
#endif
#endif
    static void* dlHandle = dlopen(libName.c_str(), RTLD_LAZY);
    if (dlHandle == NULL)
        {
            fprintf( stderr, "Could not open dynamic loader library %s\n", libName.c_str());
            fprintf( stderr, "%s\n", dlerror());
            //fclose(f);
            return NULL;
        }
    // Use the handle to find the "construct" function
#if UNDERSCORE_NEEDED
#define UNDERSCORE "_"
#else
#define UNDERSCORE
#endif
    SYMMATCH_FACTORY pFcn = (SYMMATCH_FACTORY) dlsym(dlHandle, UNDERSCORE FACTORY_PROC);
#else						// Else MSVC, MinGW
    libName += ".dll";		// Example: ElfBinaryFile.dll (same dir as boomerang.exe)
#ifdef __MINGW32__
    libName = "lib/lib" + libName;
#endif

    static HMODULE hModule = LoadLibrary(libName.c_str());
    if(hModule == NULL)
        {
            int err = GetLastError();
            fprintf( stderr, "Could not open dynamic loader library %s (error #%d)\n", libName.c_str(), err);
            return NULL;
        }
    // Use the handle to find the "construct" function
    SYMMATCH_FACTORY pFcn = (SYMMATCH_FACTORY) GetProcAddress((HINSTANCE)hModule, FACTORY_PROC);
#endif

    if (pFcn == NULL)
        {
            fprintf( stderr, "Loader library %s does not have a "FACTORY_PROC" function\n", libName.c_str());
#ifndef _WIN32
            fprintf( stderr, "dlerror returns %s\n", dlerror());
#endif
            return NULL;
        }
    // Call the construct function
    res = (*pFcn)(prog, sSymbolContainer, hint);
    return res;
}
