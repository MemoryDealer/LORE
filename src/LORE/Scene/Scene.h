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

#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/Color.h>
#include <LORE/Resource/Registry.h>
#include <LORE/Scene/Skybox.h>
#include <LORE/Scene/Light.h>
#include <LORE/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Scene
  /// \brief Contains all information to render a scene to an area in a window,
  ///     or to a texture.
  /// \details A Scene contains a collection of Nodes, all of which inherit from
  ///     the Scene's root Node.
  class LORE_EXPORT Scene final : public Alloc<Scene>
  {

    LORE_OBJECT_BODY()

  public:

    using NodeMap = Registry<std::unordered_map, Node>;
    using LightMap = Registry<std::unordered_map, Light>;

  public:

    Scene();

    virtual ~Scene() override;

    NodePtr createNode( const string& name );

    void destroyNode( NodePtr node );
    void destroyNode( const string& name );

    NodePtr getNode( const string& name );

    LightPtr createLight( const string& name );

    void destroyLight( LightPtr light );
    void destroyLight( const string& name );

    ///
    /// \brief Traverses scene graph and updates node transforms based on parent nodes.
    void updateSceneGraph();

    //
    // Setters.

    inline void setRenderer( RendererPtr renderer )
    {
      _renderer = renderer;
    }

    inline void setSkyboxColor( const Color& color )
    {
      _bgColor = color;
    }

    inline void setAmbientLightColor( const Color& color )
    {
      _ambientLightColor = color;
    }

    inline void setSkybox( SkyboxPtr skybox )
    {
      _skybox = skybox;
    }

    //
    // Getters.

    inline NodePtr getRootNode()
    {
      return &_root;
    }

    inline RendererPtr getRenderer() const
    {
      return _renderer;
    }

    inline Color getSkyboxColor() const
    {
      return _bgColor;
    }

    Color getAmbientLightColor() const
    {
      return _ambientLightColor;
    }

    inline LightPtr getLight( const string& name ) const
    {
      return _lights.get( name );
    }

    inline SkyboxPtr getSkybox() const
    {
      return _skybox;
    }

  private:

    void _addActiveLight( LightPtr light );

    friend class Node;

  private:

    Color _bgColor { StockColor::Black };

    // The type of renderer this scene uses.
    RendererPtr _renderer { nullptr };

    // The scene's root node.
    Node _root {};

    // Convenient hash map of all nodes for quick lookup.
    NodeMap _nodes {};

    //
    // Lighting.

    Color _ambientLightColor { StockColor::Black };

    LightMap _lights {};

    SkyboxPtr _skybox { nullptr };

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
