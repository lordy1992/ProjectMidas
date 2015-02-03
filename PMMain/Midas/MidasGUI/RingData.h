#include <vector>
#pragma once
class ringData
{
public:
    ringData();
    ~ringData();

    struct keyboard_value
    {
        char main, hold;
     //   keyboard_value(char mainVal = '\0', char holdVal = '\0') : main(mainVal), hold(holdVal) { }
    };


    std::vector<keyboard_value>* getRingInVectorHandle();
    std::vector<keyboard_value>* getRingOutVectorHandle();

    void setRingInVector(std::vector<keyboard_value> ringInVal);
    void setRingOutVector(std::vector<keyboard_value> ringOutVal);

private:

    std::vector<keyboard_value> *ringInVector;
    std::vector<keyboard_value> *ringOutVector;

};


