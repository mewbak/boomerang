#  transform/transformation-parser.cpp transform/transformation-parser.h: transform/transformation.y transform/transformation.l
#	cd transform && $(BISONPP) -d -v -otransformation-parser.cpp -htransformation-parser.h transformation.y
#
#  transform/transformation-scanner.cpp transform/transformation-scanner.h: transform/transformation.l transform/transformation.y
#	cd transform && $(FLEXPP) -otransformation-scanner.cpp1 -htransformation-scanner.h transformation.l
#	# Fix warnings with same sed file
#	sed -f db/sslscanner.sed transform/transformation-scanner.cpp1 > transform/transformation-scanner.cpp

add_custom_command(
	COMMENT "Building Parser - Bison++"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/transformation-parser.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/transformation-parser.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/transformation.y
	COMMAND ${BISONPP_EXECUTABLE} -d -v -o${CMAKE_CURRENT_SOURCE_DIR}/transformation-parser.cpp -h${CMAKE_CURRENT_SOURCE_DIR}/transformation-parser.h ${CMAKE_CURRENT_SOURCE_DIR}/transformation.y
)

add_custom_command(
	COMMENT "Building Scanner - Flex++"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.cpp  ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.h
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/transformation.l
	COMMAND ${FLEXPP_EXECUTABLE} -o${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.cpp -h${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.h ${CMAKE_CURRENT_SOURCE_DIR}/transformation.l
	COMMAND sed -f ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.sed ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.cpp > ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.cpp.1
	COMMAND mv ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.cpp.1 ${CMAKE_CURRENT_SOURCE_DIR}/transformation-scanner.cpp
)

SET(boomerang_transform_sources
		transformer.cpp
		rdi.cpp
		generic.cpp
		transformation-parser.cpp
		transformation-scanner.cpp
		rdi.h
		generic.h
		transformation-parser.h
		transformation-scanner.h
)

ADD_LIBRARY(boomerang_transform STATIC ${boomerang_transform_sources})
