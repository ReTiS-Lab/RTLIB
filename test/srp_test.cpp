#include "catch.hpp"
#include <rttask.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>
#include <srpresmanager.hpp>


using namespace MetaSim;
using namespace RTSim;
using namespace std;

#define REQUIRE_STEP1(i, sched, sysCeil, et1, et2, et3, et4) \
        SIMUL.run_to(i);                                    \
        REQUIRE(sched.getSysCeiling() == sysCeil);          \
        REQUIRE(t1.getExecTime() == et1);                   \
        REQUIRE(t2.getExecTime() == et2);                   \
        REQUIRE(t3.getExecTime() == et3);                   \
        REQUIRE(t4.getExecTime() == et4)

#define REQUIRE_STEP2(i, sched, sysCeil, et1, et2, et3)     \
        SIMUL.run_to(i);                                    \
        REQUIRE(sched.getSysCeiling() == sysCeil);          \
        REQUIRE(t1.getExecTime() == et1);                   \
        REQUIRE(t2.getExecTime() == et2);                   \
        REQUIRE(t3.getExecTime() == et3)

TEST_CASE("SRP test")
{
    try{
        TextTrace ttrace("trace_SRP_Test_1.txt");

       // create the scheduler and the kernel
        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);

        /*Abilitazione debugging*/
        ofstream f("debug_srp.txt");
        SIMUL.dbg.setStream(f);
        SIMUL.dbg.enable("SRPManager");

        SRPManager rm("");
        rm.addResource("R1");
        rm.addResource("R2");
        rm.addResource("R3");
        kern.setResManager(&rm);        

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

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);
        ttrace.attachToTask(&t4);

        kern.addTask(t1, "");
        kern.addTask(t2, "");
        kern.addTask(t3, "");
        kern.addTask(t4, "");

        rm.InitializeManager();

        int vetCeil[] = {0, 2, 2, 2, 2, 3, 3, 2, 3, 2, 2, 2,0,2,2,0,0,0};
        int vetex1[] = {0,0,0,0,0,0,0,0,1,2,2,2,2,2,2,2,2,2};
        int vetex2[] = {0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
        int vetex3[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4};
        int vetex4[] = {0,1,2,3,4,5,6,7,7,7,7,8,9,9,9,9,9,10};

        SIMUL.initSingleRun();
        
        REQUIRE((sched.getTaskModel(&t1))->getPLevel() == 3);
        REQUIRE((sched.getTaskModel(&t2))->getPLevel() == 4);
        REQUIRE((sched.getTaskModel(&t3))->getPLevel() == 2);
        REQUIRE((sched.getTaskModel(&t4))->getPLevel() == 1);
        REQUIRE(rm.getResCeil("R1") == 3);
        REQUIRE(rm.getResCeil("R2") == 2);
        REQUIRE(rm.getResCeil("R3") == 2);

        for (int i=0; i<18; i++)
        {
            REQUIRE_STEP1(i,sched,vetCeil[i],vetex1[i],vetex2[i],vetex3[i],vetex4[i]);
        }
                SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }
}

TEST_CASE("SRP: test su preemption level e System Ceiling")
{
    try{
        TextTrace ttrace("trace_SRP_Test_2.txt");

        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);

        SRPManager rm("");
        rm.addResource("R1");
        rm.addResource("R2");
        rm.addResource("R3");
        kern.setResManager(&rm);

        PeriodicTask t1(20, 8, 4, "T1");
        t1.insertCode("fixed(1);wait(R3);delay(1);wait(R1);delay(1);signal(R1);delay(1);signal(R3);");
        t1.setAbort(false);

        PeriodicTask t2(20, 17, 2, "T2");
        t2.insertCode("fixed(1);wait(R3);delay(1);wait(R2);delay(1);signal(R2);signal(R3);fixed(1);");
        t2.setAbort(false);

        PeriodicTask t3(20, 20, 0, "T3");
        t3.insertCode("fixed(1);wait(R2);delay(1);wait(R1);delay(3);signal(R1);delay(1);signal(R2);fixed(1);wait(R3);delay(1);signal(R3);");
        t3.setAbort(false);

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);

        kern.addTask(t1, "");
        kern.addTask(t2, "");
        kern.addTask(t3, "");

        rm.InitializeManager();

        int vetCeil[] = {0, 2, 3, 3, 3, 2, 3, 3, 3, 2, 0, 3, 3,0,0,3,0,0};
        int vetex1[] = {0,0,0,0,0,0,1,2,3,4,4,4,4,4,4,4,4,4};
        int vetex2[] = {0,0,0,0,0,0,0,0,0,0,0,1,2,3,4,4,4,4};
        int vetex3[] = {0,1,2,3,4,5,5,5,5,5,6,6,6,6,6,7,8,8};

        SIMUL.initSingleRun();
        /**
        *   check if all preemption levels were correctly assigned
        */
        REQUIRE((sched.getTaskModel(&t1))->getPLevel() == 3);
        REQUIRE((sched.getTaskModel(&t2))->getPLevel() == 2);
        REQUIRE((sched.getTaskModel(&t3))->getPLevel() == 1);
        REQUIRE(rm.getResCeil("R1") == 3);
        REQUIRE(rm.getResCeil("R2") == 2);
        REQUIRE(rm.getResCeil("R3") == 3);


        for (int i=0; i<18; i++)
        {
            REQUIRE_STEP2(i,sched,vetCeil[i],vetex1[i],vetex2[i],vetex3[i]);
        }

        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }
}
