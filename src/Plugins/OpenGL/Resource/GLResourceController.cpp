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

#include <Plugins/OpenGL/Resource/GLFont.h>
#include <Plugins/OpenGL/Resource/GLStockResource.h>
#include <Plugins/OpenGL/Resource/GLTexture.h>
#include <Plugins/OpenGL/Shader/GLGPUProgram.h>
#include <Plugins/OpenGL/Shader/GLShader.h>
#include <Plugins/OpenGL/Shader/GLVertexBuffer.h>
#include <Plugins/OpenGL/Window/GLRenderTarget.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLResourceController::GLResourceController()
{
  // Add creation functors.
  addCreationFunctor<Font>( std::bind( &GLResourceController::createFont, this ) );
  addCreationFunctor<GPUProgram>( std::bind( &GLResourceController::createGPUProgram, this ) );
  addCreationFunctor<RenderTarget>( std::bind( &GLResourceController::createRenderTarget, this ) );
  addCreationFunctor<Shader>( std::bind( &GLResourceController::createShader, this ) );
  addCreationFunctor<Texture>( std::bind( &GLResourceController::createTexture, this ) );
  addCreationFunctor<VertexBuffer>( std::bind( &GLResourceController::createVertexBuffer, this ) );

  // Create default vertex buffers.
  auto texturedQuadVB = create<VertexBuffer>( "TexturedQuad" );
  texturedQuadVB->init( MeshType::TexturedQuad );
  texturedQuadVB->build();

  auto quadVB = create<VertexBuffer>( "Quad" );
  quadVB->init( MeshType::Quad );
  quadVB->build();
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

Lore::IResourcePtr GLResourceController::createVertexBuffer()
{
  return MemoryAccess::GetPrimaryPoolCluster()->create<VertexBuffer, GLVertexBuffer>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //