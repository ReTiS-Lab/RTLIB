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


TEST_CASE("H-SRP under CBS servers test")
{
    try{
        TextTrace ttrace("trace0.txt");

        // create the scheduler and the kernel
        EDFScheduler sched;
        //RMScheduler sched;
        RTKernel kern(&sched);
        Resource R11("R11",LOCAL_RES),R12("R12",LOCAL_RES),R21("R21",LOCAL_RES),R31("R31",LOCAL_RES),RG1("RG1"),RG2("RG2"),RG3("RG3");

        HSRPManager hm("HSRPMan");
        hm.addResource(&RG1);
        hm.addResource(&RG2);
        hm.addResource(&RG3);

        SRPManager rm1("SRPLocalManagerS1",LOCAL_SRP);
        rm1.addResource(&R11);
        rm1.addResource(&R12);
        rm1.addResource(&RG1);

        SRPManager rm2("SRPLocalManagerS2",LOCAL_SRP);
        rm2.addResource(&R21);
        rm2.addResource(&RG2);
        rm2.addResource(&RG3);
        rm2.addResource(&RG1);

        SRPManager rm3("SRPLocalManagerS3",LOCAL_SRP);
        rm3.addResource(&R31);
        rm3.addResource(&RG2);
        rm3.addResource(&RG3);



        PeriodicTask t11(20, 4, 6, "T11");
        t11.insertCode("wait(R11);wait(R12);");
        t11.setAbort(false);

        PeriodicTask t12(20, 6, 8, "T12");
        t12.insertCode("wait(RG1);wait(R11);");
        t12.setAbort(false);

        PeriodicTask t13(20, 13, 4, "T13");
        t13.insertCode("wait(R12);");
        t13.setAbort(false);



        PeriodicTask t21(20, 5, 0, "T21");
        t21.insertCode("wait(RG1);wait(R21);");
        t21.setAbort(false);

        PeriodicTask t22(20, 19, 0, "T22");
        t22.insertCode("wait(RG2);wait(RG3);wait(R21);");
        t22.setAbort(false);


        PeriodicTask t31(20, 5, 6, "T31");
        t31.insertCode("wait(R31);wait(RG2);");
        t31.setAbort(false);

        PeriodicTask t32(20, 10, 8, "T32");
        t32.insertCode("wait(RG3);wait(R31);");
        t32.setAbort(false);

        PeriodicTask t33(20, 13, 4, "T33");
        t33.insertCode("wait(R31);");
        t33.setAbort(false);

        CBServer serv1(20, 20,20,CBServer::HARD, "Server1", "EDFSched");
        serv1.setLocalResManager(&rm1);
        serv1.setGlobalResManager(&hm);

        serv1.addTask(t11, "");
        serv1.addTask(t12, "");
        serv1.addTask(t13, "");

        CBServer serv2(30, 30,30,CBServer::HARD, "Server2", "EDFSched");
        serv2.setLocalResManager(&rm2);
        serv2.setGlobalResManager(&hm);

        serv2.addTask(t21, "");
        serv2.addTask(t22, "");


        CBServer serv3(40, 40,40,CBServer::HARD, "Server3", "EDFSched");
        serv3.setLocalResManager(&rm3);
        serv3.setGlobalResManager(&hm);

        serv3.addTask(t31, "");
        serv3.addTask(t32, "");
        serv3.addTask(t33, "");

        kern.setResManager(&hm);

        /*ttrace.attachToTask(&t1);
        ttrace.attachToTask(&t2);
        ttrace.attachToTask(&t3);
        ttrace.attachToTask(&t4);*/



        kern.addTask(serv1, "");
        kern.addTask(serv2, "");
        kern.addTask(serv3, "");

        hm.InitializeManager();

        SIMUL.initSingleRun();



        SIMUL.run_to(8);
        SIMUL.run_to(9);

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
