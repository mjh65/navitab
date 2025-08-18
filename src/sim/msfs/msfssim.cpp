/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "msfssim.h"
#include "navitab/core.h"
#include <fmt/format.h>
#include <chrono>
#include <strsafe.h>


std::shared_ptr<navitab::Simulator> navitab::Simulator::Factory()
{
    return std::make_shared<navitab::MsfsSimulator>();
}

namespace navitab {

enum {
    AIRCRAFT_LOC_INFO
};
enum {
    USER_AIRCRAFT_LOCATION,
    OTHER_AIRCRAFT_LOCATIONS
};
enum {
    EVENT_SIM_STATE,
    EVENT_PAUSE_STATE
};
static const DWORD REQUEST_DATA_RANGE = 200000; // in metres = 108 nm


MsfsSimulator::MsfsSimulator()
:   LOG(std::make_unique<logging::Logger>("msfssim")),
    hSimConnect(nullptr),
    tikTok(false),
    running(false)
{
    ResetSimAircraftData();
    simData[1].loopCount = simData[0].loopCount = 0;
    simData[1].zuluTime = simData[0].zuluTime = 0;
    simData[1].fps = simData[0].fps = 1;
}

MsfsSimulator::~MsfsSimulator()
{
    if (hSimConnect) {
        (void)SimConnect_Close(hSimConnect);
        hSimConnect = nullptr;
    }
}

void MsfsSimulator::Connect(std::shared_ptr<CoreServices> c)
{
    core = c;
    prefs = core->GetSettingsManager();
    handler = core->GetSimulatorCallbacks();
    running = true;
    worker = std::make_unique<std::thread>([this]() { AsyncPollSimulator(); });
}

void MsfsSimulator::Disconnect()
{
    running = false;
    worker->join();
    handler.reset();
    prefs.reset();
    core.reset();
}

void MsfsSimulator::AsyncPollSimulator()
{
    using namespace std::chrono_literals;
    unsigned long n = 0;
    unsigned int goodCount = 0;
    unsigned int badCount = 0;
    const std::chrono::microseconds measurePeriod = 2s;
    auto nextMeasureTime = std::chrono::steady_clock::now() + measurePeriod;
    std::chrono::microseconds loopDelay = 1ms;
    while (running) {
        bool updated = true;
        if (hSimConnect) {
            std::this_thread::sleep_for(loopDelay);
            updated = RetrieveMsfsObjectData();
            if (updated) {
                ++goodCount;
            } else {
                ++badCount;
            }
        }
        else {
            if (!TryConnectToMsfsSim()) {
                std::this_thread::sleep_for(1s);
                nextMeasureTime = std::chrono::steady_clock::now() + 10s;
                if (goodCount || badCount) {
                    goodCount = badCount = 0;
                    loopDelay = 1ms;
                    simData[1].fps = simData[0].fps = 1;
                }
            }
        }
        if (updated) {
            simData[tikTok].loopCount = ++n;
            handler->PostSimUpdates(simData[tikTok]);
            tikTok = !tikTok;
        }
        auto t = std::chrono::steady_clock::now();
        if (t > nextMeasureTime) {
            LOGD(fmt::format("Measured FPS: {} sleep, {} updates {} futile", loopDelay.count(), goodCount, badCount));
            if (badCount == 0) {
                // this suggests the loop might not be keeping up, so we need to go a bit faster, say 10%
                loopDelay *= 10; loopDelay /= 11;
            } else if (goodCount && ((badCount * 20) > goodCount)) {
                // try to tune the loop period so that less than 5% updates are futile
                loopDelay *= (goodCount + badCount); loopDelay /= goodCount;
            }
            simData[tikTok].fps = goodCount / std::chrono::round<std::chrono::seconds>(measurePeriod).count();
            nextMeasureTime = std::chrono::steady_clock::now() + measurePeriod;
            goodCount = badCount = 0;
        }
    }
}


bool MsfsSimulator::TryConnectToMsfsSim()
{
    if (SUCCEEDED(SimConnect_Open(&hSimConnect, "Avitab", NULL, 0, 0, 0)))
    {
        LOGS("SimConnect connected to MS Flight Simulator!");

        // Request an event when the simulation starts or stops
        SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_SIM_STATE, "Sim");
        SimConnect_SubscribeToSystemEvent(hSimConnect, EVENT_PAUSE_STATE, "Pause");

        // Set up the data definition for aircraft locations
        SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_LOC_INFO, "Title", NULL, SIMCONNECT_DATATYPE_STRING256);
        SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_LOC_INFO, "Plane Altitude", "feet");
        SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_LOC_INFO, "Plane Latitude", "degrees");
        SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_LOC_INFO, "Plane Longitude", "degrees");
        SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_LOC_INFO, "Plane Heading Degrees True", "degrees");
        SimConnect_AddToDataDefinition(hSimConnect, AIRCRAFT_LOC_INFO, "Zulu Time", "Seconds", SIMCONNECT_DATATYPE_INT32);

        // Register for updates about the user aircraft location - we can use this to measure frame rate too.
        (void)SimConnect_RequestDataOnSimObject(hSimConnect, USER_AIRCRAFT_LOCATION, AIRCRAFT_LOC_INFO, SIMCONNECT_OBJECT_ID_USER, SIMCONNECT_PERIOD_VISUAL_FRAME);
    } else {
        LOGD("SimConnect failed to connect");
        hSimConnect = nullptr;
    }
    return (hSimConnect != nullptr);
}

bool MsfsSimulator::RetrieveMsfsObjectData()
{
    bool gotSomething = false;
    // Ask for updates about other aircraft locations - seems like this needs to be done every time an update is wanted
    HRESULT hr = SimConnect_RequestDataOnSimObjectType(hSimConnect, OTHER_AIRCRAFT_LOCATIONS, AIRCRAFT_LOC_INFO, REQUEST_DATA_RANGE, SIMCONNECT_SIMOBJECT_TYPE_AIRCRAFT);
    while (1) {
        // we use SimConnect_GetNextDispatch() rather than the callback because we don't really know how many
        // callbacks we need to trigger, so we might as well poll and check the result codes
        SIMCONNECT_RECV* pData;
        DWORD cbData;
        HRESULT hr = SimConnect_GetNextDispatch(hSimConnect, &pData, &cbData);
        if ((hr == S_OK) && (pData->dwID != SIMCONNECT_RECV_ID_NULL)) {
            HandleMsfsDispatch(pData, cbData);
            gotSomething = true;
        } else {
            // no more dispatches, so drop out
            break;
        }
    }
    return gotSomething;
}

void MsfsSimulator::HandleMsfsDispatch(SIMCONNECT_RECV* pData, DWORD cbData)
{
    // handle callback data
    LOGD(fmt::format("HandleMsfsDispatch ({},{},{})", pData->dwSize, pData->dwVersion, pData->dwID));
    switch (pData->dwID) {

    case SIMCONNECT_RECV_ID_QUIT: // user has quit MSFS, so our sim connection has gone
        LOGS("SimConnect disconnected since MS Flight Simulator was shut down!");
        (void)SimConnect_Close(hSimConnect);
        hSimConnect = nullptr;
        ResetSimAircraftData();
        break;

    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA: // user aircraft location
        UpdateAircraftLocation(reinterpret_cast<SIMCONNECT_RECV_SIMOBJECT_DATA*>(pData), true);
        break;

    case SIMCONNECT_RECV_ID_SIMOBJECT_DATA_BYTYPE: // other aircraft locations
        UpdateAircraftLocation(reinterpret_cast<SIMCONNECT_RECV_SIMOBJECT_DATA*>(pData), false);
        break;

    case SIMCONNECT_RECV_ID_EVENT: // some event (we subscribed to) has occurred ??
        break;

    }
}

void MsfsSimulator::UpdateAircraftLocation(SIMCONNECT_RECV_SIMOBJECT_DATA* pObjData, bool isUserAircraft)
{
    LOGD(fmt::format("dwRequestID = {}, dwObjectID = {}, dwDefineID = {}, dwFlags = {}, dwentrynumber = {}, dwoutof = {}, dwDefineCount = {}",
        pObjData->dwRequestID, pObjData->dwObjectID, pObjData->dwDefineID, pObjData->dwFlags, pObjData->dwentrynumber, pObjData->dwoutof, pObjData->dwDefineCount));

    struct SimObjectLocation
    {
        char    title[256];
        double  altitude;
        double  latitude;
        double  longitude;
        double  heading;
        int32_t zulutime;
    };

    SimObjectLocation* pLoc = reinterpret_cast<SimObjectLocation*>(&pObjData->dwData);
    if (SUCCEEDED(StringCbLengthA(&pLoc->title[0], sizeof(pLoc->title), nullptr))) // security check
    {
        LOGD(fmt::format("Title=\"{}\", Lat={}  Lon={}  Alt={}  Heading={}",
            pLoc->title, pLoc->latitude, pLoc->longitude, pLoc->altitude, pLoc->heading));
        auto& sd = simData[tikTok];
        if (isUserAircraft) {
            sd.myPlane = Position(Trajectory(Location(pLoc->latitude, pLoc->longitude, Location::DEGREES), pLoc->heading, Location::DEGREES), pLoc->altitude, Position::FEET);
            sd.zuluTime = (unsigned)pLoc->zulutime;
        }
        else {
            size_t id = pObjData->dwentrynumber - 1;
            sd.nOtherPlanes = std::max((unsigned)pObjData->dwoutof, (unsigned)SimStateData::MAX_OTHER_AIRCRAFT);
            if (id < SimStateData::MAX_OTHER_AIRCRAFT) {
                sd.otherPlanes[id] = Position(Trajectory(Location(pLoc->latitude, pLoc->longitude, Location::DEGREES), pLoc->heading, Location::DEGREES), pLoc->altitude, Position::FEET);
            }
        }
    }
}

void MsfsSimulator::ResetSimAircraftData()
{
    for (size_t i = 0; i < 2; ++i) {
        simData[i].nOtherPlanes = 0;
        simData[i].myPlane = Position(Trajectory(Location(0, 0), 0), 100);
    }
}

} // namespace navitab
