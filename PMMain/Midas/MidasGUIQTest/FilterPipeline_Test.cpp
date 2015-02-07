#include "FilterPipeline.h"
#include <gtest\gtest.h>

#define TEST_KEY_1 "test_key_1"
#define TEST_KEY_2 "test_key_2"
#define TEST_KEY_3 "test_key_3"

#define TEST_VALUE_1 "test_value_1"
#define TEST_VALUE_2 "test_value_2"
#define TEST_VALUE_3 "test_value_3"

class TestFilter : public Filter
{
public:
    TestFilter(filterStatus testStatus, filterError testError)
    {
        this->testStatus = testStatus;
        this->testError = testError;
    }

    void process()
    {
        filterDataMap input = Filter::getInput();
        std::string val1 = boost::any_cast<std::string>(input[TEST_KEY_1]);
        
        ASSERT_EQ(TEST_VALUE_1, val1);

        Filter::setFilterError(testError);
        Filter::setFilterStatus(testStatus);

        filterDataMap output;
        std::string outValue2 = TEST_VALUE_2;
        output[TEST_KEY_2] = outValue2;

        Filter::setOutput(output);
    }

private:
    filterStatus testStatus;
    filterError testError;
};

class UnreachableFilter : public Filter
{
public:
    void process()
    {
        // This should never be called. This filter is used to 
        // test that the pipeline breaks when an error or status of 
        // END_CHAIN is encountered.
        ASSERT_EQ(0, 1);
    }
};

class TestFilter2 : public Filter
{
public:
    void process()
    {
        filterDataMap input = Filter::getInput();
        std::string inputVal = boost::any_cast<std::string>(input[TEST_KEY_2]);

        ASSERT_EQ(TEST_VALUE_2, inputVal);

        Filter::setFilterError(filterError::NO_FILTER_ERROR);
        Filter::setFilterStatus(filterStatus::OK);

        filterDataMap output;
        std::string outValue = TEST_VALUE_3;
        output[TEST_KEY_3] = outValue;

        Filter::setOutput(output);
    }
};

TEST(FilterPipelineTest1, testBasicPipeline)
{
    filterDataMap filterInput;
    std::string inValue1 = TEST_VALUE_1;
    filterInput[TEST_KEY_1] = inValue1;

    TestFilter testFilter(filterStatus::OK, filterError::NO_FILTER_ERROR);
    FilterPipeline filterPipeline;

    filterPipeline.registerFilter(&testFilter);

    filterPipeline.startPipeline(filterInput);

    ASSERT_EQ(TEST_VALUE_2, boost::any_cast<std::string>(testFilter.getOutput()[TEST_KEY_2]));
    ASSERT_EQ(filterStatus::OK, testFilter.getFilterStatus());
    ASSERT_EQ(filterError::NO_FILTER_ERROR, testFilter.getFilterError());
}

TEST(FilterPipelineTest2, testErrorInFilter)
{
    filterDataMap filterInput;
    std::string inValue1 = TEST_VALUE_1;
    filterInput[TEST_KEY_1] = inValue1;

    TestFilter testFilter(filterStatus::FILTER_ERROR, filterError::PROCESSING_ERROR);
    UnreachableFilter secondFilter;
    FilterPipeline filterPipeline;

    filterPipeline.registerFilter(&testFilter);
    filterPipeline.registerFilter(&secondFilter);

    filterPipeline.startPipeline(filterInput);

    ASSERT_EQ(0, secondFilter.getOutput().size());
    ASSERT_EQ(filterStatus::FILTER_ERROR, testFilter.getFilterStatus());
    ASSERT_EQ(filterError::PROCESSING_ERROR, testFilter.getFilterError());
}

TEST(FilterPipelineTest3, testEndChain)
{
    filterDataMap filterInput;
    std::string inValue1 = TEST_VALUE_1;
    filterInput[TEST_KEY_1] = inValue1;

    TestFilter testFilter(filterStatus::END_CHAIN, filterError::NO_FILTER_ERROR);
    UnreachableFilter secondFilter;
    FilterPipeline filterPipeline;

    filterPipeline.registerFilter(&testFilter);
    filterPipeline.registerFilter(&secondFilter);

    filterPipeline.startPipeline(filterInput);

    ASSERT_EQ(0, secondFilter.getOutput().size());
    ASSERT_EQ(TEST_VALUE_2, boost::any_cast<std::string>(testFilter.getOutput()[TEST_KEY_2]));
    ASSERT_EQ(filterStatus::END_CHAIN, testFilter.getFilterStatus());
    ASSERT_EQ(filterError::NO_FILTER_ERROR, testFilter.getFilterError());
}

TEST(FilterPipelineTest4, testFilterChain)
{
    filterDataMap filterInput;
    std::string inValue1 = TEST_VALUE_1;
    filterInput[TEST_KEY_1] = inValue1;

    TestFilter testFilter(filterStatus::OK, filterError::NO_FILTER_ERROR);
    TestFilter2 secondFilter;
    FilterPipeline filterPipeline;

    filterPipeline.registerFilter(&testFilter);
    filterPipeline.registerFilter(&secondFilter);

    filterPipeline.startPipeline(filterInput);

    ASSERT_EQ(1, secondFilter.getOutput().size());
    ASSERT_EQ(TEST_VALUE_2, boost::any_cast<std::string>(testFilter.getOutput()[TEST_KEY_2]));
    ASSERT_EQ(TEST_VALUE_3, boost::any_cast<std::string>(secondFilter.getOutput()[TEST_KEY_3]));
    ASSERT_EQ(filterStatus::OK, testFilter.getFilterStatus());
    ASSERT_EQ(filterError::NO_FILTER_ERROR, testFilter.getFilterError());
    ASSERT_EQ(filterStatus::OK, secondFilter.getFilterStatus());
    ASSERT_EQ(filterError::NO_FILTER_ERROR, secondFilter.getFilterError());
}