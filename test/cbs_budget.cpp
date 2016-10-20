#include "catch.hpp"
#include <rttask.hpp>
#include <cbserver.hpp>
#include <kernel.hpp>
#include <edfsched.hpp>
#include <texttrace.hpp>

using namespace MetaSim;
using namespace RTSim;
    

#define simulstep(x, obj)																		\
						SIMUL.run_to(x);														\
        				cout << "vtime(" << x << ") = " << obj.getVirtualTime() << endl;		\
        				cout << "status(" << x << ") = " << obj.getStatus() << endl;			\
        				cout << "q(" << x << ") = " << obj.get_remaining_budget() << endl;		\
        				cout << "---------------------------------" << endl
    
TEST_CASE("CBS algorithm: test with Resource")
{   
    try
    {
        TextTrace ttrace("cbs Budget Trace.txt");
  
		PeriodicTask t1(13, 13, 0, "TaskA");
    	t1.insertCode("fixed(5);");
    	t1.setAbort(false); 

	    EDFScheduler sched;
	    RTKernel kern(&sched);
	    
    	CBServer serv1(3, 6, 6, CBServer::SOFT,  "server1", "FIFOSched");
    	serv1.addTask(t1);
	    kern.addTask(serv1);

	    ttrace.attachToTask(&t1);
        SIMUL.initSingleRun();

	    for (int i=0; i < 13; i++)
	    {
	    	simulstep(i, serv1);
	    }

        SIMUL.endSingleRun();
    }  catch (BaseExc &e) {
            cout << e.what() << endl;
    } catch (parse_util::ParseExc &e2) {
        cout << e2.what() << endl;

    } 
}        