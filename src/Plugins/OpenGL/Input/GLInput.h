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

#include <LORE2D/Input/Input.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace Lore { namespace OpenGL {

  class GLWindow;

  class GLInputController : public Lore::InputController
  {

  public:

    GLInputController();
    ~GLInputController() override;

    virtual void createCallbacks( WindowPtr window ) override;

    virtual bool getKeyState( WindowPtr window, const Keycode key ) override;
    virtual bool getKeymodState( const Keymod key ) override;
    virtual void getCursorPos( WindowPtr window, int32_t& x, int32_t& y ) override;
    virtual bool getMouseButtonState( WindowPtr window, const MouseButton button ) override;

    virtual void setCursorEnabled( WindowPtr window, const bool enabled ) override;

    //
    // Getters.

    KeyCallback getKeyCallback() const
    {
      return _keyCallback;
    }

    CharCallback getCharCallback() const
    {
      return _charCallback;
    }

    MouseButtonCallback getMouseButtonCallback() const
    {
      return _mouseButtonCallback;
    }

    MousePosCallback getMousePosCallback() const
    {
      return _mousePosCallback;
    }

    MouseScrollCallback getMouseScrollCallback() const
    {
      return _mouseScrollCallback;
    }

    const KeyListenerList& getKeyListeners() const
    {
      return _keyListeners;
    }

    const CharListenerList& getCharListeners() const
    {
      return _charListeners;
    }

    const MouseListenerList getMouseListeners() const
    {
      return _mouseListeners;
    }

  };

}}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
