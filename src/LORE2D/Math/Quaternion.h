#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016-2017 Jordan Sparks
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files ( the "Software" ), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include "Matrix.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class Quaternion
    /// \brief Representation of a quaternion.
    struct LORE_EXPORT Quaternion
    {

    public:

        constexpr Quaternion()
        : w( 1.f )
        , x( 0.f )
        , y( 0.f )
        , z( 0.f )
        { }

        Quaternion( const real w_,
                    const real x_,
                    const real y_,
                    const real z_ )
        : w( w_ )
        , x( x_ )
        , y( y_ )
        , z( z_ )
        { }

        ///
        /// \brief Returns normal length of Quaternion.
        real getNormalLength() const;

        ///
        /// \brief Normalizes the Quaternion.
        real normalize();

        ///
        /// \brief Returns 3x3 matrix representing orientation of Quaternion.
        Matrix3 createRotationMatrix() const;

        ///
        /// \brief Returns 3D vector representing quaternion's orientation in
        ///   euler angles (pitch, yaw, roll).
        Vec3 getEulerAngles() const;

        //
        // Operator overloading.

        Quaternion operator * ( const Quaternion& rhs ) const;

        Quaternion operator * ( const real r ) const;

    public:

        real w, x, y, z;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
