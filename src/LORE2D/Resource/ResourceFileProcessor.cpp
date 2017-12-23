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

#include "ResourceFileProcessor.h"

#include <LORE2D/Core/Util.h>
#include <LORE2D/Resource/ResourceController.h>
#include <LORE2D/Resource/StockResource.h>

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

ResourceType ResourceFileProcessor::GetResourceFileType( const string& file )
{
  static const std::map<string, ResourceType> ExtensionMapping = {
    { "font", ResourceType::Font },
    { "material", ResourceType::Material },
    { "sprite", ResourceType::Sprite }
  };

  const string extension = Util::GetFileExtension( file );
  if ( !extension.empty() ) {
    auto lookup = ExtensionMapping.find( extension );
    if ( ExtensionMapping.end() != lookup ) {
      return lookup->second;
    }
  }

  return ResourceType::Count;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceFileProcessor::ResourceFileProcessor( const string& file, const ResourceType type )
: _file( file )
, _type( type )
{
  process();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::process()
{
  _serializer.deserialize( _file );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

string ResourceFileProcessor::getName() const
{
  return _serializer.getValue( "name" ).toString();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceType ResourceFileProcessor::getType() const
{
  return _type;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::load( const string& groupName, ResourceControllerPtr resourceController )
{
  switch ( _type ) {
  default:
    break;

  case ResourceType::Font: {
    const auto file = _serializer.getValue( "file" ).toString();
    const auto size = _serializer.getValue( "size" ).toInt();
    resourceController->loadFont( getName(), file, size, groupName );
  } break;

  case ResourceType::Material:
  {
    auto material = resourceController->createMaterial( getName(), groupName );
    processMaterial( material, _serializer.getValue( "settings" ), resourceController );
  } break;

  case ResourceType::Sprite: {
    const auto image = _serializer.getValue( "image" ).toString();
    resourceController->loadTexture( getName(), image, groupName );
  } break;

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::processMaterial( MaterialPtr material, const SerializerValue& settings, ResourceControllerPtr resourceController )
{
  for ( const auto& value : settings.getValues() ) {
    string setting = Util::ToLower( value.first );

    if ( "sprite" == setting ) {
      material->texture = resourceController->getTexture( value.second.toString() );
    }
    else if ( "program" == setting ) {
      material->program = resourceController->getGPUProgram( value.second.toString() );
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
