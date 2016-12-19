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

#include "Quaternion.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Matrix3 Quaternion::createRotationMatrix() const
{
    real tx = x + x;
    real ty = y + y;
    real tz = z + z;
    real twx = tx * w;
    real twy = ty * w;
    real twz = tz * w;
    real txx = tx * x;
    real txy = ty * x;
    real txz = tz * x;
    real tyy = ty * y;
    real tyz = tz * y;
    real tzz = tz * z;

    Matrix3 rot;
    rot[0][0] = 1.f - ( tyy + tzz );
    rot[0][1] = txy - twz;
    rot[0][2] = txz + twy;
    rot[1][0] = txy + twz;
    rot[1][1] = 1.f - ( txx + tzz );
    rot[1][2] = tyz - twx;
    rot[2][0] = txz - twy;
    rot[2][1] = tyz + twx;
    rot[2][2] = 1.f - ( txx + tyy );

    return rot;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //