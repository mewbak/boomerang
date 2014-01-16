ADD_SUBDIRECTORY(machine)

SET(boomerang_frontend_common_sources
	frontend.cpp
	njmcDecoder.cpp
)
# for ansi-c parser includes
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/c)

ADD_LIBRARY(Frontend_common STATIC ${boomerang_frontend_common_sources})

BOOMERANG_ADD_FRONTEND(pentium)
BOOMERANG_ADD_FRONTEND(mips)
BOOMERANG_ADD_FRONTEND(ppc)
BOOMERANG_ADD_FRONTEND(sparc)
BOOMERANG_ADD_FRONTEND(st20)

SET(BOOMERANG_FRONTENDS Frontend_common ${_BOOMERANG_FRONTENDS} CACHE INTERNAL "")

ADD_DEPENDENCIES(Frontend_common st20)