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
    /**
     * Class that exted Base Exeption
     * for launch SRP exeption
     */
    class SRPManagerExc : public BaseExc {
    public:
        SRPManagerExc(const string& m, const string& cl, const string &module = "SRPManager") :
            BaseExc(m,cl,module) {};
    };

    typedef enum{LOCAL_SRP,GLOBAL_SRP} SRPScope_t;

    /**
       \ingroup resman

       This class implements the stack resource policy (SRP)
       by means a resource manager;

       @authors Modica Paolo, Marco Celia
       @version 1.0


    */
    class SRPManager : public ResManager {

    protected:

        /// Contains the SRP ceiling table
        map<string,int> ceilingTable;
        /// Contains the System Ceiling ingrements
        stack<int>      SysCeilingIncrement;
        /// Contains the SRPManager scope;
        /// Use LOCAL_SRP only if the manager is used inside
        /// a server as local manager, GLOBAL_SRP in all other cases;
        SRPScope_t      scope;
        /// Contains the max value of the resources' ceilings
        int             max_ceiling;

        /**
        *   Sorts tasks based on the
        *   scheduler policy (RM or EDF)
        *   @param tasks task set as vector
        */
        void SortTasksBySched(vector<AbsRTTask *> *tasks);

        /**
        *   Locks resource r, rises system ceiling and sets t as owner then returns true.
        *   Rises an exception if r is already locked, 
        *   (see SRP's policy).
        *   @param t requesting task
        *   @param r requested resource
        *   @param n number of requested units
        *
        *   @todo Implement the support for n > 1
        *
        *   @return True if t locks r
        */
        virtual bool request(AbsRTTask *t, Resource *r, int n=1);

        /**
           Releases the resource and reduces system ceiling.
           @param t releasing task
           @param r relesed resource
           @param n number of released units

           @todo Implement the support for n > 1
         */
        virtual void release(AbsRTTask *t, Resource *r, int n=1);

        /**
        *   Returns a list of resources' names
        *   used from task t
        *
        *   @param t task
        *
        *   @return Vector of resource name
        */
        vector<string> getUsedRes(Task* t);

    public:

        /**
         * The Constructor
         * @param n Name of the manager
         * @param s Scope of the manager
         */
        SRPManager(const std::string &n = "", SRPScope_t s = GLOBAL_SRP);

        /**
          The Destructor
        */
        virtual ~SRPManager();

        /**
        *   Computes Tasks' preemption levels
        *   and Initializes Resources'
        *   Ceiling Table.
        *
        *   @warning Call this only after:
        *            - Add all the task to the kernel or server
        *            - Call the setResMan() on the kernel or the setLocalResManager() on the server
        *            - Add all the resources to the manager
        */
        virtual void InitializeManager();


        void newRun();

        void endRun();

        /**
        *   Check if at least one of
        *   the resources linked to the manager
        *   is locked from task t
        *   @param t task to be checked
        *
        *   @return True if t locks at least one resource
        */
        bool isLocked(AbsRTTask* t) const;

        /**
        *   Returns the next executing task
        *   according to SRP policy
        *
        *   @return Pointer to next executing task or nullptr
        */
        AbsRTTask* getNewExeTask() const;

        /**
         *  Return the resource ceiling
         *  @param  r resource (pointer)
         *  @return r's ceiling
        */
        int getResCeil(Resource* r) const;
        /**
         *  Return the resource ceiling
         *  @param  name resource's name
         *  @return r's ceiling
        */
        int getResCeil(string name) const;
    };
}

#endif
