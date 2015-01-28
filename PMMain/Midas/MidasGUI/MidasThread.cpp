#include "MidasThread.h"
#include "MidasMain.h"
#include <qdebug.h>
#include <Windows.h>
#include "RingData.h"

MidasThread::MidasThread()
{
}

MidasThread::~MidasThread()
{

}

void MidasThread::run()
{
    qDebug() << "running spawned thread.";
    threadEmitString("test within run func.");
    midasMain(this);

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
    
    std::ifstream readRingData;
    std::string temp;
    int ring_num = 0;
    int count = 0;
    int ringDataSet = 0;
    bool holdkey_check = false;

    ringData::keyboard_value key_temp; //lvl1
    std::vector<ringData::keyboard_value> tempRingData; //lvl2
    ringData ringTemp; //lvl3
    std::vector<ringData> keyboardData; //lvl4


    readRingData.open("keyboardData.txt");
    if (readRingData.is_open())
    {
        while (!readRingData.eof())
        {
            getline(readRingData, temp);

            char* temp_char = new char[temp.length() + 1];
            std::strcpy(temp_char, temp.c_str());

            for (int i = 0; i < temp.length();  i++)
            {

                if (temp_char[i] = !' ' && holdkey_check == false)
                {
                    key_temp.main = temp_char[i];
                

                    if (temp_char[i + 1] == ' ')
                    {
                        holdkey_check = false;
                        tempRingData.push_back(key_temp);
                    }
                    else
                        holdkey_check = true;
                }
                else if (holdkey_check = true)
                {
                    key_temp.hold = temp_char[i];
                    tempRingData.push_back(key_temp);
                    holdkey_check = false;
                }

            }

            if (count == 0)
            {
                ringTemp.setRingInVector(tempRingData); 
                count++;
            }
            else if (count == 1)
            {
                ringTemp.setRingOutVector(tempRingData);
                count--;
                keyboardData.push_back(ringTemp);
            }
           
           
        }

       
    }
    else
    {
        std::cout << "Error in opening file!";
    }
    readRingData.close();

    return keyboardData;
}