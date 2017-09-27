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

#include "StockResource.h"

#include <LORE2D/Core/Context.h>
#include <LORE2D/Resource/Material.h>
#include <LORE2D/Resource/ResourceController.h>
#include <LORE2D/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::StockResourceController()
  : _controller( nullptr )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::~StockResourceController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResourceController::createStockResources()
{
  //
  // Load stock programs.

  // Lit programs.
  {
    UberProgramParameters params;

    createUberProgram( "StandardTextured", params );

    params.numTextures = 0;
    createUberProgram( "Standard", params );
  }

  // Unlit programs.
  {
    UberProgramParameters params;
    params.maxLights = 0;

    createUberProgram( "UnlitTextured", params );

    params.numTextures = 0;
    createUberProgram( "UnlitStandard", params );
  }

  //
  // Create stock textures.

  {
    auto texture = _controller->createTexture( "White" );
    texture->create( 128, 128, StockColor::White );
  }

  //
  // Load stock materials.

  // Lit materials.
  {
    auto material = _controller->createMaterial( "StandardTextured" );
    material->lighting = true;
    material->texture = _controller->getTexture( "White" );
    material->program = _controller->getGPUProgram( "StandardTextured" );
  }

  {
    auto material = _controller->createMaterial( "Standard" );
    material->lighting = true;
    material->program = _controller->getGPUProgram( "Standard" );
  }

  // Unlit materials.
  {
    auto material = _controller->createMaterial( "UnlitTextured" );
    material->lighting = false;
    material->texture = _controller->getTexture( "White" );
    material->program = _controller->getGPUProgram( "UnlitTextured" );
  }

  {
    auto material = _controller->createMaterial( "UnlitStandard" );
    material->lighting = false;
    material->program = _controller->getGPUProgram( "UnlitStandard" );
  }

  //
  // Create stock vertex buffers.

  {
    auto vb = _controller->createVertexBuffer( "TexturedQuad", MeshType::TexturedQuad );
    vb->build();
  }

  {
    auto vb = _controller->createVertexBuffer( "Quad", MeshType::Quad );
    vb->build();
  }

  //
  // Create stock meshes (uses previously created vertex buffers).

  {
    auto mesh = _controller->createMesh( "TexturedQuad", MeshType::TexturedQuad );
    _meshTable.insert( { MeshType::TexturedQuad, mesh } );
  }

  {
    auto mesh = _controller->createMesh( "Quad", MeshType::Quad );
    _meshTable.insert( { MeshType::Quad, mesh } );
  }

  // ::::::::::::::::::::::::: //

  //
  // Background stock resources.

  {
    BackgroundProgramParameters params;

    createBackgroundProgram( "Background", params );
  }

  {
    auto material = _controller->createMaterial( "Background" );
    material->lighting = false;
    material->texture = _controller->getTexture( "White" );
    material->program = _controller->getGPUProgram( "Background" );
  }

  {
    auto vb = _controller->createVertexBuffer( "Background", MeshType::Background );
    vb->build();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr StockResourceController::getGPUProgram( const string& name )
{
  return _controller->getGPUProgram( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResourceController::getMaterial( const string& name )
{
  return _controller->getMaterial( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr StockResourceController::getMesh( const MeshType& type )
{
  return _meshTable.at( type );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr StockResourceController::getTexture( const string& name )
{
  return _controller->getTexture( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBufferPtr StockResourceController::getVertexBuffer( const string& name )
{
  return _controller->getVertexBuffer( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResourceController::cloneMaterial( const string& name, const string& cloneName )
{
  return _controller->cloneMaterial( name, cloneName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr StockResource::GetGPUProgram( const string& name )
{
  return ActiveContext->getStockResourceController()->getGPUProgram( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResource::GetMaterial( const string& name )
{
  return ActiveContext->getStockResourceController()->getMaterial( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr StockResource::GetMesh( const MeshType& type )
{
  return ActiveContext->getStockResourceController()->getMesh( type );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr StockResource::GetTexture( const string& name )
{
  return ActiveContext->getStockResourceController()->getTexture( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBufferPtr StockResource::GetVertexBuffer( const string& name )
{
  return ActiveContext->getStockResourceController()->getVertexBuffer( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResource::CloneMaterial( const string& name, const string& newName )
{
  return ActiveContext->getStockResourceController()->cloneMaterial( name, newName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResource::AssignContext( ContextPtr context )
{
  ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
