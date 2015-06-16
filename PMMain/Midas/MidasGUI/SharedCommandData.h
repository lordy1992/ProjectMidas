#ifndef _SHARED_COMMAND_DATA_H
#define _SHARED_COMMAND_DATA_H

#include <mutex>
#include <queue>
#include "MidasCommon.h"
#include "CommandData.h"
#include "Filter.h"

#define COMMAND_INPUT "command"
#define VELOCITY_INPUT "velocity"
#define ISCONNECTED_INPUT "isConnected"
#define DELTA_INPUT "deltaInput"

/**
 * Acts as the shared data between the main thread and the device threads. Contains the 
 * queue of mouse and keyboard commands for the main thread to send to Windows, and contains 
 * the mouse velocity information and rssi information.
 */
class SharedCommandData : public Filter
{
public:
    SharedCommandData() : Filter(), mouseVelocity() { }

    /**
     * Adds a command to the queue of commands. If another thread is modifying the command queue, 
     * this will block until the other thread is done.
     *
     * @param dat The data that will be added to the queue.
     */
    void addCommand(CommandData dat);

    /**
    * Adds a command to the queue of commands. If another thread is modifying the command queue,
    * this will exit without adding the command and return false. Otherwise, it will add the 
    * command and return true.
    *
    * @param dat The data that will be added to the queue.
    * @return True if a command was successfully added, otherwise false.
    */
    bool tryAddCommand(CommandData dat);

    /**
    * Takes a command from the queue of commands, removing the command from the queue and 
    * setting the supplied reference to the command. If another thread is modifying the command queue,
    * this will block until the other thread is done.
    *
    * @param dat This will be set to the next command in the queue.
    * @return True if a command was successfully taken, false if the queue is empty.
    */
    bool consumeCommand(CommandData& dat);

    /**
    * Takes a command from the queue of commands, removing the command from the queue and
    * setting the supplied reference to the command. If another thread is modifying the command queue,
    * this will exit without taking the command and return false. Otherwise, it will take the 
    * command and return true.
    *
    * @param outCommandData This will be set to the next command in the queue.
    * @return True if the command was successfully taken from the queue, otherwise false.
    */
    bool tryConsumeCommand(CommandData& outCommandData);

    /**
     * Sets the cursor velocity in the shared data. If another thread is accessing/changing
     * the velocity, this will block until the other thread is done. 
     *
     * @param velocity The new velocity of the cursor, as a percentage of total velocity.
     */
    void setVelocity(point velocity);

    /**
    * Sets the cursor velocity in the shared data. If another thread is accessing/changing
    * the velocity, this will return false and not update the velocity. Otherwise, it will 
    * return true and set the cursor velocity.
    *
    * @param velocity The new velocity of the cursor, as a percentage of total velocity.
    * @return True if the velocity was successfully set, otherwise false.
    */
    bool trySetVelocity(point velocity);

    /**
     * Return the velocity in the shared data. If another thread is accessing/changing
     * the velocity, this will block until the other thread is done.
     *
     * @return The velocity in the SCD.
     */
    point getVelocity();

    /**
    * Return the velocity in the shared data. If another thread is accessing/changing
    * the velocity, this will return false and not retrieve the velocity. Otherwise, it will 
    * return true and set the cursor velocity.
    *
    * @param outVelocity The retrieved velocity from the SCD will be placed here.
    * @return True if the velocity is successfully received, false otherwise.
    */
    bool tryGetVelocity(point& outVelocity);

	void SharedCommandData::setDelta(vector2D delta);

	bool SharedCommandData::trySetDelta(vector2D delta);

	vector2D SharedCommandData::getDelta();

	bool SharedCommandData::tryGetDelta(vector2D& outDelta);

    /**
     * Returns whether the device is connected or not
     * 
     * @return A boolean for whether or not the device is connected
     */
    bool getIsConnected(void);

    /**
     * Sets the device connected flag
     *
     * @param bool isConnected
     */
    void setIsConnected(bool connected);

    /**
     * Returns true if the command queue is empty, otherwise false.
     *
     * @return True if the command queue is empty, otherwise false.
     */
    bool isCommandQueueEmpty();

    /**
     * The SCD can be put into a pipeline as a Filter, but it never passes data on; it always ends
     * the pipeline. It will accept command data and cursor velocity input and modify the shared
     * data.
     */
    void process();
    
    /**
     * Empties the command queue. If another thread is accessing/modifying the command queue, this 
     * will block until the thread is done.
     */
    void empty();

    /**
    * Empties the command queue. If another thread is accessing/modifying the command queue, this
    * will return false and not modify the queue. Otherwise, it will return true and empty the queue.
    *
    * @return True if the command queue was emptied. Otherwise, false.
    */
    bool tryEmpty();

private:
    point mouseVelocity;
    float rssiAVG;
    bool  isConnected;

	// point to indicate offset from current mouse position, while a pose is being held 
	vector2D mouseDelta;
	std::mutex mouseDeltaMutex;
   
    // together, these 2 vars define which wheel/RingData the keyboard should show on the GUI
    std::queue<CommandData> commandQueue;
    std::mutex commandQueueMutex;
    std::mutex velocityMutex;
    std::mutex kybdGuiSelMutex;
    std::mutex myoOrientationMutex;
    std::mutex keySelectAngleMutex;
    std::mutex rssiMutex;
    std::mutex isConnectedMutex;

    void extractCommand(boost::any value);
    void extractPoint(boost::any value);
    void extractIsConnected(boost::any value);
	void extractVector2D(boost::any value);
};

#endif /* _SHARED_COMMAND_DATA_H */