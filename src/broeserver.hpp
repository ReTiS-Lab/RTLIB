#ifndef __BROESERVER_H__
#define __BROESERVER_H__

#include <cbserver.hpp>
#include <cassert>

namespace RTSim 
{
    using namespace MetaSim;

    using namespace std;

    /**
     * Class that exted Base Exeption
     * for launch BROEExc exeption
     */
    class BROEExc : public ServerExc
    {
    public:
        BROEExc(const string& m, const string& cl) :
            ServerExc(m,cl,"BROEServer") {};
    };

    typedef map<const string, Tick> RHTmap_t;

    /**
       \ingroup resman

       This class implements the BROE Server.
       Based on an Hard CBS Server with local SRP manager and
       global H-SRP manager.

       @authors Modica Paolo, Marco Celia
       @version 1.0
    */
    class BROEServer : public CBServer 
    {

        /// List of all Worst Case Resource Holding Time of a resource for each tasks
        RHTmap_t *RHTs;

        ///Param to save the data for block the resource at the recharging time (requesting task)
        AbsRTTask *task_wait_res;
        ///Param to save the data for block the resource at the recharging time (requested resource's name)
        string res_for_wait;
        ///Param to save the data for block the resource at the recharging time (requested resource's units)
        int n_res_manager;
    	
    public:
        /**
        The Constractor
        @param q Budget
        @param p Period
        @param d Relative Deadline
        @param name Name of the Server
        @param R Table of all Tasks' resources holding times
        @param sched Local scheduler policy

        @warning Broe server supports only EDF and RM scheduler
        @todo Implement support for other scheduler policy
        */
        BROEServer(Tick q, Tick p, Tick d, const string &name, RHTmap_t *R,
                   const string &sched = "EDFSched");

    protected:

        /// Recharging after wait event
        GEvent<BROEServer> _recwaitEvt;

        /**
          Recharging after wait event (_recwaitEvt) handler
          @param e Event
        */
        void rechargeAfterWaiting(Event *e);
    public:
    /**
    *   Forwards the request of resource r from task t to
    *   the global or local resource manager.
    *   If resource managers were
    *   not been set, a BROEExc exception is raised.
    *
    *   @param t requesting task
    *   @param r requested resource's name
    *   @param n requested resource's units
    *
    *   @return True if the local ora global resource has been locked
    *
    *   @todo implements the support for n > 1
    */
    virtual bool requestResource(AbsRTTask *t, const string &r, int n=1)
        throw(BROEExc);

    };




}


#endif
