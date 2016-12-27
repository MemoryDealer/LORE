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

            Vec3 position;
            Quaternion orientation;
            Vec3 scale;

            Matrix4 matrix; // Local transformation matrix.
            bool dirty; // True if matrix needs update.

            Matrix4 worldMatrix; // Derived transformation in scene graph.

            Transform()
            : position()
            , orientation()
            , scale()
            , matrix()
            , dirty( true )
            , worldMatrix()
            { }

            enum Space {
                Local,
                Parent,
                World
            };

        };

    public:

        ~Node();

        NodePtr createChildNode( const string& name );

        void attachChildNode( NodePtr child );

        void removeChildNode( NodePtr child );

        NodePtr getChild( const string& name );

        ChildNodeIterator getChildIterator();

        ConstChildNodeIterator getConstChildNodeIterator() const;

        void detachFromParent();

        //
        // Renderable attachments.

        void attachObject( RenderablePtr r );

        //
        // Modifiers.

        void setPosition( const Vec3& position );

        void translate( const Vec3& offset );

        void setScale( const Vec3& scale );

        void scale( const Vec3& scale );

        ///
        /// \brief Sets node's transformation matrix to dirty - it will be updated before
        ///     next frame is rendered.
        void dirty();

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

        inline Vec3 getPosition() const
        {
            return _transform.position;
        }

        inline Vec3 getScale() const
        {
            return _transform.scale;
        }

        inline bool isTransformDirty() const
        {
            return _transform.dirty;
        }

        Matrix4 getTransformationMatrix();
        Matrix4 getWorldTransformationMatrix();

        //
        // Setters.

        void setWorldTransformationMatrix( const Matrix4& w );

        //
        // Deleted functions/operators.

        Node( const Node& rhs ) = delete;
        Node& operator = ( const Node& rhs ) = delete;

    private:

        // Only scenes can construct nodes.
        friend class Scene;

        Node( const string& name, ScenePtr scene, NodePtr parent );

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