#include "catch.hpp"
#include <rttask.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>
#include <srpresmanager.hpp>
#include <cbserver.hpp>
#include <fstream>

using namespace MetaSim;
using namespace RTSim;
using namespace std;

#define REQUIRE_STEP1(i, sysCeil, et1, et2, et3, et4, bud)   \
        SIMUL.run_to(i);                                     \
        REQUIRE(serv.getServCeiling() == sysCeil);           \
        REQUIRE(t1.getExecTime() == et1);                    \
        REQUIRE(t2.getExecTime() == et2);                    \
        REQUIRE(t3.getExecTime() == et3);                    \
        REQUIRE(t4.getExecTime() == et4);                    \
        REQUIRE(serv.get_remaining_budget() == bud)

TEST_CASE("SRP under CBS server test")
{
    try{
        TextTrace ttrace("Trace_SRP_on_CBS.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);

        SRPManager rm("");
        rm.addResource("R1");
        rm.addResource("R2");
        rm.addResource("R3");
        //kern.setResManager(&rm);

        PeriodicTask t1(20, 5, 6, "T1");
        t1.insertCode("fixed(1);wait(R1);delay(1);signal(R1);");
        t1.setAbort(false);

        PeriodicTask t2(20, 4, 8, "T2");
        t2.insertCode("fixed(1);");
        t2.setAbort(false);

        PeriodicTask t3(20, 13, 4, "T3");
        t3.insertCode("fixed(1);wait(R2);fixed(1);signal(R2);wait(R3);fixed(1);signal(R3);fixed(1);");
        t3.setAbort(false);

        PeriodicTask t4(20, 19, 0, "T4");
        t4.insertCode("fixed(1);wait(R2);fixed(2);wait(R3);fixed(2);wait(R1);fixed(2);signal(R1);fixed(1);signal(R3);fixed(1);signal(R2);fixed(1);");
        t4.setAbort(false);

        CBServer serv(20, 20,20,CBServer::HARD, "Server1", "EDFSched");
        serv.setLocalResManager(&rm);

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);
        ttrace.attachToTask(&t4);

        serv.addTask(t1, "");
        serv.addTask(t2, "");
        serv.addTask(t3, "");
        serv.addTask(t4, "");

        kern.addTask(serv, "");

        rm.InitializeManager();

        int vetCeil[] = {0, 2, 2, 2, 2, 3, 3, 2, 3, 2, 2, 2,0,2,2,0,0,0};
        int vetex1[] = {0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2};
        int vetex2[] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
        int vetex3[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4};
        int vetex4[] = {0,1,2,3,4,5,6,7,7,7,7,8,9,9,9,9,9,10};

        SIMUL.initSingleRun();

        REQUIRE(rm.getResCeil("R1") == 3);
        REQUIRE(rm.getResCeil("R2") == 2);
        REQUIRE(rm.getResCeil("R3") == 2);

        for (int i=0; i<18; i++)
        {
            REQUIRE_STEP1(i,vetCeil[i],vetex1[i],vetex2[i],vetex3[i],vetex4[i],20-i);
        }

        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }
}
