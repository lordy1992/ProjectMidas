#pragma once
#include <string>
#include <map>
#include <boost/any.hpp>

enum class filterError
{
    NO_ERROR,
    INVALID_INPUT,
    PROCESSING_ERROR
};

enum class filterStatus
{
    OK,  /* Successful case */
    END_CHAIN,  /* End the pipeline after this filter, no output */
    ERROR  /* An error occurred in the filter, check the error field for 
            * specific errors. */
};

typedef std::map<std::string, boost::any> filterDataMap;

class Filter
{
public:
    Filter();
    ~Filter();

    virtual void process() = 0;
    
    void addDataAsInput(std::string name, boost::any value);
    void setInput(filterDataMap input);
    filterDataMap getOutput();
    filterStatus getFilterStatus();
    filterError getFilterError();

protected:
    filterDataMap inputData;
    filterDataMap outputData;

    void setFilterStatus(filterStatus status);
    void setFilterError(filterError error);

private:
    filterStatus status;
    filterError error;
};

