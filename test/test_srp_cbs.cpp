#include "catch.hpp"
#include <rttask.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>
#include <srpresmanager.hpp>
#include <cbserver.hpp>


using namespace MetaSim;
using namespace RTSim;
using namespace std;


TEST_CASE("SRP under CBS server test")
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

        SIMUL.initSingleRun();

        //REQUIRE((sched.getTaskModel(&t1))->getPLevel() == 3);
        //REQUIRE((sched.getTaskModel(&t2))->getPLevel() == 4);
        //REQUIRE((sched.getTaskModel(&t3))->getPLevel() == 2);
        //REQUIRE((sched.getTaskModel(&t4))->getPLevel() == 1);
        REQUIRE(rm.getResCeil("R1") == 3);
        REQUIRE(rm.getResCeil("R2") == 2);
        REQUIRE(rm.getResCeil("R3") == 2);

        SIMUL.run_to(8);
        SIMUL.run_to(9);
       // REQUIRE(sched.getSysCeiling()==2);

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
