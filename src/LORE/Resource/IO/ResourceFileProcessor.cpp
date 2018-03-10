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

#include "ResourceFileProcessor.h"

#include <LORE/Core/Util.h>
#include <LORE/Resource/Sprite.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Scene/SpriteController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::LoadConfiguration( const string& file, ResourceControllerPtr resourceController )
{
  Serializer serializer;
  serializer.deserialize( file );

  // Each JSON object should contain a group.
  for ( const auto& value : serializer.getValues() ) {
    const string& resourceGroup = value.first;

    // Within each JSON object should be a single array of directories.
    const auto& directories = value.second.toArray();
    for ( const auto& directoryValue : directories ) {
      const string& directory = directoryValue.toString();
      resourceController->indexResourceLocation( directory, resourceGroup );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SerializableResource ResourceFileProcessor::GetResourceFileType( const string& file )
{
  static const std::multimap<string, SerializableResource> ExtensionMapping = {
    { "spriteanimation", SerializableResource::SpriteAnimation },
    { "font", SerializableResource::Font },
    { "material", SerializableResource::Material },
    { "sprite", SerializableResource::Sprite },
    { "spritelist", SerializableResource::SpriteList },
    { "bmp", SerializableResource::Texture },
    { "jpg", SerializableResource::Texture },
    { "png", SerializableResource::Texture }
  };

  const string extension = Util::GetFileExtension( file );
  if ( !extension.empty() ) {
    auto lookup = ExtensionMapping.find( extension );
    if ( ExtensionMapping.end() != lookup ) {
      return lookup->second;
    }
  }

  return SerializableResource::Count;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceFileProcessor::ResourceFileProcessor( const string& file, const SerializableResource type )
: _file( file )
, _type( type )
{
  process();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::process()
{
  switch ( _type ) {
  default:
    _serializer.deserialize( _file );
    break;

  case SerializableResource::Texture:
    // Don't try to deserialize texture files.
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string ResourceFileProcessor::getName() const
{
  switch ( _type ) {
  default:
    return _serializer.getValue( "name" ).toString();

  case SerializableResource::SpriteList:
  case SerializableResource::Texture:
    return Util::GetFileName( _file );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

SerializableResource ResourceFileProcessor::getType() const
{
  return _type;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::load( const string& groupName, ResourceControllerPtr resourceController )
{
  switch ( _type ) {
  default:
    break;

  case SerializableResource::SpriteAnimation: {
    auto animationSet = resourceController->create<SpriteAnimationSet>( getName(), groupName );
    processAnimation( animationSet, _serializer.getValue( "animations" ), resourceController );
  } break;

  case SerializableResource::Font: {
    const auto file = _serializer.getValue( "file" ).toString();
    const auto size = _serializer.getValue( "size" ).toInt();
    auto font = resourceController->create<Font>( getName(), groupName );
    font->loadFromFile( file, size );
  } break;

  case SerializableResource::Material: {
    auto material = resourceController->create<Material>( getName(), groupName );
    processMaterial( material, _serializer.getValue( "settings" ), resourceController );
  } break;

  case SerializableResource::Sprite: {
    ;
  } break;

  case SerializableResource::SpriteList:
    processSpriteList( groupName, resourceController );
    break;

  case SerializableResource::Texture: {
    auto textureName = Util::GetFileName( _file );
    auto texture = resourceController->create<Texture>( textureName, groupName );
    texture->loadFromFile( _file );
  } break;

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::processAnimation( SpriteAnimationSetPtr animationSet, const SerializerValue& animations, ResourceControllerPtr resourceController )
{
  for ( const auto& animation : animations.getValues() ) {
    const auto& name = animation.first;
    const auto& settings = animation.second;
    const auto& frames = settings.get( "frames" ).toArray();
    const auto& deltaTimes = settings.get( "deltaTimes" ).toArray();

    // Create an animation to add to the animation set.
    SpriteController::Animation animation;

    // Process frames and delta times.
    for ( const auto& frame : frames ) {
      animation.frames.push_back( static_cast<size_t>( frame.toInt() ) );
    }
    for ( const auto& dt : deltaTimes ) {
      animation.deltaTimes.push_back( static_cast< long >( dt.toInt() ) );
    }

    animationSet->addAnimation( name, animation );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::processMaterial( MaterialPtr material, const SerializerValue& settings, ResourceControllerPtr resourceController )
{
  for ( const auto& value : settings.getValues() ) {
    string setting = Util::ToLower( value.first );

    if ( "sprite" == setting ) {
      material->sprite = resourceController->get<Sprite>( value.second.toString() );
    }
    else if ( "program" == setting ) {
      material->program = resourceController->get<GPUProgram>( value.second.toString() );
    }
    else if ( "stockprogram" == setting ) {
      material->program = StockResource::GetGPUProgram( value.second.toString() );
    }
    else if ( "diffuse" == setting ) {
      material->diffuse = value.second.toVec4();
    }
    else if ( "blend" == setting ) {
      material->blendingMode.enabled = value.second.toBool();
    }
    else if ( "sampleregion" == setting ) {
      auto region = value.second.toRect();
      material->setTextureSampleRegion( region );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::processSpriteList( const string& groupName, ResourceControllerPtr resourceController )
{
  for ( const auto& spriteValue : _serializer.getValues() ) {
    const string& name = spriteValue.first;

    // Create a sprite for this entry.
    auto sprite = resourceController->create<Sprite>( name, groupName );

    if ( SerializerValue::Type::Array == spriteValue.second.getType() ) {
      const auto& textureNames = spriteValue.second.toArray();

      if ( textureNames.empty() ) {
        log_warning( "Sprite " + name + " has no textures associated with it, ignoring..." );
        return;
      }

      // Retrieve all textures.
      for ( const auto& textureName : textureNames ) {
        sprite->addTexture( resourceController->get<Texture>( textureName.toString(), groupName ) );
      }
    }
    else if ( SerializerValue::Type::String == spriteValue.second.getType() ) {
      // Just a single texture.
      const auto& textureName = spriteValue.second.toString();
      sprite->addTexture( resourceController->get<Texture>( textureName, groupName ) );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //