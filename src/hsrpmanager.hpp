#ifndef __HSRPMANAGER_H__
#define __HSRPMANAGER_H__

#include <srpresmanager.hpp>
#include <server.hpp>


namespace RTSim {

    class Server;

    using namespace MetaSim;
    using namespace std;

    /**
     * Class that exted Base Exeption
     * for launch HSRP exeption
     */
    class HSRPManagerExc : public BaseExc {
    public:
        HSRPManagerExc(const string& m, const string& cl, const string &module = "HSRPManager") :
            BaseExc(m,cl,module) {};
    };

    /**
       \ingroup resman

       This class implements the hierachical stack resource policy (H-SRP)
       by means a resource manager;

       @authors Modica Paolo, Marco Celia
       @version 1.0
    */
    class HSRPManager : public SRPManager {

    public:

        /**
         * The Constructor
         * @param n Name of the manager
         */
        HSRPManager(const std::string &n = "");


        /**
        *   Computes Servers' preemption levels
        *   and Initializes Global Resources'
        *   Ceiling Table.
        *   It also performs all SRP local managers
        *   initializations by calling the InizializeManager()
        *   on each of them.
        *
        *   @warning Call this only after:
        *            - Add all the task to the server
        *            - Add all the global and local resource to the local managers
        *            - Call the setLocalManager() on each server
        *            - Add all the server to the kernel
        *            - Add all the global resources to the hsrp global manager
        *            - Call the setGlobalManager() on the kernel
        */
        virtual void InitializeManager();



        /**
        *   Locks resource r, disable local preemption (rising local server ceiling),
        *   sets t as task owner and s as server owner.
        *   Rises an exception if r is already locked, (see H-SRP's policy).
        *   @param t requesting task
        *   @param s server that contains t
        *   @param r requested resource
        *   @param n number of requested units
        *
        *   @todo Implement the support for n > 1
        *
        *   @return True if t and s locks r, this means that the global resource is
        *           locked in the local and global manager.
        */
        virtual bool request(AbsRTTask *t, AbsRTTask *s, Resource *r, int n);

        /**
           Releases the resource and reduces system and local ceilings.
           @param t releasing task
           @param r relesed resource
           @param n number of released units

           @todo Implement the support for n > 1
         */
        virtual void release(AbsRTTask *t, Resource *r, int n);

    protected:

        /**
        *   Returns a list of resources' names
        *   used from all tasks inside server s.
        *
        *   @param s server
        *   @param r server's local SRPManager
        *
        *   @return Vector of resource name
        */
        vector<string> getUsedRes(Server *s, SRPManager *r);
    };
}


#endif
