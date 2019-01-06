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

#include "Rectangle.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  enum class TransformSpace
  {
    Local,
    Parent,
    World
  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static const auto Vec3Zero = glm::vec3( 0.f, 0.f, 0.f );
  static const auto Vec3PosX = glm::vec3( 1.f, 0.f, 0.f );
  static const auto Vec3PosY = glm::vec3( 0.f, 1.f, 0.f );
  static const auto Vec3PosZ = glm::vec3( 0.f, 0.f, 1.f );
  static const auto Vec3NegX = glm::vec3( -1.f, 0.f, 0.f );
  static const auto Vec3NegY = glm::vec3( 0.f, -1.f, 0.f );
  static const auto Vec3NegZ = glm::vec3( 0.f, 0.f, -1.f );

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  namespace Math {

    inline static glm::mat4 CreateTransformationMatrix( const glm::vec3& pos,
                                                        const glm::quat& orientation,
                                                        const glm::vec3& scale )
    {
      glm::mat4 m( 1.f );

      glm::mat3 rot3x3 = glm::mat3_cast( orientation );

      // Setup scale, rotation, and translation. 
      m[0][0] = scale.x * rot3x3[0][0];
      m[1][0] = scale.y * rot3x3[0][1];
      m[2][0] = scale.z * rot3x3[0][2];
      m[3][0] = pos.x;

      m[0][1] = scale.x * rot3x3[1][0];
      m[1][1] = scale.y * rot3x3[1][1];
      m[2][1] = scale.z * rot3x3[1][2];
      m[3][1] = pos.y;

      m[0][2] = scale.x * rot3x3[2][0];
      m[1][2] = scale.y * rot3x3[2][1];
      m[2][2] = scale.z * rot3x3[2][2];
      m[3][2] = pos.z;

      m[0][3] = m[1][3] = m[2][3] = 0.f;
      m[3][3] = 1.f;

      return m;
    }

  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
