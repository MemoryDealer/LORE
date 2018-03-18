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

#include <LORE/Core/Iterator.h>
#include <LORE/Math/Math.h>
#include <LORE/Memory/Alloc.h>
#include <LORE/Resource/Color.h>
#include <LORE/Resource/Registry.h>
#include <LORE/Scene/AABB.h>
#include <LORE/Scene/SpriteController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Node
  /// \brief A scene node in its scene graph. May contain any renderable or
  ///     attachable objects.
  class LORE_EXPORT Node : public Alloc<Node>
  {

  public:

    // TODO: Use a RenderableList for textboxes, textures, boxes, etc?
    using NodeMap = Registry<std::map, Node>;
    using ChildNodeIterator = NodeMap::Iterator;
    using ConstChildNodeIterator = NodeMap::ConstIterator;
    using EntityList = Registry<std::map, Entity>;
    using EntityListConstIterator = EntityList::ConstIterator;
    using BoxList = Registry<std::map, Box>;
    using BoxListConstIterator = BoxList::ConstIterator;
    using TextboxList = Registry<std::map, Textbox>;
    using TextboxListConstIterator = TextboxList::ConstIterator;
    using LightList = Registry<std::map, Light>;
    using LightListConstIterator = LightList::ConstIterator;
    using CameraList = std::vector<CameraPtr>;

    struct Transform
    {
      glm::vec3 position { 0.f };
      glm::quat orientation { 1.f, 0.f, 0.f, 0.f };
      glm::vec3 scale { 1.f };
      glm::vec3 derivedScale { 1.f };

      glm::mat4 local { 1.f }; // Local transformation matrix.
      bool dirty { true }; // True if matrix needs update.

      glm::mat4 world { 1.f }; // Derived transformation in scene graph.
    };

    struct Depth {
      static constexpr const real Max = 1000.f;
      static constexpr const real Default = 0.f;
      static constexpr const real Min = -1000.f;
    };

  public:

    Node();

    ~Node() override;

    NodePtr clone( const string& name, const bool cloneChildNodes = false );

    NodePtr createChildNode( const string& name );

    void attachChildNode( NodePtr child );

    void removeChildNode( NodePtr child );

    NodePtr getChild( const string& name );

    ChildNodeIterator getChildNodeIterator();

    ConstChildNodeIterator getConstChildNodeIterator();

    void detachFromParent();

    //
    // Renderable attachments.

    void attachObject( EntityPtr e );

    void attachObject( LightPtr l );

    void attachObject( BoxPtr b );

    void attachObject( TextboxPtr t );

    inline EntityListConstIterator getEntityListConstIterator() const
    {
      return _entities.getConstIterator();
    }

    inline BoxListConstIterator getBoxListConstIterator() const
    {
      return _boxes.getConstIterator();
    }

    inline TextboxListConstIterator getTextboxListConstIterator() const
    {
      return _textboxes.getConstIterator();
    }

    inline LightListConstIterator getLightListConstIterator() const
    {
      return _lights.getConstIterator();
    }

    // Misc.

    bool intersects( NodePtr rhs ) const;

    //
    // Modifiers.

    void setName( const string& name );

    void setPosition( const glm::vec2& position );

    void setPosition( const real x, const real y );

    void setPosition( const glm::vec3& position );

    void setPosition( const real x, const real y, const real z );

    void translate( const glm::vec2& offset );

    void translate( const real xOffset, const real yOffset );

    void rotate( const real angle, const TransformSpace& ts = TransformSpace::Local );

    void rotate( const glm::vec3& axis, const real angle, const TransformSpace& ts = TransformSpace::Local );

    void rotate( const glm::quat& q, const TransformSpace& ts = TransformSpace::Local );

    void setScale( const glm::vec2& scale );

    void scale( const glm::vec2& s );

    void scale( const real s );

    void setDepth( const real depth )
    {
      // Depth has a range of [-1000, 1000].
      // TODO: Match these values with z-near and z-far in renderer.
      //assert( depth >= -1000.f && depth <= 1000.f ); // Removed for now due to renderer setting depth for UI elements.
      _depth = depth;
    }

    void updateWorldTransform();

    ///
    /// \brief Allocates a SpriteController for this node, which can be used to animate the sprite (if one exists)
    /// associated with this node.
    SpriteControllerPtr createSpriteController();

    //
    // Getters.

    string getName() const;

    inline NodePtr getParent() const
    {
      return _parent;
    }

    inline glm::vec3 getPosition() const
    {
      return _transform.position;
    }

    inline glm::vec3 getDerivedPosition() const
    {
      return glm::vec3( _transform.world[3][0], _transform.world[3][1], _transform.world[3][2] );
    }

    inline glm::quat getOrientation() const
    {
      return _transform.orientation;
    }

    inline glm::mat4 getFullTransform() const
    {
      return _transform.world;
    }

    inline glm::vec3 getScale() const
    {
      return _transform.scale;
    }

    inline glm::vec3 getDerivedScale() const
    {
      return _transform.derivedScale;
    }

    inline bool hasChildNodes() const
    {
      return !( _childNodes.empty() );
    }

    inline real getDepth() const
    {
      return _depth;
    }

    AABBPtr getAABB() const;

    SpriteControllerPtr getSpriteController() const;


    //
    // Deleted functions/operators.

    Node( const Node& rhs )
    {
      // TODO: Fill out.
    }

    Node& operator = ( const Node& rhs ) = delete;

  private:

    friend class AABB;
    friend class Entity;
    friend class Scene; // Only scenes can construct nodes.
    friend class SceneGraphVisitor;
    friend class MemoryPool<Node>;

  private:

    //
    // Scene graph operations.

    ///
    /// \brief Sets node's transformation matrix to dirty - it will be updated before
    ///     next frame is rendered.
    void _dirty();

    bool _transformDirty() const;

    glm::mat4 _getLocalTransform();

    void _updateWorldTransform( const glm::mat4& m );

    void _updateChildrenScale();

    void _updateDepthValue();

  private:

    // TODO: Node is getting large, consider holding node -> entity mappings somewhere else.

    string _name {};

    std::unique_ptr<AABB> _aabb { nullptr };
    std::unique_ptr<SpriteController> _spriteController { nullptr };

    Transform _transform {};

    real _depth { Depth::Default };

    EntityList _entities {};
    size_t _instanceID { 0 };

    BoxList _boxes {};

    TextboxList _textboxes {};

    // The creator of this node.
    ScenePtr _scene { nullptr };

    NodePtr _parent { nullptr };
    NodeMap _childNodes {};

    LightList _lights {};
  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //