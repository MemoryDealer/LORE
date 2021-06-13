#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include <LORE/Shader/GPUProgram.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

    class GLGPUProgram : public Lore::GPUProgram,
                         public Alloc<GLGPUProgram>
    {

    public:

        GLGPUProgram();
        ~GLGPUProgram() override;

        void init() override;

        void attachShader( ShaderPtr shader ) override;

        bool link() override;

        void use() override;

        //
        // Uniform value updating.

        void addTransformVar( const string& id ) override;

        void setTransformVar( const glm::mat4& m ) override;

        void addUniformVar( const string& id ) override;

        void setUniformVar( const string& id, const glm::mat4& m ) override;
        void setUniformVar( const string& id, const glm::vec2& v ) override;
        void setUniformVar( const string& id, const glm::vec3& v ) override;
        void setUniformVar( const string& id, const glm::vec4& v ) override;
        void setUniformVar( const string& id, const real r ) override;
        void setUniformVar( const string& id, const uint32_t i ) override;
        void setUniformVar( const string& id, const int i ) override;

    private:

        using UniformMap = std::unordered_map<string, GLint>;

    private:

        GLuint _getUniform( const string& id );

        void _updateUniform( const GLint id, const glm::mat4& m );

    private:

        GLuint _program;

        UniformMap _uniforms;

        GLint _transform;

    };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
