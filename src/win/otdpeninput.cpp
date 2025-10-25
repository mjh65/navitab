/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "otdpeninput.h"
#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN 1
#define NOMINMAX 1
#include <Windows.h>
#endif
#include "navitab/logger.h"
#include <fmt/core.h>
#include <thread>
#include <mutex>

namespace navitab {

#if defined(_WIN32)

class OtdPenInputWin32 : public OtdPenInput
{
public:
    OtdPenInputWin32();
    virtual ~OtdPenInputWin32();

    bool GetPenState(float& x, float& y, float& pressure) override;

    bool SupportsPenInput() const { return otdIpcMsgHandle != INVALID_HANDLE_VALUE; }

private:
    void AsyncRunOtdPenIpc();
    void ConfigureTablet();
    void UpdateState();

private:
    std::unique_ptr<logging::Logger> LOG;
    unsigned tabletVid;
    unsigned tabletPid;

private:
    std::mutex stateMutex;
    unsigned reportWatchdog;
    bool isInProximity;
    float xPos;
    float yPos;
    float pressure;

private:
    bool running;
    std::unique_ptr<std::thread> worker;
    HANDLE otdIpcMsgHandle;
    OVERLAPPED otdIpcOverlap;

private:
    struct Header {
        uint32_t messageType;
        uint32_t size;
        uint16_t vid;
        uint16_t pid;
    };

    struct DeviceInfo {
        Header hdr;
        bool isValid;
        float maxX;
        float maxY;
        uint32_t maxPressure;
        wchar_t name[64];
    };

    struct Ping {
        Header hdr;
        uint64_t sequenceNumber;
    };

    struct State {
        Header hdr;
        bool positionValid;
        float x;
        float y;
        bool pressureValid;
        uint32_t pressure;
        bool penButtonsValid;
        uint32_t penButtons;
        bool auxButtonsValid;
        uint32_t auxButtons;
        bool proximityValid;
        uint32_t hoverDistance;
        bool nearProximity;
    };

    union Message {
        Header      hdr;
        DeviceInfo  devinfo;
        Ping        ping;
        State       state;
    };

    Message message;
    float tabletWidth;
    float tabletHeight;
    uint32_t maxPressure;

};

std::shared_ptr<OtdPenInput> OtdPenInput::Factory()
{
    auto opi = std::make_shared<OtdPenInputWin32>();
    if (opi->SupportsPenInput()) {
        return opi;
    }
    return nullptr;
}

OtdPenInputWin32::OtdPenInputWin32()
:   LOG(std::make_unique<logging::Logger>("otdpen")),
    tabletVid(0),
    tabletPid(0),
    reportWatchdog(0),
    isInProximity(false),
    xPos(0.0f),
    yPos(0.0f),
    pressure(0.0f),
    running(true),
    otdIpcMsgHandle(INVALID_HANDLE_VALUE),
    otdIpcOverlap(),
    message(),
    tabletWidth(0.0f),
    tabletHeight(0.0f),
    maxPressure(0)
{
    // TODO - initialise the VID and PID from the prefs file

    otdIpcMsgHandle = CreateFileW(L"\\\\.\\pipe\\com.fredemmott.openkneeboard.OTDIPC/v0.1",
        GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (otdIpcMsgHandle == INVALID_HANDLE_VALUE) {
        LOGW("Failed to open IPC message pipe to OpenTabletDriver. Exclusive/background pen input will not work.");
        LOGI("Check OTD installed? running? OpenKneeboard OTD-IPC plugin installed? enabled?");
        return;
    }

    memset(&otdIpcOverlap, 0, sizeof(otdIpcOverlap));
    otdIpcOverlap.hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    if (otdIpcOverlap.hEvent == INVALID_HANDLE_VALUE) {
        LOGE("win32::CreateEvent() failed. ");
        return;
    }

    worker = std::make_unique<std::thread>([this]() { AsyncRunOtdPenIpc(); });
}

OtdPenInputWin32::~OtdPenInputWin32()
{
    running = false;
    CancelIoEx(otdIpcMsgHandle, &otdIpcOverlap);
    if (worker) worker->join();
    CloseHandle(otdIpcMsgHandle);
    otdIpcMsgHandle = INVALID_HANDLE_VALUE;
}

void OtdPenInputWin32::AsyncRunOtdPenIpc()
{
    while (running) {
        BOOL ok = ReadFile(otdIpcMsgHandle, &message, (DWORD)sizeof(message), nullptr, &otdIpcOverlap);
        DWORD e = ok ? 0 : GetLastError();

        if (e == ERROR_OPERATION_ABORTED) {
            break;
        }
        if (!ok && (e != ERROR_IO_PENDING)) {
            LOGE(fmt::format("Error {} when reading from message queue. Giving up.", e));
            break;
        }

        DWORD wait = WaitForSingleObject(otdIpcOverlap.hEvent, INFINITE);
        if (wait == WAIT_OBJECT_0) {
            DWORD bytesRead = 0;
            BOOL ok = GetOverlappedResult(otdIpcMsgHandle, &otdIpcOverlap, &bytesRead, FALSE);
            DWORD e = ok ? 0 : GetLastError();
            ResetEvent(otdIpcOverlap.hEvent);

            if (bytesRead == 0) {
                continue;
            }
            if (message.hdr.size != bytesRead) {
                LOGE(fmt::format("Ignoring message due to mismatched length ({}, {} received).", message.hdr.size, bytesRead));
                continue;
            }
            if (tabletVid && (message.hdr.messageType != 3) && ((message.hdr.vid != tabletVid) || (message.hdr.pid != tabletPid))) {
                LOGD(fmt::format("Ignoring message from non-selected tablet ({}/{}).", message.hdr.vid, message.hdr.pid));
                continue;
            }
            switch (message.hdr.messageType) {
            case 1:
                ConfigureTablet();
                break;
            case 2:
                UpdateState();
                break;
            case 3: // PING - ignored
                break;
            default:
                LOGE(fmt::format("Ignoring message with unknown type {}.", message.hdr.messageType));
                break;
            }

        }
    }

    CloseHandle(otdIpcOverlap.hEvent);
}

void OtdPenInputWin32::ConfigureTablet()
{
    if (message.devinfo.isValid) {
        tabletVid = message.hdr.vid;
        tabletPid = message.hdr.pid;
        tabletWidth = message.devinfo.maxX;
        tabletHeight = message.devinfo.maxY;
        maxPressure = message.devinfo.maxPressure;
    }
}

void OtdPenInputWin32::UpdateState()
{
    if (tabletWidth && tabletHeight) {
        LOGD(fmt::format("State: {} / {},{},{} / {},{}", message.state.nearProximity,
            message.state.positionValid, message.state.x, message.state.y,
            message.state.pressureValid, message.state.pressure));
        std::lock_guard<std::mutex> lock(stateMutex);
        reportWatchdog = 0;
        if (message.state.positionValid) {
            xPos = message.state.x / tabletWidth;
            yPos = message.state.y / tabletHeight;
        }
        if (message.state.pressureValid) {
            pressure = (float)message.state.pressure / maxPressure;
        }
        isInProximity = message.state.nearProximity;
    }
}

bool OtdPenInputWin32::GetPenState(float& x, float& y, float& p)
{
    std::lock_guard<std::mutex> lock(stateMutex);
    if (++reportWatchdog > 30) isInProximity = false;
    if (isInProximity) {
        x = xPos;
        y = yPos;
        p = pressure;
        return true;
    }
    return false;
}

#else

std::shared_ptr<OtdPenInput> OtdPenInput::Factory()
{
    return nullptr;
}

#endif

}
