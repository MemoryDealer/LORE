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

#include "Vector.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    // Since Matrix classes are generated using templates, provide macros for
    // generate scalable default member functions for template class specializations.
#define DEFAULT_MEMBERS( C, R )\
    Matrix()\
    : m()\
    {\
        for( int i=0; i<C; ++i ){\
            for( int j=0; j<R; ++j){\
                if( i == j ){\
                    m[i][j] = static_cast<T>( 1 );\
                }\
                else{\
                    m[i][j] = static_cast<T>( 0 );\
                }\
            }\
        }\
    }\
    explicit Matrix( const T t )\
    {\
        for( int i=0; i<C; ++i){\
            for( int j=0; j<R; ++j){\
                m[i][j] = t;\
            }\
        }\
    }\
    inline Vec& operator [] ( size_t idx )\
    {\
        assert( idx < C );\
        return m[idx];\
    }\
    inline Vec operator [] ( size_t idx) const\
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

    ///
    /// \class Matrix
    /// \brief Represents a matrix of CxR elements, using column vectors.
    /// For example, a 4x4 matrix is represented as so:
    /// | v1.x  v2.x  v3.x  v4.x |
    /// | v1.y  v2.y  v3.y  v4.y |
    /// | v1.z  v2.z  v3.z  v4.z |
    /// | v1.w  v2.w  v3.w  v4.w |
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

        ///
        /// \brief Converts matrix into identity matrix.
        void makeIdentity()
        {
            for ( int i = 0; i<C; ++i ) {
                for ( int j = 0; j<R; ++j ) {
                    if ( i == j ) {
                        m[i][j] = static_cast< T >( 1 );
                    }
                    else {
                        m[i][j] = static_cast< T >( 0 );
                    }
                }
            }
        }
        

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
    // Specialize Matrix4.

    template<typename T>
    struct Matrix<T, 4, 4>
    {

    public:

        using Vec = Vector<T, 4>;

    public:

        DEFAULT_MEMBERS( 4, 4 );

        Matrix( const T m00, const T m01, const T m02, const T m03,
                const T m10, const T m11, const T m12, const T m13,
                const T m20, const T m21, const T m22, const T m23,
                const T m30, const T m31, const T m32, const T m33 )
        : m()
        {
            m[0][0] = m00; m[0][1] = m10; m[0][2] = m20; m[0][3] = m30;
            m[1][0] = m01; m[1][1] = m11; m[1][2] = m21; m[1][3] = m31;
            m[2][0] = m02; m[2][1] = m12; m[2][2] = m22; m[2][3] = m32;
            m[3][0] = m03; m[3][1] = m13; m[3][2] = m23; m[3][3] = m33;
        }

        Vec3 toEulerAngles() const
        {
          Vec3 euler;
          euler.x = std::atan2f( m[2][1], m[2][2] );
          const auto c2 = std::sqrtf( m[0][0] * m[0][0] + m[1][0] * m[1][0] );
          euler.y = std::atan2f( -m[2][0], c2 );
          const auto s1 = std::sinf( euler.x );
          const auto c1 = std::cosf( euler.x );
          euler.z = std::atan2f( s1 * m[0][2] - c1 * m[0][1], c1 * m[1][1] - s1 * m[1][2] );
          return euler;
        }

    private:

        Vec m[4];

    };

    // Provide macros for all operators we'll define for a Matrix, to avoid duplicate code.
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

#define DEFINE_BINARY_SCALAR_OP( op )\
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
    DEFINE_BINARY_SCALAR_OP( op );

#define DEFINE_INPLACE_SCALAR_OP( op )\
template<typename T, int C, int R>\
    Matrix<T, C, R>& operator op ( Matrix<T, C, R>& a, const T b )\
    {\
        for( int i=0; i<C; ++i ){\
            a[i] = a[i] op b;\
        }\
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
    DEFINE_INPLACE_SCALAR_OP( op );

    DEFINE_UNARY_OP( ! );
    DEFINE_UNARY_OP( ~ );
    DEFINE_UNARY_OP( - );

    DEFINE_BINARY_OP( + );
    DEFINE_BINARY_OP( - );
    DEFINE_BINARY_OP( & );
    DEFINE_BINARY_OP( | );
    DEFINE_BINARY_OP( ^ );
    DEFINE_BINARY_SCALAR_OP( * );
    DEFINE_BINARY_SCALAR_OP( / );

    DEFINE_INPLACE_OP( += );
    DEFINE_INPLACE_OP( -= );
    DEFINE_INPLACE_OP( &= );
    DEFINE_INPLACE_OP( |= );
    DEFINE_INPLACE_OP( ^= );
    DEFINE_INPLACE_SCALAR_OP( *= );
    DEFINE_INPLACE_SCALAR_OP( /= );

#undef DEFINE_UNARY_OP
#undef DEFINE_BINARY_OP
#undef DEFINE_BINARY_SCALAR_OP
#undef DEFINE_INPLACE_OP
#undef DEFINE_INPLACE_SCALAR_OP

    // Matrix multiplication.
    template<typename T, int C, int R>
    Matrix<T, C, R> operator * ( Matrix<T, C, R> const& a, Matrix<T, R, C> const& b )
    {
        // See https://en.wikipedia.org/wiki/Matrix_multiplication_algorithm for optimization.
        Matrix<T, C, R> re( T( 0 ) );
        // For each column.
        for ( int i = 0; i < C; ++i ) {

            // For each element in column.
            for ( int j = 0; j < R; ++j ) {

                for ( int k = 0; k < C; ++k ) {
                    re[j][i] += ( a[k][i] * b[j][k] );
                }
            }
        }

        return re;
    }

#undef DEFAULT_MEMBERS

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
