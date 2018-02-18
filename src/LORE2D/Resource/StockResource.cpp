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
#include <LORE2D/Resource/Renderable/Sprite.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceController::StockResourceController()
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

  // RTT programs.
  {
    UberProgramParameters params;

    // When rendering to texture, don't flip Y texture coordinates.
    //params.texYCoordinateFlipped = false;
    params.maxLights = 0;
    createUberProgram( "UnlitTexturedRTT", params );
  }

  //
  // Create stock textures/sprites.

  {
    auto texture = _controller->create<Texture>( "White" );
    texture->create( 128, 128, StockColor::White );
    auto sprite = _controller->create<Sprite>( "White" );
    sprite->addTexture( texture );
  }

  //
  // Load stock materials.

  // Lit materials.
  {
    auto material = _controller->create<Material>( "StandardTextured" );
    material->lighting = true;
    material->sprite = _controller->get<Sprite>( "White" );
    material->program = _controller->get<GPUProgram>( "StandardTextured" );
  }

  {
    auto material = _controller->create<Material>( "Standard" );
    material->lighting = true;
    material->program = _controller->get<GPUProgram>( "Standard" );
  }

  // Unlit materials.
  {
    auto material = _controller->create<Material>( "UnlitTextured" );
    material->lighting = false;
    material->sprite = _controller->get<Sprite>( "White" );
    material->program = _controller->get<GPUProgram>( "UnlitTextured" );
  }

  {
    auto material = _controller->create<Material>( "UnlitStandard" );
    material->lighting = false;
    material->program = _controller->get<GPUProgram>( "UnlitStandard" );
  }

  //
  // Create stock meshes (uses previously created vertex buffers).

  {
    auto mesh = _controller->create<Mesh>( "TexturedQuad" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "TexturedQuad" ) );
    _meshTable.insert( { MeshType::TexturedQuad, mesh } );
  }

  {
    auto mesh = _controller->create<Mesh>( "Quad" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "Quad" ) );
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
    auto material = _controller->create<Material>( "Background" );
    material->lighting = false;
    material->sprite = _controller->get<Sprite>( "White" );
    material->program = _controller->get<GPUProgram>( "Background" );
  }

  {
    auto vb = _controller->create<VertexBuffer>( "Background" );
    vb->init( MeshType::Background );
    vb->build();
  }

  // ::::::::::::::::::::::::: //

  //
  // Box stock resources.

  {
    createBoxProgram( "StandardBox" );
  }

  // ::::::::::::::::::::::::: //

  //
  // Font stock resources.

  {
    auto defaultFont = _controller->create<Font>( "Default" );
    defaultFont->loadFromFile( "res/stock/fonts/core.ttf", 12 );
    auto debugUIFont = _controller->create<Font>( "DebugUI" );
    debugUIFont->loadFromFile( "res/stock/fonts/core.ttf", 7 );
  }

  {
    // Generate shaders and vertex buffer for text rendering.
    createTextProgram( "StandardText" );
    auto vb = _controller->create<VertexBuffer>( "StandardText" );
    vb->init( MeshType::Text );
    vb->build();
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr StockResourceController::getMesh( const MeshType& type )
{
  return _meshTable.at( type );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GPUProgramPtr StockResource::GetGPUProgram( const string& name )
{
  return ActiveContext->getStockResourceController()->get<GPUProgram>( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr StockResource::GetMaterial( const string& name )
{
  return ActiveContext->getStockResourceController()->get<Material>( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr StockResource::GetMesh( const MeshType& type )
{
  return ActiveContext->getStockResourceController()->getMesh( type );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TexturePtr StockResource::GetTexture( const string& name )
{
  return ActiveContext->getStockResourceController()->get<Texture>( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

VertexBufferPtr StockResource::GetVertexBuffer( const string& name )
{
  return ActiveContext->getStockResourceController()->get<VertexBuffer>( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

FontPtr StockResource::GetFont( const string& name )
{
  return ActiveContext->getStockResourceController()->get<Font>( name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResource::AssignContext( ContextPtr context )
{
  ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
