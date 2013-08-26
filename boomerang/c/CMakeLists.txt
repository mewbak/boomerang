#  c/ansi-c-parser.cpp c/ansi-c-parser.h: c/ansi-c.y c/ansi-c.l
#	$(BISONPP) -d -v -oc/ansi-c-parser.cpp -hc/ansi-c-parser.h c/ansi-c.y

#  c/ansi-c-scanner.cpp c/ansi-c-scanner.h: c/ansi-c.l c/ansi-c.y
#	$(FLEXPP) -oc/ansi-c-scanner.cpp -hc/ansi-c-scanner.h c/ansi-c.l
#	# Fix warnings with same sed file
#	sed -f db/ansi-c-scanner.sed c/ansi-c-scanner.cpp > c/ansi-c-scanner.cpp1
#	mv c/ansi-c-scanner.cpp1 c/ansi-c-scanner.cpp

add_custom_command(
	COMMENT "Building Parser - Bison++"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-parser.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-parser.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c.y
	COMMAND ${BISONPP_EXECUTABLE} -d -v --no-lines -o${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-parser.cpp -h${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-parser.h ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c.y
)

add_custom_command(
	COMMENT "Building Scanner - Flex++"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c.l
	COMMAND ${FLEXPP_EXECUTABLE} -L -o${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.cpp -h${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.h ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c.l
	COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.sed ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.cpp > ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.cpp.1
	COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.cpp.1 ${CMAKE_CURRENT_SOURCE_DIR}/ansi-c-scanner.cpp
)

SET(boomerang_DSLs_sources
	ansi-c-parser.cpp
	ansi-c-scanner.cpp
	ansi-c-parser.h
	ansi-c-scanner.h
)

ADD_LIBRARY(boomerang_DSLs STATIC ${boomerang_DSLs_sources} )
