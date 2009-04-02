SET(boomerang_frontend_common_sources
frontend.cpp
njmcDecoder.cpp
)
# for ansi-c parser includes
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/c)

ADD_LIBRARY(Frontend_common STATIC ${boomerang_frontend_common_sources})
SET(BOOMERANG_FRONTENDS ${BOOMERANG_FRONTENDS} Frontend_common CACHE INTERNAL "")		

BOOMERANG_ADD_FRONTEND(pentium)
BOOMERANG_ADD_FRONTEND(mips)
BOOMERANG_ADD_FRONTEND(ppc)
BOOMERANG_ADD_FRONTEND(sparc)
BOOMERANG_ADD_FRONTEND(st20)
