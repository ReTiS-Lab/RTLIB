#ifndef __HSRPMANAGER_H__
#define __HSRPMANAGER_H__

#include <srpresmanager.hpp>
#include <server.hpp>


namespace RTSim {

    class Server;

    using namespace MetaSim;
    using namespace std;

    class HSRPManagerExc : public BaseExc {
    public:
        HSRPManagerExc(const string& m, const string& cl, const string &module = "HSRPManager") :
            BaseExc(m,cl,module) {};
    };

    /**
       \ingroup resman

       This class implements the H-SRP resource manager
    */
    class HSRPManager : public SRPManager {

    public:

        /**
         * Constructor
         */
        HSRPManager(const std::string &n = "");

        /** Modica Celia - 12/10/2016
        *   Computes Tasks' preemption levels
        *   and Resources' Ceiling Table
        */
        virtual void InitializeManager();


        /** Modica Celia - 18/10/2016
        *   Redefinition for global manager
        */
        virtual bool request(AbsRTTask *t, AbsRTTask *s, Resource *r, int n);

        /** Modica Celia - 18/10/2016
         *   Redefinition for global manager
         */
        virtual void release(AbsRTTask *t, Resource *r, int n);

    protected:

        /** Modica Celia - 12/10/2016
        *   Returns a list of resources' names
        *   used from task t
        */
        vector<string> getUsedRes(Server *s, SRPManager *r);
    };
}


#endif
