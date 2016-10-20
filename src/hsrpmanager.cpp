#include <hsrpmanager.hpp>


namespace RTSim {

    using namespace MetaSim;
    using namespace std;


    HSRPManager::HSRPManager(const std::string &n):SRPManager(n){}

    void HSRPManager::InitializeManager()
    {
        vector<AbsRTTask*> tasks = _kernel->getTasks();
        vector<AbsRTTask*>::iterator I = tasks.begin();
        SortTasksBySched(&tasks);
        int plevel = tasks.size();
        for(; I != tasks.end(); I++)
        {
            Server *s = dynamic_cast<Server *>(*I);
            if (!s)
                throw HSRPManagerExc("HSRP needs only server task","HSRPManager::Initializemanager()");

            ResManager* r= s->getLocalResManager();
            SRPManager* sr= dynamic_cast<SRPManager*>(r);
            if (!sr)
               throw HSRPManagerExc("Server without local SRPManager","HSRPManager::Initializemanager()");

            sr->InitializeManager();

            TaskModel *tm = _sched->find(*I);
            if (tm == nullptr)
                throw HSRPManagerExc("Model not found","HSRPManager::InitializeManager");
            tm->setPLevel(plevel--);

            vector<string> res = getUsedRes(s,sr);


            for(vector<string>::iterator J=res.begin(); J != res.end(); J++)
            {
                map<string,int>::iterator item = ceilingTable.find((*J));
                if (item == ceilingTable.end())    
                    ceilingTable[(*J)] = tm->getPLevel();
             }
        }

    }

   vector<string> HSRPManager::getUsedRes(Server *s, SRPManager *r)
    {
        vector<string> ret;
        vector<AbsRTTask*> tasks = s->getTasks();
        vector<AbsRTTask*>::iterator I = tasks.begin();
        for (; I != tasks.end(); I++)
        {
            Task *t = dynamic_cast<Task*>(*I);
            if (!t)
                throw HSRPManagerExc("AbsRTTAsk is not a Task","HSRPManager::getUsedRes()");
            vector<string> res = SRPManager::getUsedRes(t);
            vector<string>::iterator J = res.begin();
            for (; J != res.end(); J++)
            {
                if (find(*J))
                    ret.push_back(*J);
            }
        }
        return ret;
    }

   bool HSRPManager::request(AbsRTTask *t, AbsRTTask *s, Resource *r, int n)
   {
        if (r->isLocked())
           throw HSRPManagerExc("Resource already locked: not possible under HSRP","SRPManager::request()");

        int NewCeiling  = ceilingTable[r->getName()];
        int CurrCeiling = _sched->getSysCeiling();
        int diff = (NewCeiling > CurrCeiling) ? (NewCeiling - CurrCeiling) : 0;
        SysCeilingIncrement.push(diff);
        if (diff > 0)
          _sched->setSysCeiling(NewCeiling);

        r->lock(t,s);

        return true;
   }

   void HSRPManager::release(AbsRTTask *t, Resource *r, int n)
   {
       int dec = SysCeilingIncrement.top();
       SysCeilingIncrement.pop();
       int CurrCeiling = _sched->getSysCeiling();
       _sched->setSysCeiling(CurrCeiling-dec);
       r->unlock();
   }
}
