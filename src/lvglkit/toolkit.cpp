/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <chrono>
#include <lvgl.h>
#include "toolkit.h"
#include "display.h"
#include "navitab/deferred.h"

static uint32_t elapsedMilliseconds() {
    static uint32_t t = 1000;
    return ++t;
}

namespace lvglkit {

Manager::Manager(std::shared_ptr<navitab::DeferredJobRunner<int>> c)
:   core(c),
    running(true),
    pendingCalls(0)
{
    looper = std::make_unique<std::thread>([this]() { RunLVGL(); });
}

Manager::~Manager()
{
    using namespace std::chrono_literals;

    running = false;
    looper->join();
    // don't destruct until all the RunLater() jobs have been dealt with
    bool done = false;
    while (!done) {
        std::this_thread::sleep_for(20ms);
        std::unique_lock<std::mutex> lock(nextTimerMutex);
        done = (pendingCalls == 0);
    }
}

void Manager::RunLVGL()
{
    using namespace std::chrono_literals;

    std::this_thread::sleep_for(20ms);
    auto start = std::chrono::steady_clock::now();
    auto last = start;
    uint32_t nextTick = 5;
    nextTimer = 10;
    while (running) {
        auto t = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t - last).count();
        last = t;
        if (elapsed >= nextTick) {
            lv_tick_inc(elapsed);
        } else {
            nextTick -= elapsed;
        }
        uint32_t nextTimerCopy = 1000;
        {
            std::unique_lock<std::mutex> lock(nextTimerMutex);
            if (elapsed >= nextTimer) {
                // LVGL is not thread safe, so the timer handler cannot run when another
                // thread is accessing LVGL. So we run the timer handler on the core thread
                // (where the rest of the LVGL accessed are carried out).
                ++pendingCalls;
                core->RunLater([this]() { DoTimerHandler(); });
            } else {
                nextTimer -= elapsed;
            }
            nextTimerCopy = nextTimer;
        }

        // now sleep until the first of the next scheduled wakeups
        auto sleepTime = std::chrono::milliseconds(std::min(nextTick, nextTimerCopy));
        std::this_thread::sleep_for(sleepTime);
    }
}

void Manager::DoTimerHandler()
{
    std::unique_lock<std::mutex> lock(nextTimerMutex);
    nextTimer = std::min(10u, lv_timer_handler()); // TODO - can we remove this 10ms minimum??
    --pendingCalls;
}

std::shared_ptr<Display> Manager::MakeDisplay(Display::Updater* u)
{
    return std::make_shared<DisplayWrapper>(u);
}




// The reference counter base class is used 

int RefCounter::libRefs = 0;

RefCounter::RefCounter()
{
    if (libRefs++ == 0) {
        lv_init();
    }
}

RefCounter::~RefCounter()
{
    if (--libRefs == 0) {
        lv_deinit();
    }
}

} // namespace lvglkit
