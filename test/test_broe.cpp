#include "catch.hpp"
#include <rttask.hpp>
#include <broeserver.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <piresman.hpp>
#include <fcfsresmanager.hpp>
#include <texttrace.hpp>

using namespace MetaSim;
using namespace RTSim;
    
    
TEST_CASE("BROE algorithm: test Bounded-delay without wait")
{   
    
   try{
        TextTrace ttrace("trace.txt");
  
		// create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        FCFSResManager *rm = new FCFSResManager("FCFSResManager");
        rm->addResource("PRes");
        kern.setResManager(rm);

        PeriodicTask t1(14,14,0, "TaskTau1");
        t1.insertCode("fixed(3);wait(PRes);delay(2);signal(PRes);fixed(2);");
        t1.setAbort(false);

        PeriodicTask t2(7,7,0, "TaskTau2");
        t2.insertCode("fixed(3);");
        t2.setAbort(false);

        ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);



        RHTmap_t map;

        map["TaskTau1_PRes"] = 2;

        BROEServer serv(4, 8,8, "Server1",&map);
        serv.setGlobalResManager(rm);
        
        serv.addTask(t1);
        kern.addTask(serv, "");
        kern.addTask(t2);
        
        SIMUL.initSingleRun();
        SIMUL.run_to(3);
        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(5);
        t2.setRelDline(20);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(6);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(7);
        REQUIRE(t1.getExecTime() == 4);
        REQUIRE(serv.get_remaining_budget() == 3);
        SIMUL.run_to(8);
        REQUIRE(t1.getExecTime() == 5);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(9);
        REQUIRE(t1.getExecTime() == 6);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(10);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(11);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(12);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(13);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(14);
        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(15);
        REQUIRE(t1.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 3);
        SIMUL.run_to(16);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(17);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.endSingleRun();
        
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }

}

TEST_CASE("BROE algorithm: test Bounded-delay with wait")
{

   try{
        TextTrace ttrace("trace.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        RTKernel kern(&sched);

        FCFSResManager *rm = new FCFSResManager("FCFSResManager2");
        rm->addResource("PRes2");
        kern.setResManager(rm);

        PeriodicTask t1(18,18,0, "TaskTau12");
        t1.insertCode("fixed(3);wait(PRes);delay(2);signal(PRes2);fixed(2);");
        t1.setAbort(false);

        ttrace.attachToTask(&t1);

        RHTmap_t map;

        map["TaskTau12_PRes2"] = 2;

        BROEServer serv(4, 8,8, "Server1",&map);
        serv.setGlobalResManager(rm);

        serv.addTask(t1);
        kern.addTask(serv, "");

        SIMUL.initSingleRun();
        SIMUL.run_to(1);
        REQUIRE(t1.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 3);
        SIMUL.run_to(2);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(3);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(4);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(5);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(6);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(7);
        REQUIRE(t1.getExecTime() == 4);
        REQUIRE(serv.get_remaining_budget() == 3);
        SIMUL.run_to(8);
        REQUIRE(t1.getExecTime() == 5);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(9);
        REQUIRE(t1.getExecTime() == 6);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.run_to(10);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(11);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(12);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(13);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 0);
        SIMUL.run_to(14);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(15);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(16);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(17);
        REQUIRE(t1.getExecTime() == 7);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(18);
        REQUIRE(t1.getExecTime() == 0);
        REQUIRE(serv.get_remaining_budget() == 4);
        SIMUL.run_to(19);
        REQUIRE(t1.getExecTime() == 1);
        REQUIRE(serv.get_remaining_budget() == 3);
        SIMUL.run_to(20);
        REQUIRE(t1.getExecTime() == 2);
        REQUIRE(serv.get_remaining_budget() == 2);
        SIMUL.run_to(21);
        REQUIRE(t1.getExecTime() == 3);
        REQUIRE(serv.get_remaining_budget() == 1);
        SIMUL.endSingleRun();

    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    }

}

