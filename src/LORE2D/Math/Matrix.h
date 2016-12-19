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

#include "Vector.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

#define DEFAULT_MEMBERS( C, R )\
    Matrix()\
    : m()\
    {}\
    inline Vec& operator [] ( size_t idx )\
    {\
        assert( idx < C );\
        return m[idx];\
    }\
    inline bool operator == ( const Matrix<T, C, R>& rhs )\
    {\
        for ( int i = 0; i < C; ++i ) {\
            if ( m[i] != rhs[i] ) {\
                return false;\
            }\
        }\
        return true;\
    }\
    inline bool operator != ( const Matrix<T, C, R>& rhs )\
    {\
        return !( operator==( rhs ) );\
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    template<typename T, int C, int R>
    struct Matrix
    {

    public:

        using Vec = Vector<T, R>;

    public:

        DEFAULT_MEMBERS( C, R );

        

        //
        // Modifiers.

        

        //
        // Operators.

        

        

    private:

        Vec m[C];

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    using float4x4 = Matrix<float, 4, 4>;
    using float3x3 = Matrix<float, 3, 3>;
    using float2x2 = Matrix<float, 2, 2>;

    using Matrix4 = float4x4;
    using Matrix3 = float3x3;
    using Matrix2 = float2x2;

    using Mat4 = Matrix4;
    using Mat3 = Matrix3;
    using Mat2 = Matrix2;

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    //
    // Specialize Matrix3/4.

    template<typename T>
    struct Matrix<T, 4, 4>
    {

    public:

        using Vec = Vector<T, 4>;

    public:

        DEFAULT_MEMBERS( 4, 4 );

        Matrix( const Vec3& position,
                const Vec3& scale )
        : m()
        {
            makeTransform( position, scale );
        }

        ///
        /// \brief Creates 4x4 transformation matrix based on translation, rotation, and scale.
        Matrix<T, 4, 4> makeTransform( const Vec3& position,
                                       const Vec3& scale )
        {
            Matrix3 rot3x3;
            

            m[0][0] = scale.x * rot3x3[0][0];
        }

    private:

        Vec m[4];

    };

#define DEFINE_UNARY_OP( op )\
    template<typename T, int C, int R>\
    Matrix<T, C, R> operator op ( const Matrix<T, C, R>& rhs )\
    {\
        Matrix<T, C, R> re;\
        for( int i=0; i<C; ++i){\
            r[i] = op rhs.m[i];\
        }\
        return re;\
    }

#define DEFINE_BINARY_OP( op )\
    template<typename T, int C, int R>\
    Matrix<T, C, R> operator op ( const Matrix<T, C, R>& a, const Matrix<T, C, R>& b )\
    {\
        Matrix<T, C, R> re;\
        for( int i=0; i<C; ++i ){\
            re[i] = a[i] op b[i];\
        }\
        return re;\
    }\
    template<typename T, int C, int R>\
    Matrix<T, C, R> operator op ( const Matrix<T, C, R>& a, const T b )\
    {\
        Matrix<T, C, R> re;\
        for( int i=0; i<C; ++i ){\
            re[i] = a[i] op b;\
        }\
        return re;\
    }\
    template<typename T, int C, int R>\
    Matrix<T, C, R> operator op ( const T a, const Matrix<T, C, R>& b )\
    {\
        Matrix<T, C, R> re;\
        for( int i=0; i<C; ++i){\
            re[i] = a op b[i];\
        }\
        return re;\
    }

#define DEFINE_INPLACE_OP( op )\
    template<typename T, int C, int R>\
    Matrix<T, C, R>& operator op ( Matrix<T, C, R>& a, const Matrix<T, C, R>& b )\
    {\
        for( int i=0; i<C; ++i ){\
            a[i] = a[i] op b[i];\
        }\
        return a;\
    }\
    template<typename T, int C, int R>\
    Matrix<T, C, R>& operator op ( Matrix<T, C, R>& a, const T b )\
    {\
        for( int i=0; i<C; ++i ){\
            a[i] = a[i] op b;\
        }\
    }

    DEFINE_UNARY_OP( ! );
    DEFINE_UNARY_OP( ~ );
    DEFINE_UNARY_OP( - );

    DEFINE_BINARY_OP( + );
    DEFINE_BINARY_OP( - );
    DEFINE_BINARY_OP( * );
    DEFINE_BINARY_OP( / );
    DEFINE_BINARY_OP( & );
    DEFINE_BINARY_OP( | );
    DEFINE_BINARY_OP( ^ );

    DEFINE_INPLACE_OP( += );
    DEFINE_INPLACE_OP( -= );
    DEFINE_INPLACE_OP( *= );
    DEFINE_INPLACE_OP( /= );
    DEFINE_INPLACE_OP( &= );
    DEFINE_INPLACE_OP( |= );
    DEFINE_INPLACE_OP( ^= );

#undef DEFINE_UNARY_OP
#undef DEFINE_BINARY_OP
#undef DEFINE_INPLACE_OP

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
