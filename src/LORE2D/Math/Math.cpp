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

#include "Math.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

const real Math::PI = real( 4.0f * atan( 1.0f ) );
const real Math::TWO_PI = real( 2.f * PI );
const real Math::HALF_PI = real( 0.5f * PI );

const real Math::_FDegToRad = PI / real( 180.f );
const real Math::_FRadToDeg = real( 180.f ) / PI;

const Vec3 Math::POSITIVE_X_AXIS = Vec3( 1.f, 0.f, 0.f );
const Vec3 Math::NEGATIVE_X_AXIS = Vec3( -1.f, 0.f, 0.f );
const Vec3 Math::POSITIVE_Y_AXIS = Vec3( 0.f, 1.f, 0.f );
const Vec3 Math::NEGATIVE_Y_AXIS = Vec3( 0.f, -1.f, 0.f );
const Vec3 Math::POSITIVE_Z_AXIS = Vec3( 0.f, 0.f, 1.f );
const Vec3 Math::NEGATIVE_Z_AXIS = Vec3( 0.f, 0.f, -1.f );

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
