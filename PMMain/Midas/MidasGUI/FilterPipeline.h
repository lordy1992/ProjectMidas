#pragma once
#include <list>
#include "Filter.h"

/**
 * The FilterPipeline class represents a linear sequence of filters through which
 * data can flow. The data enters the pipeline and is passed into the first filter. The
 * output of the first filter is piped into the second, and so on.
 */
class FilterPipeline
{
public:
    /* Basic Construction/Destruction */
    FilterPipeline() { }
    ~FilterPipeline() { }
    
    /**
     * Registers a new filter with the pipeline. Adds it to the end of the pipeline.
     * 
     * @param filter The filter to add to the pipeline. The input of the filter must match
     * the output of the previous filter in the pipeline.
     */
    void registerFilter(Filter* filter);

    /**
     * Starts the pipeline with the given input.
     *
     * @param input A map of name-value pairs that will be passed as the input to the first
     * filter in the pipeline.
     */
    void startPipeline(filterDataMap input);

    /**
     * Returns the list of registered filters.
     *
     * @return The list of registered filters.
     */
    std::list<Filter*>* getFilters(void);

private:
    std::list<Filter*> filters;
};

