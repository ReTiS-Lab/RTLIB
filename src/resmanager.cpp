/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
/*
 * $Id: resmanager.cpp,v 1.4 2005/04/28 01:34:48 cesare Exp $
 *
 * $Log: resmanager.cpp,v $
 * Revision 1.4  2005/04/28 01:34:48  cesare
 * Moved to sstream. Headers install. Code cleaning.
 *
 * Revision 1.3  2004/11/26 03:47:10  cesare
 * Finished merging the main trunk with Lipari's branch.
 *
 */
#include <entity.hpp>

#include <abskernel.hpp>
#include <abstask.hpp>
#include <resmanager.hpp>
#include <resource.hpp>

namespace RTSim {

    using namespace std;
    using namespace MetaSim;

    ResManager::ResManager(const string &n) : Entity(n), 
                                              _kernel(0), 
                                              _res()
    {
    }

    ResManager::~ResManager()
    {
        std::vector<Resource *>::iterator i= _allocated.begin();
        for (;i != _allocated.end();i++) delete *i;

        // delete the resource list
        _res.clear();
        _allocated.clear();
    }

    void ResManager::setKernel(AbsKernel *k, Scheduler *s) 
    {
        _kernel = k;
        _sched = s;
    }

    void ResManager::addResource(const string &name, int n, res_scope_t s)
    { 
        Resource *r = new Resource(name, n, s);
        _res.push_back(r);
        _allocated.push_back(r);
    }

    void ResManager::addResource(Resource *r)
    {
        _res.push_back(r);
    }

    bool ResManager::request(AbsRTTask *t, const string &name, int n) 
    {
        DBGENTER(_RESMAN_DBG_LEV);

        Resource *r = dynamic_cast<Resource *>( Entity::_find(name) );
        bool ret = request(t,r,n);
        return ret;
    }

    bool ResManager::request(AbsRTTask *t, AbsRTTask *s, const string &name, int n)
    {
        DBGENTER(_RESMAN_DBG_LEV);

        Resource *r = dynamic_cast<Resource *>( Entity::_find(name) );
        bool ret = request(t,s,r,n);
        return ret;
    }

    void ResManager::release(AbsRTTask *t, const string &name, int n)
    {
        DBGENTER(_RESMAN_DBG_LEV);

        Resource *r = dynamic_cast<Resource *>( Entity::_find(name) );
        release(t,r,n);       
    }

    bool ResManager::find(Resource* r) const
    {
        return find(r->getName());
    }
    
    bool ResManager::find(string s) const
    {
        vector<Resource*>::const_iterator I = _res.begin();
        for (; I != _res.end(); I++)
            if ((*I)->getName() == s)
                return true;
        return false;
    }

    res_scope_t ResManager::getResScope(Resource *r) const
    {
        return getResScope(r->getName());
    }
    res_scope_t ResManager::getResScope(string s) const
    {
        vector<Resource*>::const_iterator I = _res.begin();
        for (; I != _res.end(); I++)
            if ((*I)->getName() == s)
                return (*I)->getResScope();
        throw BaseExc("Resource not present","ResManager::getResScope()","resmanager.cpp");
    }

}
