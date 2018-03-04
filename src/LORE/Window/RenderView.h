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

#include <LORE/Math/Rectangle.h>
#include <LORE/Scene/Camera.h>
#include <LORE/Scene/Scene.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  ///
  /// \class RenderView
  /// \brief Contains the information needed to render a scene to a window.
  /// \details ...
  struct RenderView final
  {

    string name {};
    ScenePtr scene { nullptr };
    CameraPtr camera { nullptr };
    RenderTargetPtr renderTarget { nullptr };
    UIPtr ui { nullptr };

    Rect viewport {};

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
      : name( name_ )
    {
    }

    RenderView( const string& name_, ScenePtr scene_ )
      : name( name_ )
      , scene( scene_ )
    {
    }

    RenderView( const string& name_, ScenePtr scene_, const Rect& viewport_ )
      : name( name_ )
      , scene( scene_ )
      , viewport( viewport_ )
    {
    }

    bool operator == ( const RenderView& rhs ) const
    {
      // RenderView names are unique.
      return ( name == rhs.name );
    }

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
