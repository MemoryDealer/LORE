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

#include <LORE/Scene/SceneLoader.h>

#include <LORE/Resource/Entity.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Scene/Scene.h>
#include <LORE/Shader/VertexBuffer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static Lore::VertexBuffer::Type StringToVertexBufferType( const Lore::string& str )
  {
    const std::unordered_map<Lore::string, Lore::VertexBuffer::Type> ConversionTable = {
      { "Quad", Lore::VertexBuffer::Type::Quad },
      { "TexturedQuad", Lore::VertexBuffer::Type::TexturedQuad }
    };

    auto lookup = ConversionTable.find( str );
    if ( ConversionTable.end() != lookup ) {
      return lookup->second;
    }
    throw Lore::Exception( "Invalid vertex buffer type specification" );
  }

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool SceneLoader::load( const string& sceneFile, ScenePtr scene )
{
  // Load the scene file into memory.
  if ( !_serializer.deserialize( sceneFile ) ) {
    throw Lore::Exception( "Unable to deserialize scene file " + sceneFile );
    return false;
  }

  _scene = scene;

  // Load each section of the scene file according to priority.
  _loadProperties();
  _loadEntities();

  return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_loadProperties()
{
  const string SceneProperties = "SceneProperties";
  auto properties = _serializer.getValue( SceneProperties );
  if ( !properties.isNull() && SerializerValue::Type::Container == properties.getType() ) {
    auto type = properties.getValue( "Type" );
    type.toString(); // Do nothing with scene type for now.

    auto resourceGroup = properties.getValue( "ResourceGroup" );
    if ( !resourceGroup.isNull() ) {
      _resourceGroupName = resourceGroup.toString();
    }
    else {
      _resourceGroupName = ResourceController::DefaultGroupName;
    }

    auto ambientLightColor = properties.getValue( "AmbientLight" );
    _scene->setAmbientLightColor( Color( ambientLightColor.toVec3(), 1.f ) );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_loadEntities()
{
  const string SceneEntities = "Entity";
  auto entities = _serializer.getValue( SceneEntities );
  if ( !entities.isNull() && SerializerValue::Type::Container == entities.getType() ) {
    // Iterate over each entity entry and load them.
    auto entityValues = entities.getValues();
    for ( const auto& entity : entityValues ) {
      const auto entityName = entity.first;
      const SerializerValue& value = entity.second;

      // Get the vertex buffer type.
      auto vbType = value.getValue( "VertexBufferType" );
      if ( vbType.isNull() ) {
        throw Lore::SerializerException( "Entity value " + entityName + " did not specify vertex buffer type" );
      }

      // Create the entity.
      auto entity = Resource::CreateEntity( entityName, StringToVertexBufferType( vbType.toString() ), _resourceGroupName );

      // Attach a sprite if specified.
      auto spriteName = value.getValue( "SpriteName" );
      if ( !spriteName.isNull() ) {
        entity->setSprite( Resource::GetSprite( spriteName.toString() ) );
      }

      // Process material settings for the entity.
      // ....
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
