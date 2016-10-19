#include "catch.hpp"
#include <rttask.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <server.hpp>
#include <fcfsresmanager.hpp>
#include <texttrace.hpp>

using namespace MetaSim;
using namespace RTSim;

    
TEST_CASE("CBS algorithm: test with Resource")
{   
    
    try{
        TextTrace ttrace("Trace_other_CBS_cases1.txt");
  
       // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        FCFSResManager rm("FCFSResManager");
        rm.addResource("PRes");
        kern.setResManager(&rm);

        PeriodicTask t1(12,12,2, "T1");
        t1.insertCode("fixed(2);wait(PRes);delay(2);signal(PRes);");
        t1.setAbort(false);

        PeriodicTask t2(17, 17, 0, "T2"); 
        t2.insertCode("fixed(1);wait(PRes);delay(4);signal(PRes);");
        t2.setAbort(false);

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        /** 3° param true = Hard, false = soft*/
        CBServer serv(3, 12,12,CBServer::HARD, "Server1", "FIFOSched");//"RRSched(2);");
        serv.setLocalResManager(&rm,true);
     
        serv.addTask(t2);
        kern.addTask(serv, "");
        kern.addTask(t1, "");
        
        SIMUL.initSingleRun();

        SIMUL.run_to(1);

        REQUIRE(t2.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(2);

        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(t2.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(3);

        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(4);

        REQUIRE(t1.getExecTime() == 1);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(5);

        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(6);

        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(10);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(12);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 3);
        SIMUL.run_to(14);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 5);
        REQUIRE(serv.get_remaining_budget() == 1);

        SIMUL.endSingleRun();

    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    } 
}

TEST_CASE("CBS algorithm: Test Hard CBS Overload")
{   
    
    try{
        TextTrace ttrace("Trace_other_CBS_cases2.txt");
  
        // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        PeriodicTask t1(23,23,0, "TaskTau1");
        t1.insertCode("fixed(16);");
        t1.setAbort(false);

        PeriodicTask t2(28, 28, 0, "TaskTau2"); 
        t2.insertCode("fixed(5);");
        t2.setAbort(false);

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);

        CBServer serv(1, 4,4,CBServer::HARD, "Server1", "FIFOSched");//"RRSched(2);");
        
        serv.addTask(t2);
        kern.addTask(serv, "");
        kern.addTask(t1, "");
        
        SIMUL.initSingleRun();
        SIMUL.run_to(1);
        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(t2.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(2);
        REQUIRE(t1.getExecTime() == 1);
        REQUIRE(t2.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(3);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(t2.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(4);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(t2.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(5);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(t2.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(8);
        REQUIRE(t1.getExecTime() == 6);
        REQUIRE(t2.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(9);
        REQUIRE(t1.getExecTime() == 6);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(12);
        REQUIRE(t1.getExecTime() == 9);
        REQUIRE(t2.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(13);
        REQUIRE(t1.getExecTime() == 9);
        REQUIRE(t2.getExecTime() == 4);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(16);
        REQUIRE(t1.getExecTime() == 12);
        REQUIRE(t2.getExecTime() == 4);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(17);
        REQUIRE(t1.getExecTime() == 12);
        REQUIRE(t2.getExecTime() == 5);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(21);
        REQUIRE(t1.getExecTime() == 16);
        REQUIRE(t2.getExecTime() == 5);
        /**
        * no other jobs, budget not replanished
        */
        //REQUIRE(serv.get_remaining_budget() == 0);

        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    } 
}
