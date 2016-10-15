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
#ifndef __RESMANAGER_HPP__
#define __RESMANAGER_HPP__

#include <map>
#include <set>
#include <string>

#include <entity.hpp>
#include <scheduler.hpp>
#include <resource.hpp>

#define _RESMAN_DBG_LEV  "ResMan"

namespace RTSim {

    class AbsRTTask;
    class Resource;
    class AbsKernel;

//   using namespace std;
    using namespace MetaSim;

    /** 
        \ingroup resman
        Generic resource manager. A specific resource manager should be derived 
        from this class.
      
        @todo: add simple documentation for this class.
      
        @see Resource
    */
    class ResManager : public Entity {
        friend class RTKernel;
        friend class Server;
    public:
        /** Constructor of ResManager */
        ResManager(const std::string &n = "");
  
        virtual ~ResManager();

        /**
           Adds the resource to the set of resources managed by the Resource
           Manager.  should check if the resource is already present in such
           set. Resource is allocated from the Manager that will be the one
           who have to destroy it
       
           @param name resource name;
           @param n number of unit (for supporting multi-unit resources), 
           by default is 1.
        */
        virtual void addResource(const std::string &name, int n=1, res_scope_t s = GLOBAL_RES);

        /**  
         * Adds the resource to the set of resources managed by the 
         * Resource Manager. 
         * WARNING: r was already allocated somewhere else, 
         * so the manager will not destroy it.
         */
        virtual void addResource(Resource *r);

        /**
         * Function called by a task instr (the WaitInstr) to perform an
         * access request to a specific resource. That access could be
         * granted or the task could be suspended. It returns true if the
         * resource has been locked succesfully; it returns false if the
         * task has been blocked.

         * @todo: Should check if the resource is among the ones handled by
         * this manager

         * @param t task
         * @param name resource name
         * @param n number of units (by default is 1).
         */
        bool request(AbsRTTask *t, const std::string &name, int n=1);

        /**
         * Function called by a task instr to perform the release of a
         * specific resource. The consequence of this call could be the
         * reactivation of one or more suspended tasks.  
   
         * @todo Should check if the resource is among the ones handled by
         * this manager, and was locked before

         * @param t task
         * @param name resource name
         * @param n number of units (by default is 1).
         */
        void release(AbsRTTask *t, const std::string &name, int n=1);

        /*
         * Function called to specify that task t uses the resource called
         * name. This function is not necessary in simple resource managers,
         * like FCFSResManager. It is useful for PCRManager, for computing
         * the ceilings! 

         * @todo Maybe, it should be moved in PCR, then!!
         *
         * @see PCRManager
         */
        // virtual void addUser(AbsRTTask *t, const std::string &name, int n=1) = 0;

        /** Modica Celia - 14/10/2016
         *  Return true if the resource
         *  is in the queue of the manager
         *  from pointer or name
         */
        bool find(Resource *r) const;
        bool find(string s) const;

        /** Modica Celia - 15/10/2016
         * Return the scope of the
         * Resource passed by param
         */
        res_scope_t getResScope(Resource *r) const;
        res_scope_t getResScope(string s) const;

    protected:

        AbsKernel *_kernel;
        Scheduler *_sched;

        /**
         * Set the kernel and the associated scheduler. Do not call
         * this function from your main program!! This function is
         * automatically called by the RTKernel::setResManager().
         *
         * @see RTKernel
         */
        void setKernel(AbsKernel *k, Scheduler *s);

        /**
        *  List of all resources managed from the manager
        */
        std::vector<Resource *> _res;

        /**
        *  List of all resources directly allocated from the manager,
        *  for these it is in charge of their deallocation
        */
        std::vector<Resource *> _allocated;;

        virtual bool request(AbsRTTask *t, Resource *r, int n=1) = 0;
        virtual void release(AbsRTTask *t, Resource *r, int n=1) = 0;
    };
} // namespace RTSim 

#endif
