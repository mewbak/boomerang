/*
 * Copyright (C) 2004, Mike Van Emmerik and Trent Waddington
 */
/*==============================================================================
 * FILE:       generic.cpp
 * OVERVIEW:   Implementation of the RDIExpTransformer and related classes.
 *============================================================================*/
/*
 * $Revision: 1.1 $
 * 17 Apr 04 - Trent: Created
 */

#include <assert.h>
#if defined(_MSC_VER) && _MSC_VER <= 1200
#pragma warning(disable:4786)
#endif 

#include <numeric>      // For accumulate
#include <algorithm>    // For std::max()
#include <map>          // In decideType()
#include <sstream>      // Need gcc 3.0 or better
#include "types.h"
#include "statement.h"
#include "cfg.h"
#include "exp.h"
#include "register.h"
#include "rtl.h"
#include "proc.h"
#include "transformer.h"
#include "rdi.h"

Exp *RDIExpTransformer::applyTo(Exp *e, bool &bMod)
{
    if (e->getOper() == opAddrOf && e->getSubExp1()->getOper() == opMemOf) {
        e = e->getSubExp1()->getSubExp1()->clone();
        bMod = true;
    }
    return e;
}

