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

#include "catch.hpp"
#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

constexpr const auto Epsilon = 0.01f;

#define REQUIRE_V2( v, f )\
    REQUIRE( v.x == Approx( f ).epsilon(Epsilon) );\
    REQUIRE( v.y == Approx( f ).epsilon(Epsilon) )

#define REQUIRE_V3( v, f )\
    REQUIRE( v.x == Approx( f ).epsilon(Epsilon) );\
    REQUIRE( v.y == Approx( f ).epsilon(Epsilon) );\
    REQUIRE( v.z == Approx( f ).epsilon(Epsilon) )

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Vector-scalar operators", "[math]" )
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

TEST_CASE( "Vector-vector operators", "[math]" )
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

    SECTION( "Cross product" )
    {
        Lore::Vec3 a( 71.f, 33.f, 21.f ), b( 83.2f, 44.f, 3.2f );

        auto re = a.cross( b );
        REQUIRE( re.x == Approx( -818.4f ) );
        REQUIRE( re.y == Approx( 1520.f ) );
        REQUIRE( re.z == Approx( 378.4f ) );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Matrix-matrix operations", "[math]" )
{

    SECTION( "Multiplication" )
    {
        Lore::Matrix4 m1( 1.f, 2.f, 3.f, 4.f,
                          4.f, 3.f, 2.f, 1.f,
                          5.f, 6.f, 7.f, 8.f,
                          8.f, 7.f, 6.f, 5.f );

        Lore::Matrix4 m2( 4.f, 3.f, 2.f, 1.f,
                          1.f, 2.f, 3.f, 4.f,
                          5.f, 6.f, 7.f, 8.f,
                          8.f, 7.f, 6.f, 5.f );

        Lore::Matrix4 r = m1 * m2;
        for ( int i = 0; i < 4; ++i ) {
            REQUIRE( Approx( 53.f ) == r[i][0] );
            REQUIRE( Approx( 37.f ) == r[i][1] );
            REQUIRE( Approx( 125.f ) == r[i][2] );
            REQUIRE( Approx( 109.f ) == r[i][3] );
        }

        Lore::Matrix4 m3, m4;
        m3[0][0] = 1.f;
        m3[0][1] = 4.f;
        m3[0][2] = 3.f;
        m3[0][3] = 33.f;
        m3[1][0] = 2.f;
        m3[1][1] = 5.f;
        m3[1][2] = 2.f;
        m3[1][3] = 2.f;
        m3[2][0] = 3.f;
        m3[2][1] = 6.f;
        m3[2][2] = 1.f;
        m3[2][3] = 4.f;
        m3[3][0] = 2.f;
        m3[3][1] = 1.f;
        m3[3][2] = 3.f;
        m3[3][3] = 1.f;

        m4[0][0] = 1.5f;
        m4[0][1] = 3.f;
        m4[0][2] = 3.f;
        m4[0][3] = 2.f;
        m4[1][0] = 3.f;
        m4[1][1] = 7.f;
        m4[1][2] = 1.f;
        m4[1][3] = 3.f;
        m4[2][0] = 2.f;
        m4[2][1] = 5.f;
        m4[2][2] = 0.f;
        m4[2][3] = 21.f;
        m4[3][0] = 0.5f;
        m4[3][1] = 33.f;
        m4[3][2] = 3.f;
        m4[3][3] = 0.1f;

        Lore::Matrix4 r2 = m3 * m4;
        REQUIRE( r2[0][0] == Approx( m3[0][0] * m4[0][0] + m3[1][0] * m4[0][1] + m3[2][0] * m4[0][2] + m3[3][0] * m4[0][3] ) );
        REQUIRE( r2[0][1] == Approx( 41.f ) );
        REQUIRE( r2[0][2] == Approx( 19.5f ) );
        REQUIRE( r2[0][3] == Approx( 69.5f ) );
        REQUIRE( r2[1][0] == Approx( 26.f ) );
        REQUIRE( r2[1][1] == Approx( 56.f ) );
        REQUIRE( r2[1][2] == Approx( 33.f ) );
        REQUIRE( r2[1][3] == Approx( 120.f ) );
        REQUIRE( r2[2][0] == Approx( 54.f ) );
        REQUIRE( r2[2][1] == Approx( 54.f ) );
        REQUIRE( r2[2][2] == Approx( 79.f ) );
        REQUIRE( r2[2][3] == Approx( 97.f ) );
        REQUIRE( r2[3][0] == Approx( 75.7f ) );
        REQUIRE( r2[3][1] == Approx( 185.1f ) );
        REQUIRE( r2[3][2] == Approx( 70.8f ) );
        REQUIRE( r2[3][3] == Approx( 94.6f ) );

        Lore::Matrix4 m5( 0.f ), m6( 0.f );
        m5[0][0] = 1.f;
        m5[1][0] = 2.f;
        m5[3][3] = 1.f;

        m6[0][0] = 1.5f;
        m6[1][0] = 3.f;
        m6[3][0] = 0.5f;
        m6[3][3] = 0.1f;

        Lore::Matrix4 r3 = m5 * m6;

        REQUIRE( Approx( 1.5f ) == r3[0][0] );
        REQUIRE( Approx( 3.f ) == r3[1][0] );
        REQUIRE( Approx( 0.5f ) == r3[3][0] );
        REQUIRE( Approx( 0.1f ) == r3[3][3] );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
