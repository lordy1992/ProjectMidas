#pragma once
#include "myo\cxx\Quaternion.hpp"
#include "SimVector3.h"

namespace myoSim
{
    // Pass-through class to myo::Quaternion. This is implemented so that the
    // user can simply replace the myo namespace with myoSim to use the simulator rather
    // than an actual myo device.
    template<typename T>
    class Quaternion
    {
    public:
        Quaternion();
        Quaternion(T x, T y, T z, T w);
        Quaternion(myo::Quaternion<T> backingQuaternion);
        Quaternion& operator=(const Quaternion other);

        T x() const;
        T y() const;
        T z() const;
        T w() const;

        Quaternion operator*(const Quaternion& rhs) const;
        Quaternion& operator*=(const Quaternion& rhs);
        Quaternion normalized() const;
        Quaternion conjugate() const;
        myo::Quaternion<T> getMyoQuaternion() const;
        static Quaternion fromAxisAngle(const Vector3<T>& axis, T angle);

    private:
        myo::Quaternion<T> myoQuaternion;
    };

    template <typename T>
    Quaternion<T>::Quaternion(myo::Quaternion<T> backingQuaternion) : myoQuaternion(backingQuaternion) { }

    template <typename T>
    Quaternion<T>::Quaternion() : myoQuaternion(myo::Quaternion<T>()) { }

    template <typename T>
    Quaternion<T>::Quaternion(T x, T y, T z, T w) : myoQuaternion(myo::Quaternion<T>(x, y, z, w)) { }

    template <typename T>
    myo::Quaternion<T> Quaternion<T>::getMyoQuaternion() const
    {
        return myoQuaternion;
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator=(const Quaternion<T> other)
    {
        myoQuaternion = other.getMyoQuaternion();
        return *this;
    }

    template <typename T>
    T Quaternion<T>::x() const
    {
        return myoQuaternion.x();
    }

    template <typename T>
    T Quaternion<T>::y() const
    {
        return myoQuaternion.y();
    }

    template <typename T>
    T Quaternion<T>::z() const
    {
        return myoQuaternion.z();
    }

    template <typename T>
    T Quaternion<T>::w() const
    {
        return myoQuaternion.w();
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::operator*(const Quaternion& rhs) const
    {
        return Quaternion(myoQuaternion * rhs.getMyoQuaternion());
    }

    template <typename T>
    Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& rhs)
    {
        *this = *this * rhs;
        return *this;
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::normalized() const
    {
        return Quaternion<T>(myoQuaternion.normalized());
    }

    template <typename T>
    Quaternion<T> Quaternion<T>::conjugate() const
    {
        return Quaternion<T>(myoQuaternion.conjugate());
    }

    template <typename T>
    static Quaternion<T> Quaternion<T>::fromAxisAngle(const Vector3<T>& axis, T angle)
    {
        return Quaternion<T>(myoQuaternion.fromAxisAngle(axis.getMyoVector3(), angle));
    }

    template<typename T>
    Quaternion<T> rotate(const Vector3<T>& from, const Vector3<T>& to)
    {
        return Quaternion<T>(myoQuaternion.rotate(from.getMyoVector3(), to.getMyoVector3()));
    }
}