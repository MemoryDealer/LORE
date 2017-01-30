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

#include <LORE2D/Shader/GPUProgram.h>

#include <Plugins/OpenGL/Math/MathConverter.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

    class GPUProgram : public Lore::GPUProgram
    {

    public:

        explicit GPUProgram( const string& name );

        virtual ~GPUProgram() override;

        virtual void attachShader( ShaderPtr shader ) override;

        virtual bool link() override;

        virtual void use() override;

        //
        // Uniform value updating.

        virtual void addTransformVar( const string& id ) override;

        virtual void setTransformVar( const Matrix4& m ) override;

        virtual void addUniformVar( const string& id ) override;

        virtual void setUniformVar( const string& id, const Matrix4& m ) override;

        void setUniformVar( const string& id, const glm::mat4x4& m );

    private:

        using UniformMap = std::unordered_map<string, GLint>;

    private:

        void _updateUniform( const GLint id, const Lore::Matrix4& m );

    private:

        GLuint _program;

        UniformMap _uniforms;

        GLint _transform;

    };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
