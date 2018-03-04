#pragma once
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

#include <LORE/Math/Matrix.h>
#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/IResource.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  class LORE_EXPORT VertexBuffer : public IResource
  {

  public:

    enum class Type
    {
      Custom,
      Quad,
      TexturedQuad,
      QuadInstanced,
      TexturedQuadInstanced,
      Skybox,
      Text
    };

    enum class AttributeType
    {
      Int,
      Float
    };

    using Vertices = std::vector<real>;
    using Matrices = std::vector<Matrix4>;

  public:

    VertexBuffer();
    virtual ~VertexBuffer() = default;

    virtual void init( const Type& type ) = 0;

    virtual void initInstanced( const Type& type, const size_t maxCount ) = 0;

    virtual void updateInstanced( const size_t idx, const Matrix4& matrix ) = 0;

    void addAttribute( const AttributeType& type, const uint size );

    virtual void bind() = 0;

    virtual void unbind() = 0;

    virtual void draw( const size_t instanceCount = 0 ) = 0;

    virtual void draw( const Vertices& verts ) = 0;

    //
    // Accessors.

    Type getType() const;

  private:

    struct Attribute
    {
      AttributeType type;
      int size;
    };

  protected:

    Type _type;
    std::vector<Attribute> _attributes;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
