#include "catch.hpp"
#include <rttask.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>
#include <srpresmanager.hpp>


using namespace MetaSim;
using namespace RTSim;
using namespace std;


TEST_CASE("SRP test")
{
    try{
        TextTrace ttrace("trace0.txt");

       // create the scheduler and the kernel
        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);

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

        vector<AbsRTTask*> task_set;
        task_set.push_back((AbsRTTask*)&t4);
        task_set.push_back((AbsRTTask*)&t3);
        task_set.push_back((AbsRTTask*)&t1);
        task_set.push_back((AbsRTTask*)&t2);
        rm.InitializeSRPManager(&task_set);

        SIMUL.initSingleRun();
        
        REQUIRE((sched.getTaskModel(&t1))->getPLevel() == 3);
        REQUIRE((sched.getTaskModel(&t2))->getPLevel() == 4);
        REQUIRE((sched.getTaskModel(&t3))->getPLevel() == 2);
        REQUIRE((sched.getTaskModel(&t4))->getPLevel() == 1);
        REQUIRE(rm.getResCeil("R1") == 3);
        REQUIRE(rm.getResCeil("R2") == 2);
        REQUIRE(rm.getResCeil("R3") == 2);

        SIMUL.run_to(8);
        SIMUL.run_to(9);
        REQUIRE(sched.getSysCeiling()==2);

        SIMUL.run_to(10);
        SIMUL.run_to(11);
        SIMUL.run_to(12);
        SIMUL.run_to(13);
        SIMUL.run_to(20);

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
        TextTrace ttrace("trace1.txt");

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

        vector<AbsRTTask*> task_set;
        task_set.push_back((AbsRTTask*)&t2);
        task_set.push_back((AbsRTTask*)&t3);
        task_set.push_back((AbsRTTask*)&t1);
        rm.InitializeSRPManager(&task_set);

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

        SIMUL.run_to(2);
        /**
        *   T3 already performed wait on R1
        */
        REQUIRE(sched.getSysCeiling() == 3);
        REQUIRE(t3.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 0);

        SIMUL.run_to(3);
        REQUIRE(t2.getExecTime() == 0);
        REQUIRE(t3.getExecTime() == 3);

        SIMUL.run_to(4);
        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(t3.getExecTime() == 4);

        SIMUL.run_to(6);
        REQUIRE(t1.getExecTime() == 1);
        REQUIRE(t2.getExecTime() == 0);
        REQUIRE(t3.getExecTime() == 5);
        REQUIRE(sched.getSysCeiling() == 3);

        SIMUL.run_to(7);
        REQUIRE(sched.getSysCeiling() == 3);
        
        SIMUL.run_to(8);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(sched.getSysCeiling() == 3);

        SIMUL.run_to(9);
        REQUIRE(t1.getExecTime() == 4);
        REQUIRE(t2.getExecTime() == 0);
        REQUIRE(sched.getSysCeiling() == 2);

        SIMUL.run_to(10);
        REQUIRE(t1.getExecTime() == 4);
        REQUIRE(t2.getExecTime() == 0);
        REQUIRE(t3.getExecTime() == 6);
        /**
        *   T3 releases R2
        */       
        REQUIRE(sched.getSysCeiling() == 0);

        SIMUL.run_to(11);
        REQUIRE(t2.getExecTime() == 1);
        REQUIRE(t3.getExecTime() == 6);
        REQUIRE(sched.getSysCeiling() == 3);

        SIMUL.run_to(14);
        REQUIRE(t2.getExecTime() == 4);
        REQUIRE(t3.getExecTime() == 6);
        REQUIRE(sched.getSysCeiling() == 0);

        SIMUL.run_to(20);
        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;
    }
}
