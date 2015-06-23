#ifndef _CONTROL_STATE_H
#define _CONTROL_STATE_H

#include <mutex>
#include "MidasCommon.h"
#include "SharedCommandData.h"

/**
 * This class keeps track of the current state (or mode) of the Midas application. This
 * state is used to determine the actions the user can or cannot perform. For example, in mouse mode,
 * the user may move around the cursor with the device. In locked mode, the cursor will
 * not move.
 */
class ControlState
{
public:

    /**
     * The constructor for ControlState.
     * The contructor must have a handle on the SharedCommandData so that 
     * unprocessed commands can be removed from the command queue when the 
     * state of the application changes.
     *
     * @param SCDHandle A handle to the SharedCommandData object that is used by the
     * main thread and device threads.
     */
    ControlState(SharedCommandData* SCDHandle);
    ~ControlState();

    /**
     * Changes the mode of the Midas application. This is a thread-safe operation, but
     * it blocks on the command queue mutex. If the command queue is being modified by another 
     * thread, this will not return until the other thread is finished.
     *
     * @param mode The new mode.
     */
    void setMode(midasMode mode);

    /**
     * Changes the mode of the Midas application. If the command queue is being modified
     * by another thread, this will exit right away with a return value of false. Otherwise,
     * it will change the mode and return true.
     *
     * @param mode The new mode.
     * @return True if the mode was successfully changed; false otherwise.
     */
    bool trySetMode(midasMode mode);

    /**
     * Gets the current mode of the Midas application.
     *
     * @return The current mode.
     */
    midasMode getMode();

	void setProfile(std::string profile);
	std::string getProfile();

	SharedCommandData* getSCD() { return SCDHandle; }

private:
    midasMode currentMode;
	std::string currentProfile;
	//std::mutex profileMutex;
    SharedCommandData *SCDHandle;
};

#endif /* _CONTROL_STATE_H */