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

#include <LORE2D/Math/Math.h>

#include <Plugins/ThirdParty/glm/glm.hpp>
#include <Plugins/ThirdParty/glm/gtc/matrix_transform.hpp>
#include <Plugins/ThirdParty/glm/gtc/type_ptr.hpp>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

    class MathConverter final
    {

    public:

        //
        // Lore -> GLM.

        static inline glm::mat4x4 LoreToGLM( const Lore::Matrix4& m )
        {
            return glm::mat4x4( m[0][0], m[0][1], m[0][2], m[0][3],
                                m[1][0], m[1][1], m[1][2], m[1][3],
                                m[2][0], m[2][1], m[2][2], m[2][3],
                                m[3][0], m[3][1], m[3][2], m[3][3] );
        }

        static inline glm::vec2 LoreToGLM( const Lore::Vec2& v )
        {
          glm::vec3 re;
          re.x = v.x;
          re.y = v.y;
          return re;
        }

        static inline glm::vec3 LoreToGLM( const Lore::Vec3& v )
        {
            glm::vec3 re;
            re.x = v.x;
            re.y = v.y;
            re.z = v.z;
            return re;
        }

        //
        // GLM -> Lore.

        static inline Lore::Matrix4 GLMToLore( const glm::mat4x4& m )
        {
            return Lore::Matrix4( m[0][0], m[0][1], m[0][2], m[0][3],
                                  m[1][0], m[1][1], m[1][2], m[1][3],
                                  m[2][0], m[2][1], m[2][2], m[2][3],
                                  m[3][0], m[3][1], m[3][2], m[3][3] );
        }

    };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
