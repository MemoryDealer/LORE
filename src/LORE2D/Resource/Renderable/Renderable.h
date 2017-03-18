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

namespace Lore {

    ///
    /// \class Renderable
    /// \brief The base class of anything that can be attached to a node.
    class LORE_EXPORT Renderable
    {

    public:

        enum class Type
        {
            Unknown,
            Camera,
            Light,
            Texture,
            Sprite
        };

    public:

        explicit Renderable( const string& name );

        virtual ~Renderable();

        virtual void bind() { }

        //
        // Getters.

        inline string getName() const
        {
            return _name;
        }

        inline Type getType() const
        {
            return _type;
        }

        inline MaterialPtr getMaterial() const
        {
            return _material;
        }

        inline uint getRenderQueue() const
        {
            return _renderQueue;
        }

        inline bool isAttached() const
        {
            return _attached;
        }

        //
        // Setters.

        inline void setMaterial( MaterialPtr mat )
        {
            _material = mat;
        }

        inline void setRenderQueue( const uint id )
        {
            _renderQueue = id;
        }

    protected:

        string _name;
        MaterialPtr _material;
        uint _renderQueue;
        bool _attached;
        Type _type;

    private:

        void _notifyAttached();

        friend class Node;

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
