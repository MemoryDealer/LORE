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

#include "Skybox.h"

#include <LORE/Core/Exception.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Scene/SpriteController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Skybox::Skybox()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Skybox::Layer& Skybox::addLayer( const string& name )
{
  Layer layer( name );
  layer.setMaterial( ResourceCast<Material>( _materialTemplate->clone( "bg_layer_" + name ) ) );

  log_information( "Added layer " + name + " to skybox " + _name );

  auto pair = _layers.insert( { name, layer } );
  return pair.first->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Skybox::Layer& Skybox::getLayer( const string& name )
{
  auto lookup = _layers.find( name );
  if ( _layers.end() == lookup ) {
    throw Lore::ItemIdentityException( "Could not find skybox layer named " + name + " in skybox " + _name );
  }

  return lookup->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Skybox::removeLayer( const string& name )
{
  auto lookup = _layers.find( name );
  if ( _layers.end() == lookup ) {
    throw Lore::ItemIdentityException( "Could not find skybox layer named " + name + " in skybox " + _name );
  }

  _layers.erase( lookup );

  log_information( "Remove layer " + name + " from skybox " + _name );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Skybox::setMaterialTemplate( const MaterialPtr material )
{
  _materialTemplate = material;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Skybox::Layer::setSprite( SpritePtr sprite )
{
  if ( _material ) {
    _material->sprite = sprite;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Skybox::Layer::setScrollSpeed( const glm::vec2& speed )
{
  if ( _material ) {
    _material->setTextureScrollSpeed( speed );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteControllerPtr Skybox::Layer::createSpriteController()
{
  _spriteController.reset();
  _spriteController = std::make_shared<SpriteController>();
  return _spriteController.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SpriteControllerPtr Skybox::Layer::getSpriteController() const
{
  return _spriteController.get();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
