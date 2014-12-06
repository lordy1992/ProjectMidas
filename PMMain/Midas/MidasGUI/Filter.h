#pragma once
#include <string>
#include <map>
#include <boost/any.hpp>

/**
 * An enumeration of the possible states of error for a filter. It includes
 * a successful run as well as possible error states.
 */
enum class filterError
{
    NO_FILTER_ERROR,    /**< No errors while running the filter. */
    INVALID_INPUT,      /**< Invalid input passed to the filter. */
    PROCESSING_ERROR    /**< An error occurred while processing. */
};

/**
 * An enumeration of the possible statuses for a filter after it is finished
 * running.
 */
enum class filterStatus
{
    OK,             /**< Successful case */
    END_CHAIN,      /**< End the pipeline after this filter, no output */
    FILTER_ERROR    /**< An error occurred in the filter, check the error field for specific errors. */
};

typedef std::map<std::string, boost::any> filterDataMap;

/**
 * Filter is an abstract class that is a component in a data flow pipeline. It
 * takes in named data, processes the data, and then outputs the data. Filters can
 * act as simple functions or they can be more complex, involving state or accumulated
 * data.
 */
class Filter
{
public:
    /* Basic Construction/Destruction */
    Filter() { }
    ~Filter() { }

    /**
     * This handles the processing of the data. It must get the named input, 
     * do something with the data, and then set the output. It is the responsibility
     * of the implementing filter to properly set the output, status and error code
     * before exiting this method.
     */
    virtual void process() = 0;
    
    /**
     * Adds a new name-value pair to the input map of this filter. This
     * value can be retrieved by name in the process() function.
     *
     * @param name The name of the parameter.
     * @param value The value corresponding with the name.
     */
    void addDataAsInput(std::string name, boost::any value);

    /**
     * Sets the input map for this filter. This can then be retrieved by
     * the process() function.
     *
     * @param input A map from string to boost::any of name-value pairs.
     */
    void setInput(filterDataMap input);

    /**
     * Retrieve the output from the filter.
     *
     * @return A map of name-value pairs.
     */
    filterDataMap getOutput();

    /**
     * Retrieve the status from the filter.
     *
     * @return The status of the filter after completion.
     */
    filterStatus getFilterStatus();

    /**
     * Retrieve an error code from the filter.
     *
     * @return The error code of the filter after completion.
     */
    filterError getFilterError();

protected:
    /**
     * Retrieve the input to the filter. Only a subclass of Filter can access
     * the provided input.
     *
     * @return The input to the filter.
     */
    filterDataMap getInput();

    /**
     * Set the output of the filter.
     *
     * @param output The map of name-value pairs.
     */
    void setOutput(filterDataMap output);

    /**
    * Clear the output of the filter.
    */
    void clearOutput(void);

    /**
     * Set the status of the filter.
     *
     * @param status The status of the filter.
     */
    void setFilterStatus(filterStatus status);

    /**
     * Set the error code of the filter.
     *
     * @param error The error code of the filter.
     */
    void setFilterError(filterError error);

private:
    filterDataMap inputData;
    filterDataMap outputData;
    filterStatus status;
    filterError error;
};

