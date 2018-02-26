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

#include <LORE2D/Core/Iterator.h>
#include <LORE2D/Math/Math.h>
#include <LORE2D/Memory/Alloc.h>
#include <LORE2D/Resource/Color.h>
#include <LORE2D/Resource/Registry.h>
#include <LORE2D/Scene/AABB.h>
#include <LORE2D/Scene/SpriteController.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  ///
  /// \class Node
  /// \brief A scene node in its scene graph. May contain any renderable or
  ///     attachable objects.
  class LORE_EXPORT Node : public Alloc<Node>
  {

    LORE_OBJECT_BODY()

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

      Vec2 position;
      Quaternion orientation;
      Vec2 scale;
      Vec2 derivedScale;

      Matrix4 local; // Local transformation matrix.
      bool dirty; // True if matrix needs update.

      Matrix4 world; // Derived transformation in scene graph.

      Transform()
        : position()
        , orientation()
        , scale( 1.f, 1.f )
        , derivedScale( 1.f, 1.f )
        , local()
        , dirty( true )
        , world()
      {
      }

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

    void setPosition( const Vec2& position );

    void setPosition( const real x, const real y );

    void translate( const Vec2& offset );

    void translate( const real xOffset, const real yOffset );

    void rotate( const Radian& angle, const TransformSpace& ts = TransformSpace::Local );

    void rotate( const Degree& angle, const TransformSpace& ts = TransformSpace::Local );

    void rotate( const Vec3& axis, const Radian& angle, const TransformSpace& ts = TransformSpace::Local );

    void rotate( const Vec3& axis, const Degree& angle, const TransformSpace& ts = TransformSpace::Local );

    void rotate( const Quaternion& q, const TransformSpace& ts = TransformSpace::Local );

    void setScale( const Vec2& scale );

    void scale( const Vec2& s );

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

    inline NodePtr getParent() const
    {
      return _parent;
    }

    inline Vec2 getPosition() const
    {
      return _transform.position;
    }

    inline Vec2 getDerivedPosition() const
    {
      return Vec2( _transform.world[3][0], _transform.world[3][1] );
    }

    inline Quaternion getOrientation() const
    {
      return _transform.orientation;
    }

    inline Matrix4 getFullTransform() const
    {
      return _transform.world;
    }

    inline Vec2 getScale() const
    {
      return _transform.scale;
    }

    inline Vec2 getDerivedScale() const
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

    }
    //Node& operator = ( const Node& rhs ) = delete;

  private:

    // Only scenes can construct nodes.
    friend class Scene;
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

    Matrix4 _getLocalTransform();

    void _updateWorldTransform( const Matrix4& m );

    void _updateChildrenScale();

    void _updateDepthValue();

  private:

    // TODO: Node is getting large, consider holding node -> entity mappings somewhere else.

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