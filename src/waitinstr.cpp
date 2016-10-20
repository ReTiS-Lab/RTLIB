/***************************************************************************
    begin                : Thu Apr 24 15:54:58 CEST 2003
    copyright            : (C) 2003 by Giuseppe Lipari
    email                : lipari@sssup.it
 ***************************************************************************/
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <simul.hpp>

#include <kernel.hpp>
#include <server.hpp>
#include <broeserver.hpp>
#include <task.hpp>
#include <waitinstr.hpp>

namespace RTSim {

    WaitInstr::WaitInstr(Task * f, const char *r, int nr, char *n)
        : Instr(f, n), _res(r), _endEvt(this), 
          _waitEvt(f, this), _numberOfRes(nr) 
    {}

    WaitInstr::WaitInstr(Task * f, const string &r, int nr, char *n)
        : Instr(f, n), _res(r), _endEvt(this), 
          _waitEvt(f, this), _numberOfRes(nr) 
    {}

    Instr* WaitInstr::createInstance(vector<string> &par)
    {
        return new WaitInstr(dynamic_cast<Task *>(Entity::_find(par[1])), par[0]);
    }

    void WaitInstr::endRun() 
    {
        _endEvt.drop(); 
        _waitEvt.drop();
    }

    void WaitInstr::schedule()
    {
        DBGENTER(_INSTR_DBG_LEV);
        DBGPRINT("Scheduling WaitInstr named: " << getName());

        _endEvt.post(SIMUL.getTime());
    }

    void WaitInstr::deschedule()
    {
        _endEvt.drop();
    }

    void WaitInstr::setTrace(Trace *t) 
    {
        _endEvt.addTrace(t); 
        _waitEvt.addTrace(t);
    }
    /** Modica Celia
    *   Modified for support server kernel
    *   ans support BroeServer kernel
    */
    void WaitInstr::onEnd() 
    {
        DBGENTER(_INSTR_DBG_LEV);

        _father->onInstrEnd();

        RTKernel    *k = dynamic_cast<RTKernel *>(_father->getKernel());
        Server      *ks = dynamic_cast<Server *>(_father->getKernel());
        BROEServer  *kb = dynamic_cast<BROEServer *>(_father->getKernel());

        if (k == nullptr && ks == nullptr && kb == nullptr)
            throw BaseExc("Kernel not found!");
        
        /// N.B. The order of the if is very Important because a BroeServer
        /// is also a Server but not vice versa.
        if (k != nullptr) /// wait from a task managed by RTKernel
            k->requestResource(_father, _res, _numberOfRes);
        else if (kb != nullptr) /// wait from a task managed by BROEServer
            kb->requestResource(_father, _res, _numberOfRes);
        else /// wait from a task managed by Server
            ks->requestResource(_father,_res,_numberOfRes);

        _waitEvt.process();
    }

    SignalInstr::SignalInstr(Task *f,  const char *r, int nr, char *n)
        : Instr(f, n), _res(r), _endEvt(this), 
          _signalEvt(f, this), _numberOfRes(nr) 
    {}

    SignalInstr::SignalInstr(Task *f, const string &r, int nr, char *n)
        : Instr(f, n), _res(r), _endEvt(this), 
          _signalEvt(f, this), _numberOfRes(nr) 
    {}

    Instr* SignalInstr::createInstance(vector<string> &par)
    {
        return new SignalInstr(dynamic_cast<Task *>(Entity::_find(par[1])), par[0]);
    }

    void SignalInstr::endRun() 
    {
        _endEvt.drop();
        _signalEvt.drop();
    }

    void SignalInstr::schedule()
    {
        _endEvt.post( SIMUL.getTime()); 
    }

    void SignalInstr::deschedule()
    {
        _endEvt.drop();
    }

    void SignalInstr::setTrace(Trace *t) 
    {
        _endEvt.addTrace(t);
        _signalEvt.addTrace(t);
    }
    /** Modica Celia
    *   Modified for support server kernel
    *   we don't need any change for
    *   support BROEServer because in the
    *   signal it's behaviour
    *   is the same of the normal server.
    */
    void SignalInstr::onEnd() 
    {
        DBGENTER(_INSTR_DBG_LEV);

        _endEvt.drop();               
        _signalEvt.process();         
        _father->onInstrEnd();        

        RTKernel    *k = dynamic_cast<RTKernel *>(_father->getKernel());
        Server      *ks = dynamic_cast<Server *>(_father->getKernel());

        if (k == nullptr && ks == nullptr) {
            throw BaseExc("SignalInstr has no kernel set!");
        }

        if (k != nullptr)
            k->releaseResource(_father, _res, _numberOfRes);
        else    ks->releaseResource(_father, _res, _numberOfRes); 
    }

}
