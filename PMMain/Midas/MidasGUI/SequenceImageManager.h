#ifndef SEQUENCE_IMAGE_MANAGER_H
#define SEQUENCE_IMAGE_MANAGER_H

#include <map>
#include <vector>
#include "SequenceDisplayer.h"

/* Rel. Image Paths */
#define FINGER_SPREAD_LATER_PATH  "Resources\\BW200PxImgs\\fingerSpread.jpg"
#define FINGER_SPREAD_NEXT_PATH   "Resources\\Post200PxImgs\\fingerSpread.jpg"
#define FIST_LATER_PATH           "Resources\\BW200PxImgs\\fist.jpg"
#define FIST_NEXT_PATH            "Resources\\Post200PxImgs\\fist.jpg"
//#define PINKY_TO_THUMB_LATER_PATH "Resources\\BW200PxImgs\\pinkyToThumb.jpg"
//#define PINKY_TO_THUMB_NEXT_PATH  "Resources\\Post200PxImgs\\pinkyToThumb.jpg"
#define DOUBLE_TAP_LATER_PATH     "Resources\\BW200PxImgs\\doubleTap.jpg"
#define DOUBLE_TAP_NEXT_PATH      "Resources\\Post200PxImgs\\doubleTap.jpg"
#define WAVE_IN_LATER_PATH        "Resources\\BW200PxImgs\\waveIn.jpg"
#define WAVE_IN_NEXT_PATH         "Resources\\Post200PxImgs\\waveIn.jpg"
#define WAVE_OUT_LATER_PATH       "Resources\\BW200PxImgs\\waveOut.jpg"
#define WAVE_OUT_NEXT_PATH        "Resources\\Post200PxImgs\\waveOut.jpg"
#define NO_HAND_LATER_PATH           "Resources\\Post200PxImgs\\noHand.jpg"
#define NO_HAND_NEXT_PATH            "Resources\\Post200PxImgs\\noHand.jpg"

#define POSE_LENGTH_IMMEDIATE_PATH "Resources\\icons\\PoseLenImmediate.png"
#define POSE_LENGTH_HOLD_PATH "Resources\\icons\\PoseLenHold.png"
#define POSE_LEN_SCALEDOWN_ICON 4

class SequenceImageManager
{
public:
    SequenceImageManager();
    ~SequenceImageManager();

    void loadImages();
    std::vector<sequenceImageSet> formSequenceSetFromIds(std::vector<int> ids, std::vector<PoseLength> lengths);

private:
    std::map<int, sequenceImageSet> idToImageMap;
};

#endif