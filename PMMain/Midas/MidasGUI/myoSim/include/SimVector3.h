#pragma once
#include "myo\cxx\Vector3.hpp"

namespace myoSim
{
    // Pass-through class to myo::Vector3. This is implemented so that the
    // user can simply replace the myo namespace with myoSim to use the simulator rather
    // than an actual myo device.
    template<typename T>
    class Vector3
    {
    public:
        Vector3();
        Vector3(T x, T y, T z);
        Vector3(const Vector3& other);
        Vector3(myo::Vector3<T> myoVec3);
        Vector3& operator=(const Vector3& other);

        /// Return a copy of the component of this vector at \a index, which should be 0, 1, or 2.
        T operator[](unsigned int index) const;

        T x() const;
        T y() const;
        T z() const;

        myo::Vector3<T> getMyoVector3() const;

        /// Return the magnitude of this vector.
        T magnitude() const;

        /// Return a normalized copy of this vector.
        Vector3 normalized() const;

        /// Return the dot product of this vector and \a rhs.
        T dot(const Vector3& rhs) const;

        /// Return the cross product of this vector and \a rhs.
        Vector3 cross(const Vector3& rhs) const;

        T angleTo(const Vector3& rhs) const;

    private:
        myo::Vector3<T> myoVector3;

    };

    template <typename T>
    Vector3<T>::Vector3() : myoVector3<T>() { }

    template <typename T>
    Vector3<T>::Vector3(T x, T y, T z) : myoVector3<T>(x, y, z) { }

    template <typename T>
    Vector3<T>::Vector3(const Vector3<T>& other) : myoVector3<T>(other.getMyoVector3()) { }

    template <typename T>
    Vector3<T>& Vector3<T>::operator=(const Vector3<T>& other)
    {
        myoVector3 = other.getMyoVector3();
        return *this;
    }

    template <typename T>
    Vector3<T>::Vector3(myo::Vector3<T> myoVec3) : myoVector3(myoVec3) { }

    template <typename T>
    T Vector3<T>::operator[](unsigned int index) const
    {
         return myoVector3[index];
    }

    template <typename T>
    T Vector3<T>::x() const
    {
        return myoVector3.x();
    }

    template <typename T>
    T Vector3<T>::y() const
    {
        return myoVector3.y();
    }

    template <typename T>
    T Vector3<T>::z() const
    {
        return myoVector3.z();
    }

    template <typename T>
    myo::Vector3<T> Vector3<T>::getMyoVector3() const
    {
        return myoVector3;
    }

    template <typename T>
    T Vector3<T>::magnitude() const
    {
        return myoVector3.magnitude();
    }

    template <typename T>
    Vector3<T> Vector3<T>::normalized() const
    {
        return Vector3<T>(myoVector3.normalized());
    }

    template <typename T>
    T Vector3<T>::dot(const Vector3<T>& rhs) const
    {
        return myoVector3.dot(rhs.getMyoVector3());
    }

    template <typename T>
    Vector3<T> Vector3<T>::cross(const Vector3<T>& rhs) const
    {
        return Vector3<T>(myoVector3.cross(rhs.getMyoVector3()));
    }

    template <typename T>
    T Vector3<T>::angleTo(const Vector3<T>& rhs) const
    {
        return myoVector3.angleTo(rhs.getMyoVector3());
    }
}