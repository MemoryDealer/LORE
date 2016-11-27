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

#include "catch.hpp"
#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#define REQUIRE_V2( v, f )\
    REQUIRE( v.x == Approx( f ) );\
    REQUIRE( v.y == Approx( f ) )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Basic vector-scalar operators", "[math]" )
{
    Lore::Vec2 v;

    REQUIRE( v.x == 0.f );
    REQUIRE( v.y == 0.f );

    SECTION( "Addition" )
    {
        v += 5.f;
        REQUIRE_V2( v, 5.f );

        v = v + 7.f;
        REQUIRE_V2( v, 12.f );

        v += 37.845f;
        REQUIRE_V2( v, 49.845f );
    }

    SECTION( "Subtraction" )
    {
        v -= 20.f;
        REQUIRE_V2( v, -20.f );

        v = v - 7.4f;
        REQUIRE_V2( v, -27.4f );

        v -= 127.64f;
        REQUIRE_V2( v, -155.04f );
    }

    SECTION( "Multiplication" )
    {
        v *= 5.f;
        REQUIRE_V2( v, 0.f );

        v += 1.f;
        REQUIRE_V2( v, 1.f );

        v *= 50.f;
        REQUIRE_V2( v, 50.f );

        v = v * 7.55f;
        REQUIRE_V2( v, 377.5f );
    }

    SECTION( "Division" )
    {
        v /= 5.f;
        REQUIRE_V2( v, 0.f );

        v += 5.56f;
        REQUIRE_V2( v, 5.56f );

        v /= 2.42f;
        REQUIRE_V2( v, 2.2975f );

        v = v / -77.84f;
        REQUIRE_V2( v, -0.02952f );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
