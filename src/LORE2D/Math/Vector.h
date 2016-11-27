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

namespace Lore {

#define DEFAULT_MEMBERS( N )\
    Vector()\
    {\
        for( int i=0; i<N; ++i ){\
            data[i] = T(0);\
        }\
    }\
    explicit Vector( T t )\
    {\
        for( int i=0; i<N; ++i ){\
            data[i] = t;\
        }\
    }\
    T& operator [] ( int idx ) { return data[idx]; }\
    const T& operator [] ( int idx ) const { return data[idx]; }

    ///
    /// \class Vector
    /// \brief Generic vector class.
    template<typename T, int N>
    struct Vector
    {
        T data[N];
        DEFAULT_MEMBERS( N );
    };

    using Vec2 = Vector<float, 2>;
    using Vec3 = Vector<float, 3>;
    using Vec4 = Vector<float, 4>;

    //
    // Specialize component access for all vector sizes.

    template<typename T>
    struct Vector<T, 2>
    {
        union {
            T data[2];
            struct { T x, y; };
            struct { T u, v; };
        };

        Vector( const T x_, const T y_ )
        : x( x_ ), y( y_ )
        { }

        DEFAULT_MEMBERS( 2 );
    };

    template<typename T>
    struct Vector<T, 3>
    {
        union {
            T data[3];
            struct { T x, y, z; };
            struct { T r, g, b; };
        };

        Vector( const T x_, const T y_, const T z_ )
        : x( x_ ), y( y_ ), z( z_ )
        { }

        DEFAULT_MEMBERS( 3 );
    };

    template<typename T>
    struct Vector<T, 4>
    {
        union {
            T data[4];
            struct { T x, y, z, w; };
            struct { T r, g, b, a; };
            Vector<T, 2> xy;
            Vector<T, 3> xyz;
            Vector<T, 3> rgb;
        };

        Vector( const T x_, const T y_, const T z_, const T w_ )
        : x( x_ ), y( y_ ), z( z_ ), w( w_ )
        { }

        DEFAULT_MEMBERS( 4 );
    };

    //
    // Operator overloads.

#define VEC_UNARY_OP( op )\
    template<typename T, int N>\
    Vector<T, N> operator op ( const Vector<T, N>& rhs )\
    {\
        Vector<T, N> re;\
        for( int i=0; i<N; ++i ) {\
            re[i] = op rhs[i];\
        }\
        return re;\
    }

#define VEC_BINARY_OP( op )\
    template<typename T, int N>\
    Vector<T, N> operator op ( const Vector<T, N>& a, const Vector<T, N>& b )\
    {\
        Vector<T, N> re;\
        for( int i=0; i<N; ++i ) {\
            re[i] = a[i] op b[i];\
        }\
        return re;\
    }\
    template<typename T, int N>\
    Vector<T, N> operator op ( const T a, const Vector<T, N>& b )\
    {\
        Vector<T, N> re;\
        for( int i=0; i<N; ++i ) {\
            re = a op b[i];\
        }\
        return re;\
    }\
    template<typename T, int N>\
    Vector<T, N> operator op ( const Vector<T, N>& a, const T b )\
    {\
        Vector<T, N> re;\
        for( int i=0; i<N; ++i ) {\
            re[i] = a[i] op b;\
        }\
        return re;\
    }

#define VEC_INPLACE_OP( op )\
    template<typename T, int N>\
    Vector<T, N>& operator op (Vector<T, N>& a, const Vector<T, N>& b )\
    {\
        for( int i=0; i<N; ++i ) {\
            a[i] op b[i];\
        }\
        return a;\
    }\
    template<typename T, int N>\
    Vector<T, N>& operator op ( Vector<T, N>& a, const T b )\
    {\
        for( int i=0; i<N; ++i ) {\
            a[i] op b;\
        }\
        return a;\
    }

    VEC_UNARY_OP( - );
    VEC_UNARY_OP( ! );
    VEC_UNARY_OP( ~ );

    VEC_BINARY_OP( + );
    VEC_BINARY_OP( - );
    VEC_BINARY_OP( * );
    VEC_BINARY_OP( / );
    VEC_BINARY_OP( & );
    VEC_BINARY_OP( | );
    VEC_BINARY_OP( ^ );

    VEC_INPLACE_OP( += );
    VEC_INPLACE_OP( -= );
    VEC_INPLACE_OP( *= );
    VEC_INPLACE_OP( /= );
    VEC_INPLACE_OP( &= );
    VEC_INPLACE_OP( |= );
    VEC_INPLACE_OP( ^= );

#undef VEC_UNARY_OP
#undef VEC_BINARY_OP
#undef VEC_INPLACE_OP

    //
    // Vector operations.

    template<typename T, int N>
    T dot( const Vector<T, N>& a, const Vector<T, N>& b )
    {
        T re( 0 );
        for ( int i = 0; i < N; ++i ) {
            re += a[i] * b[i];
        }
        return re;
    }

    template<typename T, int N>
    T lengthSquared( const Vector<T, N> v )
    {
        return dot( v, v );
    }

    template<typename T, int N>
    T length( const Vector<T, N> v )
    {
        return sqrt( lengthSquared( v ) );
    }

    template<typename T, int N>
    Vector<T, N> normalize( Vector<T, N>& v )
    {
        v = v / length( v );
    }

    template<typename T, int N>
    Vector<T, N> normalizedCopy( const Vector<T, N>& v )
    {
        return v / length( v );
    }

    template<typename T>
    Vector<T, 3> cross( const Vector<T, 3>& a, const Vector<T, 3>& b )
    {
        return Vector<T, 3>(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
