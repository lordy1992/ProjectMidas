#include "AveragingFilter.h"


AveragingFilter::AveragingFilter() : avgCount(DEFAULT_AVG_COUNT) {}

AveragingFilter::AveragingFilter(unsigned int avgCount) : avgCount(avgCount) {}

AveragingFilter::~AveragingFilter() {}

void AveragingFilter::setAvgCount(unsigned int avgCount)
{
    this->avgCount = avgCount;
}

unsigned int AveragingFilter::getAvgCount() const
{
    return avgCount;
}

void AveragingFilter::process()
{
    filterDataMap input = Filter::getInput();

    float quatX = boost::any_cast<float>(input["quatDataX"]);
    float quatY = boost::any_cast<float>(input["quatDataY"]);
    float quatZ = boost::any_cast<float>(input["quatDataZ"]);
    float quatW = boost::any_cast<float>(input["quatDataW"]);
    float accelX = boost::any_cast<float>(input["accelDataX"]);
    float accelY = boost::any_cast<float>(input["accelDataY"]);
    float accelZ = boost::any_cast<float>(input["accelDataZ"]);
    float gyroX = boost::any_cast<float>(input["gyroDataX"]);
    float gyroY = boost::any_cast<float>(input["gyroDataY"]);
    float gyroZ = boost::any_cast<float>(input["gyroDataZ"]);

    insertAvgElement(quatX, quatXQueue);    
    insertAvgElement(quatY, quatYQueue);
    insertAvgElement(quatZ, quatZQueue);
    insertAvgElement(quatW, quatWQueue);
    insertAvgElement(accelX,accelXQueue);
    insertAvgElement(accelY,accelYQueue);
    insertAvgElement(accelZ,accelZQueue);
    insertAvgElement(gyroX, gyroXQueue);
    insertAvgElement(gyroY, gyroYQueue);
    insertAvgElement(gyroZ, gyroZQueue);















    if (cmd == 0)
    {
        command.type = KEYBOARD_COMMAND;
        command.kbd = UNDO;
    }
    else if (cmd == 1)
    {
        command.type = KEYBOARD_COMMAND;
        command.kbd = COPY;
    }
    else
    {
        command.type = MOUSE_COMMAND;
        command.mouse = LEFT_CLICK;
    }

    filterDataMap output;
    output[COMMAND_INPUT] = command;
    Filter::setOutput(output);
}

void AveragingFilter::insertAvgElement(float elem, std::queue<float>q)
{
    q.push(elem);
    while (q.size() > avgCount) {
        // discard element so that average is only based on
        // avgCount elements.
        q.pop();
    }
}

float AveragingFilter::calcAvg(std::queue<float>q)
{
    return -1; // TODO.
}