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

#include "Keys.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  using KeyCallback = void(*)( const Keycode, const bool );
  using MouseButtonCallback = void(*)( const int, const bool );
  using MouseMovedCallback = void(*)( const double x, const double y );

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT InputController
  {

  public:

    InputController() = default;
    virtual ~InputController() = default;

    void setKeyCallback( const KeyCallback callback );
    void setMouseButtonCallback( const MouseButtonCallback callback );
    void setMouseMovedCallback( const MouseMovedCallback callback );

    virtual void createCallbacks( WindowPtr window ) = 0;

    virtual bool getKeyState( WindowPtr window, const Keycode key ) = 0;

  protected:

    KeyCallback _keyCallback { nullptr };
    MouseButtonCallback _mouseButtonCallback { nullptr };
    MouseMovedCallback _mouseMovedCallback { nullptr };

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT Input final
  {

  public:

    static void SetKeyCallback( const KeyCallback callback );
    static void SetMouseButtonCallback( const MouseButtonCallback callback );
    static void SetMouseMovedCallback( const MouseMovedCallback callback );

    static bool GetKeyState( const Keycode key );

  private:

    friend class Context;

  private:

    static void AssignContext( ContextPtr context );

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
