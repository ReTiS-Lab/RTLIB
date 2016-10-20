#include "catch.hpp"
#include <rttask.hpp>
#include <broeserver.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <hsrpmanager.hpp>
#include <texttrace.hpp>

using namespace MetaSim;
using namespace RTSim;

#define REQUIRE_STEP(sysCeil, sc1,sc2,sc3,sb1,sb2,sb3,et1,et2,et3)      \
        REQUIRE(sched.getSysCeiling() == sysCeil);                      \
        REQUIRE(serv1.getServCeiling() == sc1);                         \
        REQUIRE(serv2.getServCeiling() == sc2);                         \
        REQUIRE(serv3.getServCeiling() == sc3);                         \
        REQUIRE(t12.getExecTime() == et1);                              \
        REQUIRE(t21.getExecTime() == et2);                              \
        REQUIRE(t31.getExecTime() == et3);                              \
        REQUIRE(serv1.get_remaining_budget() == sb1);                   \
        REQUIRE(serv2.get_remaining_budget() == sb2);                   \
        REQUIRE(serv3.get_remaining_budget() == sb3)                    \
    
TEST_CASE("BROE algorithm: test Bounded-delay with wait and preemption")
{   
    
   try{
        TextTrace ttrace("Trace_Broe_cpp_1.txt");
  
		// create the scheduler and the kernel
        EDFScheduler sched;
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

        PeriodicTask t12(17,3,3,"T12");
        t12.insertCode("wait(G1);delay(1);signal(G1);");
        t12.setAbort(false);

        PeriodicTask t21(17,6,7,"T21");
        t21.insertCode("fixed(2);");
        t21.setAbort(false);

        PeriodicTask t31(17,13,0,"T31");
        t31.insertCode("fixed(2);wait(G1);delay(2);signal(G1);fixed(1);");
        t31.setAbort(false);

        RHTmap_t map1;
        RHTmap_t map2;
        RHTmap_t map3;

        map1["T12_G1"] = 1;

        map3["T31_G1"] = 2;


        BROEServer serv1(1, 3,3, "Server1", &map1, "EDFSched");
        serv1.setLocalResManager(&rm1);
        serv1.setGlobalResManager(&hm);

        serv1.addTask(t12, "");

        BROEServer serv2(2,6,6, "Server2", &map2,"EDFSched");
        serv2.setLocalResManager(&rm2);
        serv2.setGlobalResManager(&hm);

        serv2.addTask(t21, "");

        BROEServer serv3(3, 9,9, "Server3", &map3,"EDFSched");
        serv3.setLocalResManager(&rm3);
        serv3.setGlobalResManager(&hm);

        serv3.addTask(t31, "");

        kern.setResManager(&hm);

        kern.addTask(serv1, "");
        kern.addTask(serv2, "");
        kern.addTask(serv3, "");

        hm.InitializeManager();

        ttrace.attachToTask(&t12);
        ttrace.attachToTask(&t21);
        ttrace.attachToTask(&t31);
        
        SIMUL.initSingleRun();
        SIMUL.run_to(1);
        REQUIRE_STEP(0,0,0,0,0,0,2,0,0,1);
        SIMUL.run_to(2);
        REQUIRE_STEP(0,0,0,1,0,0,1,0,0,2);
        SIMUL.run_to(3);
        REQUIRE_STEP(3,1,0,1,1,0,1,0,0,2);
        SIMUL.run_to(4);
        REQUIRE_STEP(0,0,0,1,0,0,1,1,0,2);
        SIMUL.run_to(5);
        REQUIRE_STEP(0,0,0,1,0,0,1,1,0,2);
        SIMUL.run_to(6);
        REQUIRE_STEP(3,0,0,1,1,0,3,1,0,2);
        SIMUL.run_to(7);
        REQUIRE_STEP(3,0,0,1,1,2,2,1,0,3);
        SIMUL.run_to(8);
        REQUIRE_STEP(0,0,0,0,1,2,1,1,0,4);
        SIMUL.run_to(9);
        REQUIRE_STEP(0,0,0,0,1,1,1,1,1,4);
        SIMUL.run_to(10);
        REQUIRE_STEP(0,0,0,0,1,0,1,1,2,4);
        SIMUL.run_to(11);
        REQUIRE_STEP(0,0,0,0,1,0,0,1,2,5);

        SIMUL.endSingleRun();
        
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }

}

#define REQUIRE_STEP2(sysCeil, sc1,sc2,sb1,sb2,et1,et2)                 \
        REQUIRE(sched.getSysCeiling() == sysCeil);                      \
        REQUIRE(serv1.getServCeiling() == sc1);                         \
        REQUIRE(serv2.getServCeiling() == sc2);                         \
        REQUIRE(t11.getExecTime() == et1);                              \
        REQUIRE(t21.getExecTime() == et2);                              \
        REQUIRE(serv1.get_remaining_budget() == sb1);                   \
        REQUIRE(serv2.get_remaining_budget() == sb2)                    \

TEST_CASE("BROE algorithm: test with enough budget")
{

   try{
        TextTrace ttrace("Trace_Broe_cpp_2.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        Resource G1("G1");

        HSRPManager hm("HSRPMan");
        hm.addResource(&G1);

        SRPManager rm1("SRPLocalManagerS1",LOCAL_SRP);
        rm1.addResource(&G1);

        SRPManager rm2("SRPLocalManagerS2",LOCAL_SRP);
        rm2.addResource(&G1);

        PeriodicTask t11(15,8,1,"T11");
        t11.insertCode("fixed(1);wait(G1);delay(1);signal(G1);");
        t11.setAbort(false);

        PeriodicTask t21(15,6,0,"T21");
        t21.insertCode("wait(G1);delay(3);signal(G1);fixed(1);");
        t21.setAbort(false);

        RHTmap_t map1;
        RHTmap_t map2;

        map1["T11_G1"] = 1;

        map2["T21_G1"] = 2;


        BROEServer serv1(5, 10,10, "Server1", &map1, "EDFSched");
        serv1.setLocalResManager(&rm1);
        serv1.setGlobalResManager(&hm);

        serv1.addTask(t11, "");

        BROEServer serv2(6,12,12, "Server2", &map2,"EDFSched");
        serv2.setLocalResManager(&rm2);
        serv2.setGlobalResManager(&hm);

        serv2.addTask(t21, "");

        kern.setResManager(&hm);

        kern.addTask(serv1, "");
        kern.addTask(serv2, "");

        hm.InitializeManager();

        ttrace.attachToTask(&t11);
        ttrace.attachToTask(&t21);

        SIMUL.initSingleRun();
        SIMUL.run_to(0);
        REQUIRE_STEP2(2, 0, 1,0,6,0,0);
        SIMUL.run_to(1);
        REQUIRE_STEP2(2, 0, 1,5,5,0,1);
        SIMUL.run_to(2);
        REQUIRE_STEP2(2, 0, 1,5,4,0,2);
        SIMUL.run_to(3);
        REQUIRE_STEP2(0, 0, 0,5,3,0,3);
        SIMUL.run_to(4);
        REQUIRE_STEP2(2, 1, 0,4,3,1,3);
        SIMUL.run_to(5);
        REQUIRE_STEP2(0, 0, 0,3,3,2,3);
        SIMUL.run_to(6);
        REQUIRE_STEP2(0, 0, 0,3,2,2,4);
        SIMUL.run_to(7);
        REQUIRE_STEP2(0, 0, 0,3,2,2,4);

        SIMUL.endSingleRun();

    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }

}

#define REQUIRE_STEP3(sysCeil, sc1,sc2,sb1,sb2,et11, et12, et21)        \
        REQUIRE(sched.getSysCeiling() == sysCeil);                      \
        REQUIRE(serv1.getServCeiling() == sc1);                         \
        REQUIRE(serv2.getServCeiling() == sc2);                         \
        REQUIRE(t11.getExecTime() == et11);                             \
        REQUIRE(t12.getExecTime() == et12);                             \
        REQUIRE(t21.getExecTime() == et21);                             \
        REQUIRE(serv1.get_remaining_budget() == sb1);                   \
        REQUIRE(serv2.get_remaining_budget() == sb2)                    \

TEST_CASE("BROE algorithm: test Bounded-delay without wait and preemption")
{

   try{
        TextTrace ttrace("Trace_Broe_cpp_3.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        Resource G1("G1");

        HSRPManager hm("HSRPMan");
        hm.addResource(&G1);

        SRPManager rm1("SRPLocalManagerS1",LOCAL_SRP);
        rm1.addResource(&G1);

        SRPManager rm2("SRPLocalManagerS2",LOCAL_SRP);
        rm2.addResource(&G1);

        PeriodicTask t11(17,6,0,"T11");
        t11.insertCode("fixed(3);");
        t11.setAbort(false);

        PeriodicTask t12(17,3,7,"T12");
        t12.insertCode("wait(G1);delay(1);signal(G1);");
        t12.setAbort(false);

        PeriodicTask t21(17,14,0,"T21");
        t21.insertCode("fixed(3);wait(G1);delay(2);signal(G1);fixed(2);");
        t21.setAbort(false);

        RHTmap_t map1;
        RHTmap_t map2;

        map1["T12_G1"] = 1;

        map2["T21_G1"] = 2;


        BROEServer serv1(3, 6,6, "Server1", &map1, "EDFSched");
        serv1.setLocalResManager(&rm1);
        serv1.setGlobalResManager(&hm);

        serv1.addTask(t11, "");
        serv1.addTask(t12,"");

        BROEServer serv2(4,8,8, "Server2", &map2,"EDFSched");
        serv2.setLocalResManager(&rm2);
        serv2.setGlobalResManager(&hm);

        serv2.addTask(t21, "");

        kern.setResManager(&hm);

        kern.addTask(serv1, "");
        kern.addTask(serv2, "");

        hm.InitializeManager();

        ttrace.attachToTask(&t11);
        ttrace.attachToTask(&t12);
        ttrace.attachToTask(&t21);

        SIMUL.initSingleRun();
        SIMUL.run_to(0);
        REQUIRE_STEP3(0, 0, 0, 3, 4, 0, 0, 0);
        SIMUL.run_to(1);
        REQUIRE_STEP3(0, 0, 0, 2, 4, 1, 0, 0);
        SIMUL.run_to(2);
        REQUIRE_STEP3(0, 0, 0, 1, 4, 2, 0, 0);
        SIMUL.run_to(3);
        REQUIRE_STEP3(0, 0, 0, 0, 4, 3, 0, 0);
        SIMUL.run_to(4);
        REQUIRE_STEP3(0, 0, 0, 0, 3, 3, 0, 1);
        SIMUL.run_to(5);
        REQUIRE_STEP3(0, 0, 0, 0, 2, 3, 0, 2);
        SIMUL.run_to(6);
        REQUIRE_STEP3(2, 0, 1, 3, 4, 3, 0, 3);
        SIMUL.run_to(7);
        REQUIRE_STEP3(2, 0, 1, 3, 3, 3, 0, 4);
        SIMUL.run_to(8);
        REQUIRE_STEP3(2, 2, 0, 3, 2, 3, 0, 5);
        SIMUL.run_to(9);
        REQUIRE_STEP3(0, 0, 0, 2, 2, 3, 1, 5);
        SIMUL.run_to(10);
        REQUIRE_STEP3(0, 0, 0, 2, 1, 3, 1, 6);
        SIMUL.run_to(11);
        REQUIRE_STEP3(0, 0, 0, 2, 0, 3, 1, 7);
        SIMUL.run_to(16);
        REQUIRE_STEP3(0, 0, 0, 2, 4, 3, 1, 7);
        SIMUL.endSingleRun();

    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }

}



