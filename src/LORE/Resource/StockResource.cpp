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

#include "StockResource.h"

#include <LORE/Core/Context.h>
#include <LORE/Resource/Material.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/Sprite.h>
#include <LORE/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

StockResourceFactory::StockResourceFactory( ResourceControllerPtr controller )
: _controller( controller )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
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
  // Create stock shaders.
  {

    UberProgramParameters params;

    params.maxPointLights = params.maxDirectionalLights = 0;
    auto& srf = _factories.at( RendererType::Forward2D );
    srf->createUberProgram( "UnlitTexturedRTT", params );
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
  // Create stock meshes (uses previously created vertex buffers from ResourceController's constructor).

  {
    auto mesh = _controller->create<Mesh>( "Quad" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "Quad" ) );
    _meshTable.insert( { VertexBuffer::Type::Quad, mesh } );
  }

  {
    auto mesh = _controller->create<Mesh>( "TexturedQuad" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "TexturedQuad" ) );
    _meshTable.insert( { VertexBuffer::Type::TexturedQuad, mesh } );
  }

  {
    auto mesh = _controller->create<Mesh>( "Cube" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "Cube" ) );
    _meshTable.insert( { VertexBuffer::Type::Cube, mesh } );
  }

  {
    auto mesh = _controller->create<Mesh>( "TexturedCube" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "TexturedCube" ) );
    _meshTable.insert( { VertexBuffer::Type::TexturedCube, mesh } );
  }

  {
    auto mesh = _controller->create<Mesh>( "Quad3D" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "Quad3D" ) );
    _meshTable.insert( { VertexBuffer::Type::Quad3D, mesh } );
  }

  {
    auto mesh = _controller->create<Mesh>( "TexturedQuad3D" );
    mesh->setVertexBuffer( _controller->get<VertexBuffer>( "TexturedQuad3D" ) );
    _meshTable.insert( { VertexBuffer::Type::TexturedQuad3D, mesh } );
  }

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
    vb->init( VertexBuffer::Type::Text );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void StockResourceController::createRendererStockResources( const RendererType type )
{
  StockResourceFactoryPtr srf = nullptr;
  auto lookup = _factories.find( type );
  if ( _factories.end() == lookup ) {
    throw Lore::Exception( "Unable to get stock resource factory" );
  }
  srf = lookup->second.get();

  string suffix;
  switch ( type ) {
  default:
  case RendererType::Forward2D:
    suffix = "2D";
    {
      auto vb = _controller->create<VertexBuffer>( "Skybox" + suffix );
      vb->init( VertexBuffer::Type::Skybox2D );
    }
    break;
    
  case RendererType::Forward3D:
    suffix = "3D";
    break;
  }

  //
  // Create stock programs.

  // Lit programs.
  {
    UberProgramParameters params;

    srf->createUberProgram( "StandardTextured" + suffix, params );

    params.numTextures = 0;
    srf->createUberProgram( "Standard" + suffix, params );
  }

  // Unlit programs.
  {
    UberProgramParameters params;
    params.maxPointLights = params.maxDirectionalLights = 0;

    srf->createUberProgram( "UnlitTextured" + suffix, params );

    params.numTextures = 0;
    srf->createUberProgram( "UnlitStandard" + suffix, params );
  }

  // Instanced programs.
  {
    UberProgramParameters params;
    params.instanced = true;

    srf->createUberProgram( "StandardTexturedInstanced" + suffix, params );

    params.numTextures = 0;
    srf->createUberProgram( "StandardInstanced" + suffix, params );
  }

  //
  // Create stock materials.

  // Lit materials.
  {
    auto material = _controller->create<Material>( "StandardTextured" + suffix );
    material->lighting = true;
    material->sprite = _controller->get<Sprite>( "White" );
    material->program = _controller->get<GPUProgram>( "StandardTextured" + suffix );
  }

  {
    auto material = _controller->create<Material>( "Standard" + suffix );
    material->lighting = true;
    material->program = _controller->get<GPUProgram>( "Standard" + suffix );
  }

  // Unlit materials.
  {
    auto material = _controller->create<Material>( "UnlitTextured" + suffix );
    material->lighting = false;
    material->sprite = _controller->get<Sprite>( "White" );
    material->program = _controller->get<GPUProgram>( "UnlitTextured" + suffix );
  }

  {
    auto material = _controller->create<Material>( "UnlitStandard" + suffix );
    material->lighting = false;
    material->program = _controller->get<GPUProgram>( "UnlitStandard" + suffix );
  }

  //
  // Skybox stock resources.

  {
    SkyboxProgramParameters params;
    srf->createSkyboxProgram( "Skybox" + suffix, params );
  }

  {
    auto material = _controller->create<Material>( "Skybox" + suffix );
    material->lighting = false;
    material->sprite = _controller->get<Sprite>( "White" );
    material->program = _controller->get<GPUProgram>( "Skybox" + suffix );
  }

  //
  // Box stock resources.

  srf->createBoxProgram( "StandardBox" + suffix );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MeshPtr StockResourceController::getMesh( const VertexBuffer::Type& type )
{
  auto it = _meshTable.find( type );
  if ( _meshTable.end() != it ) {
    return it->second;
  }
  return nullptr;
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

MeshPtr StockResource::GetMesh( const VertexBuffer::Type& type )
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
