#include <srpresmanager.hpp>


namespace RTSim {

    using namespace MetaSim;
    using namespace std;

    SRPManager::SRPManager(const std::string &n ):ResManager(n),ceilingTable(),SysCeilingIncrement()
    {
    }

    SRPManager::~SRPManager()
    {
        ceilingTable.clear();
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

    void SRPManager::InitializeSRPManager(vector<AbsRTTask*> *tasks)
    {

        SortTasksBySched(tasks);
        vector<AbsRTTask*>::iterator I = tasks->begin();
        int plevel=tasks->size();
        while (I != tasks->end())
        {
            TaskModel * tm = _sched->find(*I);
            if (tm == nullptr)
                throw SRPManagerExc("Model not Found","InizializeSRPManager()");
            tm->setPLevel(plevel--);

            Task *t = dynamic_cast<Task*>(*I);
            if (t == nullptr)
                throw SRPManagerExc("AbsRTTAsk is not a Task","InizializeSRPManager()");

            vector<string> res = getUsedRes(t);

            for(vector<string>::iterator I=res.begin(); I<res.end(); I++)
            {
                map<string,int>::iterator item = ceilingTable.find((*I));
                if (item == ceilingTable.end())
                    ceilingTable[(*I)] = tm->getPLevel();
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

         r->lock(t);
         int NewCeiling  = ceilingTable[r->getName()];
         int CurrCeiling = _sched->getSysCeiling();
         int diff = (NewCeiling > CurrCeiling) ? (NewCeiling - CurrCeiling) : 0;
         SysCeilingIncrement.push(diff);
         if (diff > 0)
           _sched->setSysCeiling(NewCeiling);
         return true;
    }

    void SRPManager::release(AbsRTTask *t, Resource *r, int n)
    {
        r->unlock();
        int dec = SysCeilingIncrement.top();
        SysCeilingIncrement.pop();
        int CurrCeiling = _sched->getSysCeiling();
        _sched->setSysCeiling(CurrCeiling-dec);
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
