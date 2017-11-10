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
#include "Mouse.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore {

  using KeyCallback = void(*)( const Keycode, const bool );
  using CharCallback = void(*)( const char c );
  using MouseButtonCallback = void(*)( const MouseButton, const bool );
  using MousePosCallback = void(*)( const int32_t x, const int32_t y );
  using MouseScrollCallback = void(*)( const double xOffset, const double yOffset );

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT InputController
  {

  public:

    InputController() = default;
    virtual ~InputController() = default;

    void setKeyCallback( const KeyCallback callback );
    void setCharCallback( const CharCallback callback );
    void setMouseButtonCallback( const MouseButtonCallback callback );
    void setMouseMovedCallback( const MousePosCallback callback );
    void setMouseScrollCallback( const MouseScrollCallback callback );

    virtual void createCallbacks( WindowPtr window ) = 0;

    virtual bool getKeyState( WindowPtr window, const Keycode key ) = 0;
    virtual bool getKeymodState( const Keymod key ) = 0;
    virtual void getCursorPos( WindowPtr window, int32_t& x, int32_t& y ) = 0;
    virtual bool getMouseButtonState( WindowPtr window, const MouseButton button ) = 0;

    virtual void setCursorEnabled( WindowPtr window, const bool enabled ) = 0;

  protected:

    KeyCallback _keyCallback { nullptr };
    CharCallback _charCallback { nullptr };
    MouseButtonCallback _mouseButtonCallback { nullptr };
    MousePosCallback _mousePosCallback { nullptr };
    MouseScrollCallback _mouseScrollCallback { nullptr };

  };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  class LORE_EXPORT Input final
  {

  public:

    static void SetKeyCallback( const KeyCallback callback );
    static void SetCharCallback( const CharCallback callback );
    static void SetMouseButtonCallback( const MouseButtonCallback callback );
    static void SetMouseMovedCallback( const MousePosCallback callback );
    static void SetMouseScrollCallback( const MouseScrollCallback callback );
    static void SetCursorEnabled( const bool enabled );

    static bool GetKeyState( const Keycode key );
    static bool GetKeymodState( const Keymod mod );
    static void GetCursorPos( int32_t& x, int32_t& y );
    static bool GetMouseButtonState( const MouseButton button );

  private:

    friend class Context;

  private:

    static void AssignContext( ContextPtr context );

  };

}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
