#include "catch.hpp"
#include <rttask.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>


#include <piresman.hpp>
#include <fcfsresmanager.hpp>
#include <texttrace.hpp>
#include <sporadicserver.hpp>

using namespace MetaSim;
using namespace RTSim;
    
    
TEST_CASE("CBS algorithm: da esempio")
{   
    
    try{
        TextTrace ttrace("trace.txt");
  
       // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        FCFSResManager *rm = new FCFSResManager("FCFSResManager");
        rm->addResource("PRes");
        kern.setResManager(rm);
      
        PeriodicTask t2(10,10 , 2, "TaskB"); 
        t2.insertCode("wait(PRes);delay(2);signal(PRes);fixed(1);");
        t2.setAbort(false);

        PeriodicTask t3(20, 20, 0, "TaskC"); 
        t3.insertCode("fixed(1);wait(PRes);delay(2);signal(PRes);");
        t3.setAbort(false);

        /*PeriodicTask t11(15, 15, 0, "TaskA1");
        t11.insertCode("fixed(2);");
        t11.setAbort(false);

        /*
        PeriodicTask t12(20, 20, 0, "TaskA2");
        t12.insertCode("fixed(4);");
        t12.setAbort(false); 
        */

        //ttrace.attachToTask(&t11);
        //ttrace.attachToTask(&t12);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);
          
        CBServer serv(4, 15,15,"hard",  "server1", "FIFOSched");//"RRSched(2);");
        serv.setGlobalResManager(rm);
        //serv.addTask(t11);
        //serv.addTask(t12);
        serv.addTask(t3);
        kern.addTask(serv, ""); 
    
        kern.addTask(t2, "");
        //kern.addTask(t3, "");

        SIMUL.initSingleRun();

        SIMUL.run_to(4);
        //REQUIRE(t2.getExecTime() == 1);
        //REQUIRE(t3.getExecTime() == 3);
        //REQUIRE(t11.getExecTime() == 0);
       // REQUIRE(t12.getExecTime() == 0);
        //REQUIRE(serv.getDeadline() == 15);
        //REQUIRE(t11.getExecTime() == 2);
       // REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(7);
        //REQUIRE(t2.getExecTime() == 3);
        //REQUIRE(t3.getExecTime() == 4);
        //REQUIRE(t12.getExecTime() == 2);
        //REQUIRE(serv.get_remaining_budget() == 4);
       // REQUIRE(serv.getDeadline() == 30);
        SIMUL.run_to(20);
        REQUIRE(serv.getBudget() == 4);
        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    } 
}