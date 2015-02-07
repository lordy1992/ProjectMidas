/*
 * HubSim.hpp
 *
 * Simulator version of Hub module in Myo SDK
 */
#pragma once

#include "SimEvent.h"
#include "MyoSim.h"
#include "myo/myo.hpp"
#include <vector>
#include <map>

namespace myoSim {

    class DeviceListener;

    enum EventType { ET_PAIRED, ET_UNPAIRED, ET_CONNECTED, ET_DISCONNECTED, ET_ARM_RECOGNIZED, ET_ARM_LOST, ET_SYNC_DAT, ET_POSE };

    /// @brief A Hub provides access to one or more Myo instances.
    class Hub {
    public:
        /// Construct a hub.
        Hub(const std::string& applicationIdentifier = "");

        /// Deallocate any resources associated with a Hub.
        ~Hub();

        /// Wait for a Myo to become paired, or time out after \a timeout_ms milliseconds if provided.
        /// If \a timeout_ms is zero, this function blocks until a Myo is found.
        /// This function must not be called concurrently with run() or runOnce().
        Myo* waitForMyo(unsigned int milliseconds = 0);

        /// Register a listener to be called when device events occur.
        void addListener(DeviceListener* listener);

        /// Remove a previously registered listener.
        void removeListener(DeviceListener* listener);

        /// Run the event loop for the specified duration (in milliseconds).
        void run(unsigned int duration_ms);

        /// Run the event loop until a single event occurs, or the specified duration (in milliseconds) has elapsed.
        void runOnce(unsigned int duration_ms);

    protected:
        void onDeviceEvent(SimEvent simEvent);
        Myo* lookupMyo(unsigned int myoIdentifier) const;

        Myo* addMyo(unsigned int identifier);

        std::vector<Myo*> myos;
        std::vector<DeviceListener*> listeners;

        /// @endcond

    private:
        // Not implemented
        Hub(const Hub&);
        Hub& operator=(const Hub&);
        float extractFloat(TCHAR* bytes);
        unsigned short extractUnsignedShort(TCHAR* bytes);
        static std::map<EventType, myoSimEvent> eventTypeToEventMap;
    };
} // namespace myo
