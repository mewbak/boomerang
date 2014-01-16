#
# db/sslparser.cpp db/sslparser.h: db/sslparser.y db/sslscanner.l
#	$(BISONPP) -d -v -odb/sslparser.cpp -hdb/sslparser.h db/sslparser.y
#	sed -e "s/int __alloca_free_ptr/static int __alloca_free_ptr/" db/sslparser.cpp > db/sslparser.cpp1
#	mv db/sslparser.cpp1 db/sslparser.cpp
#
# db/sslscanner.cpp db/sslscanner.h: db/sslscanner.l db/sslparser.y
#	$(FLEXPP) -odb/sslscanner.cpp -hdb/sslscanner.h db/sslscanner.l
#	# Fix warnings re array subscript has type char
#	sed -f db/sslscanner.sed db/sslscanner.cpp > db/sslscanner.cpp1
#	mv db/sslscanner.cpp1 db/sslscanner.cpp

IF(BISONPP_EXECUTABLE)
add_custom_command(
	COMMENT "Building Parser - Bison++"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.y
	COMMAND ${BISONPP_EXECUTABLE} -d -v -o${CMAKE_CURRENT_SOURCE_DIR}/sslparser.cpp -h${CMAKE_CURRENT_SOURCE_DIR}/sslparser.h ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.y
	COMMAND sed -e "s/int __alloca_free_ptr/static int __alloca_free_ptr/" ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.cpp > ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.cpp.1
	COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.cpp.1 ${CMAKE_CURRENT_SOURCE_DIR}/sslparser.cpp
)
ENDIF(BISONPP_EXECUTABLE)

IF(FLEXPP_EXECUTABLE) 
add_custom_command(
	COMMENT "Building Scanner - Flex++"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.l
	COMMAND ${FLEXPP_EXECUTABLE} -o${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.cpp -h${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.h ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.l
	COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.sed ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.cpp > ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.cpp.1
	COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.cpp.1 ${CMAKE_CURRENT_SOURCE_DIR}/sslscanner.cpp
)
ENDIF(FLEXPP_EXECUTABLE)


SET(boomerang_db_sources
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
