PROJECT(Boomerang)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# this will set th


SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_scripts;${CMAKE_MODULE_PATH})

OPTION(BUILD_TESTING "Build the testing tree." OFF)
IF(BUILD_TESTING)
	ENABLE_TESTING()
ENDIF(BUILD_TESTING)

INCLUDE(CheckIncludeFile)
INCLUDE(CheckTypeSize)
INCLUDE(CheckLibraryExists)
INCLUDE (TestBigEndian)

INCLUDE(BOOMERANG_Macros)

# The BOOMERANG_FRONTENDS will be filled ba appropriate ADD_FRONTEND macros
SET(BOOMERANG_FRONTENDS "" CACHE INTERNAL "")
SET(BOOMERANG_LOADERS "" CACHE INTERNAL "")
SET(BOOMERANG_CODE_GENERATORS "" CACHE INTERNAL "")
IF(MSVC)
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF(MSVC)
# this is the list of libraries that will be extended
# by various configured libraries
SET(boomerang_depends_on "")

# 
CHECK_INCLUDE_FILE(byteswap.h HAVE_BYTESWAP_H)
CHECK_INCLUDE_FILE(dlfcn.h HAVE_DLFCN_H)
CHECK_INCLUDE_FILE(fcntl.h HAVE_FCNTL_H)
CHECK_INCLUDE_FILE(gc.h HAVE_GC_H)
CHECK_INCLUDE_FILE(inttypes.h HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(malloc.h HAVE_MALLOC_H)
CHECK_INCLUDE_FILE(memory.h HAVE_MEMORY_H)
CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)
CHECK_INCLUDE_FILE(stdlib.h HAVE_STDLIB_H)
CHECK_INCLUDE_FILE(strings.h HAVE_STRINGS_H)
CHECK_INCLUDE_FILE(string.h HAVE_STRING_H)
CHECK_INCLUDE_FILE(sys/stat.h HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE(sys/time.h HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE(sys/types.h HAVE_SYS_TYPES_H)
CHECK_INCLUDE_FILE(unistd.h HAVE_UNISTD_H)
CHECK_INCLUDE_FILE(/opt/local/include/gc/gc.h HAVE__OPT_LOCAL_INCLUDE_GC_GC_H)
CHECK_INCLUDE_FILE(/opt/local/include/gc.h HAVE__OPT_LOCAL_INCLUDE_GC_H)
CHECK_INCLUDE_FILE(/sw/include/gc.h HAVE__SW_INCLUDE_GC_H)
CHECK_INCLUDE_FILE(/usr/include/gc/gc.h HAVE__USR_INCLUDE_GC_GC_H)
CHECK_INCLUDE_FILE(/usr/include/gc.h HAVE__USR_INCLUDE_GC_H)
CHECK_INCLUDE_FILE(/usr/local/include/gc.h HAVE__USR_LOCAL_INCLUDE_GC_H)


CHECK_TYPE_SIZE(char SIZEOF_CHAR)
CHECK_TYPE_SIZE(double SIZEOF_DOUBLE)
CHECK_TYPE_SIZE(float SIZEOF_FLOAT)
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE("int *" SIZEOF_INT_P)
CHECK_TYPE_SIZE(long SIZEOF_LONG)
CHECK_TYPE_SIZE("long double" SIZEOF_LONG_DOUBLE)
CHECK_TYPE_SIZE("long long" SIZEOF_LONG_LONG)
CHECK_TYPE_SIZE(short SIZEOF_SHORT)

SET(VERSION "0.3.1")
# this creates config.h boomerang config.h in-place
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.cmake 
				${CMAKE_CURRENT_SOURCE_DIR}/include/config.h)
# if this is a problem comment out the above, and uncomment below
#CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake 
#				${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/include)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
IF(USE_GC)
	FIND_PACKAGE(BoehmGC REQUIRED)
	SET(boomerang_libs ${boomerang_libs} ${BOEHMGC_LIBRARIES})
	INCLUDE_DIRECTORIES(${BOEHMGC_INCLUDE_DIRS})
	ADD_DEFINITIONS(${BOEHMGC_DEFINITIONS})
ELSE(USE_GC)
	ADD_DEFINITIONS(-DNO_GARBAGE_COLLECTOR)
ENDIF(USE_GC)

ADD_SUBDIRECTORY(loader)
ADD_SUBDIRECTORY(c)
ADD_SUBDIRECTORY(codegen)
ADD_SUBDIRECTORY(db)
ADD_SUBDIRECTORY(type)
ADD_SUBDIRECTORY(transform)
ADD_SUBDIRECTORY(util)
ADD_SUBDIRECTORY(frontend)


SET(CMAKE_INCLUDE_PATH ${PROJECT_SOURCE_DIR}/include)

ADD_LIBRARY(driver STATIC driver.cpp)

SET(boomerang_SRCS
   boomerang.cpp
   log.cpp
   loader/BinaryFileFactory.cpp
)

ADD_EXECUTABLE(boomerang ${boomerang_SRCS})
TARGET_LINK_LIBRARIES(boomerang 
	driver 
	${BOOMERANG_LOADERS} 
	${BOOMERANG_FRONTENDS} 
	boomerang_db
	boomerang_type_solvers
	boomerang_transform
	boomerang_util
	boomerang_DSLs
	${BOOMERANG_CODE_GENERATORS}
	${boomerang_libs}
	${CMAKE_DL_LIBS}
)

install(TARGETS boomerang DESTINATION bin)
install(FILES  include/*.h DESTINATION include)

# this is put at the end so that first cmake configure will assume USE_GC NO, 
# and will not check for existence of Boehm GC
OPTION(USE_GC "Use Boehm garbage collector, only developers should turn this off" true)

IF(BUILD_TESTING)
	FIND_PACKAGE(CppUnit REQUIRED)
	MESSAGE(WARNING CPPUNIT_INCLUDE_DIR ${CPPUNIT_INCLUDE_DIR})
	INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIR})

	ENABLE_TESTING()
	FILE(GLOB_RECURSE UnitTests_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*Test.cpp" )
	FILE(GLOB_RECURSE UnitTests_INCLUDES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*Test.h" )
	ADD_EXECUTABLE(UnitTester testAll.cpp ${UnitTests_SRCS} ${UnitTests_INCLUDES})
	FOREACH(test ${UnitTests_SRCS})
			GET_FILENAME_COMPONENT(TestName ${test} NAME_WE)
			ADD_TEST(${TestName} UnitTester ${TestName})
	ENDFOREACH(test)
	TARGET_LINK_LIBRARIES(UnitTester ${CPPUNIT_LIBRARIES}
		${BOOMERANG_LOADERS} 
		${BOOMERANG_FRONTENDS} 
		boomerang_db
		boomerang_type_solvers
		boomerang_transform
		boomerang_util
		boomerang_DSLs
		${BOOMERANG_CODE_GENERATORS}
		${boomerang_libs}
		${CMAKE_DL_LIBS}	
	)

ENDIF(BUILD_TESTING)
