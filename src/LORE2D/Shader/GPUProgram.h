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
#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Shader/Shader.h>
#include <LORE2D/Shader/VertexBuffer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    class LORE_EXPORT GPUProgram
    {

        LORE_OBJECT_BODY()

    public:

        GPUProgram();

        virtual ~GPUProgram();

        virtual void attachShader( ShaderPtr shader );

        virtual ShaderPtr getAttachedShader( const Shader::Type& type );

        void setVertexBuffer( VertexBufferPtr vb );

        virtual bool link() = 0;

        virtual void use() = 0;

        inline bool hasAttachedShader( const Shader::Type& type )
        {
            return ( _shaders.find( type ) != _shaders.end() );
        }

        //
        // Getters.

        inline VertexBufferPtr getVertexBuffer() const
        {
            return _vertexBuffer;
        }

        //
        // Uniform value updating.

        virtual void addTransformVar( const string& id ) = 0;

        virtual void setTransformVar( const Matrix4& m ) = 0;

        virtual void addUniformVar( const string& id ) = 0;

        virtual void setUniformVar( const string& id, const Matrix4& m ) = 0;

    protected:

        using ShaderMap = std::unordered_map<Shader::Type, ShaderPtr>;

    protected:

        ShaderMap _shaders;
        
        VertexBufferPtr _vertexBuffer;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
