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

#include "GLResourceController.h"

#include <LORE2D/Resource/Material.h>
#include <LORE2D/Scene/Camera.h>

#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <Plugins/OpenGL/Resource/Renderable/GLTexture.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Shader/GLVertexBuffer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::ResourceController()
: Lore::ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceController::~ResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::TexturePtr ResourceController::loadTexture( const string& name, const string& file, const string& groupName )
{
    auto texture = MemoryAccess::GetPrimaryPoolCluster()->create<Lore::Texture, GLTexture>();
    texture->setName( name );
    texture->setResourceGroupName( groupName );
    texture->setMaterial( StockResource::GetMaterial( "StandardTexturedQuad" ) );
    texture->loadFromFile( file );

    _getGroup( groupName )->textures.insert( name, texture );
    return texture;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::GPUProgramPtr ResourceController::createGPUProgram( const string& name, const string& groupName )
{
    auto program = MemoryAccess::GetPrimaryPoolCluster()->create<GPUProgram, GLGPUProgram>();
    program->setName( name );
    program->setResourceGroupName( groupName );
    program->init();

    _getGroup( groupName )->programs.insert( name, program );
    return program;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ShaderPtr ResourceController::createVertexShader( const string& name, const string& groupName )
{
    auto shader = MemoryAccess::GetPrimaryPoolCluster()->create<Shader, GLShader>();
    shader->setName( name );
    shader->setResourceGroupName( groupName );
    shader->init( Shader::Type::Vertex );

    _getGroup( groupName )->vertexShaders.insert( name, shader );
    return shader;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::ShaderPtr ResourceController::createFragmentShader( const string& name, const string& groupName )
{
    auto shader = MemoryAccess::GetPrimaryPoolCluster()->create<Shader, GLShader>();
    shader->setName( name );
    shader->setResourceGroupName( groupName );
    shader->init( Shader::Type::Fragment );

    _getGroup( groupName )->fragmentShaders.insert( name, shader );
    return shader;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::VertexBufferPtr ResourceController::createVertexBuffer( const string& name, const Lore::VertexBuffer::Type& type, const string& groupName )
{
    auto vb = MemoryAccess::GetPrimaryPoolCluster()->create<VertexBuffer, GLVertexBuffer>();
    vb->setName( name );
    vb->setResourceGroupName( groupName );
    vb->init( type );

    _getGroup( groupName )->vertexBuffers.insert( name, vb );
    return vb;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::MaterialPtr ResourceController::createMaterial( const string& name, const string& groupName )
{
    auto mat = MemoryAccess::GetPrimaryPoolCluster()->create<Material>();
    mat->setName( name );
    mat->setResourceGroupName( groupName );

    _getGroup( groupName )->materials.insert( name, mat );
    return mat;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::CameraPtr ResourceController::createCamera( const string& name, const string& groupName )
{
    auto cam = MemoryAccess::GetPrimaryPoolCluster()->create<Camera>();
    cam->setName( name );

    _getGroup( groupName )->cameras.insert( name, cam );
    return cam;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyTexture( Lore::TexturePtr texture )
{
    auto groupName = texture->getResourceGroupName();
    _getGroup( groupName )->textures.remove( texture->getName() );

    MemoryAccess::GetPrimaryPoolCluster()->destroy<Texture, GLTexture>( texture );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceController::destroyTexture( const string& name, const string& groupName )
{
    auto texture = _getGroup( groupName )->textures.get( name );
    if ( texture ) {
        destroyTexture( texture );
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
