######################################################
# File: Makefile
# Desc: Makefile for boomerang
#       Makes and tests all object files all directories
#
######################################################

# $Revision: 1.5 $
# 20 May 02 - Trent: Created
# 31 May 02 - Mike: Make lib directory if needed
# 15 Jul 01 - Mike: Added static target

# Front end
MACHS     = sparc pentium
MACH_LIBS = $(patsubst %, lib/libfront%.so, $(MACHS))
MACH_OBJS = $(patsubst %, frontend/%decoder.o, $(MACHS))
MACH_SRC  = $(patsubst %, frontend/%decoder.cpp, $(MACHS))
MACH_FE_SRC=$(patsubst %, frontend/%frontend.cpp, $(MACHS))
MACH_FE_OBJ=$(patsubst %, frontend/%frontend.o, $(MACHS))


C = gcc -g -Wall
CC = g++ -g -Wall
CXX=$(CC)

# HOST_GNU_LD is true if the linker on this host machine is GNU
HOST_GNU_LD = no

# These should be autoconfigured one day:
BISONPP=bison++
FLEXPP=flex++

BOOMDIR="$(shell pwd)"
BOOMSET=-DBOOMDIR=\"$(BOOMDIR)\"
LIBDIR=$(BOOMDIR)/lib
ifeq ($(HOST_GNU_LD), yes)
RUNPATH=-Wl,-rpath -Wl,$(LIBDIR)
else # Assume Solaris
RUNPATH=-R$(LIBDIR)
endif

.PHONY: test clean all static

all: lib test
static: boomerang

# Make the lib directory if needed
lib:
	mkdir lib

testDyn: lib
	cd util     && ${MAKE} test
	cd loader   && ${MAKE} test
	cd db       && ${MAKE} test
	cd frontend && ${MAKE} test

test:	bigtest
	cd loader   && ${MAKE} test
	./bigtest

clean:
	cd util     && ${MAKE} clean
	cd loader   && ${MAKE} clean
	cd db       && ${MAKE} clean
	cd frontend && ${MAKE} clean

distclean: clean
	rm -f config.guess util/Makefile loader/Makefile db/Makefile frontend/Makefile


TEST_OBJS = util/UtilTest.o db/ExpTest.o db/ParserTest.o \
	db/ProcTest.o db/ProgTest.o db/RtlTest.o db/TypeTest.o \
	frontend/FrontSparcTest.o frontend/FrontPentTest.o \
	analysis/AnalysisTest.o

UTIL_OBJS = 	  util/util.o
DB_OBJS = db/basicblock.o db/proc.o db/sslscanner.o db/cfg.o db/prog.o \
  db/table.o db/dataflow.o db/register.o db/sslparser.o db/exp.o db/rtl.o \
  db/type.o db/hrtl.o db/sslinst.o db/insnameelem.o db/signature.o
FRONT_OBJS = frontend/frontend.o frontend/njmcDecoder.o \
	frontend/sparcdecoder.o frontend/pentiumdecoder.o frontend/sparcfrontend.o \
	frontend/pentiumfrontend.o
AN_OBJS = analysis/analysis.o
STATIC_OBJS = ${UTIL_OBJS} ${DB_OBJS} ${FRONT_OBJS} ${AN_OBJS}

lib/libBinaryFile.so:
	cd loader && make

bigtest: testAll.o ${STATIC_OBJS} ${TEST_OBJS}
	cd loader && ${MAKE} test
	$(CC) -o $@ testAll.o $(STATIC_OBJS) ${TEST_OBJS} lib/libBinaryFile.so \
	  $(RUNPATH) -Llib -ldl -lcppunit

boomerang: driver.o ${STATIC_OBJS} lib/libBinaryFile.so
	$(CC) -o $@ driver.o $(STATIC_OBJS) lib/libBinaryFile.so $(RUNPATH) \
	  -Llib -ldl

db/sslparser.cpp db/sslparser.h: db/sslparser.y db/sslscanner.l
	cd db ; ${BISONPP} -d -v -osslparser.cpp -hsslparser.h sslparser.y

db/sslscanner.cpp db/sslscanner.h: db/sslscanner.l db/sslparser.y
	cd db ; ${FLEXPP} -osslscanner.cpp -hsslscanner.h sslscanner.l

# The .cpp's depend on the .m's
$(MACH_SRC): frontend/%decoder.cpp : frontend/machine/%/core.spec \
                                   frontend/machine/%/synth.spec \
                                   frontend/machine/%/dis.spec \
                                   frontend/machine/%/decoder.m
	frontend/mltk.sh -o $@ $^ 2> mltk.err



#
# Extra flags
#
db/prog.o:					EXTRA = $(BOOMSET)
frontend/frontend.o:		EXTRA = $(BOOMSET)
frontend/FrontSparcTest.o:	EXTRA = $(BOOMSET)
frontend/FrontPentTest.o:	EXTRA = $(BOOMSET)
analysis/AnalysisTest.o:	EXTRA = $(BOOMSET) -Ifrontend
db/ProgTest.o:				EXTRA = $(BOOMSET) -Ifrontend
db/ProcTest.o:				EXTRA = $(BOOMSET) -Ifrontend
db/ParserTest.o:			EXTRA = $(BOOMSET)

# Compile ordinary files
$(STATIC_OBJS): %.o : %.cpp
	$(CC) -c -o $@ -Iinclude $(EXTRA) $<

$(TEST_OBJS): %.o : %.cpp
	$(CC) -c -o $@ -Iinclude $(EXTRA) $<

driver.o: driver.cpp
	$(CC) -c -o $@ -Iinclude $(EXTRA) $<

testAll.o: testAll.cpp
	$(CC) -c -o $@ -Iinclude $(EXTRA) $< -Idb -Ifrontend -Ianalysis

#
# Dependencies
#

db/exp.o: include/exp.h include/operator.h
db/prog.o: include/prog.h
db/cfg.o: include/cfg.h
db/basicblock.o: include/cfg.h
db/rtl.o: include/rtl.h include/exp.h include/operator.h
db/hrtl.o: include/rtl.h include/exp.h include/operator.h
db/register.o: include/register.h
db/sslparser.o: db/sslparser.cpp db/sslparser.h db/sslscanner.h
db/sslscanner.o: db/sslscanner.cpp db/sslscanner.h
db/insnameelem.o: db/insnameelem.h
db/signature.o: include/signature.h include/frontend.h include/BinaryFile.h \
	include/exp.h include/prog.h include/type.h

util/UtilTest.o: util/UtilTest.h
util/util.o: include/util.h

frontend/frontend.o: frontend/frontend.cpp include/frontend.h include/decoder.h
frontend/FrontendTest.o: frontend/FrontendTest.h
frontend/FrontSparcTest.o: frontend/FrontSparcTest.h
frontend/FrontPentTest.o:  frontend/FrontPentTest.h
frontend/njmcDecoder.o: frontend/njmcDecoder.cpp include/decoder.h

