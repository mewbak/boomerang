#fdef M4
# GENSSL = frontend/machine/st20/st20.ssl
#
# frontend/machine/st20/st20.ssl: frontend/machine/st20/st20.ssl.m
#	$(M4) < frontend/machine/st20/st20.ssl.m > frontend/machine/st20/st20.ssl
#	
#endif # ifdef M4

add_custom_command(
	COMMENT "Generating SSL - ST20"
	OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/st20.ssl 
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/st20.ssl.m
	COMMAND ${M4_PROGRAM} < ${CMAKE_CURRENT_SOURCE_DIR}/st20.ssl.m > ${CMAKE_CURRENT_SOURCE_DIR}/st20.ssl
)

SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/st20.ssl PROPERTIES GENERATED TRUE)

add_custom_target(st20 DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/st20.ssl)