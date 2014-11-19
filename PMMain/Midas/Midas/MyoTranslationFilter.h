#ifndef _MYO_TRANSLATION_FILTER_H
#define _MYO_TRANSLATION_FILTER_H

#include "Filter.h"
#include "ControlState.h"

#define INPUT_QUATERNION_X "quatDataX"
#define INPUT_QUATERNION_Y "quatDataY"
#define INPUT_QUATERNION_Z "quatDataZ"
#define INPUT_QUATERNION_W "quatDataW"

// Testing Constants -- Modify until seems reasonable
#define MAX_ANGLE 1.04719755f /* Maximum delta angle in radians */

class MyoTranslationFilter : public Filter
{
public:
    MyoTranslationFilter(ControlState* controlState);
    ~MyoTranslationFilter();

    void process();

private:
    float getPitchFromQuaternion(float x, float y, float z, float w);
    float getYawFromQuaternion(float x, float y, float z, float w);
    point getMouseUnitVelocity(float pitch, float yaw);

    ControlState* controlStateHandle;
    midasMode previousMode;
    float basePitch, baseYaw;
};

#endif