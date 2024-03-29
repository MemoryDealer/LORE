// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include "Model.h"

#include <LORE/Resource/Prefab.h>
#include <LORE/Resource/ResourceController.h>
#include <LORE/Resource/StockResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Model::~Model()
{
  // Free meshes if any.
  for ( auto mesh : _meshes ) {
    Resource::DestroyMesh( mesh );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Model::updateInstanced( const size_t idx, const glm::mat4& matrix )
{
  for ( const auto& mesh : _meshes ) {
    mesh->updateInstanced( idx, matrix );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Model::draw( const GPUProgramPtr program, const size_t instanceCount, const bool bindTextures, const bool applyMaterial )
{
  for ( const auto& mesh : _meshes ) {
    mesh->draw( program, instanceCount, bindTextures, applyMaterial );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Model::draw( const Vertices& verts )
{
  for ( const auto& mesh : _meshes ) {
    mesh->draw( verts );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Model::attachMesh( const MeshPtr mesh )
{
  _meshes.push_back( mesh );

  // Update type based on the attached mesh.
  switch ( mesh->getType() ) {
  case Mesh::Type::Custom:
    _type = Mesh::Type::Custom;
    break;

  case Mesh::Type::CustomInstanced:
    _type = Mesh::Type::CustomInstanced;
    break;

  default:
    _type = mesh->getType();
    if ( _meshes.size() > 1 ) {
      throw Lore::Exception( "No more than one built-in mesh type allowed for a single model" );
    }
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Model::setupShader( PrefabPtr prefab )
{
  for ( const auto& mesh : _meshes ) {
    SpritePtr sprite = mesh->getSprite();
    if ( sprite->getTextureCount( 0, Texture::Type::Normal ) > 0 ) {
      prefab->_material->program = StockResource::GetGPUProgram( "StandardTexturedNormalMapping3D" );
      return;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Mesh::Type Model::getType() const
{
  return _type;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
