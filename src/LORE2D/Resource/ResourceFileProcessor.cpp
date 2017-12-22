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

ResourceFileProcessor::ResourceFileProcessor( const string& file ) : _file( file )
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
  return _serializer.getValue( "name" ).getString();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ResourceType ResourceFileProcessor::getType() const
{
  // Get type of resource.
  const static std::map<string, ResourceType> ResourceTypeMap = {
    { "material", ResourceType::Material },
    { "sprite", ResourceType::Sprite }
  };

  const auto resourceTypeValue = _serializer.getValue( "type" );
  auto resourceTypeName = resourceTypeValue.getString();
  Util::ToLower( resourceTypeName );
  const auto resourceTypeIt = ResourceTypeMap.find( resourceTypeValue.getString() );
  if ( ResourceTypeMap.end() == resourceTypeIt ) {
    throw Lore::Exception( "Invalid resource type " + resourceTypeValue.getString() );
  }

  return resourceTypeIt->second;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::load( const string& groupName, ResourceControllerPtr resourceController )
{
  switch ( getType() ) {
  default:
    break;

  case ResourceType::Material:
  {
    auto material = resourceController->createMaterial( getName(), groupName );
    processMaterial( material, _serializer.getValue( "settings" ), resourceController );
  } break;

  case ResourceType::Sprite: {
    auto image = _serializer.getValue( "image" ).getString();
    resourceController->loadTexture( getName(), image, groupName );
  } break;

  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::loadConfiguration( const string& file, ResourceControllerPtr resourceController )
{
  Serializer serializer;
  serializer.deserialize( file );

  // Each JSON object should contain a group.
  for ( const auto& value : serializer.getValues() ) {
    const string& resourceGroup = value.first;

    // Within each JSON object should be a single array of directories.
    const auto& directories = value.second.getArray();
    for ( const auto& directoryValue : directories ) {
      const string& directory = directoryValue.getString();
      resourceController->indexResourceLocation( directory, resourceGroup );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ResourceFileProcessor::processMaterial( MaterialPtr material, const SerializerValue& settings, ResourceControllerPtr resourceController )
{
  for ( const auto& value : settings.getValues() ) {
    string setting = Util::ToLower( value.first );

    if ( "sprite" == setting ) {
      material->texture = resourceController->getTexture( value.second.getString() );
    }
    else if ( "program" == setting ) {
      try {
        material->program = resourceController->getGPUProgram( value.second.getString() );
      }
      catch ( Lore::ItemIdentityException& ) {
        material->program = StockResource::GetGPUProgram( value.second.getString() );
      }
    }
    else if ( "diffuse" == setting ) {
      auto& color = value.second.getArray();
      material->diffuse = Color( color[0].getReal(), color[1].getReal(), color[2].getReal(), color[3].getReal() );
    }
    else if ( "blend" == setting ) {
      material->blendingMode.enabled = value.second.getBool();
    }
    else if ( "sampleregion" == setting ) {
      auto& region = value.second.getArray();
      material->setTextureSampleRegion( region[0].getReal(), region[1].getReal(), region[2].getReal(), region[3].getReal() );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
