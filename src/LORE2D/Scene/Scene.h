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

#include <LORE2D/Resource/Color.h>
#include <LORE2D/Scene/Node.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class Scene
    /// \brief Contains all information to render a scene to an area in a window,
    ///     or to a texture.
    class LORE_EXPORT Scene final
    {

    public:

        explicit Scene( const string& name );

        ~Scene();

        NodePtr createNode( const string& name );

        void destroyNode( NodePtr node );
        void destroyNode( const string& name );

        NodePtr getNode( const string& name );

        //
        // Setters.

        void setRenderer( RendererPtr renderer )
        {
            _renderer = renderer;
        }

        void setBackgroundColor( const Color& color )
        {
            _bgColor = color;
        }

        //
        // Getters.

        NodePtr getRootNode()
        {
            return &_root;
        }

        RendererPtr getRenderer() const
        {
            return _renderer;
        }

        string getName() const
        {
            return _name;
        }

        Color getBackgroundColor() const
        {
            return _bgColor;
        }

    private:

        using NodeHashMap = std::unordered_map<string, std::unique_ptr<Node>>;

    private:

        string _name;
        Color _bgColor;

        // The type of renderer this scene uses.
        RendererPtr _renderer;

        // The scene's root node.
        Node _root;

        // Convenient hash map of all nodes for quick lookup.
        // This is the owner of all nodes; they are stored as unique_ptrs.
        NodeHashMap _nodes;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
