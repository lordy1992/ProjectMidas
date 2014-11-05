#pragma once
#include <string>
#include <map>
#include <boost/any.hpp>

enum class filterError
{
    NO_FILTER_ERROR,
    INVALID_INPUT,
    PROCESSING_ERROR
};

enum class filterStatus
{
    OK,  /* Successful case */
    END_CHAIN,  /* End the pipeline after this filter, no output */
    FILTER_ERROR  /* An error occurred in the filter, check the error field for 
            * specific errors. */
};

typedef std::map<std::string, boost::any> filterDataMap;

class Filter
{
public:
    Filter() { }
    ~Filter() { }

    virtual void process() = 0;
    
    void addDataAsInput(std::string name, boost::any value);
    void setInput(filterDataMap input);
    filterDataMap getOutput();
    filterStatus getFilterStatus();
    filterError getFilterError();

protected:
    filterDataMap getInput();
    void setOutput(filterDataMap output);
    void setFilterStatus(filterStatus status);
    void setFilterError(filterError error);

private:
    filterDataMap inputData;
    filterDataMap outputData;
    filterStatus status;
    filterError error;
};

