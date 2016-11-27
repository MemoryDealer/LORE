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

TEST_CASE( "Basic vector-vector operators", "[math]" )
{
    Lore::Vec2 v;

    SECTION( "Addition" )
    {
        Lore::Vec2 r( 5.f, 5.f );

        v += r;
        REQUIRE_V2( v, 5.f );

        v += Lore::Vec2( 123.54f, 123.54f );
        REQUIRE_V2( v, 128.54f );

        v = v / r;
        REQUIRE_V2( v, 128.54f / 5.f );
    }

    SECTION( "Subtraction" )
    {
        Lore::Vec2 r( 17.08f, 17.08f );

        v -= r;
        REQUIRE_V2( v, -17.08f );

        v -= Lore::Vec2( 0.071f, 0.071f );
        REQUIRE_V2( v, -17.151f );

        v = v - r;
        REQUIRE_V2( v, -17.151f - 17.08f );
    }

    SECTION( "Multiplication" )
    {
        Lore::Vec2 r( 5.f, 5.f );

        v += 1.f;
        v *= r;
        REQUIRE_V2( v, 5.f );

        v *= Lore::Vec2( -3.f, -3.f );
        REQUIRE_V2( v, -15.f );

        v = v * r;
        REQUIRE_V2( v, -15.f * 5.f );
    }

    SECTION( "Division" )
    {
        Lore::Vec2 r( 21.f, 21.f );

        v /= r;
        REQUIRE_V2( v, 0.f );
        
        v += 1.f;
        v /= r;
        REQUIRE_V2( v, 0.04762f );

        v /= Lore::Vec2( -.03f, -.03f );
        REQUIRE_V2( v, -1.5873f );

        v = v / r;
        REQUIRE_V2( v, -1.5873f / 21.f );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Vector operations", "[math]" )
{
    Lore::Vec2 v;

    SECTION( "Dot product" )
    {
        Lore::Vec2 r( 2.5f, 7.5f );
        v += 5.f;
        REQUIRE( v.dot( r ) == Approx( 50.f ) );
        REQUIRE( v.dot( Lore::Vec2( -20.f, -78.3f ) ) == Approx( -983.f / 2.f ) );
    }

    SECTION( "Length" )
    {
        v += Lore::Vec2( 2.5f, 7.5f );
        REQUIRE( v.squaredLength() == Approx( 62.5f ) );
        REQUIRE( v.length() == Approx( 7.9057f ) );
    }

    SECTION( "Normalization" )
    {
        v += 15.f;
        v.normalize();
        REQUIRE_V2( v, 0.7071f );

        Lore::Vec2 r( 121.f, 33.f );
        auto copy = r.normalizedCopy();
        REQUIRE( copy.x == Approx( 0.96476f ) );
        REQUIRE( copy.y == Approx( 0.26312f ) );
    }

    SECTION( "Distance" )
    {
        v += 5.f;
        Lore::Vec2 r( 120.f, 5.f );
        REQUIRE( v.squaredDistance( r ) == Approx( 13225.f ) );
        REQUIRE( v.distance( r ) == Approx( 115.f ) );
    }

    SECTION( "Pow" )
    {
        v += 2.f;
        v.pow( 3.f );
        REQUIRE_V2( v, 8.f );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
