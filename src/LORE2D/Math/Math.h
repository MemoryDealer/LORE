#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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
#include "Quaternion.h"
#include "Vector.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    static Matrix4 CreateTransformationMatrix( const Vec3& position,
                                               const Quaternion& orientation,
                                               const Vec3& scale )
    {
        Matrix4 m;
        Matrix3 rot3x3 = orientation.createRotationMatrix();
        
        // Setup scale, rotation, and translation.
        m[0][0] = scale.x * rot3x3[0][0];
        m[0][1] = scale.y * rot3x3[0][1];
        m[0][2] = scale.z * rot3x3[0][2];
        m[0][3] = position.x;

        m[1][0] = scale.x * rot3x3[1][0];
        m[1][1] = scale.y * rot3x3[1][1];
        m[1][2] = scale.z * rot3x3[1][2];
        m[1][3] = position.y;

        m[2][0] = scale.x * rot3x3[2][0];
        m[2][1] = scale.y * rot3x3[2][1];
        m[2][2] = scale.z * rot3x3[2][2];
        m[2][3] = position.z;

        // Transformation matrices don't need projection column, set it to [0, 0, 0, 1].
        m[3][0] = m[3][1] = m[3][2] = 0.f;
        m[3][3] = 1.f;

        return m;
    }

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
