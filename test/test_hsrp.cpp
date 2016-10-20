#include "catch.hpp"
#include <rttask.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>
#include <srpresmanager.hpp>
#include <cbserver.hpp>
#include <hsrpmanager.hpp>


using namespace MetaSim;
using namespace RTSim;
using namespace std;

#define REQUIRE_STEP(sysCeil, sc1,sc2,sc3,sb1,sb2,sb3,et11,et12,et21,et31,et32)     \
        REQUIRE(sched.getSysCeiling() == sysCeil);                                  \
        REQUIRE(serv1.getServCeiling() == sc1);                                     \
        REQUIRE(serv2.getServCeiling() == sc2);                                     \
        REQUIRE(serv3.getServCeiling() == sc3);                                     \
        REQUIRE(t11.getExecTime() == et11);                                         \
        REQUIRE(t12.getExecTime() == et12);                                         \
        REQUIRE(t21.getExecTime() == et21);                                         \
        REQUIRE(t31.getExecTime() == et31);                                         \
        REQUIRE(t32.getExecTime() == et32);                                         \
        REQUIRE(serv1.get_remaining_budget() == sb1);                               \
        REQUIRE(serv2.get_remaining_budget() == sb2);                               \
        REQUIRE(serv3.get_remaining_budget() == sb3)                                \


TEST_CASE("H-SRP under CBS servers test")
{
    try{
        TextTrace ttrace("trace_HSRP_Under_CBS_server.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);
        Resource L1("L1", 1, LOCAL_RES);
        Resource G1("G1");

        HSRPManager hm("HSRPMan");
        hm.addResource(&G1);



        SRPManager rm1("SRPLocalManagerS1",LOCAL_SRP);
        rm1.addResource(&L1);
        rm1.addResource(&G1);

        SRPManager rm2("SRPLocalManagerS2",LOCAL_SRP);

        SRPManager rm3("SRPLocalManagerS3",LOCAL_SRP);
        rm3.addResource(&G1);




        PeriodicTask t11(20, 5, 5, "T11");
        t11.insertCode("fixed(1);wait(L1);delay(1);signal(L1);");
        t11.setAbort(false);

        PeriodicTask t12(20, 8, 3, "T12");
        t12.insertCode("wait(L1);delay(1);wait(G1);delay(2);signal(G1);signal(L1);");
        t12.setAbort(false);


        PeriodicTask t21(20, 11, 2, "T21");
        t21.insertCode("fixed(2);");
        t21.setAbort(false);


        PeriodicTask t31(20, 20, 0, "T31");
        t31.insertCode("fixed(1);wait(G1);delay(3);signal(G1);");
        t31.setAbort(false);

        PeriodicTask t32(25, 25, 1, "T32");
        t32.insertCode("fixed(1);");
        t32.setAbort(false);

        CBServer serv1(7, 16,16,CBServer::HARD, "Server1", "EDFSched");
        serv1.setLocalResManager(&rm1);
        serv1.setGlobalResManager(&hm);

        serv1.addTask(t11, "");
        serv1.addTask(t12, "");

        CBServer serv2(4, 18,18,CBServer::HARD, "Server2", "EDFSched");
        serv2.setLocalResManager(&rm2);
        serv2.setGlobalResManager(&hm);

        serv2.addTask(t21, "");

        CBServer serv3(6, 21,21,CBServer::HARD, "Server3", "EDFSched");
        serv3.setLocalResManager(&rm3);
        serv3.setGlobalResManager(&hm);

        serv3.addTask(t31, "");
        serv3.addTask(t32, "");

        kern.setResManager(&hm);

        ttrace.attachToTask(&t11);
        ttrace.attachToTask(&t12);
        ttrace.attachToTask(&t21);
        ttrace.attachToTask(&t31);
        ttrace.attachToTask(&t32);

        kern.addTask(serv1, "");
        kern.addTask(serv2, "");
        kern.addTask(serv3, "");

        hm.InitializeManager();

        SIMUL.initSingleRun();
        SIMUL.run_to(0);
        REQUIRE_STEP(0, 0, 0, 0, 0, 0, 6, 0, 0, 0, 0, 0);
        SIMUL.run_to(1);
        REQUIRE_STEP(3, 0, 0, 2, 0, 0, 5, 0, 0, 0, 1, 0);
        SIMUL.run_to(2);
        REQUIRE_STEP(3, 0, 0, 2, 0, 4, 4, 0, 0, 0, 2, 0);
        SIMUL.run_to(3);
        REQUIRE_STEP(3, 0, 0, 2, 7, 4, 3, 0, 0, 0, 3, 0);
        SIMUL.run_to(4);
        REQUIRE_STEP(0, 2, 0, 0, 7, 4, 2, 0, 0, 0, 4, 0);
        SIMUL.run_to(5);
        REQUIRE_STEP(3, 2, 0, 0, 6, 4, 2, 0, 1, 0, 4, 0);
        SIMUL.run_to(6);
        REQUIRE_STEP(3, 2, 0, 0, 5, 4, 2, 0, 2, 0, 4, 0);
        SIMUL.run_to(7);
        REQUIRE_STEP(0, 0, 0, 0, 4, 4, 2, 0, 3, 0, 4, 0);
        SIMUL.run_to(8);
        REQUIRE_STEP(0, 2, 0, 0, 3, 4, 2, 1, 3, 0, 4, 0);
        SIMUL.run_to(9);
        REQUIRE_STEP(0, 0, 0, 0, 2, 4, 2, 2, 3, 0, 4, 0);
        SIMUL.run_to(10);
        REQUIRE_STEP(0, 0, 0, 0, 2, 3, 2, 2, 3, 1, 4, 0);
        SIMUL.run_to(11);
        REQUIRE_STEP(0, 0, 0, 0, 2, 2, 2, 2, 3, 2, 4, 0);
        SIMUL.run_to(12);
        REQUIRE_STEP(0, 0, 0, 0, 2, 2, 1, 2, 3, 2, 4, 1);

        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }
}
