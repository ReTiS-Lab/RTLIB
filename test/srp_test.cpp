#include "catch.hpp"
#include <rttask.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>
#include <srpresmanager.hpp>


using namespace MetaSim;
using namespace RTSim;
using namespace std;


TEST_CASE("SRP: test su preemption level e System Ceiling")
{
    try{
        TextTrace ttrace("trace.txt");

       // create the scheduler and the kernel
        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);

        SRPManager *rm = new SRPManager("");
        rm->addResource("R1");
        rm->addResource("R2");
        rm->addResource("R3");
        kern.setResManager(rm);


        PeriodicTask t1(20,8,4, "TaskTau1");
        t1.insertCode("fixed(1);wait(R3);delay(1);wait(R1);delay(1);signal(R1);delay(1);signal(R3);");
        t1.setAbort(false);

        PeriodicTask t2(20, 17, 2, "TaskTau2");
        t2.insertCode("fixed(1);wait(R3);delay(1);wait(R2);delay(1);signal(R2);signal(R3);fixed(1);");
        t2.setAbort(false);

        PeriodicTask t3(20, 20, 0, "TaskTau3");
        t3.insertCode("fixed(1);wait(R2);delay(1);wait(R1);delay(3);signal(R1);delay(1);signal(R2);fixed(1);wait(R3);delay(1);signal(R3);");
        t3.setAbort(false);




        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);

        kern.addTask(t1, "");
        kern.addTask(t2, "");
        kern.addTask(t3, "");

        vector<AbsRTTask*> v;
        v.push_back((AbsRTTask*)&t2);
        v.push_back((AbsRTTask*)&t3);
        v.push_back((AbsRTTask*)&t1);
        rm->InitializeSRPManager(&v);




        TaskModel *tm1 = sched.getTaskModel(&t1);
        tm1->setPLevel(3);
        TaskModel *tm2 = sched.getTaskModel(&t2);
        tm2->setPLevel(2);
        TaskModel *tm3 = sched.getTaskModel(&t3);
        tm3->setPLevel(1);
        SIMUL.initSingleRun();

        SIMUL.run_to(1);
        SIMUL.run_to(2);
        SIMUL.run_to(3);

        SIMUL.run_to(4);
        SIMUL.run_to(8);
        SIMUL.run_to(10);
        t1.setRelDline(30);
        SIMUL.run_to(11);

        SIMUL.run_to(12);

        SIMUL.run_to(14);
        SIMUL.run_to(15);

        SIMUL.run_to(16);

        SIMUL.run_to(17);

        SIMUL.run_to(21);

        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }
}
