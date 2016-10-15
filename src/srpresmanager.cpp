#include <srpresmanager.hpp>


namespace RTSim {

    using namespace MetaSim;
    using namespace std;

    SRPManager::SRPManager(const std::string &n , SRPScope_t s):ResManager(n),ceilingTable(),SysCeilingIncrement(),scope(s),max_ceiling(0)
    {
    }

    SRPManager::~SRPManager()
    {
        //ceilingTable.clear();
    }

    void SRPManager::SortTasksBySched(vector<AbsRTTask*> *tasks)
    {
        RMScheduler* rmsched = dynamic_cast<RMScheduler*>(_sched);
        EDFScheduler* edfsched = dynamic_cast<EDFScheduler*>(_sched);

        if (rmsched)
        {
            sort(tasks->begin(),tasks->end(),
                 [](AbsRTTask *t1, AbsRTTask *t2) -> bool
                {
                    PeriodicTask *pt1 = dynamic_cast<PeriodicTask*>(t1);
                    PeriodicTask *pt2 = dynamic_cast<PeriodicTask*>(t2);
                    if (!pt1 || !pt2)
                        throw SRPManagerExc("RM needs Periodic Task","SRPManager::SortTasksBySched()");
                    return pt1->getPeriod() < pt2->getPeriod();
                });
        }
        else if(edfsched)
        {
            sort(tasks->begin(),tasks->end(),
                 [](const AbsRTTask *t1, const AbsRTTask *t2) -> bool
                {
                    return t1->getRelDline() < t2->getRelDline();
                });
        }else
            throw SRPManagerExc("Scheduler Type non supported","SRPManager::InizializeSRPManager()");
    }

    void SRPManager::InitializeManager()
    {

        vector<AbsRTTask*> tasks = _kernel->getTasks();
        SortTasksBySched(&tasks);
        vector<AbsRTTask*>::iterator I = tasks.begin();
        int plevel=tasks.size();
        max_ceiling = plevel;
        while (I != tasks.end())
        {
            TaskModel * tm = _sched->find(*I);
            if (tm == nullptr)
                throw SRPManagerExc("Model not Found","InizializeSRPManager()");
            tm->setPLevel(plevel--);

            Task *t = dynamic_cast<Task*>(*I);
            if (t == nullptr)
                throw SRPManagerExc("AbsRTTAsk is not a Task","InizializeSRPManager()");

            vector<string> res = getUsedRes(t);

            cout << "Task: "<<taskname(*I) << ": plevel: " <<plevel+1 <<endl;

            for(vector<string>::iterator I=res.begin(); I<res.end(); I++)
            {
                if (!find(*I))
                    throw SRPManagerExc("Task uses a Resource not present is such ResMan","InizializeSRPManager()");

                map<string,int>::iterator item = ceilingTable.find((*I));
                if (item == ceilingTable.end())
                {
                   if (scope == LOCAL_SRP && getResScope(*I) == GLOBAL_RES)
                        /**
                        *   case in which task uses a global resource,  
                        *   inside the local SRP
                        */
                       ceilingTable[(*I)] = max_ceiling;
                   
                   else
                        ceilingTable[(*I)] = tm->getPLevel();
                       
                   cout << getName() << " table: C(" << (*I) << ") = " << 
                           ceilingTable[(*I)] << "(from " << 
                           ((scope == LOCAL_SRP && getResScope(*I) == GLOBAL_RES)? "MAX_CEIL":taskname(t)) <<
                           ")" << endl;
                }
            }
            I++;
        }
    }

    vector<string> SRPManager::getUsedRes(Task* t)
    {
        vector<string> ret;
        vector<Task::InstrInstance> iv = t->getInstrVector();

        vector<Task::InstrInstance>::iterator I = iv.begin();

        for(;I<iv.end();I++)
        {
            if ((*I).first == "wait")
                ret.push_back((*I).second[0]);
        }
        return ret;
    }

    bool SRPManager::request(AbsRTTask *t, Resource *r, int n)
    {
         if (r->isLocked())
            throw SRPManagerExc("Resource already locked: not possible under SRP","SRPManager::request()");

         int NewCeiling  = ceilingTable[r->getName()];
         int CurrCeiling = _sched->getSysCeiling();
         int diff = (NewCeiling > CurrCeiling) ? (NewCeiling - CurrCeiling) : 0;
         SysCeilingIncrement.push(diff);
         if (diff > 0)
           _sched->setSysCeiling(NewCeiling);

         if (!(scope == LOCAL_SRP && getResScope(r)==GLOBAL_RES))
           r->lock(t);

         return true;
    }

    void SRPManager::release(AbsRTTask *t, Resource *r, int n)
    {     
        int dec = SysCeilingIncrement.top();
        SysCeilingIncrement.pop();
        int CurrCeiling = _sched->getSysCeiling();
        _sched->setSysCeiling(CurrCeiling-dec);
        if (!(scope == LOCAL_SRP && getResScope(r)==GLOBAL_RES))
            r->unlock();
    }

    void SRPManager::newRun(){}

    void SRPManager::endRun(){}

    bool SRPManager::isLocked(AbsRTTask* t) const
    {
        vector<Resource*>::const_iterator I = _res.begin();
        while (I != _res.end())
        {
            if ((*I)->getOwner() == t)
                return true;
            I++;
        }
        return false;
    }

    AbsRTTask* SRPManager::getNewExeTask() const
    {
        int i = 0;
        AbsRTTask *newExe = _sched->getTaskN(i);

        while (newExe != nullptr && !_sched->checkPLevel(newExe))
        {
           if (isLocked(newExe))
               return newExe;
           i++;
           newExe = _sched->getTaskN(i);
        }
        return newExe;
    }

    int SRPManager::getResCeil(Resource* r) const
    {
        return getResCeil(r->getName());
    }

    int SRPManager::getResCeil(string name) const
    {
        map<string, int>::const_iterator I = ceilingTable.find(name);
        if (I == ceilingTable.end())
            throw SRPManagerExc("Resource unknown", "SRPManager::getResCeil()");
        return I->second;
    }

}
