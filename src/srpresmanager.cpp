#include <srpresmanager.hpp>



namespace RTSim {

    using namespace MetaSim;
    using namespace std;

    SRPManager::SRPManager(const std::string &n ):ResManager(n),ceilingTable(),SysCeilingIncrement()
    {
    }

    void SRPManager::InitializeSRPManager(vector<AbsRTTask*> *tasks)
    {
        sort(tasks->begin(),tasks->end(),
             [](const AbsRTTask *t1, const AbsRTTask *t2) -> bool
            {
                return t1->getRelDline() < t2->getRelDline();
            });

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
                throw SRPManagerExc("AbsRTTAsk not is a Task","InizializeSRPManager()");

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

}

