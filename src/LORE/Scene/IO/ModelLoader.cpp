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

#include "ModelLoader.h"

#include <LORE/Resource/Material.h>
#include <LORE/Resource/Sprite.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Util/FileUtils.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelLoader::ModelLoader( const string& resourceGroupName )
: _resourceGroupName( resourceGroupName )
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

ModelPtr ModelLoader::load( const string& path, const bool loadTextures )
{
  _loadTextures = loadTextures;

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile( path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );

  if ( !scene || ( scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE ) || !scene->mRootNode ) {
    throw Exception( "Failed to load model at " + path + ": " + importer.GetErrorString() );
  }

  _name = Util::GetFileName( path );
  _directory = Util::GetFileDir( path );

  // Allocate a model, processed meshes will be attached to this.
  _model = Resource::CreateModel( _name, Mesh::Type::Custom, _resourceGroupName );

  _processNode( scene->mRootNode, scene );

  return _model;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ModelLoader::_processNode( aiNode* node, const aiScene* scene )
{
  // Convert all of this node's meshes to a Lore mesh.
  for ( uint32_t i = 0; i < node->mNumMeshes; ++i ) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    _processMesh( mesh, scene );
  }

  // Process all node's children.
  for ( uint32_t i = 0; i < node->mNumChildren; ++i ) {
    _processNode( node->mChildren[i], scene );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ModelLoader::_processMesh( aiMesh* mesh, const aiScene* scene )
{
  Mesh::CustomMeshData data;

  // Process mesh data.
  for ( uint32_t i = 0; i < mesh->mNumVertices; ++i ) {
    Mesh::Vertex vertex;

    // Vertices.
    vertex.position = glm::vec3( mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z );

    // Normals.
    vertex.normal = glm::vec3( mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z );

    // Texture coordinates.
    if ( mesh->mTextureCoords[0] ) {
      // Flip Y coordinate value to account for STBI flipping during load.
      vertex.texCoords = glm::vec2( mesh->mTextureCoords[0][i].x, -mesh->mTextureCoords[0][i].y );
    }

    // Tangents and bitangents.
    vertex.tangent = glm::vec3( mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z );
    vertex.bitangent = glm::vec3( mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z );

    data.verts.push_back( vertex );
  }

  // Gather indices by walking through the mesh faces.
  for ( uint32_t i = 0; i < mesh->mNumFaces; ++i ) {
    const aiFace face = mesh->mFaces[i];
    // Retrieve all indices of this face.
    for ( uint32_t j = 0; j < face.mNumIndices; ++j ) {
      data.indices.push_back( face.mIndices[j] );
    }
  }

  // Create a LORE mesh and attach it to the model.
  auto loreMesh = Resource::CreateMesh( _name + "." + mesh->mName.C_Str(), Mesh::Type::Custom, _resourceGroupName );
  loreMesh->init( data );
  _model->attachMesh( loreMesh );

  // Process material and load the associated textures.
  if ( _loadTextures ) {
    if ( mesh->mMaterialIndex >= 0 ) {
      aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
      _processTexture( material, aiTextureType_DIFFUSE, loreMesh );
      _processTexture( material, aiTextureType_SPECULAR, loreMesh );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void ModelLoader::_processTexture( aiMaterial* material, const aiTextureType type, MeshPtr mesh )
{
  auto ConvertTextureType = [] ( const aiTextureType type ) {
    switch ( type ) {
    default:
    case aiTextureType_DIFFUSE:
      return Texture::Type::Diffuse;

    case aiTextureType_SPECULAR:
      return Texture::Type::Specular;
    }
  };

  for ( uint32_t i = 0; i < material->GetTextureCount( type ); ++i ) {
    aiString str;
    material->GetTexture( type, i, &str );

    // Load the texture into the resource group.
    const string texturePath = str.C_Str();
    const string textureName = Util::GetFileName( texturePath );

    auto rc = Resource::GetResourceController();
    if ( !rc->resourceExists<Texture>( textureName, _resourceGroupName ) ) {
      // Create the LORE texture and add it to the provided material's sprite.
      auto texture = rc->create<Texture>( textureName, _resourceGroupName );
      texture->loadFromFile( _directory + "/" + texturePath );
      mesh->getSprite()->addTexture( ConvertTextureType(type), texture );

      log_information( "Texture " + textureName + " loaded for model " + _name );
    }
    else {
      log_warning( "Tried loading texture " + textureName + " which already exists" );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
