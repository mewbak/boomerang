#define YY_SSLParser_h_included

/*  A Bison++ parser, made from sslparser.y  */

 /* with Bison++ version bison++ Version 1.21-7, adapted from GNU bison by coetmeur@icdc.fr
  */


#line 1 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* HEADER SECTION */
#ifndef _MSDOS
#ifdef MSDOS
#define _MSDOS
#endif
#endif
/* turboc */
#ifdef __MSDOS__
#ifndef _MSDOS
#define _MSDOS
#endif
#endif

#ifndef alloca
#if defined( __GNUC__)
#define alloca __builtin_alloca

#elif (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc)  || defined (__sgi)
#include <alloca.h>

#elif defined (_MSDOS)
#include <malloc.h>
#ifndef __TURBOC__
/* MS C runtime lib */
#define alloca _alloca
#endif

#elif defined(_AIX)
#include <malloc.h>
#pragma alloca

#elif defined(__hpux)
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */

#endif /* not _AIX  not MSDOS, or __TURBOC__ or _AIX, not sparc.  */
#endif /* alloca not defined.  */
#ifdef c_plusplus
#ifndef __cplusplus
#define __cplusplus
#endif
#endif
#ifdef __cplusplus
#ifndef YY_USE_CLASS
#define YY_USE_CLASS
#endif
#else
#ifndef __STDC__
#define const
#endif
#endif
#include <stdio.h>
#define YYBISON 1  

/* #line 77 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 89 "sslparser.cpp"
#line 36 "sslparser.y"

#include <assert.h>
#if defined(_MSC_VER) && _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif 

#include <sstream>
#include "types.h"
#include "rtl.h"
#include "table.h"
#include "insnameelem.h"
#include "util.h"           // E.g. str()

#ifdef WIN32
#include <malloc.h>
#endif

class SSLScanner;

#line 61 "sslparser.y"
typedef union {
    Exp*            exp;
    char*           str;
    int             num;
    double          dbl;
    Statement*      regtransfer;
    
    Table*          tab;
    InsNameElem*    insel;
    std::list<std::string>*   parmlist;
    std::list<std::string>*   strlist;
    std::deque<Exp*>*    exprlist;
    std::deque<std::string>*  namelist;
    std::list<Exp*>*     explist;
    RTL*            rtlist;
} yy_SSLParser_stype;
#define YY_SSLParser_STYPE yy_SSLParser_stype
#line 79 "sslparser.y"

#include "sslscanner.h"
OPER strToTerm(char* s);        // Convert string to a Terminal (if possible)
Exp* listExpToExp(std::list<Exp*>* le);  // Convert a STL list of Exp* to opList
Exp* listStrToExp(std::list<std::string>* ls);// Convert a STL list of strings to opList
#define STD_SIZE    32          // Standard size
#define YY_SSLParser_DEBUG  1 
#define YY_SSLParser_PARSE_PARAM  \
    RTLInstDict& Dict
#define YY_SSLParser_CONSTRUCTOR_PARAM  \
    const std::string& sslFile, \
    bool trace
#define YY_SSLParser_CONSTRUCTOR_INIT  : \
   sslFile(sslFile), bFloat(false)
#define YY_SSLParser_CONSTRUCTOR_CODE  \
    std::fstream *fin = new std::fstream(sslFile.c_str(), std::ios::in); \
    theScanner = NULL; \
    if (!*fin) { \
        std::cerr << "can't open `" << sslFile << "' for reading\n"; \
	return; \
    } \
    theScanner = new SSLScanner(*fin, trace); \
    if (trace) yydebug = 1;
#define YY_SSLParser_MEMBERS  \
public: \
        SSLParser(std::istream &in, bool trace); \
        virtual ~SSLParser(); \
OPER    strToOper(const char*s); /* Convert string to an operator */ \
static  Statement* parseExp(const char *str); /* Parse an expression or assignment from a string */ \
/* The code for expanding tables and saving to the dictionary */ \
void    expandTables(InsNameElem* iname, std::list<std::string>* params, RTL* o_rtlist, \
  RTLInstDict& Dict); \
Exp*	makeSuccessor(Exp* e);	/* Get successor (of register expression) */ \
\
    /* \
     * The scanner. \
     */ \
    SSLScanner* theScanner; \
protected: \
\
    /* \
     * The file from which the SSL spec is read. \
     */ \
    std::string sslFile; \
\
    /* \
     * Result for parsing an assignment. \
     */ \
    Statement *the_asgn; \
\
    /* \
     * Maps SSL constants to their values. \
     */ \
    std::map<std::string,int> ConstTable; \
\
    /* \
     * maps index names to instruction name-elements \
     */ \
    std::map<std::string, InsNameElem*> indexrefmap; \
\
    /* \
     * Maps table names to Table's.\
     */ \
    std::map<std::string, Table*> TableDict; \
\
    /* \
     * True when FLOAT keyword seen; false when INTEGER keyword seen \
     * (in @REGISTER section) \
     */ \
    bool bFloat;

#line 77 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
/* %{ and %header{ and %union, during decl */
#define YY_SSLParser_BISON 1
#ifndef YY_SSLParser_COMPATIBILITY
#ifndef YY_USE_CLASS
#define  YY_SSLParser_COMPATIBILITY 1
#else
#define  YY_SSLParser_COMPATIBILITY 0
#endif
#endif

#if YY_SSLParser_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YYLTYPE
#ifndef YY_SSLParser_LTYPE
#define YY_SSLParser_LTYPE YYLTYPE
#endif
#endif
#ifdef YYSTYPE
#ifndef YY_SSLParser_STYPE 
#define YY_SSLParser_STYPE YYSTYPE
#endif
#endif
#ifdef YYDEBUG
#ifndef YY_SSLParser_DEBUG
#define  YY_SSLParser_DEBUG YYDEBUG
#endif
#endif
#ifdef YY_SSLParser_STYPE
#ifndef yystype
#define yystype YY_SSLParser_STYPE
#endif
#endif
/* use goto to be compatible */
#ifndef YY_SSLParser_USE_GOTO
#define YY_SSLParser_USE_GOTO 1
#endif
#endif

/* use no goto to be clean in C++ */
#ifndef YY_SSLParser_USE_GOTO
#define YY_SSLParser_USE_GOTO 0
#endif

#ifndef YY_SSLParser_PURE

/* #line 121 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 247 "sslparser.cpp"

#line 121 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
/*  YY_SSLParser_PURE */
#endif

/* section apres lecture def, avant lecture grammaire S2 */

/* #line 125 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 256 "sslparser.cpp"

#line 125 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
/* prefix */
#ifndef YY_SSLParser_DEBUG

/* #line 127 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 263 "sslparser.cpp"

#line 127 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
/* YY_SSLParser_DEBUG */
#endif


#ifndef YY_SSLParser_LSP_NEEDED

/* #line 132 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 273 "sslparser.cpp"

#line 132 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
 /* YY_SSLParser_LSP_NEEDED*/
#endif



/* DEFAULT LTYPE*/
#ifdef YY_SSLParser_LSP_NEEDED
#ifndef YY_SSLParser_LTYPE
typedef
  struct yyltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  yyltype;

#define YY_SSLParser_LTYPE yyltype
#endif
#endif
/* DEFAULT STYPE*/
      /* We used to use `unsigned long' as YY_SSLParser_STYPE on MSDOS,
	 but it seems better to be consistent.
	 Most programs should declare their own type anyway.  */

#ifndef YY_SSLParser_STYPE
#define YY_SSLParser_STYPE int
#endif
/* DEFAULT MISCELANEOUS */
#ifndef YY_SSLParser_PARSE
#define YY_SSLParser_PARSE yyparse
#endif
#ifndef YY_SSLParser_LEX
#define YY_SSLParser_LEX yylex
#endif
#ifndef YY_SSLParser_LVAL
#define YY_SSLParser_LVAL yylval
#endif
#ifndef YY_SSLParser_LLOC
#define YY_SSLParser_LLOC yylloc
#endif
#ifndef YY_SSLParser_CHAR
#define YY_SSLParser_CHAR yychar
#endif
#ifndef YY_SSLParser_NERRS
#define YY_SSLParser_NERRS yynerrs
#endif
#ifndef YY_SSLParser_DEBUG_FLAG
#define YY_SSLParser_DEBUG_FLAG yydebug
#endif
#ifndef YY_SSLParser_ERROR
#define YY_SSLParser_ERROR yyerror
#endif
#ifndef YY_SSLParser_PARSE_PARAM
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
#define YY_SSLParser_PARSE_PARAM
#ifndef YY_SSLParser_PARSE_PARAM_DEF
#define YY_SSLParser_PARSE_PARAM_DEF
#endif
#endif
#endif
#endif
#ifndef YY_SSLParser_PARSE_PARAM
#define YY_SSLParser_PARSE_PARAM void
#endif
#endif
#if YY_SSLParser_COMPATIBILITY != 0
/* backward compatibility */
#ifdef YY_SSLParser_LTYPE
#ifndef YYLTYPE
#define YYLTYPE YY_SSLParser_LTYPE
#else
/* WARNING obsolete !!! user defined YYLTYPE not reported into generated header */
#endif
#endif
#ifndef YYSTYPE
#define YYSTYPE YY_SSLParser_STYPE
#else
/* WARNING obsolete !!! user defined YYSTYPE not reported into generated header */
#endif
#ifdef YY_SSLParser_PURE
#ifndef YYPURE
#define YYPURE YY_SSLParser_PURE
#endif
#endif
#ifdef YY_SSLParser_DEBUG
#ifndef YYDEBUG
#define YYDEBUG YY_SSLParser_DEBUG 
#endif
#endif
#ifndef YY_SSLParser_ERROR_VERBOSE
#ifdef YYERROR_VERBOSE
#define YY_SSLParser_ERROR_VERBOSE YYERROR_VERBOSE
#endif
#endif
#ifndef YY_SSLParser_LSP_NEEDED
#ifdef YYLSP_NEEDED
#define YY_SSLParser_LSP_NEEDED YYLSP_NEEDED
#endif
#endif
#endif
#ifndef YY_USE_CLASS
/* TOKEN C */

/* #line 240 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 386 "sslparser.cpp"
#define	COND_OP	258
#define	BIT_OP	259
#define	ARITH_OP	260
#define	LOG_OP	261
#define	NAME	262
#define	REG_ID	263
#define	REG_NUM	264
#define	COND_TNAME	265
#define	DECOR	266
#define	FARITH_OP	267
#define	FPUSH	268
#define	FPOP	269
#define	TEMP	270
#define	SHARES	271
#define	CONV_FUNC	272
#define	TRUNC_FUNC	273
#define	TRANSCEND	274
#define	FABS_FUNC	275
#define	BIG	276
#define	LITTLE	277
#define	NAME_CALL	278
#define	NAME_LOOKUP	279
#define	ENDIANNESS	280
#define	COVERS	281
#define	INDEX	282
#define	NOT	283
#define	THEN	284
#define	LOOKUP_RDC	285
#define	BOGUS	286
#define	ASSIGN	287
#define	TO	288
#define	COLON	289
#define	S_E	290
#define	AT	291
#define	ADDR	292
#define	REG_IDX	293
#define	EQUATE	294
#define	MEM_IDX	295
#define	TOK_INTEGER	296
#define	TOK_FLOAT	297
#define	FAST	298
#define	OPERAND	299
#define	FETCHEXEC	300
#define	CAST_OP	301
#define	FLAGMACRO	302
#define	SUCCESSOR	303
#define	NUM	304
#define	ASSIGNSIZE	305
#define	FLOATNUM	306


#line 240 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
 /* #defines tokens */
#else
/* CLASS */
#ifndef YY_SSLParser_CLASS
#define YY_SSLParser_CLASS SSLParser
#endif
#ifndef YY_SSLParser_INHERIT
#define YY_SSLParser_INHERIT
#endif
#ifndef YY_SSLParser_MEMBERS
#define YY_SSLParser_MEMBERS 
#endif
#ifndef YY_SSLParser_LEX_BODY
#define YY_SSLParser_LEX_BODY  
#endif
#ifndef YY_SSLParser_ERROR_BODY
#define YY_SSLParser_ERROR_BODY  
#endif
#ifndef YY_SSLParser_CONSTRUCTOR_PARAM
#define YY_SSLParser_CONSTRUCTOR_PARAM
#endif
#ifndef YY_SSLParser_CONSTRUCTOR_CODE
#define YY_SSLParser_CONSTRUCTOR_CODE
#endif
#ifndef YY_SSLParser_CONSTRUCTOR_INIT
#define YY_SSLParser_CONSTRUCTOR_INIT
#endif
/* choose between enum and const */
#ifndef YY_SSLParser_USE_CONST_TOKEN
#define YY_SSLParser_USE_CONST_TOKEN 0
/* yes enum is more compatible with flex,  */
/* so by default we use it */
#endif
#if YY_SSLParser_USE_CONST_TOKEN != 0
#ifndef YY_SSLParser_ENUM_TOKEN
#define YY_SSLParser_ENUM_TOKEN yy_SSLParser_enum_token
#endif
#endif

class YY_SSLParser_CLASS YY_SSLParser_INHERIT
{
public:
#if YY_SSLParser_USE_CONST_TOKEN != 0
/* static const int token ... */

/* #line 284 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 485 "sslparser.cpp"
static const int COND_OP;
static const int BIT_OP;
static const int ARITH_OP;
static const int LOG_OP;
static const int NAME;
static const int REG_ID;
static const int REG_NUM;
static const int COND_TNAME;
static const int DECOR;
static const int FARITH_OP;
static const int FPUSH;
static const int FPOP;
static const int TEMP;
static const int SHARES;
static const int CONV_FUNC;
static const int TRUNC_FUNC;
static const int TRANSCEND;
static const int FABS_FUNC;
static const int BIG;
static const int LITTLE;
static const int NAME_CALL;
static const int NAME_LOOKUP;
static const int ENDIANNESS;
static const int COVERS;
static const int INDEX;
static const int NOT;
static const int THEN;
static const int LOOKUP_RDC;
static const int BOGUS;
static const int ASSIGN;
static const int TO;
static const int COLON;
static const int S_E;
static const int AT;
static const int ADDR;
static const int REG_IDX;
static const int EQUATE;
static const int MEM_IDX;
static const int TOK_INTEGER;
static const int TOK_FLOAT;
static const int FAST;
static const int OPERAND;
static const int FETCHEXEC;
static const int CAST_OP;
static const int FLAGMACRO;
static const int SUCCESSOR;
static const int NUM;
static const int ASSIGNSIZE;
static const int FLOATNUM;


#line 284 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
 /* decl const */
#else
enum YY_SSLParser_ENUM_TOKEN { YY_SSLParser_NULL_TOKEN=0

/* #line 287 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 543 "sslparser.cpp"
	,COND_OP=258
	,BIT_OP=259
	,ARITH_OP=260
	,LOG_OP=261
	,NAME=262
	,REG_ID=263
	,REG_NUM=264
	,COND_TNAME=265
	,DECOR=266
	,FARITH_OP=267
	,FPUSH=268
	,FPOP=269
	,TEMP=270
	,SHARES=271
	,CONV_FUNC=272
	,TRUNC_FUNC=273
	,TRANSCEND=274
	,FABS_FUNC=275
	,BIG=276
	,LITTLE=277
	,NAME_CALL=278
	,NAME_LOOKUP=279
	,ENDIANNESS=280
	,COVERS=281
	,INDEX=282
	,NOT=283
	,THEN=284
	,LOOKUP_RDC=285
	,BOGUS=286
	,ASSIGN=287
	,TO=288
	,COLON=289
	,S_E=290
	,AT=291
	,ADDR=292
	,REG_IDX=293
	,EQUATE=294
	,MEM_IDX=295
	,TOK_INTEGER=296
	,TOK_FLOAT=297
	,FAST=298
	,OPERAND=299
	,FETCHEXEC=300
	,CAST_OP=301
	,FLAGMACRO=302
	,SUCCESSOR=303
	,NUM=304
	,ASSIGNSIZE=305
	,FLOATNUM=306


#line 287 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
 /* enum token */
     }; /* end of enum declaration */
#endif
public:
 int YY_SSLParser_PARSE (YY_SSLParser_PARSE_PARAM);
 virtual void YY_SSLParser_ERROR(char *msg) YY_SSLParser_ERROR_BODY;
#ifdef YY_SSLParser_PURE
#ifdef YY_SSLParser_LSP_NEEDED
 virtual int  YY_SSLParser_LEX (YY_SSLParser_STYPE *YY_SSLParser_LVAL,YY_SSLParser_LTYPE *YY_SSLParser_LLOC) YY_SSLParser_LEX_BODY;
#else
 virtual int  YY_SSLParser_LEX (YY_SSLParser_STYPE *YY_SSLParser_LVAL) YY_SSLParser_LEX_BODY;
#endif
#else
 virtual int YY_SSLParser_LEX() YY_SSLParser_LEX_BODY;
 YY_SSLParser_STYPE YY_SSLParser_LVAL;
#ifdef YY_SSLParser_LSP_NEEDED
 YY_SSLParser_LTYPE YY_SSLParser_LLOC;
#endif
 int   YY_SSLParser_NERRS;
 int    YY_SSLParser_CHAR;
#endif
#if YY_SSLParser_DEBUG != 0
 int YY_SSLParser_DEBUG_FLAG;   /*  nonzero means print parse trace     */
#endif
public:
 YY_SSLParser_CLASS(YY_SSLParser_CONSTRUCTOR_PARAM);
public:
 YY_SSLParser_MEMBERS 
};
/* other declare folow */
#if YY_SSLParser_USE_CONST_TOKEN != 0

/* #line 318 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 629 "sslparser.cpp"
const int YY_SSLParser_CLASS::COND_OP=258;
const int YY_SSLParser_CLASS::BIT_OP=259;
const int YY_SSLParser_CLASS::ARITH_OP=260;
const int YY_SSLParser_CLASS::LOG_OP=261;
const int YY_SSLParser_CLASS::NAME=262;
const int YY_SSLParser_CLASS::REG_ID=263;
const int YY_SSLParser_CLASS::REG_NUM=264;
const int YY_SSLParser_CLASS::COND_TNAME=265;
const int YY_SSLParser_CLASS::DECOR=266;
const int YY_SSLParser_CLASS::FARITH_OP=267;
const int YY_SSLParser_CLASS::FPUSH=268;
const int YY_SSLParser_CLASS::FPOP=269;
const int YY_SSLParser_CLASS::TEMP=270;
const int YY_SSLParser_CLASS::SHARES=271;
const int YY_SSLParser_CLASS::CONV_FUNC=272;
const int YY_SSLParser_CLASS::TRUNC_FUNC=273;
const int YY_SSLParser_CLASS::TRANSCEND=274;
const int YY_SSLParser_CLASS::FABS_FUNC=275;
const int YY_SSLParser_CLASS::BIG=276;
const int YY_SSLParser_CLASS::LITTLE=277;
const int YY_SSLParser_CLASS::NAME_CALL=278;
const int YY_SSLParser_CLASS::NAME_LOOKUP=279;
const int YY_SSLParser_CLASS::ENDIANNESS=280;
const int YY_SSLParser_CLASS::COVERS=281;
const int YY_SSLParser_CLASS::INDEX=282;
const int YY_SSLParser_CLASS::NOT=283;
const int YY_SSLParser_CLASS::THEN=284;
const int YY_SSLParser_CLASS::LOOKUP_RDC=285;
const int YY_SSLParser_CLASS::BOGUS=286;
const int YY_SSLParser_CLASS::ASSIGN=287;
const int YY_SSLParser_CLASS::TO=288;
const int YY_SSLParser_CLASS::COLON=289;
const int YY_SSLParser_CLASS::S_E=290;
const int YY_SSLParser_CLASS::AT=291;
const int YY_SSLParser_CLASS::ADDR=292;
const int YY_SSLParser_CLASS::REG_IDX=293;
const int YY_SSLParser_CLASS::EQUATE=294;
const int YY_SSLParser_CLASS::MEM_IDX=295;
const int YY_SSLParser_CLASS::TOK_INTEGER=296;
const int YY_SSLParser_CLASS::TOK_FLOAT=297;
const int YY_SSLParser_CLASS::FAST=298;
const int YY_SSLParser_CLASS::OPERAND=299;
const int YY_SSLParser_CLASS::FETCHEXEC=300;
const int YY_SSLParser_CLASS::CAST_OP=301;
const int YY_SSLParser_CLASS::FLAGMACRO=302;
const int YY_SSLParser_CLASS::SUCCESSOR=303;
const int YY_SSLParser_CLASS::NUM=304;
const int YY_SSLParser_CLASS::ASSIGNSIZE=305;
const int YY_SSLParser_CLASS::FLOATNUM=306;


#line 318 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
 /* const YY_SSLParser_CLASS::token */
#endif
/*apres const  */
YY_SSLParser_CLASS::YY_SSLParser_CLASS(YY_SSLParser_CONSTRUCTOR_PARAM) YY_SSLParser_CONSTRUCTOR_INIT
{
#if YY_SSLParser_DEBUG != 0
YY_SSLParser_DEBUG_FLAG=0;
#endif
YY_SSLParser_CONSTRUCTOR_CODE;
};
#endif

/* #line 329 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 695 "sslparser.cpp"


#define	YYFINAL		296
#define	YYFLAG		-32768
#define	YYNTBASE	65

#define YYTRANSLATE(x) ((unsigned)(x) <= 306 ? yytranslate[x] : 111)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    59,     2,    61,     2,     2,    60,    63,
    58,     2,     2,    53,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,    52,     2,
     2,     2,    64,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    56,     2,    57,     2,    62,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    54,     2,    55,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
    26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
    46,    47,    48,    49,    50,    51
};

#if YY_SSLParser_DEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,    10,    13,    15,    18,    20,    22,
    24,    26,    28,    30,    33,    37,    39,    45,    51,    55,
    56,    57,    61,    62,    66,    70,    72,    76,    83,    94,
   109,   120,   129,   133,   135,   142,   146,   152,   156,   158,
   160,   162,   165,   167,   171,   176,   178,   182,   184,   188,
   192,   195,   197,   199,   201,   203,   207,   213,   217,   221,
   227,   231,   232,   237,   239,   242,   244,   246,   249,   253,
   257,   261,   266,   271,   275,   278,   280,   282,   286,   290,
   293,   295,   299,   301,   305,   307,   308,   310,   314,   316,
   317,   324,   329,   331,   333,   336,   338,   340,   342,   346,
   348,   356,   365,   369,   377,   381,   385,   387,   389,   393,
   397,   401,   405,   408,   411,   414,   418,   422,   426,   430,
   434,   440,   442,   444,   448,   450,   454,   456,   464,   467,
   471,   475,   478,   480,   482,   485,   489,   491
};

static const short yyrhs[] = {   101,
     0,   103,     0,    66,     0,    66,    67,    52,     0,    67,
    52,     0,    90,     0,    45,    95,     0,    78,     0,    79,
     0,   106,     0,   108,     0,    71,     0,    77,     0,    44,
    68,     0,    68,    53,    69,     0,    69,     0,    99,    39,
    54,    98,    55,     0,    99,    98,    70,    50,   103,     0,
    56,    98,    57,     0,     0,     0,    41,    72,    74,     0,
     0,    42,    73,    74,     0,    74,    53,    75,     0,    75,
     0,     8,    27,    49,     0,     8,    56,    49,    57,    27,
    49,     0,     8,    56,    49,    57,    27,    49,    26,     8,
    33,     8,     0,     8,    56,    49,    57,    27,    49,    16,
     8,    36,    56,    49,    33,    49,    57,     0,    56,    76,
    57,    56,    49,    57,    27,    49,    33,    49,     0,    56,
    76,    57,    56,    49,    57,    27,    49,     0,    76,    53,
     8,     0,     8,     0,    23,    98,    58,    54,    95,    55,
     0,     7,    39,    49,     0,     7,    39,    49,     5,    49,
     0,     7,    39,    80,     0,    81,     0,    86,     0,    88,
     0,    81,    83,     0,    83,     0,    82,    53,    81,     0,
    82,    53,    59,    59,     0,    81,     0,    54,    82,    55,
     0,    84,     0,    60,     7,    60,     0,    59,     7,    59,
     0,    61,     7,     0,     7,     0,     4,     0,     5,     0,
    12,     0,    54,    87,    55,     0,    87,    53,    59,    85,
    59,     0,    59,    85,    59,     0,    54,    89,    55,     0,
    89,    53,    59,   103,    59,     0,    59,   103,    59,     0,
     0,    92,    91,    98,    95,     0,    93,     0,    92,    11,
     0,     7,     0,    94,     0,    93,    94,     0,    60,     7,
    60,     0,    24,    49,    57,     0,    24,     7,    57,     0,
    61,    24,    49,    57,     0,    61,    24,     7,    57,     0,
    59,     7,    59,     0,    95,    96,     0,    96,     0,   101,
     0,    23,   100,    58,     0,    47,    97,    58,     0,    47,
    58,     0,    62,     0,    97,    53,     8,     0,     8,     0,
    98,    53,    99,     0,    99,     0,     0,     7,     0,   100,
    53,   103,     0,   103,     0,     0,    50,   103,    29,   104,
    39,   103,     0,    50,   104,    39,   103,     0,    13,     0,
    14,     0,    50,   103,     0,    49,     0,    51,     0,    15,
     0,    63,   103,    58,     0,   104,     0,    56,   103,    64,
   103,    34,   103,    57,     0,    56,   103,    64,   103,    34,
   103,    57,   105,     0,    37,   103,    58,     0,    17,    49,
    53,    49,    53,   103,    58,     0,    18,   103,    58,     0,
    20,   103,    58,     0,    13,     0,    14,     0,    19,   103,
    58,     0,    24,     7,    57,     0,    23,   100,    58,     0,
    48,   103,    58,     0,   103,    35,     0,   103,   105,     0,
    28,   103,     0,   103,    12,   103,     0,   103,     5,   103,
     0,   103,     4,   103,     0,   103,     3,   103,     0,   103,
     6,   103,     0,   103,    24,     7,    57,   102,     0,   102,
     0,     8,     0,    38,   103,    57,     0,     9,     0,    40,
   103,    57,     0,     7,     0,   103,    36,    56,   103,    34,
   103,    57,     0,   104,    60,     0,    48,   103,    58,     0,
    54,    49,    55,     0,    25,   107,     0,    21,     0,    22,
     0,    43,   109,     0,   109,    53,   110,     0,   110,     0,
     7,    27,     7,     0
};

#endif

#if YY_SSLParser_DEBUG != 0
static const short yyrline[] = { 0,
   214,   218,   223,   226,   228,   231,   234,   239,   241,   244,
   249,   252,   256,   259,   263,   265,   268,   286,   303,   304,
   307,   311,   311,   314,   316,   317,   320,   326,   331,   366,
   388,   403,   414,   418,   425,   433,   440,   453,   459,   465,
   469,   475,   486,   491,   500,   503,   508,   512,   517,   523,
   526,   541,   559,   564,   568,   574,   580,   586,   593,   599,
   605,   611,   616,   623,   627,   650,   654,   657,   663,   667,
   680,   689,   700,   709,   714,   724,   731,   738,   755,   759,
   762,   767,   775,   785,   792,   796,   801,   806,   811,   816,
   821,   831,   837,   842,   848,   853,   858,   862,   866,   870,
   874,   878,   888,   894,   899,   904,   909,   912,   917,   923,
   951,   978,   983,   993,  1001,  1005,  1009,  1013,  1017,  1021,
  1028,  1054,  1059,  1090,  1094,  1100,  1104,  1122,  1126,  1129,
  1134,  1140,  1145,  1149,  1156,  1160,  1163,  1167
};

static const char * const yytname[] = {   "$","error","$illegal.","COND_OP",
"BIT_OP","ARITH_OP","LOG_OP","NAME","REG_ID","REG_NUM","COND_TNAME","DECOR",
"FARITH_OP","FPUSH","FPOP","TEMP","SHARES","CONV_FUNC","TRUNC_FUNC","TRANSCEND",
"FABS_FUNC","BIG","LITTLE","NAME_CALL","NAME_LOOKUP","ENDIANNESS","COVERS","INDEX",
"NOT","THEN","LOOKUP_RDC","BOGUS","ASSIGN","TO","COLON","S_E","AT","ADDR","REG_IDX",
"EQUATE","MEM_IDX","TOK_INTEGER","TOK_FLOAT","FAST","OPERAND","FETCHEXEC","CAST_OP",
"FLAGMACRO","SUCCESSOR","NUM","ASSIGNSIZE","FLOATNUM","';'","','","'{'","'}'",
"'['","']'","')'","'\"'","'\\''","'$'","'_'","'('","'?'","specorasgn","specification",
"parts","operandlist","operand","func_parameter","reglist","@1","@2","a_reglists",
"a_reglist","reg_table","flag_fnc","constants","table_assign","table_expr","str_expr",
"str_array","str_term","name_expand","bin_oper","opstr_expr","opstr_array","exprstr_expr",
"exprstr_array","instr","@3","instr_name","instr_elem","name_contract","rt_list",
"rt","flag_list","list_parameter","param","list_actualparameter","assign_rt",
"exp_term","exp","var_op","cast","endianness","esize","fastlist","fastentries",
"fastentry",""
};
#endif

static const short yyr1[] = {     0,
    65,    65,    65,    66,    66,    67,    67,    67,    67,    67,
    67,    67,    67,    67,    68,    68,    69,    69,    70,    70,
    72,    71,    73,    71,    74,    74,    75,    75,    75,    75,
    75,    75,    76,    76,    77,    78,    78,    79,    80,    80,
    80,    81,    81,    82,    82,    82,    83,    83,    84,    84,
    84,    84,    85,    85,    85,    86,    87,    87,    88,    89,
    89,    91,    90,    92,    92,    93,    93,    93,    94,    94,
    94,    94,    94,    94,    95,    95,    96,    96,    96,    96,
    96,    97,    97,    98,    98,    98,    99,   100,   100,   100,
   101,   101,   101,   101,   101,   102,   102,   102,   102,   102,
   102,   102,   102,   102,   102,   102,   102,   102,   102,   102,
   102,   102,   103,   103,   103,   103,   103,   103,   103,   103,
   103,   103,   104,   104,   104,   104,   104,   104,   104,   104,
   105,   106,   107,   107,   108,   109,   109,   110
};

static const short yyr2[] = {     0,
     1,     1,     1,     3,     2,     1,     2,     1,     1,     1,
     1,     1,     1,     2,     3,     1,     5,     5,     3,     0,
     0,     3,     0,     3,     3,     1,     3,     6,    10,    14,
    10,     8,     3,     1,     6,     3,     5,     3,     1,     1,
     1,     2,     1,     3,     4,     1,     3,     1,     3,     3,
     2,     1,     1,     1,     1,     3,     5,     3,     3,     5,
     3,     0,     4,     1,     2,     1,     1,     2,     3,     3,
     3,     4,     4,     3,     2,     1,     1,     3,     3,     2,
     1,     3,     1,     3,     1,     0,     1,     3,     1,     0,
     6,     4,     1,     1,     2,     1,     1,     1,     3,     1,
     7,     8,     3,     7,     3,     3,     1,     1,     3,     3,
     3,     3,     2,     2,     2,     3,     3,     3,     3,     3,
     5,     1,     1,     3,     1,     3,     1,     7,     2,     3,
     3,     2,     1,     1,     2,     3,     1,     3
};

static const short yydefact[] = {     0,
    66,   123,   125,   107,   108,    98,     0,     0,     0,     0,
    86,     0,     0,     0,     0,     0,     0,    21,    23,     0,
     0,     0,     0,    96,     0,    97,     0,     0,     0,     0,
     0,     3,     0,    12,    13,     8,     9,     6,    62,    64,
    67,     1,   122,     2,   100,    10,    11,     0,     0,   127,
   107,   108,    90,     0,     0,     0,     0,   127,     0,    85,
     0,    89,     0,     0,   133,   134,   132,   115,     0,     0,
     0,     0,     0,     0,   135,   137,    87,    14,    16,    86,
    93,    94,    90,     0,    81,     7,    76,    77,     0,    95,
   100,     0,     0,     0,     0,     0,    66,    86,     0,     0,
     5,    65,    86,    68,     0,     0,     0,     0,     0,     0,
   113,     0,     0,   114,   129,    52,    36,     0,     0,     0,
     0,    38,    39,    43,    48,    40,    41,     0,     0,   105,
   109,   106,     0,     0,     0,   111,    71,    70,   103,   124,
   126,     0,     0,    22,    26,    24,     0,     0,     0,     0,
    20,     0,    83,    80,     0,    75,   112,     0,     0,     0,
    74,    69,     0,     0,    99,     0,     4,     0,   119,   118,
   117,   120,   116,     0,     0,     0,     0,     0,     0,    46,
     0,     0,     0,     0,     0,    51,    42,     0,   110,    84,
     0,    88,     0,     0,    34,     0,     0,   138,   136,    15,
    86,    86,     0,    78,     0,    79,     0,   100,    92,     0,
    73,    72,    71,    63,     0,     0,   131,    37,    53,    54,
   127,    55,     0,     0,     0,    47,     0,    56,     0,    59,
    50,    49,     0,     0,    27,     0,     0,     0,    25,     0,
     0,     0,    82,     0,     0,   121,     0,    58,    61,     0,
    44,     0,     0,     0,    35,     0,    33,     0,    17,    19,
    18,    91,     0,     0,    45,     0,     0,   104,     0,     0,
   101,   128,    57,    60,    28,     0,   102,     0,     0,     0,
     0,     0,    32,     0,     0,     0,     0,    29,    31,     0,
     0,     0,    30,     0,     0,     0
};

static const short yydefgoto[] = {   294,
    32,    33,    78,    79,   203,    34,    72,    73,   144,   145,
   196,    35,    36,    37,   122,   180,   181,   124,   125,   223,
   126,   182,   127,   183,    38,   103,    39,    40,    41,    86,
    87,   155,    59,    60,    61,    88,    43,    62,    45,   114,
    46,    67,    47,    75,    76
};

static const short yypact[] = {   241,
   492,-32768,-32768,    12,    69,-32768,   -17,    75,    75,    75,
   298,    26,   131,    75,    75,    75,    75,-32768,-32768,    64,
    74,   330,    75,-32768,    75,-32768,    75,    95,   104,    49,
    75,   430,   112,-32768,-32768,-32768,-32768,-32768,   118,    86,
-32768,-32768,-32768,   627,    82,-32768,-32768,     4,   116,-32768,
-32768,-32768,    75,   169,   216,   327,   352,    67,    90,-32768,
    91,   627,   154,   155,-32768,-32768,-32768,   138,   362,   464,
   502,    18,    18,   190,   165,-32768,-32768,   170,-32768,    65,
-32768,-32768,    75,    -1,-32768,   330,-32768,-32768,   367,   565,
   -26,    16,   198,   175,    36,   423,   223,    74,    48,   177,
-32768,-32768,    74,-32768,    75,    75,    75,    75,    75,   256,
-32768,   211,   219,-32768,-32768,-32768,   267,    80,   269,   273,
   280,-32768,   107,-32768,-32768,-32768,-32768,   239,   236,-32768,
-32768,-32768,    74,   240,    75,-32768,   151,-32768,-32768,-32768,
-32768,    23,   287,   243,-32768,   243,   291,    64,    74,   245,
    66,   124,-32768,-32768,   150,-32768,    61,    75,    75,    75,
-32768,-32768,   246,   251,-32768,   252,-32768,   184,   512,   406,
     6,   512,   138,   253,    75,   259,   270,   107,   176,   107,
   183,   188,   218,   261,   263,-32768,-32768,   271,-32768,-32768,
   330,   627,   276,   278,-32768,   153,    18,-32768,-32768,-32768,
    74,    74,   279,-32768,   320,-32768,   627,    77,   627,   575,
-32768,-32768,-32768,   330,    75,   600,-32768,-32768,-32768,-32768,
   261,-32768,   275,    32,   111,-32768,   281,-32768,   282,-32768,
-32768,-32768,    75,   346,-32768,   285,   329,   289,-32768,   222,
   173,    75,-32768,    75,    75,-32768,    75,-32768,-32768,    17,
   107,   197,    75,   428,-32768,   321,-32768,   301,-32768,-32768,
   627,   627,   529,   539,-32768,   293,    42,-32768,   326,   325,
   324,-32768,-32768,-32768,    33,   356,-32768,   376,   381,   341,
   358,   366,   371,   339,   392,   360,   363,-32768,-32768,   372,
   364,   350,-32768,   414,   415,-32768
};

static const short yypgoto[] = {-32768,
-32768,   385,-32768,   274,-32768,-32768,-32768,-32768,   349,   227,
-32768,-32768,-32768,-32768,-32768,   -47,-32768,  -119,-32768,   167,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   396,  -129,
   -84,-32768,   -74,   -16,   355,   439,   228,     0,   -22,   174,
-32768,-32768,-32768,-32768,   296
};


#define	YYLAST		681


static const short yytable[] = {    44,
   123,   156,    91,   187,    80,   151,   153,    55,    56,    57,
   116,   -93,   159,    68,    69,    70,    71,   109,   105,   106,
   107,   108,    89,   184,    90,   142,    92,   109,   168,   110,
    96,    49,    63,   115,   105,   106,   107,   108,   214,   110,
   111,   112,   163,   109,   105,   106,   107,   108,   278,   193,
   111,   112,   117,   109,   166,   110,   154,   118,   279,   113,
   187,   234,   119,   120,   121,   110,   111,   112,   -94,   113,
    74,    77,    95,   143,    64,   265,   111,   112,   194,   160,
    77,    50,     2,     3,   164,   113,   116,    51,    52,     6,
   249,     7,     8,     9,    10,   113,    64,    53,    54,  -130,
   274,    93,    14,   150,   169,   170,   171,   172,   173,    99,
    94,    15,    16,   116,    17,   244,   190,   116,   133,   -87,
  -130,   202,    23,    24,   -87,    26,   240,   241,   102,   156,
    27,   187,    80,   178,   192,   208,   115,    31,   179,   120,
   121,   115,   133,   135,    28,    29,    30,   134,   136,   156,
  -110,    65,    66,  -110,  -110,  -110,  -110,   207,   209,   210,
   178,   110,  -110,   101,   178,   119,   120,   121,   128,   250,
   120,   121,   111,   112,   216,   129,   135,   251,   224,   219,
   220,   204,   221,     2,     3,  -110,  -110,   222,    51,    52,
     6,   113,     7,     8,     9,    10,    81,    82,    53,    54,
   219,   220,   205,    14,  -110,   237,    83,   206,   222,   238,
   137,   138,    15,    16,   207,    17,   147,   148,   105,   106,
   107,   108,   149,    23,    24,   133,    26,   109,   167,   260,
    84,    27,   254,    25,   162,   225,   133,   226,    31,   110,
   227,   261,   228,   262,   263,    85,   264,     1,     2,     3,
   111,   112,   267,     4,     5,     6,   161,     7,     8,     9,
    10,    48,   174,    11,    12,    13,   175,   176,    14,   113,
   229,   177,   230,   130,   133,   184,   259,    15,    16,   185,
    17,    18,    19,    20,    21,    22,   186,   188,    23,    24,
    25,    26,   189,   191,   195,   197,    27,   198,   201,    28,
    29,    30,   211,    31,    58,     2,     3,   212,   213,   215,
    51,    52,     6,   217,     7,     8,     9,    10,   218,   231,
    53,    54,   232,   233,   235,    14,   236,   243,   242,   105,
   106,   107,   108,   248,    15,    16,   257,    17,   109,   252,
   253,   256,    81,    82,   258,    23,    24,   269,    26,   270,
   110,   273,    83,    27,   105,   106,   107,   108,    81,    82,
    31,   111,   112,   109,   105,   106,   107,   108,    83,   105,
   106,   107,   108,   109,   275,   110,    84,   113,   109,    25,
   113,   276,   280,   281,   131,   110,   111,   112,   282,   283,
   110,    85,    84,   284,   287,    25,   111,   112,   285,   288,
   255,   111,   112,   286,   291,   113,   293,    85,   289,   132,
   107,   290,   292,   295,   296,   113,   100,   109,   266,   139,
   113,   146,   200,   239,   157,   105,   106,   107,   108,   110,
   105,   106,   107,   108,   109,   104,    97,   152,    42,   109,
   111,   112,   246,   199,   277,     0,   110,     0,     0,     0,
     0,   110,    98,    99,    13,     0,     0,   111,   112,   113,
     0,     0,   111,   112,     0,     0,   105,   106,   107,   108,
    18,    19,    20,    21,    22,   109,   113,     0,     0,     0,
   165,   113,     0,     0,     0,   268,     0,   110,    28,    29,
    30,  -127,     0,     0,  -127,  -127,  -127,  -127,   111,   112,
     0,     0,     0,  -127,   105,   106,   107,   108,     0,     0,
     0,     0,     0,   109,   105,   106,   107,   113,     0,     0,
   140,     0,     0,   109,     0,   110,  -127,  -127,     0,     0,
    48,   105,   106,   107,   108,   110,   111,   112,     0,     0,
   109,   105,   106,   107,   108,  -127,   111,   112,     0,     0,
   109,     0,   110,     0,     0,   113,     0,     0,   141,     0,
     0,     0,   110,   111,   112,   113,     0,   105,   106,   107,
   108,     0,     0,   111,   112,     0,   109,   105,   106,   107,
   108,     0,   113,     0,     0,   271,   109,     0,   110,     0,
     0,     0,   113,   158,     0,   272,     0,     0,   110,   111,
   112,     0,   105,   106,   107,   108,     0,     0,   245,   111,
   112,   109,     0,     0,     0,     0,     0,     0,   113,     0,
     0,     0,     0,   110,     0,     0,     0,     0,   113,   105,
   106,   107,   108,   247,   111,   112,     0,     0,   109,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   110,     0,     0,   113,     0,     0,     0,     0,     0,     0,
     0,   111,   112,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   113
};

static const short yycheck[] = {     0,
    48,    86,    25,   123,    21,    80,     8,     8,     9,    10,
     7,     0,    39,    14,    15,    16,    17,    12,     3,     4,
     5,     6,    23,     7,    25,     8,    27,    12,   103,    24,
    31,    49,     7,    60,     3,     4,     5,     6,   168,    24,
    35,    36,     7,    12,     3,     4,     5,     6,    16,    27,
    35,    36,    49,    12,     7,    24,    58,    54,    26,    54,
   180,   191,    59,    60,    61,    24,    35,    36,     0,    54,
     7,     7,    24,    56,    49,    59,    35,    36,    56,    64,
     7,     7,     8,     9,    49,    54,     7,    13,    14,    15,
    59,    17,    18,    19,    20,    54,    49,    23,    24,    39,
    59,     7,    28,    39,   105,   106,   107,   108,   109,    24,
     7,    37,    38,     7,    40,    39,   133,     7,    53,    53,
    60,    56,    48,    49,    58,    51,   201,   202,    11,   214,
    56,   251,   149,    54,   135,   158,    60,    63,    59,    60,
    61,    60,    53,    53,    59,    60,    61,    58,    58,   234,
     0,    21,    22,     3,     4,     5,     6,   158,   159,   160,
    54,    24,    12,    52,    54,    59,    60,    61,    53,    59,
    60,    61,    35,    36,   175,     7,    53,   225,   179,     4,
     5,    58,     7,     8,     9,    35,    36,    12,    13,    14,
    15,    54,    17,    18,    19,    20,    13,    14,    23,    24,
     4,     5,    53,    28,    54,    53,    23,    58,    12,    57,
    57,    57,    37,    38,   215,    40,    27,    53,     3,     4,
     5,     6,    53,    48,    49,    53,    51,    12,    52,    57,
    47,    56,   233,    50,    60,    53,    53,    55,    63,    24,
    53,   242,    55,   244,   245,    62,   247,     7,     8,     9,
    35,    36,   253,    13,    14,    15,    59,    17,    18,    19,
    20,    39,     7,    23,    24,    25,    56,    49,    28,    54,
    53,     5,    55,    58,    53,     7,    55,    37,    38,     7,
    40,    41,    42,    43,    44,    45,     7,    49,    48,    49,
    50,    51,    57,    54,     8,    53,    56,     7,    54,    59,
    60,    61,    57,    63,     7,     8,     9,    57,    57,    57,
    13,    14,    15,    55,    17,    18,    19,    20,    49,    59,
    23,    24,    60,    53,    49,    28,    49,     8,    50,     3,
     4,     5,     6,    59,    37,    38,     8,    40,    12,    59,
    59,    57,    13,    14,    56,    48,    49,    27,    51,    49,
    24,    59,    23,    56,     3,     4,     5,     6,    13,    14,
    63,    35,    36,    12,     3,     4,     5,     6,    23,     3,
     4,     5,     6,    12,    49,    24,    47,    54,    12,    50,
    54,    57,    27,     8,    58,    24,    35,    36,     8,    49,
    24,    62,    47,    36,    56,    50,    35,    36,    33,     8,
    55,    35,    36,    33,    33,    54,    57,    62,    49,    58,
     5,    49,    49,     0,     0,    54,    32,    12,   252,    58,
    54,    73,   149,   197,    58,     3,     4,     5,     6,    24,
     3,     4,     5,     6,    12,    40,     7,    83,     0,    12,
    35,    36,   215,   148,   271,    -1,    24,    -1,    -1,    -1,
    -1,    24,    23,    24,    25,    -1,    -1,    35,    36,    54,
    -1,    -1,    35,    36,    -1,    -1,     3,     4,     5,     6,
    41,    42,    43,    44,    45,    12,    54,    -1,    -1,    -1,
    58,    54,    -1,    -1,    -1,    58,    -1,    24,    59,    60,
    61,     0,    -1,    -1,     3,     4,     5,     6,    35,    36,
    -1,    -1,    -1,    12,     3,     4,     5,     6,    -1,    -1,
    -1,    -1,    -1,    12,     3,     4,     5,    54,    -1,    -1,
    57,    -1,    -1,    12,    -1,    24,    35,    36,    -1,    -1,
    39,     3,     4,     5,     6,    24,    35,    36,    -1,    -1,
    12,     3,     4,     5,     6,    54,    35,    36,    -1,    -1,
    12,    -1,    24,    -1,    -1,    54,    -1,    -1,    57,    -1,
    -1,    -1,    24,    35,    36,    54,    -1,     3,     4,     5,
     6,    -1,    -1,    35,    36,    -1,    12,     3,     4,     5,
     6,    -1,    54,    -1,    -1,    57,    12,    -1,    24,    -1,
    -1,    -1,    54,    29,    -1,    57,    -1,    -1,    24,    35,
    36,    -1,     3,     4,     5,     6,    -1,    -1,    34,    35,
    36,    12,    -1,    -1,    -1,    -1,    -1,    -1,    54,    -1,
    -1,    -1,    -1,    24,    -1,    -1,    -1,    -1,    54,     3,
     4,     5,     6,    34,    35,    36,    -1,    -1,    12,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    24,    -1,    -1,    54,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    35,    36,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    54
};

#line 329 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
 /* fattrs + tables */

/* parser code folow  */


/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: dollar marks section change
   the next  is replaced by the list of actions, each action
   as one case of the switch.  */ 

#if YY_SSLParser_USE_GOTO != 0
/*
 SUPRESSION OF GOTO : on some C++ compiler (sun c++)
  the goto is strictly forbidden if any constructor/destructor
  is used in the whole function (very stupid isn't it ?)
 so goto are to be replaced with a 'while/switch/case construct'
 here are the macro to keep some apparent compatibility
*/
#define YYGOTO(lb) {yy_gotostate=lb;continue;}
#define YYBEGINGOTO  enum yy_labels yy_gotostate=yygotostart; \
                     for(;;) switch(yy_gotostate) { case yygotostart: {
#define YYLABEL(lb) } case lb: {
#define YYENDGOTO } }
#define YYBEGINDECLARELABEL enum yy_labels {yygotostart
#define YYDECLARELABEL(lb) ,lb
#define YYENDDECLARELABEL  };
#else
/* macro to keep goto */
#define YYGOTO(lb) goto lb
#define YYBEGINGOTO
#define YYLABEL(lb) lb:
#define YYENDGOTO
#define YYBEGINDECLARELABEL
#define YYDECLARELABEL(lb)
#define YYENDDECLARELABEL
#endif
/* LABEL DECLARATION */
YYBEGINDECLARELABEL
  YYDECLARELABEL(yynewstate)
  YYDECLARELABEL(yybackup)
/* YYDECLARELABEL(yyresume) */
  YYDECLARELABEL(yydefault)
  YYDECLARELABEL(yyreduce)
  YYDECLARELABEL(yyerrlab)   /* here on detecting error */
  YYDECLARELABEL(yyerrlab1)   /* here on error raised explicitly by an action */
  YYDECLARELABEL(yyerrdefault)  /* current state does not do anything special for the error token. */
  YYDECLARELABEL(yyerrpop)   /* pop the current state because it cannot handle the error token */
  YYDECLARELABEL(yyerrhandle)
YYENDDECLARELABEL

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (YY_SSLParser_CHAR = YYEMPTY)
#define YYEMPTY         -2
#define YYEOF           0
#define YYACCEPT        return(0)
#define YYABORT         return(1)
#define YYERROR         YYGOTO(yyerrlab1)
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL          YYGOTO(yyerrlab)
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do                                                              \
  if (YY_SSLParser_CHAR == YYEMPTY && yylen == 1)                               \
    { YY_SSLParser_CHAR = (token), YY_SSLParser_LVAL = (value);                 \
      yychar1 = YYTRANSLATE (YY_SSLParser_CHAR);                                \
      YYPOPSTACK;                                               \
      YYGOTO(yybackup);                                            \
    }                                                           \
  else                                                          \
    { YY_SSLParser_ERROR ("syntax error: cannot back up"); YYERROR; }   \
while (0)

#define YYTERROR        1
#define YYERRCODE       256

#ifndef YY_SSLParser_PURE
/* UNPURE */
#define YYLEX           YY_SSLParser_LEX()
#ifndef YY_USE_CLASS
/* If nonreentrant, and not class , generate the variables here */
int     YY_SSLParser_CHAR;                      /*  the lookahead symbol        */
YY_SSLParser_STYPE      YY_SSLParser_LVAL;              /*  the semantic value of the */
				/*  lookahead symbol    */
int YY_SSLParser_NERRS;                 /*  number of parse errors so far */
#ifdef YY_SSLParser_LSP_NEEDED
YY_SSLParser_LTYPE YY_SSLParser_LLOC;   /*  location data for the lookahead     */
			/*  symbol                              */
#endif
#endif


#else
/* PURE */
#ifdef YY_SSLParser_LSP_NEEDED
#define YYLEX           YY_SSLParser_LEX(&YY_SSLParser_LVAL, &YY_SSLParser_LLOC)
#else
#define YYLEX           YY_SSLParser_LEX(&YY_SSLParser_LVAL)
#endif
#endif
#ifndef YY_USE_CLASS
#if YY_SSLParser_DEBUG != 0
int YY_SSLParser_DEBUG_FLAG;                    /*  nonzero means print parse trace     */
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif
#endif



/*  YYINITDEPTH indicates the initial size of the parser's stacks       */

#ifndef YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif


#if __GNUC__ > 1                /* GNU C and GNU C++ define this.  */
#define __yy_bcopy(FROM,TO,COUNT)       __builtin_memcpy(TO,FROM,COUNT)
#else                           /* not GNU C or C++ */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */

#ifdef __cplusplus
static void __yy_bcopy (char *from, char *to, int count)
#else
#ifdef __STDC__
static void __yy_bcopy (char *from, char *to, int count)
#else
static void __yy_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
#endif
#endif
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}
#endif

int
#ifdef YY_USE_CLASS
 YY_SSLParser_CLASS::
#endif
     YY_SSLParser_PARSE(YY_SSLParser_PARSE_PARAM)
#ifndef __STDC__
#ifndef __cplusplus
#ifndef YY_USE_CLASS
/* parameter definition without protypes */
YY_SSLParser_PARSE_PARAM_DEF
#endif
#endif
#endif
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YY_SSLParser_STYPE *yyvsp;
  int yyerrstatus;      /*  number of tokens to shift before error messages enabled */
  int yychar1=0;          /*  lookahead token as an internal (translated) token number */

  short yyssa[YYINITDEPTH];     /*  the state stack                     */
  YY_SSLParser_STYPE yyvsa[YYINITDEPTH];        /*  the semantic value stack            */

  short *yyss = yyssa;          /*  refer to the stacks thru separate pointers */
  YY_SSLParser_STYPE *yyvs = yyvsa;     /*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YY_SSLParser_LSP_NEEDED
  YY_SSLParser_LTYPE yylsa[YYINITDEPTH];        /*  the location stack                  */
  YY_SSLParser_LTYPE *yyls = yylsa;
  YY_SSLParser_LTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;

#ifdef YY_SSLParser_PURE
  int YY_SSLParser_CHAR;
  YY_SSLParser_STYPE YY_SSLParser_LVAL;
  int YY_SSLParser_NERRS;
#ifdef YY_SSLParser_LSP_NEEDED
  YY_SSLParser_LTYPE YY_SSLParser_LLOC;
#endif
#endif

  YY_SSLParser_STYPE yyval;             /*  the variable used to return         */
				/*  semantic values from the action     */
				/*  routines                            */

  int yylen;
/* start loop, in which YYGOTO may be used. */
YYBEGINGOTO

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    fprintf(stderr, "Starting parse\n");
#endif
  yystate = 0;
  yyerrstatus = 0;
  YY_SSLParser_NERRS = 0;
  YY_SSLParser_CHAR = YYEMPTY;          /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YY_SSLParser_LSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
YYLABEL(yynewstate)

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YY_SSLParser_STYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YY_SSLParser_LSP_NEEDED
      YY_SSLParser_LTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YY_SSLParser_LSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YY_SSLParser_LSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  YY_SSLParser_ERROR("parser stack overflow");
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_bcopy ((char *)yyss1, (char *)yyss, size * sizeof (*yyssp));
      yyvs = (YY_SSLParser_STYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_bcopy ((char *)yyvs1, (char *)yyvs, size * sizeof (*yyvsp));
#ifdef YY_SSLParser_LSP_NEEDED
      yyls = (YY_SSLParser_LTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_bcopy ((char *)yyls1, (char *)yyls, size * sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YY_SSLParser_LSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YY_SSLParser_DEBUG != 0
      if (YY_SSLParser_DEBUG_FLAG)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  YYGOTO(yybackup);
YYLABEL(yybackup)

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* YYLABEL(yyresume) */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    YYGOTO(yydefault);

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (YY_SSLParser_CHAR == YYEMPTY)
    {
#if YY_SSLParser_DEBUG != 0
      if (YY_SSLParser_DEBUG_FLAG)
	fprintf(stderr, "Reading a token: ");
#endif
      YY_SSLParser_CHAR = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (YY_SSLParser_CHAR <= 0)           /* This means end of input. */
    {
      yychar1 = 0;
      YY_SSLParser_CHAR = YYEOF;                /* Don't call YYLEX any more */

#if YY_SSLParser_DEBUG != 0
      if (YY_SSLParser_DEBUG_FLAG)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(YY_SSLParser_CHAR);

#if YY_SSLParser_DEBUG != 0
      if (YY_SSLParser_DEBUG_FLAG)
	{
	  fprintf (stderr, "Next token is %d (%s", YY_SSLParser_CHAR, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, YY_SSLParser_CHAR, YY_SSLParser_LVAL);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    YYGOTO(yydefault);

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	YYGOTO(yyerrlab);
      yyn = -yyn;
      YYGOTO(yyreduce);
    }
  else if (yyn == 0)
    YYGOTO(yyerrlab);

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    fprintf(stderr, "Shifting token %d (%s), ", YY_SSLParser_CHAR, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (YY_SSLParser_CHAR != YYEOF)
    YY_SSLParser_CHAR = YYEMPTY;

  *++yyvsp = YY_SSLParser_LVAL;
#ifdef YY_SSLParser_LSP_NEEDED
  *++yylsp = YY_SSLParser_LLOC;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  YYGOTO(yynewstate);

/* Do the default action for the current state.  */
YYLABEL(yydefault)

  yyn = yydefact[yystate];
  if (yyn == 0)
    YYGOTO(yyerrlab);

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
YYLABEL(yyreduce)
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


/* #line 783 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 1568 "sslparser.cpp"

  switch (yyn) {

case 1:
#line 215 "sslparser.y"
{
            the_asgn = yyvsp[0].regtransfer;
        ;
    break;}
case 2:
#line 218 "sslparser.y"
{
            the_asgn = new Assign(
                new Terminal(opNil),
                yyvsp[0].exp);
        ;
    break;}
case 7:
#line 234 "sslparser.y"
{
            Dict.fetchExecCycle = yyvsp[0].rtlist;
        ;
    break;}
case 14:
#line 259 "sslparser.y"
{ Dict.fixupParams(); ;
    break;}
case 17:
#line 272 "sslparser.y"
{
            // Note: the below copies the list of strings!
            Dict.DetParamMap[yyvsp[-4].str].params = *yyvsp[-1].parmlist;
            Dict.DetParamMap[yyvsp[-4].str].kind = PARAM_VARIANT;
            //delete $4;
        ;
    break;}
case 18:
#line 286 "sslparser.y"
{
            std::map<std::string, InsNameElem*> m;
            ParamEntry &param = Dict.DetParamMap[yyvsp[-4].str];
            Statement* asgn = new Assign(yyvsp[-1].num, new Terminal(opNil), yyvsp[0].exp);
            // Note: The below 2 copy lists of strings (to be deleted below!)
            param.params = *yyvsp[-3].parmlist;
            param.funcParams = *yyvsp[-2].parmlist;
            param.asgn = asgn;
            param.kind = PARAM_ASGN;
            
            if( param.funcParams.size() != 0 )
                param.kind = PARAM_LAMBDA;
            //delete $2;
            //delete $3;
        ;
    break;}
case 19:
#line 303 "sslparser.y"
{ yyval.parmlist = yyvsp[-1].parmlist; ;
    break;}
case 20:
#line 304 "sslparser.y"
{ yyval.parmlist = new std::list<std::string>(); ;
    break;}
case 21:
#line 308 "sslparser.y"
{
                    bFloat = false;
                ;
    break;}
case 23:
#line 311 "sslparser.y"
{
                    bFloat = true;
                ;
    break;}
case 27:
#line 321 "sslparser.y"
{
                if (Dict.RegMap.find(yyvsp[-2].str) != Dict.RegMap.end())
                    yyerror("Name reglist decared twice\n");
                Dict.RegMap[yyvsp[-2].str] = yyvsp[0].num;
            ;
    break;}
case 28:
#line 326 "sslparser.y"
{
                if (Dict.RegMap.find(yyvsp[-5].str) != Dict.RegMap.end())
                    yyerror("Name reglist declared twice\n");
                Dict.addRegister( yyvsp[-5].str, yyvsp[0].num, yyvsp[-3].num, bFloat);
            ;
    break;}
case 29:
#line 331 "sslparser.y"
{
                if (Dict.RegMap.find(yyvsp[-9].str) != Dict.RegMap.end())
                    yyerror("Name reglist declared twice\n");
                Dict.RegMap[yyvsp[-9].str] = yyvsp[-4].num;
                // Now for detailed Reg information
                if (Dict.DetRegMap.find(yyvsp[-4].num) != Dict.DetRegMap.end())
                    yyerror("Index used for more than one register\n");
                Dict.DetRegMap[yyvsp[-4].num].s_name(yyvsp[-9].str);
                Dict.DetRegMap[yyvsp[-4].num].s_size(yyvsp[-7].num);
                Dict.DetRegMap[yyvsp[-4].num].s_address(NULL);
                // check range is legitimate for size. 8,10
                if ((Dict.RegMap.find(yyvsp[-2].str) == Dict.RegMap.end())
                    || (Dict.RegMap.find(yyvsp[0].str) == Dict.RegMap.end()))
                   yyerror("Undefined range\n");
                else {
                    int bitsize = Dict.DetRegMap[Dict.RegMap[yyvsp[0].str]].g_size();
                    for (int i = Dict.RegMap[yyvsp[-2].str]; i != Dict.RegMap[yyvsp[0].str]; i++) {
                        if (Dict.DetRegMap.find(i) == Dict.DetRegMap.end()) {
                            yyerror("Not all regesters in range defined\n");
                            break;
                        }
                        bitsize += Dict.DetRegMap[i].g_size();
                        if (bitsize > yyvsp[-7].num) {
                            yyerror("Range exceeds size of register\n");
                            break;
                        }
                    }
                if (bitsize < yyvsp[-7].num) 
                    yyerror("Register size is exceeds regesters in range\n");
                    // copy information
                }
                Dict.DetRegMap[yyvsp[-4].num].s_mappedIndex(Dict.RegMap[yyvsp[-2].str]);
                Dict.DetRegMap[yyvsp[-4].num].s_mappedOffset(0);
                Dict.DetRegMap[yyvsp[-4].num].s_float(bFloat);
            ;
    break;}
case 30:
#line 366 "sslparser.y"
{
                if (Dict.RegMap.find(yyvsp[-13].str) != Dict.RegMap.end())
                    yyerror("Name reglist declared twice\n");
                Dict.RegMap[yyvsp[-13].str] = yyvsp[-8].num;
                // Now for detailed Reg information
                if (Dict.DetRegMap.find(yyvsp[-8].num) != Dict.DetRegMap.end())
                    yyerror("Index used for more than one register\n");
                Dict.DetRegMap[yyvsp[-8].num].s_name(yyvsp[-13].str);
                Dict.DetRegMap[yyvsp[-8].num].s_size(yyvsp[-11].num);
                Dict.DetRegMap[yyvsp[-8].num].s_address(NULL);
                // Do checks
                if (yyvsp[-11].num != (yyvsp[-1].num - yyvsp[-3].num) + 1) 
                    yyerror("Size does not equal range\n");
                    if (Dict.RegMap.find(yyvsp[-6].str) != Dict.RegMap.end()) {
                        if (yyvsp[-1].num >= Dict.DetRegMap[Dict.RegMap[yyvsp[-6].str]].g_size())
                            yyerror("Range extends over target register\n");
                    } else 
                        yyerror("Shared index not yet defined\n");
                Dict.DetRegMap[yyvsp[-8].num].s_mappedIndex(Dict.RegMap[yyvsp[-6].str]);
                Dict.DetRegMap[yyvsp[-8].num].s_mappedOffset(yyvsp[-3].num);
                Dict.DetRegMap[yyvsp[-8].num].s_float(bFloat);
            ;
    break;}
case 31:
#line 388 "sslparser.y"
{
                if ((int)yyvsp[-8].strlist->size() != (yyvsp[0].num - yyvsp[-2].num + 1)) {
                    std::cerr << "size of register array does not match mapping to r["
                         << yyvsp[-2].num << ".." << yyvsp[0].num << "]\n";
                    exit(1);
                } else {
                    std::list<std::string>::iterator loc = yyvsp[-8].strlist->begin();
                    for (int x = yyvsp[-2].num; x <= yyvsp[0].num; x++, loc++) {
                        if (Dict.RegMap.find(*loc) != Dict.RegMap.end())
                            yyerror("Name reglist declared twice\n");
                        Dict.addRegister( loc->c_str(), x, yyvsp[-5].num, bFloat);
                    }
                    //delete $2;
                }
            ;
    break;}
case 32:
#line 403 "sslparser.y"
{
                std::list<std::string>::iterator loc = yyvsp[-6].strlist->begin();
                for (; loc != yyvsp[-6].strlist->end(); loc++) {
                    if (Dict.RegMap.find(*loc) != Dict.RegMap.end())
                        yyerror("Name reglist declared twice\n");
		    Dict.addRegister(loc->c_str(), yyvsp[0].num, yyvsp[-3].num, bFloat);
                }
                //delete $2;
            ;
    break;}
case 33:
#line 415 "sslparser.y"
{
                yyvsp[-2].strlist->push_back(yyvsp[0].str);
            ;
    break;}
case 34:
#line 418 "sslparser.y"
{
                yyval.strlist = new std::list<std::string>;
                yyval.strlist->push_back(yyvsp[0].str);
            ;
    break;}
case 35:
#line 427 "sslparser.y"
{
                // Note: $2 is a list of strings
                Dict.FlagFuncs[yyvsp[-5].str] = new FlagDef(listStrToExp(yyvsp[-4].parmlist), yyvsp[-1].rtlist);
            ;
    break;}
case 36:
#line 434 "sslparser.y"
{
                if (ConstTable.find(yyvsp[-2].str) != ConstTable.end())
                    yyerror("Constant declared twice");
                ConstTable[std::string(yyvsp[-2].str)] = yyvsp[0].num;
            ;
    break;}
case 37:
#line 440 "sslparser.y"
{
                if (ConstTable.find(yyvsp[-4].str) != ConstTable.end())
                    yyerror("Constant declared twice");
                else if (yyvsp[-1].str == std::string("-"))
                    ConstTable[std::string(yyvsp[-4].str)] = yyvsp[-2].num - yyvsp[0].num;
                else if (yyvsp[-1].str == std::string("+"))
                    ConstTable[std::string(yyvsp[-4].str)] = yyvsp[-2].num + yyvsp[0].num;
                else
                    yyerror("Constant expression must be NUM + NUM or NUM - NUM");
            ;
    break;}
case 38:
#line 454 "sslparser.y"
{
            TableDict[yyvsp[-2].str] = yyvsp[0].tab;
        ;
    break;}
case 39:
#line 460 "sslparser.y"
{
            yyval.tab = new Table(*yyvsp[0].namelist);
            //delete $1;
        ;
    break;}
case 40:
#line 465 "sslparser.y"
{
            yyval.tab = new OpTable(*yyvsp[0].namelist);
            //delete $1;
        ;
    break;}
case 41:
#line 469 "sslparser.y"
{
            yyval.tab = new ExprTable(*yyvsp[0].exprlist);
            //delete $1;
        ;
    break;}
case 42:
#line 476 "sslparser.y"
{
            // cross-product of two str_expr's
            std::deque<std::string>::iterator i, j;
            yyval.namelist = new std::deque<std::string>;
            for (i = yyvsp[-1].namelist->begin(); i != yyvsp[-1].namelist->end(); i++)
                for (j = yyvsp[0].namelist->begin(); j != yyvsp[0].namelist->end(); j++)
                    yyval.namelist->push_back((*i) + (*j));
            //delete $1;
            //delete $2;
        ;
    break;}
case 43:
#line 486 "sslparser.y"
{
            yyval.namelist = yyvsp[0].namelist;
        ;
    break;}
case 44:
#line 492 "sslparser.y"
{
            // want to append $3 to $1
            // The following causes a massive warning message about mixing
            // signed and unsigned
            yyvsp[-2].namelist->insert(yyvsp[-2].namelist->end(), yyvsp[0].namelist->begin(), yyvsp[0].namelist->end());
            //delete $3;
            yyval.namelist = yyvsp[-2].namelist;
        ;
    break;}
case 45:
#line 500 "sslparser.y"
{
            yyvsp[-3].namelist->push_back("");
        ;
    break;}
case 46:
#line 503 "sslparser.y"
{
            yyval.namelist = yyvsp[0].namelist;
        ;
    break;}
case 47:
#line 509 "sslparser.y"
{
            yyval.namelist = yyvsp[-1].namelist;
        ;
    break;}
case 48:
#line 512 "sslparser.y"
{
            yyval.namelist = yyvsp[0].namelist;
        ;
    break;}
case 49:
#line 518 "sslparser.y"
{
            yyval.namelist = new std::deque<std::string>;
            yyval.namelist->push_back("");
            yyval.namelist->push_back(yyvsp[-1].str);
        ;
    break;}
case 50:
#line 523 "sslparser.y"
{
            yyval.namelist = new std::deque<std::string>(1, yyvsp[-1].str);
        ;
    break;}
case 51:
#line 526 "sslparser.y"
{
            std::ostringstream o;
            // expand $2 from table of names
            if (TableDict.find(yyvsp[0].str) != TableDict.end())
                if (TableDict[yyvsp[0].str]->getType() == NAMETABLE)
                    yyval.namelist = new std::deque<std::string>(TableDict[yyvsp[0].str]->records);
                else {
                    o << "name " << yyvsp[0].str << " is not a NAMETABLE.\n";
                    yyerror(STR(o));
                }
            else {
                o << "could not dereference name " << yyvsp[0].str << "\n";
                yyerror(STR(o));
            }
        ;
    break;}
case 52:
#line 541 "sslparser.y"
{
            // try and expand $1 from table of names
            // if fail, expand using '"' NAME '"' rule
            if (TableDict.find(yyvsp[0].str) != TableDict.end())
                if (TableDict[yyvsp[0].str]->getType() == NAMETABLE)
                    yyval.namelist = new std::deque<std::string>(TableDict[yyvsp[0].str]->records);
                else {
                    std::ostringstream o;
                    o << "name " << yyvsp[0].str << " is not a NAMETABLE.\n";
                    yyerror(STR(o));
                }
            else {
                yyval.namelist = new std::deque<std::string>;
                yyval.namelist->push_back(yyvsp[0].str);
            }
        ;
    break;}
case 53:
#line 560 "sslparser.y"
{
            yyval.str = yyvsp[0].str;
        ;
    break;}
case 54:
#line 564 "sslparser.y"
{
            yyval.str = yyvsp[0].str;
        ;
    break;}
case 55:
#line 568 "sslparser.y"
{
            yyval.str = yyvsp[0].str;
        ;
    break;}
case 56:
#line 575 "sslparser.y"
{
            yyval.namelist = yyvsp[-1].namelist;
        ;
    break;}
case 57:
#line 582 "sslparser.y"
{
            yyval.namelist = yyvsp[-4].namelist;
            yyval.namelist->push_back(yyvsp[-1].str);
        ;
    break;}
case 58:
#line 586 "sslparser.y"
{
            yyval.namelist = new std::deque<std::string>;
            yyval.namelist->push_back(yyvsp[-1].str);
        ;
    break;}
case 59:
#line 594 "sslparser.y"
{
            yyval.exprlist = yyvsp[-1].exprlist;
        ;
    break;}
case 60:
#line 601 "sslparser.y"
{
            yyval.exprlist = yyvsp[-4].exprlist;
            yyval.exprlist->push_back(yyvsp[-1].exp);
        ;
    break;}
case 61:
#line 605 "sslparser.y"
{
            yyval.exprlist = new std::deque<Exp*>;
            yyval.exprlist->push_back(yyvsp[-1].exp);
        ;
    break;}
case 62:
#line 613 "sslparser.y"
{
            yyvsp[0].insel->getrefmap(indexrefmap);
        //     $3           $4
        ;
    break;}
case 63:
#line 616 "sslparser.y"
{
            // This function expands the tables and saves the expanded RTLs
            // to the dictionary
            expandTables(yyvsp[-3].insel, yyvsp[-1].parmlist, yyvsp[0].rtlist, Dict);
        ;
    break;}
case 64:
#line 624 "sslparser.y"
{
            yyval.insel = yyvsp[0].insel;
        ;
    break;}
case 65:
#line 627 "sslparser.y"
{
            unsigned i;
            InsNameElem *temp;
            std::string nm = yyvsp[0].str;
            
            if (nm[0] == '^')
	            nm.replace(0, 1, "");

            // remove all " and _, from the decoration
            while ((i = nm.find("\"")) != nm.npos)
                nm.replace(i,1,"");
            // replace all '.' with '_'s from the decoration
            while ((i = nm.find(".")) != nm.npos)
	            nm.replace(i,1,"_");
            while ((i = nm.find("_")) != nm.npos)
	            nm.replace(i,1,"");
 
            temp = new InsNameElem(nm.c_str());
            yyval.insel = yyvsp[-1].insel;
            yyval.insel->append(temp);
        ;
    break;}
case 66:
#line 651 "sslparser.y"
{
            yyval.insel = new InsNameElem(yyvsp[0].str);
        ;
    break;}
case 67:
#line 654 "sslparser.y"
{
            yyval.insel = yyvsp[0].insel;
        ;
    break;}
case 68:
#line 657 "sslparser.y"
{
            yyval.insel = yyvsp[-1].insel;
            yyval.insel->append(yyvsp[0].insel);
        ;
    break;}
case 69:
#line 664 "sslparser.y"
{
            yyval.insel = new InsOptionElem(yyvsp[-1].str);
        ;
    break;}
case 70:
#line 667 "sslparser.y"
{
            std::ostringstream o;
            if (TableDict.find(yyvsp[-2].str) == TableDict.end()) {
                o << "Table " << yyvsp[-2].str << " has not been declared.\n";
                yyerror(STR(o));
            } else if ((yyvsp[-1].num < 0) || (yyvsp[-1].num >= (int)TableDict[yyvsp[-2].str]->records.size())) {
                o << "Can't get element " << yyvsp[-1].num << " of table " << yyvsp[-2].str << ".\n";
                yyerror(STR(o));
            } else
                yyval.insel = new InsNameElem(TableDict[yyvsp[-2].str]->records[yyvsp[-1].num].c_str());
        ;
    break;}
case 71:
#line 680 "sslparser.y"
{
            std::ostringstream o;
            if (TableDict.find(yyvsp[-2].str) == TableDict.end()) {
                o << "Table " << yyvsp[-2].str << " has not been declared.\n";
                yyerror(STR(o));
            } else
                yyval.insel = new InsListElem(yyvsp[-2].str, TableDict[yyvsp[-2].str], yyvsp[-1].str);
        ;
    break;}
case 72:
#line 689 "sslparser.y"
{
            std::ostringstream o;
            if (TableDict.find(yyvsp[-2].str) == TableDict.end()) {
                o << "Table " << yyvsp[-2].str << " has not been declared.\n";
                yyerror(STR(o));
            } else if ((yyvsp[-1].num < 0) || (yyvsp[-1].num >= (int)TableDict[yyvsp[-2].str]->records.size())) {
                o << "Can't get element " << yyvsp[-1].num << " of table " << yyvsp[-2].str << ".\n";
                yyerror(STR(o));
            } else
                yyval.insel = new InsNameElem(TableDict[yyvsp[-2].str]->records[yyvsp[-1].num].c_str());
        ;
    break;}
case 73:
#line 700 "sslparser.y"
{
            std::ostringstream o;
            if (TableDict.find(yyvsp[-2].str) == TableDict.end()) {
                o << "Table " << yyvsp[-2].str << " has not been declared.\n";
                yyerror(STR(o));
            } else
                yyval.insel = new InsListElem(yyvsp[-2].str, TableDict[yyvsp[-2].str], yyvsp[-1].str);
        ;
    break;}
case 74:
#line 709 "sslparser.y"
{
            yyval.insel = new InsNameElem(yyvsp[-1].str);
        ;
    break;}
case 75:
#line 715 "sslparser.y"
{
            // append any automatically generated register transfers and clear
            // the list they were stored in. Do nothing for a NOP (i.e. $2 = 0)
            if (yyvsp[0].regtransfer != NULL) {
                yyvsp[-1].rtlist->appendStmt(yyvsp[0].regtransfer);
            }
            yyval.rtlist = yyvsp[-1].rtlist;
        ;
    break;}
case 76:
#line 724 "sslparser.y"
{
            yyval.rtlist = new RTL(STMT_ASSIGN);
            if (yyvsp[0].regtransfer != NULL)
                yyval.rtlist->appendStmt(yyvsp[0].regtransfer);
        ;
    break;}
case 77:
#line 732 "sslparser.y"
{
            yyval.regtransfer = yyvsp[0].regtransfer;
        ;
    break;}
case 78:
#line 738 "sslparser.y"
{
            std::ostringstream o;
            if (Dict.FlagFuncs.find(yyvsp[-2].str) != Dict.FlagFuncs.end()) {
                // Note: SETFFLAGS assigns to the floating point flags
                // All others to the integer flags
                bool bFloat = strcmp(yyvsp[-2].str, "SETFFLAGS") == 0;
                OPER op = bFloat ? opFflags : opFlags;
                yyval.regtransfer = new Assign(
                    new Terminal(op),
                    new Binary(opFlagCall,
                        new Const(yyvsp[-2].str),
                        listExpToExp(yyvsp[-1].explist)));
            } else {
                o << yyvsp[-2].str << " is not declared as a flag function.\n";
                yyerror(STR(o));
            }
        ;
    break;}
case 79:
#line 755 "sslparser.y"
{
            yyval.regtransfer = 0;
        ;
    break;}
case 80:
#line 759 "sslparser.y"
{
            yyval.regtransfer = 0;
        ;
    break;}
case 81:
#line 762 "sslparser.y"
{
        yyval.regtransfer = NULL;
    ;
    break;}
case 82:
#line 768 "sslparser.y"
{
            // Not sure why the below is commented out (MVE)
/*          Location* pFlag = Location::regOf(Dict.RegMap[$3]);
            $1->push_back(pFlag);
            $$ = $1;
*/          yyval.explist = 0;
        ;
    break;}
case 83:
#line 775 "sslparser.y"
{
/*          std::list<Exp*>* tmp = new std::list<Exp*>;
            Unary* pFlag = new Unary(opIdRegOf, Dict.RegMap[$1]);
            tmp->push_back(pFlag);
            $$ = tmp;
*/          yyval.explist = 0;
        ;
    break;}
case 84:
#line 786 "sslparser.y"
{
            assert(yyvsp[0].str != 0);
            yyvsp[-2].parmlist->push_back(yyvsp[0].str);
            yyval.parmlist = yyvsp[-2].parmlist;
        ;
    break;}
case 85:
#line 792 "sslparser.y"
{
            yyval.parmlist = new std::list<std::string>;
            yyval.parmlist->push_back(yyvsp[0].str);
        ;
    break;}
case 86:
#line 796 "sslparser.y"
{
            yyval.parmlist = new std::list<std::string>;
        ;
    break;}
case 87:
#line 801 "sslparser.y"
{
            Dict.ParamSet.insert(yyvsp[0].str);       // Not sure if we need this set
            yyval.str = yyvsp[0].str;
        ;
    break;}
case 88:
#line 807 "sslparser.y"
{
            yyval.explist->push_back(yyvsp[0].exp);
        ;
    break;}
case 89:
#line 811 "sslparser.y"
{
            yyval.explist = new std::list<Exp*>;
            yyval.explist->push_back(yyvsp[0].exp);
        ;
    break;}
case 90:
#line 816 "sslparser.y"
{
            yyval.explist = new std::list<Exp*>;
        ;
    break;}
case 91:
#line 824 "sslparser.y"
{
            Assign* a = new Assign(yyvsp[-5].num, yyvsp[-2].exp, yyvsp[0].exp);
            a->setGuard(yyvsp[-4].exp);
            yyval.regtransfer = a;
        ;
    break;}
case 92:
#line 831 "sslparser.y"
{
            // update the size of any generated RT's
            yyval.regtransfer = new Assign(yyvsp[-3].num, yyvsp[-2].exp, yyvsp[0].exp);
        ;
    break;}
case 93:
#line 837 "sslparser.y"
{
            yyval.regtransfer = new Assign(
                new Terminal(opNil),
                new Terminal(opFpush));
        ;
    break;}
case 94:
#line 842 "sslparser.y"
{
            yyval.regtransfer = new Assign(
                new Terminal(opNil),
                new Terminal(opFpop));
        ;
    break;}
case 95:
#line 848 "sslparser.y"
{
            yyval.regtransfer = new Assign(yyvsp[-1].num, 0, yyvsp[0].exp);
        ;
    break;}
case 96:
#line 854 "sslparser.y"
{
            yyval.exp = new Const(yyvsp[0].num);
        ;
    break;}
case 97:
#line 858 "sslparser.y"
{
            yyval.exp = new Const(yyvsp[0].dbl);
        ;
    break;}
case 98:
#line 862 "sslparser.y"
{
            yyval.exp = new Unary(opTemp, new Const(yyvsp[0].str));
        ;
    break;}
case 99:
#line 866 "sslparser.y"
{
            yyval.exp = yyvsp[-1].exp;
        ;
    break;}
case 100:
#line 870 "sslparser.y"
{
            yyval.exp = yyvsp[0].exp;
        ;
    break;}
case 101:
#line 874 "sslparser.y"
{
            yyval.exp = new Ternary(opTern, yyvsp[-5].exp, yyvsp[-3].exp, yyvsp[-1].exp);
        ;
    break;}
case 102:
#line 878 "sslparser.y"
{
            Ternary* t = new Ternary(opTern, yyvsp[-6].exp, yyvsp[-4].exp, yyvsp[-2].exp);
            Exp* e = t;
            if (yyvsp[0].num != STD_SIZE) {
	        e = new TypedExp(new IntegerType(yyvsp[0].num), t);                
            }
            yyval.exp = e;
        ;
    break;}
case 103:
#line 888 "sslparser.y"
{
            yyval.exp = new Unary(opAddrOf, yyvsp[-1].exp);
        ;
    break;}
case 104:
#line 894 "sslparser.y"
{
            yyval.exp = new Ternary(strToOper(yyvsp[-6].str), new Const(yyvsp[-5].num), new Const(yyvsp[-3].num), yyvsp[-1].exp);
        ;
    break;}
case 105:
#line 899 "sslparser.y"
{
            yyval.exp = new Unary(opFtrunc, yyvsp[-1].exp);
        ;
    break;}
case 106:
#line 904 "sslparser.y"
{
            yyval.exp = new Unary(opFabs, yyvsp[-1].exp);
        ;
    break;}
case 107:
#line 909 "sslparser.y"
{
            yyval.exp = new Terminal(opFpush);
        ;
    break;}
case 108:
#line 912 "sslparser.y"
{
            yyval.exp = new Terminal(opFpop);
        ;
    break;}
case 109:
#line 917 "sslparser.y"
{
            yyval.exp = new Unary(strToOper(yyvsp[-2].str), yyvsp[-1].exp);
        ;
    break;}
case 110:
#line 923 "sslparser.y"
{
            std::ostringstream o;
            if (indexrefmap.find(yyvsp[-1].str) == indexrefmap.end()) {
                o << "index " << yyvsp[-1].str << " not declared for use.\n";
                yyerror(STR(o));
            } else if (TableDict.find(yyvsp[-2].str) == TableDict.end()) {
                o << "table " << yyvsp[-2].str << " not declared for use.\n";
                yyerror(STR(o));
            } else if (TableDict[yyvsp[-2].str]->getType() != EXPRTABLE) {
                o << "table " << yyvsp[-2].str << " is not an expression table but "
                  "appears to be used as one.\n";
                yyerror(STR(o));
            } else if ((int)((ExprTable*)TableDict[yyvsp[-2].str])->expressions.size() <
              indexrefmap[yyvsp[-1].str]->ntokens()) {
                o << "table " << yyvsp[-2].str << " (" <<
                  ((ExprTable*)TableDict[yyvsp[-2].str])->expressions.size() <<
                  ") is too small to use " << yyvsp[-1].str << " (" <<
                  indexrefmap[yyvsp[-1].str]->ntokens() << ") as an index.\n";
                yyerror(STR(o));
            }
            // $1 is a map from string to Table*; $2 is a map from string to
            // InsNameElem*
            yyval.exp = new Binary(opExpTable, new Const(yyvsp[-2].str), new Const(yyvsp[-1].str));
        ;
    break;}
case 111:
#line 951 "sslparser.y"
{
        std::ostringstream o;
        if (Dict.ParamSet.find(yyvsp[-2].str) != Dict.ParamSet.end() ) {
            if (Dict.DetParamMap.find(yyvsp[-2].str) != Dict.DetParamMap.end()) {
                ParamEntry& param = Dict.DetParamMap[yyvsp[-2].str];
                if (yyvsp[-1].explist->size() != param.funcParams.size() ) {
                    o << yyvsp[-2].str << " requires " << param.funcParams.size()
                      << " parameters, but received " << yyvsp[-1].explist->size() << ".\n";
                    yyerror(STR(o));
                } else {
                    // Everything checks out. *phew* 
                    // Note: the below may not be right! (MVE)
                    yyval.exp = new Binary(opFlagDef,
                            new Const(yyvsp[-2].str),
                            listExpToExp(yyvsp[-1].explist));
                    //delete $2;          // Delete the list of char*s
                }
            } else {
                o << yyvsp[-2].str << " is not defined as a OPERAND function.\n";
                yyerror(STR(o));
            }
        } else {
            o << yyvsp[-2].str << ": Unrecognized name in call.\n";
            yyerror(STR(o));
        }
    ;
    break;}
case 112:
#line 978 "sslparser.y"
{
			yyval.exp = makeSuccessor(yyvsp[-1].exp);
		;
    break;}
case 113:
#line 984 "sslparser.y"
{
            yyval.exp = new Unary(opSignExt, yyvsp[-1].exp);
        ;
    break;}
case 114:
#line 993 "sslparser.y"
{
            // opSize is deprecated, but for old SSL files we'll make a TypedExp
            if (yyvsp[0].num == STD_SIZE)
                yyval.exp = yyvsp[-1].exp;
            else
                yyval.exp = new TypedExp(new IntegerType(yyvsp[0].num), yyvsp[-1].exp);
        ;
    break;}
case 115:
#line 1001 "sslparser.y"
{
            yyval.exp = new Unary(opNot, yyvsp[0].exp);
        ;
    break;}
case 116:
#line 1005 "sslparser.y"
{
            yyval.exp = new Binary(strToOper(yyvsp[-1].str), yyvsp[-2].exp, yyvsp[0].exp);
        ;
    break;}
case 117:
#line 1009 "sslparser.y"
{
            yyval.exp = new Binary(strToOper(yyvsp[-1].str), yyvsp[-2].exp, yyvsp[0].exp);
        ;
    break;}
case 118:
#line 1013 "sslparser.y"
{
            yyval.exp = new Binary(strToOper(yyvsp[-1].str), yyvsp[-2].exp, yyvsp[0].exp);
        ;
    break;}
case 119:
#line 1017 "sslparser.y"
{
            yyval.exp = new Binary(strToOper(yyvsp[-1].str), yyvsp[-2].exp, yyvsp[0].exp);
        ;
    break;}
case 120:
#line 1021 "sslparser.y"
{
            yyval.exp = new Binary(strToOper(yyvsp[-1].str), yyvsp[-2].exp, yyvsp[0].exp);
        ;
    break;}
case 121:
#line 1028 "sslparser.y"
{
            std::ostringstream o;
            if (indexrefmap.find(yyvsp[-2].str) == indexrefmap.end()) {
                o << "index " << yyvsp[-2].str << " not declared for use.\n";
                yyerror(STR(o));
            } else if (TableDict.find(yyvsp[-3].str) == TableDict.end()) {
                o << "table " << yyvsp[-3].str << " not declared for use.\n";
                yyerror(STR(o));
            } else if (TableDict[yyvsp[-3].str]->getType() != OPTABLE) {
                o << "table " << yyvsp[-3].str <<
                  " is not an operator table but appears to be used as one.\n";
                yyerror(STR(o));
            } else if ((int)TableDict[yyvsp[-3].str]->records.size() <
              indexrefmap[yyvsp[-2].str]->ntokens()) {
                o << "table " << yyvsp[-3].str << " is too small to use with " << yyvsp[-2].str
                  << " as an index.\n";
                yyerror(STR(o));
            }
            yyval.exp = new Ternary(opOpTable, new Const(yyvsp[-3].str), new Const(yyvsp[-2].str),
                new Binary(opList,
                    yyvsp[-4].exp,
                    new Binary(opList,
                        yyvsp[0].exp,
                        new Terminal(opNil))));
        ;
    break;}
case 122:
#line 1054 "sslparser.y"
{
            yyval.exp = yyvsp[0].exp;
        ;
    break;}
case 123:
#line 1065 "sslparser.y"
{
            bool isFlag = strstr(yyvsp[0].str, "flags") != 0;
            std::map<std::string, int>::const_iterator it = Dict.RegMap.find(yyvsp[0].str);
            if (it == Dict.RegMap.end() && !isFlag) {
                std::ostringstream ost;
                ost << "register `" << yyvsp[0].str << "' is undefined";
                yyerror(STR(ost));
            } else if (isFlag || it->second == -1) {
                // A special register, e.g. %npc or %CF
                // Return a Terminal for it
                OPER op = strToTerm(yyvsp[0].str);
                if (op) {
                    yyval.exp = new Terminal(op);
                } else {
                    yyval.exp = new Unary(opMachFtr,    // Machine specific feature
                            new Const(yyvsp[0].str));
                }
            }
            else {
                // A register with a constant reg nmber, e.g. %g2.
                // In this case, we want to return r[const 2]
                yyval.exp = Location::regOf(it->second);
            }
        ;
    break;}
case 124:
#line 1090 "sslparser.y"
{
            yyval.exp = Location::regOf(yyvsp[-1].exp);
        ;
    break;}
case 125:
#line 1094 "sslparser.y"
{
            int regNum;
            sscanf(yyvsp[0].str, "r%d", &regNum);
            yyval.exp = Location::regOf(regNum);
        ;
    break;}
case 126:
#line 1100 "sslparser.y"
{
            yyval.exp = Location::memOf(yyvsp[-1].exp);
        ;
    break;}
case 127:
#line 1104 "sslparser.y"
{
        // This is a mixture of the param: PARM {} match
        // and the value_op: NAME {} match
            Exp* s;
            std::set<std::string>::iterator it = Dict.ParamSet.find(yyvsp[0].str);
            if (it != Dict.ParamSet.end()) {
                s = new Unary(opParam, new Const(yyvsp[0].str));
            } else if (ConstTable.find(yyvsp[0].str) != ConstTable.end()) {
                s = new Const(ConstTable[yyvsp[0].str]);
            } else {
                std::ostringstream ost;
                ost << "`" << yyvsp[0].str << "' is not a constant, definition or a";
                ost << " parameter of this instruction\n";
                yyerror(STR(ost));
            }
            yyval.exp = s;
        ;
    break;}
case 128:
#line 1122 "sslparser.y"
{
            yyval.exp = new Ternary(opAt, yyvsp[-6].exp, yyvsp[-3].exp, yyvsp[-1].exp);
        ;
    break;}
case 129:
#line 1126 "sslparser.y"
{
            yyval.exp = new Unary(opPostVar, yyvsp[-1].exp);
        ;
    break;}
case 130:
#line 1129 "sslparser.y"
{
			yyval.exp = makeSuccessor(yyvsp[-1].exp);
		;
    break;}
case 131:
#line 1135 "sslparser.y"
{
            yyval.num = yyvsp[-1].num;
        ;
    break;}
case 132:
#line 1141 "sslparser.y"
{
            Dict.bigEndian = (strcmp(yyvsp[0].str, "BIG") == 0);
        ;
    break;}
case 133:
#line 1146 "sslparser.y"
{
            yyval.str = yyvsp[0].str;
        ;
    break;}
case 134:
#line 1149 "sslparser.y"
{
            yyval.str = yyvsp[0].str;
        ;
    break;}
case 138:
#line 1168 "sslparser.y"
{
            Dict.fastMap[std::string(yyvsp[-2].str)] = std::string(yyvsp[0].str);
        ;
    break;}
}

#line 783 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc"
   /* the action file gets copied in in place of this dollarsign  */
  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YY_SSLParser_LSP_NEEDED
  yylsp -= yylen;
#endif

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YY_SSLParser_LSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = YY_SSLParser_LLOC.first_line;
      yylsp->first_column = YY_SSLParser_LLOC.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  YYGOTO(yynewstate);

YYLABEL(yyerrlab)   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++YY_SSLParser_NERRS;

#ifdef YY_SSLParser_ERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       (unsigned)x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       (unsigned)x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      YY_SSLParser_ERROR(msg);
	      free(msg);
	    }
	  else
	    YY_SSLParser_ERROR ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YY_SSLParser_ERROR_VERBOSE */
	YY_SSLParser_ERROR("parse error");
    }

  YYGOTO(yyerrlab1);
YYLABEL(yyerrlab1)   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (YY_SSLParser_CHAR == YYEOF)
	YYABORT;

#if YY_SSLParser_DEBUG != 0
      if (YY_SSLParser_DEBUG_FLAG)
	fprintf(stderr, "Discarding token %d (%s).\n", YY_SSLParser_CHAR, yytname[yychar1]);
#endif

      YY_SSLParser_CHAR = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;              /* Each real token shifted decrements this */

  YYGOTO(yyerrhandle);

YYLABEL(yyerrdefault)  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) YYGOTO(yydefault);
#endif

YYLABEL(yyerrpop)   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YY_SSLParser_LSP_NEEDED
  yylsp--;
#endif

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

YYLABEL(yyerrhandle)

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    YYGOTO(yyerrdefault);

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    YYGOTO(yyerrdefault);

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	YYGOTO(yyerrpop);
      yyn = -yyn;
      YYGOTO(yyreduce);
    }
  else if (yyn == 0)
    YYGOTO(yyerrpop);

  if (yyn == YYFINAL)
    YYACCEPT;

#if YY_SSLParser_DEBUG != 0
  if (YY_SSLParser_DEBUG_FLAG)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = YY_SSLParser_LVAL;
#ifdef YY_SSLParser_LSP_NEEDED
  *++yylsp = YY_SSLParser_LLOC;
#endif

  yystate = yyn;
  YYGOTO(yynewstate);
/* end loop, in which YYGOTO may be used. */
  YYENDGOTO
}

/* END */

/* #line 982 "/home/02/binary/u1.luna.tools/bison++/lib/bison.cc" */
#line 2832 "sslparser.cpp"
#line 1171 "sslparser.y"


/*==============================================================================
 * FUNCTION:        SSLParser::SSLParser
 * OVERVIEW:        Constructor for an existing stream.
 * PARAMETERS:      The stream, whether or not to debug
 * RETURNS:         <nothing>
 *============================================================================*/
SSLParser::SSLParser(std::istream &in, bool trace) : sslFile("input"), bFloat(false)
{
    theScanner = new SSLScanner(in, trace);
    if (trace) yydebug = 1; else yydebug=0;
}

/*==============================================================================
 * FUNCTION:        SSLParser::parseExp
 * OVERVIEW:        Parses an assignment from a string.
 * PARAMETERS:      the string
 * RETURNS:         an Assignment or NULL.
 *============================================================================*/
Statement* SSLParser::parseExp(const char *str) {
    std::istringstream ss(str);
    SSLParser p(ss, false);     // Second arg true for debugging
    RTLInstDict d;
    p.yyparse(d);
    return p.the_asgn;
}

/*==============================================================================
 * FUNCTION:        SSLParser::~SSLParser
 * OVERVIEW:        Destructor.
 * PARAMETERS:      <none>
 * RETURNS:         <nothing>
 *============================================================================*/
SSLParser::~SSLParser()
{
    std::map<std::string, Table*>::iterator loc;
    if (theScanner != NULL)
        delete theScanner;
    for(loc = TableDict.begin(); loc != TableDict.end(); loc++)
        delete loc->second;
}

/*==============================================================================
 * FUNCTION:        SSLParser::yyerror
 * OVERVIEW:        Display an error message and exit.
 * PARAMETERS:      msg - an error message
 * RETURNS:         <nothing>
 *============================================================================*/
void SSLParser::yyerror(char* msg)
{
    std::cerr << sslFile << ":" << theScanner->theLine << ": " << msg << std::endl;
}

/*==============================================================================
 * FUNCTION:        SSLParser::yylex
 * OVERVIEW:        The scanner driver than returns the next token.
 * PARAMETERS:      <none>
 * RETURNS:         the next token
 *============================================================================*/
int SSLParser::yylex()
{
    int token = theScanner->yylex(yylval);
    return token;
}

/*==============================================================================
 * FUNCTION:        SSLParser::strToOper
 * OVERVIEW:        Convert a string operator (e.g. "+f") to an OPER (opFPlus)
 * NOTE:            An attempt is made to make this moderately efficient, else
 *                    we might have a skip chain of string comparisons
 * NOTE:            This is a member of SSLParser so we can call yyerror and
 *                    have line number etc printed out
 * PARAMETERS:      s: pointer to the operator C string
 * RETURNS:         An OPER, or -1 if not found (enum opWild)
 *============================================================================*/
OPER SSLParser::strToOper(const char* s) {
    switch (s[0]) {
        case '*':
            // Could be *, *!, *f, *fsd, *fdq, *f[sdq]
            switch (s[1]) {
                case '\0': return opMult;
                case '!' : return opMults;
                case 'f' :
                    if ((s[2] == 's') && (s[3] == 'd')) return opFMultsd;
                    if ((s[2] == 'd') && (s[3] == 'q')) return opFMultdq;
                    return opFMult;
                default: break;
            }
            break;
        case '/':
            // Could be /, /!, /f, /f[sdq]
            switch (s[1]) {
                case '\0': return opDiv;
                case '!' : return opDivs;
                case 'f' : return opFDiv;
                default: break;
            }
            break;
        case '%':
            // Could be %, %!
            switch (s[1]) {
                case '\0': return opMod;
                case '!' : return opMods;
                default: break;
            }
            break;
        case '+':
            // Could be +, +f, +f[sdq]
            switch (s[1]) {
                case '\0': return opPlus;
                case 'f' : return opFPlus;
                default: break;
            }
            break;
        case '-':
            // Could be -, -f, -f[sdq]
            switch (s[1]) {
                case '\0': return opMinus;
                case 'f' : return opFMinus;
                default: break;
            }
            break;
        case 'a':
            // and, arctan, addr
            if (s[1] == 'n') return opAnd;
            if (s[1] == 'r') return opArcTan;
            if (s[1] == 'd') return opAddrOf;
                break;
        case 'c':
            // cos
            return opCos;
        case 'e':
            // execute
            return opExecute;
        case 'f':
            // fsize, ftoi, fround NOTE: ftrunc handled separately
            // because it is a unary
            if (s[1] == 's') return opFsize;
            if (s[1] == 't') return opFtoi;
            if (s[1] == 'r') return opFround;
            break;
        case 'i':
            // itof
            return opItof;
        case 'l':
            // log2, log10, loge
            if (s[3] == '2') return opLog2;
            if (s[3] == '1') return opLog10;
            if (s[3] == 'e') return opLoge;
            break;
        case 'o':
            // or
            return opOr;
        case 'p':
            // pow
            return opPow;
        case 'r':
            // rlc, rrc, rl, rr
            if (s[1] == 'l') {
                if (s[2] == 'c') return opRotateLC;
                return opRotateL;
            } else if (s[1] == 'r') {
                if (s[2] == 'c') return opRotateRC;
                return opRotateR;
            }
            break;
        case 's':
            // sgnex, sin, sqrt
            if (s[1] == 'g') return opSgnEx;
            if (s[1] == 'i') return opSin;
            if (s[1] == 'q') return opSqrt;
            break;
        case 't':
            // truncu, truncs, tan
            // 012345
            if (s[1] == 'a') return opTan;
            if (s[5] == 'u') return opTruncu;
            if (s[5] == 's') return opTruncs;
            break;
        case 'z':
            // zfill
            return opZfill;

        case '>':
            // >, >u, >=, >=u, >>, >>A
            switch (s[1]) {
                case '\0': return opGtr;
                case 'u': return opGtrUns;
                case '=':
                    if (s[2] == '\0') return opGtrEq;
                    return opGtrEqUns;
                case '>':
                    if (s[2] == '\0') return opShiftR;
                    return opShiftRA;
                default: break;
            }
            break;
        case '<':
            // <, <u, <=, <=u, <<
            switch (s[1]) {
                case '\0': return opLess;
                case 'u': return opLessUns;
                case '=':
                    if (s[2] == '\0') return opLessEq;
                    return opLessEqUns;
                case '<':
                    return opShiftL;
                default: break;
            }
            break;
        case '=':
            // =
            return opEquals;
        case '!':
            // !
            return opSgnEx;
            break;
        case '~':
            // ~=, ~
            if (s[1] == '=') return opNotEqual;
            return opNot;       // Bit inversion
        case '@': return opAt;
        case '&': return opBitAnd;
        case '|': return opBitOr;
        case '^': return opBitXor;
       
        default: break; 
    }
    std::ostringstream ost;
    ost << "Unknown operator " << s << std::endl;
    yyerror(STR(ost));
    return opWild;
}

OPER strToTerm(char* s) {
    // s could be %pc, %afp, %agp, %CF, %ZF, %OF, %NF, %DF, %flags, %fflags
    if (s[2] == 'F') {
        if (s[1] <= 'N') {
            if (s[1] == 'C') return opCF;
            if (s[1] == 'N') return opNF;
            return opDF;
        } else {
            if (s[1] == 'O') return opOF;
            return opZF;
        }
    }
    if (s[1] == 'p') return opPC;
    if (s[1] == 'a') {
        if (s[2] == 'f') return opAFP;
        if (s[2] == 'g') return opAGP;
    } else if (s[1] == 'f') {
        if (s[2] == 'l') return opFlags;
        if (s[2] == 'f') return opFflags;
    }
    return (OPER) 0;
}

/*==============================================================================
 * FUNCTION:        listExpToExp
 * OVERVIEW:        Convert a list of actual parameters in the form of a
 *                    STL list of Exps into one expression (using opList)
 * NOTE:            The expressions in the list are not cloned; they are
 *                    simply copied to the new opList
 * PARAMETERS:      le: the list of expressions
 * RETURNS:         The opList Expression
 *============================================================================*/
Exp* listExpToExp(std::list<Exp*>* le) {
    Exp* e;
    Exp** cur = &e;
    for (std::list<Exp*>::iterator it = le->begin(); it != le->end(); it++) {
        *cur = new Binary(opList);
        ((Binary*)*cur)->setSubExp1(*it);
        // cur becomes the address of the address of the second subexpression
        // In other words, cur becomes a reference to the second subexp ptr
        // Note that declaring cur as a reference doesn't work (remains a
        // reference to e)
        cur = &(*cur)->refSubExp2();
    }
    *cur = new Terminal(opNil);         // Terminate the chain
    return e;
}

/*==============================================================================
 * FUNCTION:        listStrToExp
 * OVERVIEW:        Convert a list of formal parameters in the form of a
 *                    STL list of strings into one expression (using opList)
 * PARAMETERS:      ls - the list of strings
 * RETURNS:         The opList expression
 *============================================================================*/
Exp* listStrToExp(std::list<std::string>* ls) {
    Exp* e;
    Exp** cur = &e;
    for (std::list<std::string>::iterator it = ls->begin(); it != ls->end(); it++) {
        *cur = new Binary(opList);
        // *it is a string. Convert it to a parameter
        ((Binary*)*cur)->setSubExp1(new Unary(opParam,
          new Const((char*)(*it).c_str())));
        cur = &(*cur)->refSubExp2();
    }
    *cur = new Terminal(opNil);          // Terminate the chain
    return e;
}

/*==============================================================================
 * FUNCTION:        SSLParser::expandTables
 * OVERVIEW:        Expand tables in an RTL and save to dictionary
 * NOTE:            This may generate many entries
 * PARAMETERS:      iname: Parser object representing the instruction name
 *                  params: Parser object representing the instruction params
 *                  o_rtlist: Original rtlist object (before expanding)
 *                  Dict: Ref to the dictionary that will contain the results
 *                    of the parse
 * RETURNS:         <nothing>
 *============================================================================*/
void SSLParser::expandTables(InsNameElem* iname, std::list<std::string>* params,
  RTL* o_rtlist, RTLInstDict& Dict)
{
    int i, m;
    std::string nam;
    std::ostringstream o;
    m = iname->ninstructions();
    // Expand the tables (if any) in this instruction
    for (i = 0, iname->reset(); i < m; i++, iname->increment()) {
        nam = iname->getinstruction();
        // Need to make substitutions to a copy of the RTL
        RTL* rtl = o_rtlist->clone();
        int n = rtl->getNumStmt();
        Exp* srchExpr = new Binary(opExpTable, new Terminal(opWild),
            new Terminal(opWild));
        Exp* srchOp = new Ternary(opOpTable, new Terminal(opWild),
            new Terminal(opWild), new Terminal(opWild));
        for (int j=0; j < n; j++) {
            Statement* s = rtl->elementAt(j);
            std::list<Exp*> le;
            // Expression tables
            assert(s->getKind() == STMT_ASSIGN);
            if (((Assign*)s)->searchAll(srchExpr, le)) {
                std::list<Exp*>::iterator it;
                for (it = le.begin(); it != le.end(); it++) {
                    char* tbl = ((Const*)((Binary*)*it)->getSubExp1())
                      ->getStr();
                    char* idx = ((Const*)((Binary*)*it)->getSubExp2())
                      ->getStr();
                    Exp* repl =((ExprTable*)(TableDict[tbl]))
                      ->expressions[indexrefmap[idx]->getvalue()];
                    s->searchAndReplace(*it, repl);
                }
            }
            // Operator tables
			Exp* res;
			while (s->search(srchOp, res)) {
				Ternary* t;
				if (res->getOper() == opTypedExp)
				   t = (Ternary *)res->getSubExp1();
				else
				   t = (Ternary *)res;
				assert(t->getOper() == opOpTable);
                // The ternary opOpTable has a table and index
                // name as strings, then a list of 2 expressions
                // (and we want to replace it with e1 OP e2)
                char* tbl = ((Const*)t->getSubExp1()) ->getStr();
                char* idx = ((Const*)t->getSubExp2()) ->getStr();
                // The expressions to operate on are in the list
                Binary* b = (Binary*)t->getSubExp3();
                assert(b->getOper() == opList);
                Exp* e1 = b->getSubExp1();
                Exp* e2 = b->getSubExp2();  // This should be an opList too
                assert(b->getOper() == opList);
                e2 = ((Binary*)e2)->getSubExp1();
                const char* ops = ((OpTable*)(TableDict[tbl]))
                  ->records[indexrefmap[idx]->getvalue()].c_str();
                Exp* repl = new Binary(strToOper(ops), e1->clone(),
                e2->clone());
                s->searchAndReplace(res, repl);
            }
        }
   
        if (Dict.appendToDict(nam, *params, *rtl) != 0) {
            o << "Pattern " << iname->getinspattern()
              << " conflicts with an earlier declaration of " << nam <<
              ".\n";
            yyerror(STR(o));
        }
    }
    //delete iname;
    //delete params;
    //delete o_rtlist;
    indexrefmap.erase(indexrefmap.begin(), indexrefmap.end());
}

/*==============================================================================
 * FUNCTION:        SSLParser::makeSuccessor
 * OVERVIEW:        Make the successor of the given expression, e.g. given
 *					  r[2], return succ( r[2] ) (using opSuccessor)
 *					We can't do the successor operation here, because the
 *					  parameters are not yet instantiated (still of the form
 *					  param(rd)). Actual successor done in Exp::fixSuccessor()
 * NOTE:            The given expression should be of the form  r[const]
 * NOTE:			The parameter expresion is copied (not cloned) in the result
 * PARAMETERS:      The expression to find the successor of
 * RETURNS:         The modified expression
 *============================================================================*/
Exp* SSLParser::makeSuccessor(Exp* e) {
	return new Unary(opSuccessor, e);
}
