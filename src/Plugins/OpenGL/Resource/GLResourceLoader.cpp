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

#include "GLResourceLoader.h"

#include <LORE2D/Resource/Material.h>
#include <LORE2D/Scene/Camera.h>

#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Shader/GLVertexBuffer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceLoader::ResourceLoader()
: _textureRegistry()
, _gpuProgramRegistry()
, _vertexShaderRegistry()
, _fragmentShaderRegistry()
, _vertexBufferRegistry()
, _materialRegistry()
, _cameraRegistry()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceLoader::~ResourceLoader()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::TexturePtr ResourceLoader::loadTexture( const string& name, const string& file )
{
    auto texture = std::make_unique<Texture>( name, file );

    auto handle = _textureRegistry.insert( name, std::move( texture ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr ResourceLoader::createGPUProgram( const string& name )
{
    auto program = std::make_unique<GPUProgram>( name );

    auto handle = _gpuProgramRegistry.insert( name, std::move( program ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ShaderPtr ResourceLoader::createVertexShader( const string& name )
{
    auto shader = std::make_unique<Shader>( name, Shader::Type::Vertex );

    auto handle = _vertexShaderRegistry.insert( name, std::move( shader ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ShaderPtr ResourceLoader::createFragmentShader( const string& name )
{
    auto shader = std::make_unique<Shader>( name, Shader::Type::Fragment );

    auto handle = _fragmentShaderRegistry.insert( name, std::move( shader ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::VertexBufferPtr ResourceLoader::createVertexBuffer( const string& name, const VertexBuffer::Type& type )
{
    auto vb = std::make_unique<VertexBuffer>( type );

    auto handle = _vertexBufferRegistry.insert( name, std::move( vb ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::MaterialPtr ResourceLoader::createMaterial( const string& name )
{
    auto mat = std::make_unique<Material>( name );

    auto handle = _materialRegistry.insert( name, std::move( mat ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::CameraPtr ResourceLoader::createCamera( const string& name )
{
    auto cam = std::make_unique<Camera>( name );

    auto handle = _cameraRegistry.insert( name, std::move( cam ) );
    return handle;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
