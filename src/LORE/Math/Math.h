#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
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
#include "Quaternion.h"
#include "Rectangle.h"
#include "Vector.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  enum class TransformSpace
  {
    Local,
    Parent,
    World
  };

  class Degree;
  class Radian;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class Math
  /// \brief Class with purely static members and functions for common procedures.
  class LORE_EXPORT Math final
  {

    static const real _FDegToRad;
    static const real _FRadToDeg;

  public:

    static const real PI;
    static const real TWO_PI;
    static const real HALF_PI;

    // Axis vectors.

    static const Vec3 POSITIVE_X_AXIS;
    static const Vec3 NEGATIVE_X_AXIS;
    static const Vec3 POSITIVE_Y_AXIS;
    static const Vec3 NEGATIVE_Y_AXIS;
    static const Vec3 POSITIVE_Z_AXIS;
    static const Vec3 NEGATIVE_Z_AXIS;

  public:

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Creates 4x4 matrix from a 3D position, orientation, and scale.
    ///     Note the scale must be uniform.
    static inline Matrix4 CreateTransformationMatrix( const Vec3& position,
                                                      const Quaternion& orientation,
                                                      const Vec3& scale )
    {
     // assert( ( scale.x == scale.y ) && ( scale.x == scale.z ) );

      Matrix4 m;
      Matrix3 rot3x3 = orientation.createRotationMatrix();

      // Setup scale, rotation, and translation.
      m[0][0] = scale.x * rot3x3[0][0];
      m[1][0] = scale.y * rot3x3[0][1];
      m[2][0] = scale.z * rot3x3[0][2];
      m[3][0] = position.x;

      m[0][1] = scale.x * rot3x3[1][0];
      m[1][1] = scale.y * rot3x3[1][1];
      m[2][1] = scale.z * rot3x3[1][2];
      m[3][1] = position.y;

      m[0][2] = scale.x * rot3x3[2][0];
      m[1][2] = scale.y * rot3x3[2][1];
      m[2][2] = scale.z * rot3x3[2][2];
      m[3][2] = position.z;

      m[0][3] = m[1][3] = m[2][3] = 0.f;
      m[3][3] = 1.f;

      return m;
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Creates 4x4 matrix from a position and orientation.
    static inline Matrix4 CreateTransformationMatrix( const Vec2& position,
                                                      const Quaternion& orientation = Quaternion() )
    {
      Matrix4 m;
      Matrix3 rot3x3 = orientation.createRotationMatrix();

      // Setup scale, rotation, and translation.
      m[0][0] = rot3x3[0][0];
      m[1][0] = rot3x3[0][1];
      m[2][0] = rot3x3[0][2];
      m[3][0] = position.x;

      m[0][1] = rot3x3[1][0];
      m[1][1] = rot3x3[1][1];
      m[2][1] = rot3x3[1][2];
      m[3][1] = position.y;

      m[0][2] = rot3x3[2][0];
      m[1][2] = rot3x3[2][1];
      m[2][2] = rot3x3[2][2];
      m[3][2] = 0.f;

      m[0][3] = m[1][3] = m[2][3] = 0.f;
      m[3][3] = 1.f;

      return m;
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Creates 4x4 matrix from a 2D position, orientation, and scale.
    ///     Note the scale must be uniform.
    static inline Matrix4 CreateTransformationMatrix( const Vec2& position,
                                                      const Quaternion& orientation,
                                                      const Vec2& scale )
    {
      // This function only supports uniform scaling.
      //assert( scale.x == scale.y );

      Matrix4 m;
      Matrix3 rot3x3 = orientation.createRotationMatrix();

      // Setup scale, rotation, and translation.
      m[0][0] = scale.x * rot3x3[0][0];
      m[1][0] = scale.y * rot3x3[0][1];
      m[2][0] = rot3x3[0][2];
      m[3][0] = position.x;

      m[0][1] = scale.x * rot3x3[1][0];
      m[1][1] = scale.y * rot3x3[1][1];
      m[2][1] = rot3x3[1][2];
      m[3][1] = position.y;

      m[0][2] = scale.x * rot3x3[2][0];
      m[1][2] = scale.y * rot3x3[2][1];
      m[2][2] = rot3x3[2][2];
      m[3][2] = 0.f;

      m[0][3] = m[1][3] = m[2][3] = 0.f;
      m[3][3] = 1.f;

      return m;
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Creates an orthographic, left-handed, projection matrix.
    static inline Matrix4 OrthoLH( const float left, const float right,
                                   const float bottom, const float top,
                                   const float zNear, const float zFar )
    {
      Matrix4 ortho;

      ortho[0][0] = 2.f / ( right - left );
      ortho[1][1] = 2.f / ( top - bottom );
      ortho[3][0] = -( right + left ) / ( right - left );
      ortho[3][1] = -( top + bottom ) / ( top - bottom );

      ortho[2][2] = 2.f / ( zFar - zNear );
      ortho[3][2] = -( zFar + zNear ) / ( zFar - zNear );

      return ortho;
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Creates an orthographic, right-handed, projection matrix.
    static inline Matrix4 OrthoRH( const float left, const float right,
                                   const float bottom, const float top,
                                   const float zNear, const float zFar )
    {
      Matrix4 ortho;

      ortho[0][0] = 2.f / ( right - left );
      ortho[1][1] = 2.f / ( top - bottom );
      ortho[3][0] = -( right + left ) / ( right - left );
      ortho[3][1] = -( top + bottom ) / ( top - bottom );

      ortho[2][2] = -2.f / ( zFar - zNear );
      ortho[3][2] = -( zFar + zNear ) / ( zFar - zNear );

      return ortho;
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Converts value from degrees to radians.
    static inline real DegreesToRadians( const real degrees )
    {
      return degrees * _FDegToRad;
    }

    ///
    /// \brief Converts value from radians to degrees.
    static inline real RadiansToDegrees( const real radians )
    {
      return radians * _FRadToDeg;
    }

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
    // Quaternion factory functions.
    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \brief Creates Quaternion from 3D axis and angle in radians.
    static inline Quaternion CreateQuaternion( const Vec3& axis,
                                               const Radian& angle );

    ///
    /// \brief Creates Quaternion from 3D axis and angle in degrees.
    static inline Quaternion CreateQuaternion( const Vec3& axis,
                                               const Degree& angle );

    //

    template<typename T>
    static inline T Clamp( const T value, const T min, const T max )
    {
      if ( value < min ) {
        return min;
      }
      else if ( value > max ) {
        return max;
      }
      return value;
    }

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class Radian
  /// \brief Representation of a radian value.
  class Radian final
  {

  public:

    explicit Radian( const real r = 0.f )
      : _value( r )
    {
    }

    explicit Radian( const Degree& d );

    //
    // Conversion.

    ///
    /// \brief Returns radian value as degree.
    real valueDegrees() const;

    ///
    /// \brief Returns radian value.
    real valueRadians() const
    {
      return _value;
    }

    real valueAngleUnits() const;

    //
    // Operator overloading.

    Radian& operator = ( const real r )
    {
      _value = r;
      return *this;
    }

    Radian& operator = ( const Radian& r )
    {
      _value = r._value;
      return *this;
    }

    Radian operator * ( const real r ) const
    {
      return Radian( _value * r );
    }

    Radian operator * ( const Radian& r ) const
    {
      return Radian( _value * r._value );
    }

    Radian& operator *= ( const real r )
    {
      _value *= r;
      return *this;
    }

    Radian& operator = ( const Degree& d );

  private:

    real _value;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class Degree
  /// \brief Represents a degree value.
  class Degree final
  {

  public:

    explicit Degree( const real r = 0.f )
      : _value( r )
    {
    }

    explicit Degree( const Radian& r )
      : _value( r.valueDegrees() )
    {
    }

    //
    // Conversion.

    ///
    /// \brief Returns degree value.
    real valueDegrees() const
    {
      return _value;
    }

    ///
    /// \brief Returns degree value in radians.
    real valueRadians() const;

    real valueAngleUnits() const;

    //
    // Operator overloading.

    Degree& operator = ( const real r )
    {
      _value = r;
      return *this;
    }

    Degree& operator = ( const Degree& d )
    {
      _value = d._value;
      return *this;
    }

    Degree& operator = ( const Radian& r )
    {
      _value = r.valueDegrees();
      return *this;
    }

  private:

    real _value;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  inline Radian::Radian( const Degree& d )
    : _value( d.valueRadians() )
  {
  }

  inline Radian& Radian::operator = ( const Degree& d )
  {
    _value = d.valueRadians();
    return *this;
  }

  inline real Radian::valueDegrees() const
  {
    return Math::RadiansToDegrees( _value );
  }

  inline real Degree::valueRadians() const
  {
    return Math::DegreesToRadians( _value );
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \brief Constructs Quaternion from 3D axis and angle in radians.
  inline Quaternion Math::CreateQuaternion( const Vec3& axis,
                                            const Radian& angle )
  {
    Quaternion q;
    Radian halfAngle( angle * 0.5f );
    real rsin = std::sinf( halfAngle.valueRadians() );

    q.w = std::cosf( halfAngle.valueRadians() );
    q.x = rsin * axis.x;
    q.y = rsin * axis.y;
    q.z = rsin * axis.z;
    return q;
  }

  ///
  /// \brief Constructs Quaternion from 3D axis and angle in degrees.
  inline Quaternion Math::CreateQuaternion( const Vec3& axis,
                                            const Degree& angle )
  {
    return CreateQuaternion( axis, Radian( angle.valueRadians() ) );
  }

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
