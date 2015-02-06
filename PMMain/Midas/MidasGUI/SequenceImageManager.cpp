#include "SequenceImageManager.h"
#include <QImage.h>
#include <qstring.h>

#include "myo\myo.hpp"

#ifdef USE_SIMULATOR
#include "MyoSimIncludes.hpp"
using namespace myoSim;
#else
using namespace myo;
#endif

SequenceImageManager::SequenceImageManager()
{
}


SequenceImageManager::~SequenceImageManager()
{
}

std::vector<sequenceImageSet> SequenceImageManager::formSequenceSetFromIds(std::vector<int> ids)
{
    std::vector<int>::iterator it;
    std::vector<sequenceImageSet> sequenceImages;

    for (it = ids.begin(); it != ids.end(); ++it)
    {
        if (idToImageMap.find(*it) != idToImageMap.end())
        {
            sequenceImages.push_back(idToImageMap[*it]);
        }
        else
        {
            return std::vector<sequenceImageSet>();
        }
    }

    return sequenceImages;
}

void SequenceImageManager::loadImages()
{
    QImage fingerSpreadLater(QString(FINGER_SPREAD_LATER_PATH));
    QImage fingerSpreadNext(QString(FINGER_SPREAD_NEXT_PATH));
    QImage fistLater(QString(FIST_LATER_PATH));
    QImage fistNext(QString(FIST_NEXT_PATH));
    QImage pinkyToThumbLater(QString(PINKY_TO_THUMB_LATER_PATH));
    QImage pinkyToThumbNext(QString(PINKY_TO_THUMB_NEXT_PATH));
    QImage waveInLater(QString(WAVE_IN_LATER_PATH));
    QImage waveInNext(QString(WAVE_IN_NEXT_PATH));
    QImage waveOutLater(QString(WAVE_OUT_LATER_PATH));
    QImage waveOutNext(QString(WAVE_OUT_NEXT_PATH));
    QImage noHandLater(QString(NO_HAND_LATER_PATH));
    QImage noHandNext(QString(NO_HAND_NEXT_PATH));
    
    sequenceImageSet fingerSpread;
    fingerSpread.actionTag = Pose::fingersSpread;
    fingerSpread.nextImage = QPixmap::fromImage(fingerSpreadNext);
    fingerSpread.laterImage = QPixmap::fromImage(fingerSpreadLater);
    idToImageMap[fingerSpread.actionTag] = fingerSpread;

    sequenceImageSet fist;
    fist.actionTag = Pose::fist;
    fist.nextImage = QPixmap::fromImage(fistNext);
    fist.laterImage = QPixmap::fromImage(fistLater);
    idToImageMap[fist.actionTag] = fist;

    sequenceImageSet pinkyToThumb;
    pinkyToThumb.actionTag = Pose::thumbToPinky;
    pinkyToThumb.nextImage = QPixmap::fromImage(pinkyToThumbNext);
    pinkyToThumb.laterImage = QPixmap::fromImage(pinkyToThumbLater);
    idToImageMap[pinkyToThumb.actionTag] = pinkyToThumb;

    sequenceImageSet waveIn;
    waveIn.actionTag = Pose::waveIn;
    waveIn.nextImage = QPixmap::fromImage(waveInNext);
    waveIn.laterImage = QPixmap::fromImage(waveInLater);
    idToImageMap[waveIn.actionTag] = waveIn;

    sequenceImageSet waveOut;
    waveOut.actionTag = Pose::waveOut;
    waveOut.nextImage = QPixmap::fromImage(waveOutNext);
    waveOut.laterImage = QPixmap::fromImage(waveOutLater);
    idToImageMap[waveOut.actionTag] = waveOut;

    sequenceImageSet rest;
    rest.actionTag = Pose::rest;
    rest.nextImage = QPixmap::fromImage(noHandNext);
    rest.laterImage = QPixmap::fromImage(noHandLater);
    idToImageMap[rest.actionTag] = rest;

}
