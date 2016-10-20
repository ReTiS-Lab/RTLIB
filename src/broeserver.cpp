#include "broeserver.hpp"

namespace RTSim
{
    using namespace MetaSim;

    BROEServer::BROEServer(Tick q, Tick p, Tick d, const string &name, RHTmap_t *R, const string &sched):
                CBServer(q,p,d,HARD,name,sched),
                RHTs(R),
                _recwaitEvt(this, &BROEServer::rechargeAfterWaiting, Event::_DEFAULT_PRIORITY - 1),
                task_wait_res(nullptr),
                res_for_wait(""),
                n_res_manager(0)
    {
        RHTmap_t::iterator I = RHTs->begin();
        while (I != RHTs->end())
        {
            if (I->second > q)
                throw BROEExc(I->first+" has a rht greater then Server budget","BROEServer::BROEServer()");
            I++;
        }
    }

    bool BROEServer::requestResource(AbsRTTask *t, const string &r, int n)
       throw(BROEExc)
    {
        DBGENTER(_KERNEL_DBG_LEV);

        SRPManager *m = dynamic_cast<SRPManager*>(localResmanager);
        HSRPManager *gm = dynamic_cast<HSRPManager*>(globResManager);
        bool ret = false;
        if (!m)
            throw BROEExc("Broe needs local SRP manager!","BROEServer::requestResource()");
        if (!gm)
            throw BROEExc("Broe needs global HSRP manager!","BROEServer::requestResource()");


        if (gm->find(r))
        {
            RHTmap_t::iterator I;
            I = RHTs->find(taskname(t)+"_"+r);
            if (I == RHTs->end())
                throw BROEExc("No RHT for "+taskname(t)+"_"+r,"BROEServer::requestResource()");


            Tick curr_RHT = I->second;
            Tick curr_budget = get_remaining_budget();

            ///Disable the local preemption of the Server
            /// rising the local Ceiling level to the max
            localResmanager->request(t,r,n);

            /// Budget enough
            if ( curr_budget >= curr_RHT)
            {
                ret = globResManager->request(t,this,r,n);
                if (!ret)
                    kernel->dispatch();
                return ret;
            }

            ///calculating time to recharg
            double alpha = double(Q) / double(P);
            Tick tr = getDeadline() - Tick::floor((double(curr_budget) / alpha) + 0.000000001);

            ///check if waiting before recharge
            if (SIMUL.getTime() < tr)
            {
                status = WAITING;
                _bandExEvt.drop();
                vtime.stop();
                _recwaitEvt.post(tr);
                task_wait_res = t;
                res_for_wait = r;
                n_res_manager = n;
                ret = false;
            }else
            {
                _bandExEvt.drop();
                vtime.stop();
                cap = Q;
                last_time = SIMUL.getTime();
                d=tr+P;
                setAbsDead(d);
                vtime.start((double)P/double(Q));
                _bandExEvt.post(last_time + cap);
                ret = globResManager->request(t,this,r,n);
            }
        } else
            ret = localResmanager->request(t,r,n);
        if (!ret)
            dispatch();
        return ret;
    }

    void BROEServer::rechargeAfterWaiting(Event *e)
    {
        assert(status == WAITING);
        if (task_wait_res == nullptr)
            throw BROEExc("No task waiting for resource","BROEServer::rechargeAfterWaiting(Event *e)");
        cap = Q;
        d=SIMUL.getTime()+P;
        setAbsDead(d);
        status = EXECUTING;
        last_time = SIMUL.getTime();
        vtime.start((double)P/double(Q));
        _bandExEvt.post(SIMUL.getTime() + cap);
        globResManager->request(task_wait_res,this,res_for_wait,n_res_manager);
        dispatch();
        kernel->dispatch();
    }
}
