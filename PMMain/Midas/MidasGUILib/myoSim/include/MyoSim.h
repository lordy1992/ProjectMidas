#pragma once
#include <Windows.h>
#include <myo\cxx\Myo.hpp>

namespace myoSim {
    class Myo
    {
    public:
        // Pass-through enum to Myo Vibration Type.
        enum VibrationType {
            vibrationShort = libmyo_vibration_short,
            vibrationMedium = libmyo_vibration_medium,
            vibrationLong = libmyo_vibration_long
        };

        Myo(unsigned int id);
        ~Myo();

        void vibrate(VibrationType type);
        void requestRssi() const;

        bool connectToPipe(unsigned int timeout);
        bool readFromPipe(TCHAR* buffer, unsigned int numBytes, DWORD* actualBytes);
        unsigned int getIdentifier() const;
        void setReadTimeout(unsigned int timeout);
        DWORD getReadTimeout();

    private:
        unsigned int identifier;
        HANDLE pipe;
        LPTSTR pipeName;
    };
}

