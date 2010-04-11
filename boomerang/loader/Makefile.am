######################################################
# File: Makefile
#
# Desc: Makefile for the Loader library files.
#   This loader is lightly adapted from the loader in UQBT
#   Purpose is to load the input binary file into memory, with calls to iterate through sections,
#	find entry points, symbols, etc.
#   The main library is BinaryFile, which calls eactly one of ElfBinaryFile etc as needed.
#
# Prerequisite libraries: none
#
######################################################

# $Revision: 1.2 $
# Note: gnu make is probably required (for patsubst etc)
# This whole makefile is dependent on finding the pattern *BinaryFile.cpp; a lib%.so file is made for each of these
# You should be able to add a simple loader without any changes to this Makefile, as long as you follow the naming
# convention <type>BinaryFile.*

# 24 Nov 08 - Markus Gothe - Automake
# 22 Aug 06 - Mike Frysinger: respect values set up by ./configure script
# 12 Jul 05 - Mike: All use SymTab.o now
# 20 Jul 04 - Mike: Removed BOOMDIR (paths relative now)
# 12 Apr 05 - Gerard: Structured
# 11 Apr 02 - Mike: Use lib%.so names and -l now; no "make install" any more

AUTOMAKE_OPTIONS = foreign

####################
# Compiler settings
#

FPIC = -fPIC


####################################
# Detection of OS specific settings
#

#EXEEXT=$(EXEEXT)
#DLLEXT=$(OBJEXT)
if HOST_WINDOWS
 WININCLUDE=-Iwin32make/include
 # RUNPATH is not needed on Windows 
# RUNPATH=
 COPYDLL=$(top_srcdir)/libBinaryFile.$(OBJEXT)
#else
 # Non-Windows
# LDL=-ldl
#if HOST_GNU_LD
# RUNPATH=-Wl,-rpath -Wl,"lib"
#endif
endif

######################
# Rules for compiling
#

LOADERS = \
	ElfBinaryFile.cpp \
	Win32BinaryFile.cpp \
	ExeBinaryFile.cpp \
	HpSomBinaryFile.cpp \
	PalmBinaryFile \
	DOS4GWBinaryFile.cpp \
	IntelCoffFile.cpp \
	$(NULL)

pkglib_LTLIBRARIES = 

if HOST_CYGWIN
 LOADERS+=
else
 # Until we can resolve the issue of compiling loader/MachOBinaryFile.cpp, don't attempt to make it so at least
 # the Cygwin hosted Boomerang can compile and use non-MachO loaders
 LOADERS+=MachOBinaryFile.cpp
endif
LOADERDLLS=$(patsubst %,$(top_srcdir)/lib/lib%$(OBJEXT),$(LOADERS))
BASEDLL=$(top_srcdir)/lib/libBinaryFile$(OBJEXT)
ALLDLLS=$(LOADERDLLS) $(BASEDLL)
ALLLIBS=$(patsubst %,-l%,$(LOADERS)) -lBinaryFile
ALLOBJS = $(patsubst %, %.o, $(LOADERS))
ALLOBJS += BinaryFile.o SymTab.o

all: $(ALLDLLS) $(COPYDLL) BinaryFileFactory.$(OBJEXT) BinaryFileStub.$(OBJEXT)

microX86dis.$(OBJEXT): microX86dis.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $(FPIC) $<

# Extra object files, etc
# These are target specific variables.
$(top_srcdir)/lib/libWin32BinaryFile.$(OBJEXT): EXTRAS = microX86dis.$(OBJEXT)
$(top_srcdir)/lib/libWin32BinaryFile.$(OBJEXT): microX86dis.$(OBJEXT)
$(top_srcdir)/lib/libDOS4GWBinaryFile.$(OBJEXT): EXTRAS = microX86dis.$(OBJEXT)
$(top_srcdir)/lib/libDOS4GWBinaryFile.$(OBJEXT): microX86dis.$(OBJEXT)

$(top_srcdir)/libBinaryFile.$(OBJEXT): $(top_srcdir)/lib/libBinaryFile.$(OBJEXT)
	cp $< $@

BinaryFile.o:		CCEXTRAS = -DBUILDING_LIBBINARYFILE
MachOBinaryFile.o:	CCEXTRAS = -I.

$(BASEDLL) : BinaryFile.$(OBJEXT) SymTab.$(OBJEXT)
	$(CXX) $(CXXFLAGS) -o $@ $(SHARED) BinaryFile.$(OBJEXT) SymTab.$(OBJEXT) $(OUTIMPLIB) $(LDFLAGS)

# This pattern generates all the main dependencies
$(LOADERDLLS): $(top_srcdir)/lib/lib%.$(OBJEXT) : %.$(OBJEXT) $(BASEDLL)
	$(CXX) $(CXXFLAGS) -o $@ $(SHARED) $< SymTab.o $(EXTRAS) -lBinaryFile $(RUNPATH) -L$(top_srcdir)/lib $(LDFLAGS)

# Compile all objects with -fPIC
$(ALLOBJS): %.$(OBJEXT) : %.cpp
	$(CXX) $(CXXFLAGS) -c $(FPIC) $(CCEXTRAS) -I$(top_srcdir)/include -o $@ $<

####################
# Conditional rules
#

# Automatic generation of dependencies
if MAKEDEPEND
# automatic is better!
depend: $(MAKEDEPEND)
	$(MAKEDEPEND) -w120 -Y -I$(top_srcdir)/include -I. -I$(top_srcdir)/c -I$(top_srcdir)/codegen \
	-I$(top_srcdir)/frontend *.cpp *.c > /dev/null 2>&1

depend.in: $(MAKEDEPEND)
	$(MAKEDEPEND) -fMakefile.in -w120 -Y -I$(top_srcdir)/include -I. -I$(top_srcdir)/c -I$(top_srcdir)/codegen \
	-I$(top_srcdir)/frontend *.cpp *.c > /dev/null 2>&1
else
depend:
@echo You need the (X11) makedepend tool to generate dependencies
depend.in:
@echo You need the (X11) makedepend tool to generate dependencies
endif

###############
# Unit testing
#

testLoader$(EXEEXT): testLoader.$(OBJEXT) LoaderTest.$(OBJEXT) microX86dis.$(OBJEXT) BinaryFileFactory.$(OBJEXT) SymTab.$(OBJEXT)
	$(CXX) $(CXXFLAGS) -o $@ $^  -lcppunit $(ALLLIBS) $(RUNPATH) $(LDL) -L$(top_srcdir)/lib $(LDFLAGS)

testLoader.$(OBJEXT): testLoader.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ -I$(top_srcdir)/include $<

LoaderTest.$(OBJEXT): LoaderTest.cpp LoaderTest.h $(top_srcdir)/include/BinaryFile.h SymTab.$(OBJEXT)
	$(CXX) $(CXXFLAGS) -c -o $@ -I$(top_srcdir)/include $<

# Because of the relative paths, testLoader will only work from the parent
# directory...
test: all testLoader$(EXEEXT)
	cd $(top_srcdir) && loader/testLoader$(EXEEXT)

