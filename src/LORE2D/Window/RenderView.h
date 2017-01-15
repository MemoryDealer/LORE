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

#include <LORE2D/Scene/Camera.h>
#include <LORE2D/Scene/Scene.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

    ///
    /// \class Viewport
    /// \brief Viewport dimensions in float coordinates (0.0f to 1.0f).
    struct Viewport final
    {

        float x;
        float y;
        float width;
        float height;

        Viewport()
        : x( 0.f )
        , y( 0.f )
        , width( 1.f )
        , height( 1.f )
        { }

        Viewport( const float x_,
                  const float y_,
                  const float width_,
                  const float height_ )
        : x (x_)
        , y( y_)
        , width( width_ )
        , height( height_ )
        { }

    };

    // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

    ///
    /// \class RenderView
    /// \brief Contains the information needed to render a scene to a window.
    /// \details ...
    struct RenderView final
    {

        string name;
        ScenePtr scene;
        CameraPtr camera;
        //PostProcesser

        Viewport viewport;

        // Viewports are stored in a union, so each render plugin can do the 
        // conversion once, when the RenderView is added to a window.
        union
        {

            struct
            {
                int x, y;
                uint width, height;
                real aspectRatio;
            }  gl_viewport;

        };

        RenderView( const string& name_ )
        : name ( name_ )
        , scene( nullptr )
        , camera( nullptr )
        , viewport()
        { }

        RenderView( const string& name_, Scene* scene_ )
        : name( name_ )
        , scene( scene_ )
        , camera( nullptr )
        , viewport( )
        {
        }

        RenderView( const string& name_, Scene* scene_, const Viewport& viewport_ )
        : name( name_ )
        , scene( scene_ )
        , camera( nullptr )
        , viewport( viewport_ )
        { }

        bool operator == ( const RenderView& rhs ) const
        {
            // RenderView names are unique.
            return ( name == rhs.name );
        }

    };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
