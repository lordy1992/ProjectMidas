#include "MidasThread.h"
#include "MidasMain.h"
#include <qdebug.h>
#include <Windows.h>
#include "RingData.h"

MidasThread::MidasThread(SequenceDisplayer *sequenceDisplayerGui, InfoIndicator *infoIndicator)
    : sequenceDisplayer(sequenceDisplayerGui), infoIndicator(infoIndicator)
{
}

MidasThread::~MidasThread()
{
}

void MidasThread::run()
{
    qDebug() << "running spawned thread.";
    threadEmitString("test within run func.");
    midasMain(this, sequenceDisplayer, infoIndicator);

    for (int i = 0; i < 100000; i++)
    {
        emit outputCount(i);
        Sleep(25);
    }

    return;
}

void MidasThread::emitInfo()
{
    emit outputCount(888);
}

void MidasThread::threadEmitString(std::string str)
{
    emit emitString(str);
}

void MidasThread::threadEmitStateString(std::string str)
{
    emit emitStateString(str);
}


std::vector<ringData> MidasThread::readKeyboardSetupFile()
{
    
    std::ifstream readRingData("C:/Users/Owen/Documents/keyboardData.txt");
    std::string temp;
    int vect_num = 0;
    int ringInOutSwitch = 0; //0 = ringIn , 1 = ringOut 
    bool holdkey_check = false;

    ringData::keyboard_value key_temp; //lvl1
    std::vector<ringData::keyboard_value> tempRingInData[100]; //lvl2
    std::vector<ringData::keyboard_value> tempRingOutData[100]; //lvl2
    ringData ringTemp; //lvl3
    std::vector<ringData> keyboardData; //lvl4


    //readRingData.open();
    if (readRingData.is_open())
    {
        while (!readRingData.eof())
        {
            getline(readRingData, temp);
            char* temp_char = (char*)malloc(sizeof(char)*(temp.length() + 1));
            strcpy(temp_char, temp.c_str());

            for (int i = 0; i < temp.length(); i++)
            {

                if (temp_char[i] != char(32) && holdkey_check == false)
                {

                    key_temp.main = temp_char[i];


                    if (temp_char[i + 1] == char(32))
                    {
                        holdkey_check = false;

                        if (ringInOutSwitch == 0)
                            tempRingInData[vect_num].push_back(key_temp);
                        else
                            tempRingOutData[vect_num].push_back(key_temp);
                    }
                    else
                    {
                        holdkey_check = true;
                    }
                }
                else if (holdkey_check == true)
                {

                    key_temp.hold = temp_char[i];

                    if (ringInOutSwitch == 0)
                        tempRingInData[vect_num].push_back(key_temp);
                    else
                        tempRingOutData[vect_num].push_back(key_temp);

                    holdkey_check = false;
                    std::cout << "loading!!!!!\n";
                }

            }

            if (ringInOutSwitch == 0)
            {
                std::cout << "Ring In Vector!!!!!\n";
                ringTemp.setRingInVector(tempRingInData[vect_num]);
                ringInOutSwitch = 1;
            }
            else if (ringInOutSwitch == 1)
            {
                std::cout << "Ring Out Vector!!!!!\n";
                ringTemp.setRingOutVector(tempRingOutData[vect_num]);

                keyboardData.push_back(ringTemp);
                ringInOutSwitch = 0;
            }

            vect_num++;
        }


    }
    else
    {
        std::cout << "Error in opening file!";
    }
    readRingData.close();

    return keyboardData;
}
