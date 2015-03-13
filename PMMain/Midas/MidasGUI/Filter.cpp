#include "Filter.h"

void Filter::addDataAsInput(std::string name, boost::any value)
{
    inputData[name] = value;
}

void Filter::setInput(filterDataMap input)
{
    inputData = input;
}

filterDataMap Filter::getOutput()
{
    return outputData;
}

filterStatus Filter::getFilterStatus()
{
    return status;
}

filterError Filter::getFilterError()
{
    return error;
}

void Filter::setFilterStatus(filterStatus status)
{
    this->status = status;
}

void Filter::setFilterError(filterError error)
{
    this->error = error;
}

filterDataMap Filter::getInput()
{
    return inputData;
}

void Filter::setOutput(filterDataMap output)
{
    outputData = output;
}

void Filter::clearOutput(void)
{
    outputData = filterDataMap();
}

filterError Filter::updateBasedOnProfile()
{
    return NO_FILTER_ERROR;
}