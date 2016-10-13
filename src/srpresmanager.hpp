#ifndef __BROESERVER_H__
#define __BROESERVER_H__

#include <map>
#include <stack>
#include <plist.hpp>
#include <rttask.hpp>
#include <resmanager.hpp>
#include <resource.hpp>
#include <scheduler.hpp>
#include <rmsched.hpp>
#include <edfsched.hpp>

namespace RTSim {


    using namespace MetaSim;
    using namespace std;

    class SRPManagerExc : public BaseExc {
    public:
        SRPManagerExc(const string& m, const string& cl, const string &module = "SRPManager") :
            BaseExc(m,cl,module) {};
    };

    /**
       \ingroup resman

       This class implements the SRP policy....
    */
    class SRPManager : public ResManager {

        map<string,int> ceilingTable;
        stack<int> SysCeilingIncrement;

    public:

        /**
         * Constructor
         */
        SRPManager(const std::string &n = "");

        /** Modica Celia - 12/10/2016
        *   Function to compute the
        *   resource ceiling table
        *   for each task-resource,
        *   and the preemption level
        *   of each task
        */
        void InitializeSRPManager(vector<AbsRTTask *> *tasks);

        /**
         * Sets the scheduler for this resmanager
         */
//         void setScheduler(Scheduler *s);

        /**
         * @todo clear the maps!!
         */
        void newRun();

        void endRun();

        /** Modica Celia - 12/10/2016
        *   Return true if a resource
        *   is locked froma task t
        */
        bool isLocked(AbsRTTask* t) const;

        /** Modica Celia - 13/10/2016
        *   Return the task that must
        *   be executing with SRP policy
        */
        AbsRTTask* getNewExeTask() const;

    protected:
        /** Modica Celia - 13/10/2016
        *   Sort tasks based on the
        *   policy of the scheduler type
        */
        void SortTasksBySched(vector<AbsRTTask *> *tasks);

        /**
           Returns true if the resource can be locked, false otherwise
           (in such a case, the task should be blocked)
         */
        virtual bool request(AbsRTTask *t, Resource *r, int n=1);

        /**
           Releases the resource.
         */
        virtual void release(AbsRTTask *t, Resource *r, int n=1);

        /** Modica Celia - 12/10/2016
        *   Return the list of resource
        *   used from the task t
        */
        vector<string> getUsedRes(Task* t);

    };
}

#endif
