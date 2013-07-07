SET(boomerang_c_codegen_sources
	chllcode.cpp
	syntax.cpp
	chllcode.h
)
BOOMERANG_ADD_CODEGEN(c_language ${boomerang_c_codegen_sources})

SET(BOOMERANG_CODE_GENERATORS ${_BOOMERANG_CODE_GENERATORS} CACHE INTERNAL "" )		

