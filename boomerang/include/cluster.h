/*
 * Copyright (C) 2004, Trent Waddington
 *
 * See the file "LICENSE.TERMS" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

/*==============================================================================
 * FILE:       cluster.h
 * OVERVIEW:   Definition of the classes that describe a Cluster, a grouping
 * 	       of functions irrespective of relationship.  For example, the
 * 	       Object Oriented Programming concept of a Class is a Cluster. 
 * 	       Clusters can contain other Clusters to form a tree.
 *============================================================================*/

/*
 * $Revision: 1.2 $
 * 03 May 04 - Trent: Created
 */

#ifndef __CLUSTER_H__
#define __CLUSTER_H__

#include <list>
#include <vector>
#include <map>
#include <set>
#include <string>

class XMLProgParser;

class Cluster
{
protected:
    std::string name;
    std::vector<Cluster*> children;
    Cluster *parent;

public:
    Cluster() : name(""), parent(NULL) { }
    Cluster(const char *name) : name(name), parent(NULL) { }
    const char *getName() { return name.c_str(); }
    void setName(const char *nam) { name = nam; }
    unsigned int getNumChildren() { return children.size(); }
    Cluster *getChild(int n) { return children[n]; }
    void addChild(Cluster *n) { children.push_back(n); n->parent = this; }
    const char *makeDirs();
    Cluster *find(const char *nam);
protected:

    friend class XMLProgParser;
};

#endif /*__CLUSTER_H__*/

