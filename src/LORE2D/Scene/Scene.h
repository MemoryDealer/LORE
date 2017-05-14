#pragma once
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

#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Resource/Color.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Scene/Light.h>
#include <LORE2D/Scene/Node.h>

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

    ~Scene();

    NodePtr createNode( const string& name );

    void destroyNode( NodePtr node );
    void destroyNode( const string& name );

    NodePtr getNode( const string& name );

    LightPtr createLight( const string& name );

    void destroyLight( LightPtr light );
    void destroyLight( const string& name );

    //
    // Setters.

    inline void setRenderer( RendererPtr renderer )
    {
      _renderer = renderer;
    }

    inline void setBackgroundColor( const Color& color )
    {
      _bgColor = color;
    }

    inline void setAmbientLightColor( const Color& color )
    {
      _ambientLightColor = color;
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

    inline Color getBackgroundColor() const
    {
      return _bgColor;
    }

    Color getAmbientLightColor() const
    {
      return _ambientLightColor;
    }

    inline int getLightCount() const
    {
      return static_cast< int >( _activeLights.size() );
    }

    LightMap::ConstIterator getLightConstIterator() const
    {
      return _activeLights.getConstIterator();
    }

  private:

    virtual void _reset() override;

    void _addActiveLight( LightPtr light );

    friend class Node;

  private:

    Color _bgColor;

    // The type of renderer this scene uses.
    RendererPtr _renderer;

    // The scene's root node.
    Node _root;

    // Convenient hash map of all nodes for quick lookup.
    NodeMap _nodes;

    //
    // Lighting.

    Color _ambientLightColor;

    LightMap _lights;

    // Lights that are attached to a node.
    LightMap _activeLights;

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
