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

#include <LORE/Core/Util.h>
#include <LORE/Resource/Material.h>
#include <LORE/Resource/Mesh.h>
#include <LORE/Resource/Registry.h>
#include <LORE/Shader/GPUProgram.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  struct UberProgramParameters
  {
    unsigned int maxLights { 4 };
    unsigned int numTextures { 1 };
    bool instanced { false };
  };

  struct BackgroundProgramParameters
  {
    bool scrolling { true };
  };

  ///
  /// \class StockResourceController
  /// \brief Holds a collection of stock resources available for common use. It wraps a ResourceController
  /// to store the stock resources internally. This is separate from the primary ResourceController.
  class LORE_EXPORT StockResourceController
  {

  public:

    enum class GPUProgramType
    {
      StandardTexturedQuad,
      StandardTexturedTriangle
    };

    enum class MaterialType
    {
      BaseWhiteNoLighting
    };

  public:

    StockResourceController();
    virtual ~StockResourceController();

    void createStockResources();

    //
    // Accessor functions.

    template<typename ResourceType>
    ResourceType* get( const string& name )
    {
      return _controller->get<ResourceType>( name );
    }

    MeshPtr getMesh( const VertexBuffer::Type& type );

    //
    // Factory functions that must be implemented by the render plugin.

    virtual GPUProgramPtr createUberProgram( const string& name, const UberProgramParameters& params ) = 0;
    virtual GPUProgramPtr createBackgroundProgram( const string& name, const BackgroundProgramParameters& params ) = 0;
    virtual GPUProgramPtr createBoxProgram( const string& name ) = 0;
    virtual GPUProgramPtr createTextProgram( const string& name ) = 0;

  protected:

    using MeshTable = Util::HashTable<VertexBuffer::Type, MeshPtr>;

  protected:

    std::unique_ptr<ResourceController> _controller { nullptr };

    MeshTable _meshTable;

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT StockResource final
  {

  public:

    //
    // Accessor functions.

    static GPUProgramPtr GetGPUProgram( const string& name );
    static MaterialPtr GetMaterial( const string& name );
    static MeshPtr GetMesh( const VertexBuffer::Type& type );
    static TexturePtr GetTexture( const string& name );
    static VertexBufferPtr GetVertexBuffer( const string& name );
    static FontPtr GetFont( const string& name );

  private:

    friend class Context;

  private:

    static void AssignContext( ContextPtr context );

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
