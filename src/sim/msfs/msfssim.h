/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/simulator.h"
#include "navitab/logger.h"
#include <thread>
#include <winsock2.h>
#include <windows.h>
#include <SimConnect.h>

namespace navitab {

class MsfsSimulator : public Simulator
{
public:
    MsfsSimulator();
    ~MsfsSimulator();
    
    void Connect(std::shared_ptr<CoreServices>) override;
    void Disconnect() override;

private:
    void AsyncPollSimulator();

    bool TryConnectToMsfsSim();
    bool RetrieveMsfsObjectData();

    void HandleMsfsDispatch(SIMCONNECT_RECV* pData, DWORD cbData);
    void UpdateAircraftLocation(SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData, bool isUserAircraft);

    void ResetSimAircraftData();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<Settings> prefs;
    std::shared_ptr<CoreServices> core;
    std::shared_ptr<Simulator2Core> handler;

    HANDLE hSimConnect;
    SimStateData simData[2];
    bool tikTok;

    bool running;
    std::unique_ptr<std::thread> worker;

};

} // namespace navitab
