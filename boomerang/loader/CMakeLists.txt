
ADD_LIBRARY(BinaryFile SHARED 
			BinaryFile.cpp 
			BinaryFileFactory.cpp
			SymTab.cpp 
			SymTab.h
			)
TARGET_LINK_LIBRARIES(BinaryFile ${CMAKE_DL_LIBS})
# This will be removed to use CMAKE default shared library 
# define : ${}_Exports
SET_TARGET_PROPERTIES(BinaryFile PROPERTIES 
						COMPILE_DEFINITIONS 
							BUILDING_LIBBINARYFILE)

SET(BOOMERANG_LOADERS ${BOOMERANG_LOADERS} BinaryFile)		


BOOMERANG_ADD_LOADER(ElfBinaryFile)
BOOMERANG_ADD_LOADER(Win32BinaryFile microX86dis.c)
BOOMERANG_ADD_LOADER(ExeBinaryFile)
BOOMERANG_ADD_LOADER(HpSomBinaryFile)
BOOMERANG_ADD_LOADER(PalmBinaryFile)
BOOMERANG_ADD_LOADER(DOS4GWBinaryFile microX86dis.c)
BOOMERANG_ADD_LOADER(IntelCoffFile)
IF(NOT ${CYGWIN})
BOOMERANG_ADD_LOADER(MachOBinaryFile)
ENDIF(NOT ${CYGWIN})

# in include files this is expressed as:
# if defined(_WIN32) && !defined(__MINGW32__) 
IF(MSVC OR CYGWIN)
	TARGET_LINK_LIBRARIES(Win32BinaryFile dbghelp) 
ENDIF(MSVC OR CYGWIN)

SET(bff_SOURCES
	bffDump.cpp
	BinaryFileFactory.cpp
)
ADD_EXECUTABLE(bffDump ${bff_SOURCES})
TARGET_LINK_LIBRARIES(bffDump BinaryFile ${CMAKE_DL_LIBS})
ADD_DEPENDENCIES(bffDump ${BOOMERANG_LOADERS})

install(TARGETS bffDump DESTINATION bin)



