#pragma once
#include "myo\cxx\Pose.hpp"

namespace myoSim
{
    class Pose
    {
    public:
        // This enum is translated to the Myo version of the enum.
        enum Type {
            rest = libmyo_pose_rest,
            fist = libmyo_pose_fist,
            waveIn = libmyo_pose_wave_in,
            waveOut = libmyo_pose_wave_out,
            fingersSpread = libmyo_pose_fingers_spread,
            reserved1 = libmyo_pose_reserved1,
            thumbToPinky = libmyo_pose_thumb_to_pinky,
            unknown = libmyo_pose_unknown
        };

        Pose();
        Pose(Type type);

        /// Returns true if and only if the two poses are of the same type.
        bool operator==(Pose other) const;

        /// Equivalent to `!(*this == other)`.
        bool operator!=(Pose other) const;

        /// Returns the type of this pose.
        Type type() const;

        // Get the corresponding Myo Pose
        myo::Pose getUnderlyingPose() const;

        /// Return a human-readable string representation of the pose.
        std::string toString() const;

    private:
        myo::Pose myoPose;
        Type simType;
    };

    /// @relates Pose
    /// Returns true if and only if the type of pose is the same as the provided type.
    bool operator==(Pose pose, Pose::Type t);

    /// @relates Pose
    /// Equivalent to `pose == type`.
    bool operator==(Pose::Type type, Pose pose);

    /// @relates Pose
    /// Equivalent to `!(pose == type)`.
    bool operator!=(Pose pose, Pose::Type type);

    /// @relates Pose
    /// Equivalent to `!(type == pose)`.
    bool operator!=(Pose::Type type, Pose pose);

    /// @relates Pose
    /// Write the name of the provided pose to the provided output stream.
    std::ostream& operator<<(std::ostream& out, const Pose& pose);
}