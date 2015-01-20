#ifndef SEQUENCE_IMAGE_MANAGER_H
#define SEQUENCE_IMAGE_MANAGER_H

#include <map>

/* Rel. Image Paths */
#define FINGER_SPREAD_LATER_PATH  "Resources\\BW200PxImgs\\fingerSpread.jpg"
#define FINGER_SPREAD_NEXT_PATH   "Resources\\Post200PxImg\\fingerSpread.jpg"
#define FIST_LATER_PATH           "Resources\\BW200PxImgs\\fist.jpg"
#define FIST_NEXT_PATH            "Resources\\Post200PxImgs\\fist.jpg"
#define PINKY_TO_THUMB_LATER_PATH "Resources\\BW200PxImgs\\pinkyToThumb.jpg"
#define PINKY_TO_THUMB_NEXT_PATH  "Resources\\Post200PxImgs\\pinkyToThumb.jpg"
#define WAVE_IN_LATER_PATH        "Resources\\BW200PxImgs\\waveIn.jpg"
#define WAVE_IN_NEXT_PATH         "Resources\\Post200PxImgs\\waveIn.jpg"
#define WAVE_OUT_LATER_PATH       "Resources\\BW200PxImgs\\waveOut.jpq"
#define WAVE_OUT_NEXT_PATH        "Resources\\Post200PxImgs\\waveOut.jpg"

class sequenceImageSet;

class SequenceImageManager
{
public:
    SequenceImageManager();
    ~SequenceImageManager();

    void loadImages();

private:
    std::map<int, sequenceImageSet> idToImageMap;
};

#endif