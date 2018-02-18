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

#include "Material.h"

#include <LORE2D/Core/Context.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Material::Material()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Material::~Material()
{
  if ( _texCoordCallback ) {
    Context::UnregisterFrameStartedCallback( _texCoordCallback );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

MaterialPtr Material::clone( const string& newName )
{
  MaterialPtr material = Resource::CreateMaterial( newName, getResourceGroupName() );
  material->lighting = lighting;
  material->ambient = ambient;
  material->diffuse = diffuse;
  material->sprite = sprite;
  material->program = program;
  material->blendingMode = blendingMode;
  material->_texCoordScrollSpeed = _texCoordScrollSpeed;
  material->_texCoordOffset = _texCoordOffset;
  material->_texSampleRegion = _texSampleRegion;

  // Re-assign callback for copied material.
  if ( _texCoordCallback ) {
    material->setTextureScrollSpeed( _texCoordScrollSpeed );
  }

  return material;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Material::setTextureScrollSpeed( const Vec2& scroll )
{
  // TODO: Put all texture scrolling in a single function (e.g., pass ref to offset).
  if ( !_texCoordCallback ) {
    // Register a callback to update the texture coordinates per frame.
    _texCoordCallback = [this] ( const FrameListener::FrameEvent& e ) {
      _texCoordOffset += ( _texCoordScrollSpeed );
    };

    Context::RegisterFrameStartedCallback( _texCoordCallback );
  }
  _texCoordScrollSpeed = scroll;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Material::setTextureSampleRegion( const Rect& region )
{
  _texSampleRegion = region;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Material::setTextureSampleRegion( const real x,
                                             const real y,
                                             const real w,
                                             const real h )
{
  setTextureSampleRegion( Rect( x, y, w, h ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
