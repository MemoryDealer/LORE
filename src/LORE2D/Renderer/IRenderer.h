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

#include <LORE2D/Window/RenderView.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

        ///
        /// \class IRenderer
        /// \brief Interface for Renderers - the object that knows how to interpret
        ///     a RenderView and a Scene's scene graph and present an image to the 
        ///     window. Render plugins shall define these implementations.
        class IRenderer
        {

        public:

            virtual ~IRenderer() { }

            virtual void addRenderable( RenderablePtr r, Matrix4& model ) = 0;

            virtual void present( const RenderView& rv, const WindowPtr window ) = 0;

        };
        using RendererPtr = IRenderer*;

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

        enum class RendererType {
            Generic
        };

        // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

        struct RenderQueue
        {

            static const uint Background = 0;
            static const uint General = 50;
            static const uint Foreground = 99;

            // :::::: //

            struct Object
            {

                RenderablePtr renderable;
                Matrix4& model;

                Object( RenderablePtr r, Matrix4& m )
                : renderable( r )
                , model( m )
                { }

            };

            // :::::: //

            // TODO: Cleanup naming of this stuff
            using Objects = std::vector<Object>;
            using ObjectList = std::map<RenderablePtr, Objects>;
            using RenderableList = std::map<MaterialPtr, ObjectList>;

            // :::::: //

            RenderableList solids;
            RenderableList sortedTransparents;

        };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
