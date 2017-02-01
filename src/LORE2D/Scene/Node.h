#pragma once
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE2D
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2016 Jordan Sparks
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

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class Node
    /// \brief A scene node in its scene graph. May contain any renderable or
    ///     attachable objects.
    class LORE_EXPORT Node
    {

    public:

        using NodeMap = std::map<string, NodePtr>;
        using ChildNodeIterator = MapIterator<NodeMap>;
        using ConstChildNodeIterator = ConstMapIterator<NodeMap>;
        using RenderableList = std::unordered_map<string, RenderablePtr>;

        struct Transform
        {

            Vec2 position;
            Quaternion orientation;
            Vec2 scale;

            Matrix4 local; // Local transformation matrix.
            bool dirty; // True if matrix needs update.

            Matrix4 world; // Derived transformation in scene graph.

            Transform()
            : position()
            , orientation()
            , scale( 1.f, 1.f )
            , local()
            , dirty( true )
            , world()
            { }

        };

    public:

        ~Node();

        NodePtr createChildNode( const string& name );

        void attachChildNode( NodePtr child );

        void removeChildNode( NodePtr child );

        NodePtr getChild( const string& name );

        ChildNodeIterator getChildNodeIterator();

        ConstChildNodeIterator getConstChildNodeIterator() const;

        void detachFromParent();

        //
        // Renderable attachments.

        void attachObject( RenderablePtr r );

        //
        // Modifiers.

        void setPosition( const Vec2& position );

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

        //
        // Getters.

        inline string getName() const
        {
            return _name;
        }

        inline NodePtr getParent() const
        {
            return _parent;
        }

        inline Vec2 getPosition() const
        {
            return _transform.position;
        }

        inline Vec2 getScale() const
        {
            return _transform.scale;
        }

        inline bool hasChildNodes() const
        {
            return !( _childNodes.empty() );
        }

        //
        // Deleted functions/operators.

        Node( const Node& rhs ) = delete;
        Node& operator = ( const Node& rhs ) = delete;

    private:

        // Only scenes can construct nodes.
        friend class Scene;
        friend class SceneGraphVisitor;

    private:

        Node( const string& name, ScenePtr scene, NodePtr parent );

        //
        // Scene graph operations.

        ///
        /// \brief Sets node's transformation matrix to dirty - it will be updated before
        ///     next frame is rendered.
        void _dirty();

        bool _transformDirty() const;

        Matrix4 _getLocalTransform();

        Matrix4 _getWorldTransform() const;

        void _updateWorldTransform( const Matrix4& m );

    private:

        string _name;

        Transform _transform;

        RenderableList _renderables;

        // The creator of this node.
        ScenePtr _scene;

        NodePtr _parent;
        NodeMap _childNodes;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //