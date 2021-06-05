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

#include <LORE/Resource/Prefab.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>
#include <LORE/Scene/IO/ModelLoader.h>
#include <LORE/Scene/Model.h>
#include <LORE/Scene/Scene.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static Lore::Mesh::Type StringToMeshType( const Lore::string& str )
  {
    const std::unordered_map<Lore::string, Lore::Mesh::Type> ConversionTable = {
      { "Quad", Lore::Mesh::Type::Quad3D },
      { "TexturedQuad", Lore::Mesh::Type::TexturedQuad3D },
      { "Cube", Lore::Mesh::Type::Cube },
      { "TexturedCube", Lore::Mesh::Type::TexturedCube }
    };
    // TODO: Check scene's renderer type to use quad or quad3D.
    auto lookup = ConversionTable.find( str );
    if ( ConversionTable.end() != lookup ) {
      return lookup->second;
    }
    throw Lore::Exception( "Invalid model type specification" );
  }

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool SceneLoader::process( const string& sceneFile, ScenePtr scene )
{
  // Load the scene file into memory.
  if ( !_serializer.deserialize( sceneFile ) ) {
    throw Lore::Exception( "Unable to deserialize scene file " + sceneFile );
    return false;
  }

  _scene = scene;

  // Load each section of the scene file according to priority.
  _loadProperties();
  _loadPrefabs();
  _loadLighting();
  _loadLayout();

  // Store the location of the scene file inside the scene for reloading.
  _scene->setSceneFile( sceneFile );

  return true;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::setUnloadPrefabs( const bool unload )
{
  _unloadPrefabs = unload;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_loadProperties()
{
  const string SceneProperties = "SceneProperties";
  const auto& properties = _serializer.getValue( SceneProperties );
  if ( !properties.isNull() && SerializerValue::Type::Container == properties.getType() ) {
    // Get the scene type.
    auto type = properties.getValue( "Type" );
    type.toString(); // Do nothing with scene type for now.

    // Get resource group name for scene resources.
    auto resourceGroup = properties.getValue( "ResourceGroup" );
    if ( !resourceGroup.isNull() ) {
      _resourceGroupName = resourceGroup.toString();
    }
    else {
      _resourceGroupName = ResourceController::DefaultGroupName;
    }

    // Get the skybox color.
    auto skyboxColor = properties.getValue( "SkyboxColor" );
    if ( !skyboxColor.isNull() ) {
      _scene->setSkyboxColor( Color( skyboxColor.toVec3(), 1.f ) );
    }

    // Get the skybox sprite.
    auto skyboxSprite = properties.getValue( "SkyboxSprite" );
    if ( !skyboxSprite.isNull() ) {
      // TODO: Add multi-layer loading.
      auto& skyboxLayer = _scene->getSkybox()->addLayer( "layer0" );
      skyboxLayer.setSprite( Lore::Resource::GetSprite( skyboxSprite.toString()) );
    }

    // Get the ambient light color.
    auto ambientLightColor = properties.getValue( "AmbientLightColor" );
    if ( !ambientLightColor.isNull() ) {
      _scene->setAmbientLightColor( Color( ambientLightColor.toVec3(), 1.f ) );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_loadPrefabs()
{
  if ( _unloadPrefabs ) {
    // Unload all prefabs in the scene's resource group.
    Resource::DestroyPrefabsInGroup( _resourceGroupName );
  }

  const string ScenePrefabs = "Prefab";
  const auto& prefabs = _serializer.getValue( ScenePrefabs );
  if ( !prefabs.isNull() && SerializerValue::Type::Container == prefabs.getType() ) {
    // Iterate over each prefab entry and load them.
    auto prefabValues = prefabs.getValues();
    for ( const auto& prefab : prefabValues ) {
      const auto prefabName = prefab.first;
      const SerializerValue& value = prefab.second;

      PrefabPtr prefab = nullptr;

      // Get the model type.
      const auto& modelType = value.getValue( "ModelType" );
      if ( modelType.isNull() ) {
        // If there is no model type, look for a model path to load.
        const auto& modelPath = value.getValue( "Model" );
        if ( modelPath.isNull() ) {
          throw Lore::SerializerException( "Prefab value " + prefabName + " did not specify model type or a model" );
        }

        // Load the prefab (load model from disk and assign a material).
        try {
          prefab = Resource::LoadPrefab( prefabName, modelPath.toString(), _resourceGroupName );
        }
        catch ( Lore::Exception& e ) {
          LogWrite( Error, "Error loading prefab %s: %s", prefabName.c_str(), e.what() );
          prefab = Resource::GetPrefab( prefabName, _resourceGroupName );
          prefab->setModel( Resource::GetModel( FileUtil::GetFileName( modelPath.toString() ), _resourceGroupName ) );
        }
      }
      else {
        // Create the prefab.
        prefab = Resource::CreatePrefab( prefabName, StringToMeshType( modelType.toString() ), _resourceGroupName );
      }

      // Enable instancing if specified.
      const auto& instanced = value.getValue( "Instanced" );
      if ( !instanced.isNull() && SerializerValue::Type::Int == instanced.getType() ) {
        const auto instanceCount = instanced.toInt();
        prefab->enableInstancing( instanceCount );
      }

      // Process material settings for the prefab.
      const auto& stockMaterial = value.getValue( "StockMaterial" );
      if ( !stockMaterial.isNull() ) {
        if ( stockMaterial.getType() == SerializerValue::Type::String ) {
          auto mat = StockResource::GetMaterial( stockMaterial.toString() );
          prefab->setMaterial( mat->clone( mat->getName() + "_" + prefab->getName() ) );
        }
      }
      const auto& materialSettings = value.getValue( "MaterialSettings" );
      if ( !materialSettings.isNull() ) {
        _processMaterialSettings( materialSettings, prefab );
      }

      // Attach a sprite if specified.
      const auto& spriteName = value.getValue( "Sprite" );
      if ( !spriteName.isNull() ) {
        prefab->setSprite( Resource::GetSprite( spriteName.toString() ) );
      }

      // Shadow settings.
      const auto& castShadows = value.getValue( "CastShadows" );
      if ( !castShadows.isNull() ) {
        prefab->castShadows = castShadows.toBool();
      }
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_loadLighting()
{
  const string Lighting = "Lighting";
  const auto& lighting = _serializer.getValue( Lighting );
  if ( !lighting.isNull() && SerializerValue::Type::Container == lighting.getType() ) {
    // Iterate over each lighting entry.
    auto lightingValues = lighting.getValues();
    for ( const auto& lightingValue : lightingValues ) {
      const auto& lightName = lightingValue.first;
      const auto& lightData = lightingValue.second;

      // Determine light type.
      Light::Type type;
      const auto& lightTypeValue = lightData.getValue( "Type" );
      if ( SerializerValue::Type::String != lightTypeValue.getType() ) {
        LogWrite( Warning, "Invalid light type for light %s", lightName.c_str() );
        continue;
      }
      const auto& lightType = lightTypeValue.toString();
      if ( "Directional" == lightType ) {
        type = Light::Type::Directional;
      }
      else if ( "Point" == lightType ) {
        type = Light::Type::Point;
      }
      else if ( "Spot" == lightType || "Spotlight" == lightType ) {
        type = Light::Type::Spot;
      }
      else {
        LogWrite( Warning, "Invalid light type for %s - should be Directional, point, or spot", lightName.c_str() );
      }

      // Create the light in the scene.
      LightPtr light = nullptr;
      switch ( type ) {
      default:
        continue;

      case Light::Type::Directional:
        light = _scene->createDirectionalLight( lightName );
        {
          // Set direction.
          const auto& directionValue = lightData.getValue( "Direction" );
          if ( SerializerValue::Type::Array == directionValue.getType() ) {
            static_cast<DirectionalLightPtr>( light )->setDirection( directionValue.toVec3() );
          }
        }
        break;

      case Light::Type::Point:
        light = _scene->createPointLight( lightName );
        {
          // Set attenuation.
          const auto& attenuationValue = lightData.getValue( "Attenuation" );
          if ( SerializerValue::Type::Array == attenuationValue.getType() ) {
            const auto& attenuation = attenuationValue.toVec4();
            static_cast<PointLightPtr>( light )->setAttenuation( attenuation.x, attenuation.y, attenuation.z, attenuation.w );
          }
        }
        break;

      case Light::Type::Spot:
        // TODO: Implement spotlights.
        break;
      }

      // Gather color data.
      Color ambient, diffuse, specular;
      const auto& ambientValue = lightData.getValue( "Ambient" );
      if ( SerializerValue::Type::Array == ambientValue.getType() ) {
        ambient = glm::vec4( ambientValue.toVec3(), 1.f );
      }
      const auto& diffuseValue = lightData.getValue( "Diffuse" );
      if ( SerializerValue::Type::Array == diffuseValue.getType() ) {
        // TODO: Write helper parser functions for JSON.
        diffuse = glm::vec4( diffuseValue.toVec3(), 1.f );
      }
      const auto& specularValue = lightData.getValue( "Specular" );
      if ( SerializerValue::Type::Array == specularValue.getType() ) {
        specular = glm::vec4( specularValue.toVec3(), 1.f );
      }

      light->setAmbient( ambient );
      light->setDiffuse( diffuse );
      light->setSpecular( specular );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_loadLayout()
{
  const string Layout = "Layout";
  const auto& layout = _serializer.getValue( Layout );
  if ( layout.isNull() ) {
    return;
  }

  // Loads individual nodes.
  auto nodes = layout.getValue( "Nodes" );
  if ( !nodes.isNull() ) {
    for ( const auto& node : nodes.getValues() ) {
      const string& nodeName = node.first;
      const auto& nodeData = node.second;
      _processNode( nodeName, nodeData );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_processMaterialSettings( const SerializerValue& value, PrefabPtr prefab )
{
  auto material = prefab->getMaterial();

  //
  // Look for material settings.

  // Colors.
  const auto& ambient = value.getValue( "Ambient" );
  if ( !ambient.isNull() ) {
    material->ambient = glm::vec4( ambient.toVec3(), 1.f );
  }
  const auto& diffuse = value.getValue( "Diffuse" );
  if ( !diffuse.isNull() ) {
    material->diffuse = glm::vec4( diffuse.toVec3(), 1.f );
  }
  const auto& specular = value.getValue( "Specular" );
  if ( !specular.isNull() ) {
    material->specular = glm::vec4( specular.toVec3(), 1.f );
  }

  const auto& blending = value.getValue( "Blending" );
  if ( !blending.isNull() ) {
    material->blendingMode.enabled = blending.toBool();
  }

  // Other properties.
  const auto& shininess = value.getValue( "Shininess" );
  if ( !shininess.isNull() ) {
    material->shininess = shininess.toReal();
  }

  // UV scaling.
  const auto& uvScale = value.getValue( "UVScale" );
  if ( !uvScale.isNull() ) {
    material->uvScale = uvScale.toVec2();
  }

  // Scrolling.
  const auto& scrollSpeed = value.getValue( "ScrollSpeed" );
  if ( !scrollSpeed.isNull() ) {
    const auto scrollSpeedXY = scrollSpeed.toVec2();
    material->setTextureScrollSpeed( scrollSpeedXY );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void SceneLoader::_processNode( const string& nodeName, const SerializerValue& nodeData, const NodePtr parent )
{
  // Create the node.
  NodePtr node = nullptr;
  if ( parent ) {
    node = parent->createChildNode( nodeName );
  }
  else {
    node = _scene->createNode( nodeName );
  }

  // Get the prefab to attach to this node and attach it.
  const auto& prefabName = nodeData.getValue( "Prefab" );
  if ( !prefabName.isNull() ) {
    auto prefab = Resource::GetPrefab( prefabName.toString(), _resourceGroupName );
    node->attachObject( prefab );
  }

  // Get the light to attach to this node if any.
  const auto& lightName = nodeData.getValue( "Light" );
  if ( !lightName.isNull() && SerializerValue::Type::String == lightName.getType() ) {
    bool lightFound = false;
    // Look for point light of this name.
    try {
      auto pointLight = _scene->getLight( Light::Type::Point, lightName.toString() );
      if ( pointLight ) {
        lightFound = true;
        node->attachObject( pointLight );
      }
    }
    catch ( ItemIdentityException& ) { }

    if ( !lightFound ) {
      // Look for spotlight of this name.
      try {
        auto spotLight = _scene->getLight( Light::Type::Spot, lightName.toString() );
        if ( spotLight ) {
          node->attachObject( spotLight );
        }
      }
      catch( ItemIdentityException& ){
        LogWrite( Error, "No light found with name %s", lightName.toString().c_str() );
      }
    }
  }

  // Get the transform data.
  const auto& position = nodeData.getValue( "Position" );
  if ( !position.isNull() ) {
    node->setPosition( position.toVec3() );
  }

  const auto& orientation = nodeData.getValue( "Orientation" );
  if ( !orientation.isNull() ) {
    const auto arraySize = orientation.toArray().size();
    if ( 3 == arraySize ) {
      node->setOrientation( orientation.toVec3() );
    }
    else if ( 4 == arraySize ) {
      // This is a quaternion.
      node->setOrientation( orientation.toVec4() );
    }
  }

  const auto& scale = nodeData.getValue( "Scale" );
  if ( !scale.isNull() ) {
    if ( SerializerValue::Type::Real == scale.getType() ) {
      node->setScale( scale.toReal() );
    }
    else if ( SerializerValue::Type::Array == scale.getType() ) {
      node->setScale( scale.toVec3() );
    }
  }

  // Recursively load child nodes.
  const auto& childNodes = nodeData.getValue( "ChildNodes" );
  if ( SerializerValue::Type::Container == childNodes.getType() ) {
    const auto& childNodeValues = childNodes.getValues();
    for ( const auto& childNodeValue : childNodeValues ) {
      const string& nodeName = childNodeValue.first;
      const auto& nodeData = childNodeValue.second;
      // Each child node value is a container.
      if ( Lore::SerializerValue::Type::Container == nodeData.getType() ) {
        _processNode( nodeName, nodeData, node );
      }
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //s
