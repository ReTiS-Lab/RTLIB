#ifndef __SRPMANAGER_H__
#define __SRPMANAGER_H__

#include <map>
#include <stack>
#include <plist.hpp>
#include <rttask.hpp>
#include <resmanager.hpp>
#include <resource.hpp>
#include <scheduler.hpp>
#include <rmsched.hpp>
#include <edfsched.hpp>
#include <task.hpp>
#include <kernevt.hpp>

namespace RTSim {

    using namespace MetaSim;
    using namespace std;

    class SRPManagerExc : public BaseExc {
    public:
        SRPManagerExc(const string& m, const string& cl, const string &module = "SRPManager") :
            BaseExc(m,cl,module) {};
    };

    typedef enum{LOCAL_SRP,GLOBAL_SRP} SRPScope_t;

    /**
       \ingroup resman

       This class implements the SRP policy....
    */
    class SRPManager : public ResManager {

    protected:

        map<string,int> ceilingTable;
        stack<int>      SysCeilingIncrement;
        SRPScope_t      scope;
        int             max_ceiling;

        /** Modica Celia - 13/10/2016
        *   Sorts tasks based on the
        *   scheduler policy (RM or EDF)
        */
        void SortTasksBySched(vector<AbsRTTask *> *tasks);

        /**
        *   Locks resource r, sets t as owner then returns true.
        *   Rises an exception if r is already locked, 
        *   (see SRP's policy).
        */
        virtual bool request(AbsRTTask *t, Resource *r, int n=1);

        /**
           Releases the resource.
         */
        virtual void release(AbsRTTask *t, Resource *r, int n=1);

        /** Modica Celia - 12/10/2016
        *   Returns a list of resources' names
        *   used from task t
        */
        vector<string> getUsedRes(Task* t);

    public:

        /**
         * Constructor
         */
        SRPManager(const std::string &n = "", SRPScope_t s = GLOBAL_SRP);

        virtual ~SRPManager();

        /** Modica Celia - 12/10/2016
        *   Computes Tasks' preemption levels
        *   and Resources' Ceiling Table
        */
        virtual void InitializeManager();

        /**
         * Sets the scheduler for this resmanager
         */
//         void setScheduler(Scheduler *s);

        void newRun();

        void endRun();

        /** Modica Celia - 12/10/2016
        *   Returns true if at least one of
        *   the resources linked to the manager
        *   is locked from task t
        */
        bool isLocked(AbsRTTask* t) const;

        /** Modica Celia - 13/10/2016
        *   Returns the next executing task
        *   according to SRP policy
        */
        AbsRTTask* getNewExeTask() const;

        /** Modica Celia - 13/10/2016
        *   Returns Resource Celing, from
        *   pointer or name
        */
        int getResCeil(Resource* r) const;
        int getResCeil(string name) const;
    };
}

#endif
