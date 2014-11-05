#include "FilterPipeline.h"

void FilterPipeline::registerFilter(Filter* filter)
{
    filters.push_back(filter);
}

void FilterPipeline::startPipeline(filterDataMap input)
{
    std::list<Filter*>::iterator filterIt;
    filterDataMap nextInput = input;

    for (filterIt = filters.begin(); filterIt != filters.end(); filterIt++)
    {
        Filter* currentFilter = *filterIt;
        currentFilter->setInput(nextInput);
        currentFilter->process();
        if (currentFilter->getFilterStatus() == filterStatus::END_CHAIN) 
        {
            break;
        }
        else if (currentFilter->getFilterStatus() == filterStatus::ERROR)
        {
            // TODO: Handle error
            break;
        }

        nextInput = currentFilter->getOutput();
    }
}