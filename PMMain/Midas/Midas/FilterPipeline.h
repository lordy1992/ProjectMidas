#pragma once
#include <list>
#include "Filter.h"

class FilterPipeline
{
public:
    /* Basic Construction/Destruction */
    FilterPipeline() { }
    ~FilterPipeline() { }
    
    void registerFilter(Filter* filter);
    void startPipeline(filterDataMap input);

private:
    std::list<Filter*> filters;
};

