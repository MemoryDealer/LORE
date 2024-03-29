#pragma once
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

#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/IResource.h>
#include <LORE/Resource/Sprite.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  using Vertices = std::vector<real>;

  class LORE_EXPORT Mesh : public IResource
  {

  public:

    using TextureList = std::vector<TexturePtr>;

    enum class Type
    {
      Custom,
      CustomInstanced,
      Quad,
      TexturedQuad,
      Skybox2D,
      Cube,
      TexturedCube,
      Cubemap,
      Quad3D,
      TexturedQuad3D,
      Text,

      FullscreenQuad,

      // Instanced.
      QuadInstanced,
      TexturedQuadInstanced,
      Quad3DInstanced,
      TexturedQuad3DInstanced,
      CubeInstanced,
      TexturedCubeInstanced
    };

    enum class AttributeType
    {
      Int,
      Float
    };

    struct Attribute
    {
      AttributeType type;
      int size;
    };

    struct Vertex
    {
      glm::vec3 position;
      glm::vec3 normal;
      glm::vec2 texCoords;

      glm::vec3 tangent;
      glm::vec3 bitangent;
    };

    struct CustomMeshData
    {
      std::vector<Vertex> verts;
      std::vector<uint32_t> indices;
    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    Type _type { Type::Custom };
    std::vector<Attribute> _attributes {};

    // A mesh can have its own sprite that overrides the material's sprite.
    // Note it is not allocated via the ResourceController but is explicitly
    // owned by the Mesh.
    Sprite _sprite {};
    MaterialPtr _material {};

  public:

    Mesh() = default;
    virtual ~Mesh();

    void initMaterial();
    virtual void init( const Type type ) = 0;
    virtual void init( const CustomMeshData& data ) = 0;
    virtual void initInstanced( const Type type, const size_t maxCount ) = 0;

    virtual void updateInstanced( const size_t idx, const glm::mat4& matrix ) = 0;

    void addAttribute( const AttributeType& type, const uint size );

    virtual void draw( const GPUProgramPtr program, const size_t instanceCount = 0, const bool bindTextures = true, const bool applyMaterial = true ) = 0;
    virtual void draw( const Vertices& verts ) = 0;

    //
    // Accessors.

    SpritePtr getSprite();

    Type getType() const;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
