set(boomerang_db_sources
	basicblock.cpp
	cfg.cpp
	dataflow.cpp
	exp.cpp
	insnameelem.cpp
	managed.cpp
	proc.cpp
	prog.cpp
	register.cpp
	rtl.cpp
	signature.cpp
	sslinst.cpp
	sslparser.cpp
	sslscanner.cpp
	statement.cpp
	table.cpp
	visitor.cpp
)
# for ansi-c parser includes
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/c)

ADD_LIBRARY(boomerang_db STATIC ${boomerang_db_sources})
