/*
 * Copyright (C) 2002, Trent Waddington
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*=============================================================================
 * FILE:        boomerang.h
 * OVERVIEW:    interface for the boomerang singleton object
 *============================================================================*/
/*
 * $Revision: 1.1 $
 * 04 Dec 2002: Trent: Created
 */

#ifndef BOOMERANG_H
#define BOOMERANG_H

#include <iostream>
#include <string>

class Boomerang {
private:
    static Boomerang *boomerang;
    std::string progPath; // String with the path to this exec

    Boomerang();
public:
    static Boomerang *get() { 
        if (!boomerang) boomerang = new Boomerang(); 
	return boomerang;
    }

    // performs command line operation
    int commandLine(int argc, const char **argv);
    void setProgPath(const char* p) { progPath = p; }
    const std::string& getProgPath() { return progPath; }
};

#endif
