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

#include "GLResourceController.h"

#include <LORE/Resource/Material.h>
#include <LORE/Scene/Camera.h>

#include <Plugins/OpenGL/Resource/GLFont.h>
#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <Plugins/OpenGL/Resource/GLTexture.h>
#include <Plugins/OpenGL/Scene/GLMesh.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Window/GLRenderTarget.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLResourceController::GLResourceController()
{
  // Add creation/destruction functors.
  addCreationFunctor<Font>( std::bind( &GLResourceController::createFont, this ) );
  addCreationFunctor<GPUProgram>( std::bind( &GLResourceController::createGPUProgram, this ) );
  addCreationFunctor<Mesh>( std::bind( &GLResourceController::createMesh, this ) );
  addCreationFunctor<RenderTarget>( std::bind( &GLResourceController::createRenderTarget, this ) );
  addCreationFunctor<Shader>( std::bind( &GLResourceController::createShader, this ) );
  addCreationFunctor<Texture>( std::bind( &GLResourceController::createTexture, this ) );
  addDestructionFunctor<Font>( std::bind( &GLResourceController::destroyFont, this, std::placeholders::_1 ) );
  addDestructionFunctor<GPUProgram>( std::bind( &GLResourceController::destroyGPUProgram, this, std::placeholders::_1 ) );
  addDestructionFunctor<Mesh>( std::bind( &GLResourceController::destroyMesh, this, std::placeholders::_1 ) );
  addDestructionFunctor<RenderTarget>( std::bind( &GLResourceController::destroyRenderTarget, this, std::placeholders::_1 ) );
  addDestructionFunctor<Shader>( std::bind( &GLResourceController::destroyShader, this, std::placeholders::_1 ) );
  addDestructionFunctor<Texture>( std::bind( &GLResourceController::destroyTexture, this, std::placeholders::_1 ) );

  // Create default models.
  auto quad = create<Model>( "Quad" );
  auto quadMesh = create<Mesh>( "Quad" );
  quadMesh->init( Mesh::Type::Quad );
  quad->attachMesh( quadMesh );

  auto texturedQuad = create<Model>( "TexturedQuad" );
  auto texturedQuadMesh = create<Mesh>( "TexturedQuad" );
  texturedQuadMesh->init( Mesh::Type::TexturedQuad );
  texturedQuad->attachMesh( texturedQuadMesh );

  auto cube = create<Model>( "Cube" );
  auto cubeMesh = create<Mesh>( "Cube" );
  cubeMesh->init( Mesh::Type::Cube );
  cube->attachMesh( cubeMesh );

  auto texturedCube = create<Model>( "TexturedCube" );
  auto texturedCubeMesh = create<Mesh>( "TexturedCube" );
  texturedCubeMesh->init( Mesh::Type::TexturedCube );
  texturedCube->attachMesh( texturedCubeMesh );

  auto quad3D = create<Model>( "Quad3D" );
  auto quad3DMesh = create<Mesh>( "Quad3D" );
  quad3DMesh->init( Mesh::Type::Quad3D );
  quad3D->attachMesh( quad3DMesh );

  auto texturedQuad3D = create<Model>( "TexturedQuad3D" );
  auto texturedQuad3DMesh = create<Mesh>( "TexturedQuad3D" );
  texturedQuad3DMesh->init( Mesh::Type::TexturedQuad3D );
  texturedQuad3D->attachMesh( texturedQuad3DMesh );

  auto boundingBox = create<Model>( "BoundingBox" );
  auto boundingBoxMesh = create<Mesh>( "BoundingBox" );
  boundingBoxMesh->init( Mesh::Type::BoundingBox );
  boundingBox->attachMesh( boundingBoxMesh );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::IResourcePtr GLResourceController::createFont()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<Font, GLFont>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::IResourcePtr GLResourceController::createGPUProgram()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<GPUProgram, GLGPUProgram>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::IResourcePtr GLResourceController::createMesh()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<Mesh, GLMesh>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::IResourcePtr GLResourceController::createRenderTarget()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<RenderTarget, GLRenderTarget>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::IResourcePtr GLResourceController::createShader()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<Shader, GLShader>();
}


// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::IResourcePtr GLResourceController::createTexture()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<Texture, GLTexture>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLResourceController::destroyFont( Lore::IResourcePtr resource )
{
  MemoryAccess::GetPrimaryPoolCluster()->destroy<Font, GLFont>( static_cast< FontPtr >( resource ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLResourceController::destroyGPUProgram( Lore::IResourcePtr resource )
{
  MemoryAccess::GetPrimaryPoolCluster()->destroy<GPUProgram, GLGPUProgram>( static_cast< GPUProgramPtr >( resource ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLResourceController::destroyMesh( Lore::IResourcePtr resource )
{
  MemoryAccess::GetPrimaryPoolCluster()->destroy<Mesh, GLMesh>( ResourceCast<Mesh>( resource ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLResourceController::destroyRenderTarget( Lore::IResourcePtr resource )
{
  MemoryAccess::GetPrimaryPoolCluster()->destroy<RenderTarget, GLRenderTarget>( static_cast< RenderTargetPtr >( resource ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLResourceController::destroyShader( Lore::IResourcePtr resource )
{
  MemoryAccess::GetPrimaryPoolCluster()->destroy<Shader, GLShader>( static_cast< ShaderPtr >( resource ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLResourceController::destroyTexture( Lore::IResourcePtr resource )
{
  MemoryAccess::GetPrimaryPoolCluster()->destroy<Texture, GLTexture>( static_cast< TexturePtr >( resource ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //