#ifndef __BROESERVER_H__
#define __BROESERVER_H__

#include <cbserver.hpp>
#include <cassert>

namespace RTSim 
{
    using namespace MetaSim;

    using namespace std;

    class BROEExc : public ServerExc
    {
    public:
        BROEExc(const string& m, const string& cl) :
            ServerExc(m,cl,"BROEServer") {};
    };

    typedef map<const string, Tick> RHTmap_t;

    /**
	*Modica Celia 07/10/2016
	*
	*
    */
    class BROEServer : public CBServer 
    {

        /** List of all Worst Case Resource Holding Time of a resource for each tasks*/
        RHTmap_t *RHTs;
        ///Param to save the data for block the resource at the recharging time
        AbsRTTask *task_wait_res;
        string res_for_wait;
        int n_res_manager;
    	
    public:
        BROEServer(Tick q, Tick p, Tick d, const string &name, RHTmap_t *R,
                   const string &sched = "FIFOSched");

    protected:
        /**
         * Event of recharging on wait
        */
        GEvent<BROEServer> _recwaitEvt;

        void rechargeAfterWaiting(Event *e);
    public:
    /**
    *   Forwards the request of resource r from task t to
    *   the resource manager. If the resource manager has
    *   not been set, a BROEExc exception is raised.
    */
    virtual bool requestResource(AbsRTTask *t, const string &r, int n=1)
        throw(BROEExc);

    };




}


#endif
