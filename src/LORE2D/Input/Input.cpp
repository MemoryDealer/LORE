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

#include "Input.h"

#include <LORE2D/Core/Context.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  Lore::ContextPtr ActiveContext = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::setKeyCallback( const KeyCallback callback )
{
  _keyCallback = callback;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::setCharCallback( const CharCallback callback )
{
  _charCallback = callback;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::setMouseButtonCallback( const MouseButtonCallback callback )
{
  _mouseButtonCallback = callback;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::setMouseMovedCallback( const MousePosCallback callback )
{
  _mousePosCallback = callback;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::setMouseScrollCallback( const MouseScrollCallback callback )
{
  _mouseScrollCallback = callback;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::addKeyListener( const KeyListenerPtr listener )
{
  _keyListeners.push_back( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::addCharListener( const CharListenerPtr listener )
{
  _charListeners.push_back( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::addMouseListener( const MouseListenerPtr listener )
{
  _mouseListeners.push_back( listener );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::removeKeyListener( const KeyListenerPtr listener )
{
  for ( auto it = _keyListeners.begin(); it != _keyListeners.end(); ++it ) {
    if ( listener == ( *it ) ) {
      _keyListeners.erase( it );
      return;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::removeCharListener( const CharListenerPtr listener )
{
  for ( auto it = _charListeners.begin(); it != _charListeners.end(); ++it ) {
    if ( listener == ( *it ) ) {
      _charListeners.erase( it );
      return;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void InputController::removeMouseListener( const MouseListenerPtr listener )
{
  for ( auto it = _mouseListeners.begin(); it != _mouseListeners.end(); ++it ) {
    if ( listener == ( *it ) ) {
      _mouseListeners.erase( it );
      return;
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::SetKeyCallback( const KeyCallback callback )
{
  ActiveContext->getInputController()->setKeyCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::SetCharCallback( const CharCallback callback )
{
  ActiveContext->getInputController()->setCharCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::SetMouseButtonCallback( const MouseButtonCallback callback )
{
  ActiveContext->getInputController()->setMouseButtonCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::SetMouseMovedCallback( const MousePosCallback callback )
{
  ActiveContext->getInputController()->setMouseMovedCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::SetMouseScrollCallback( const MouseScrollCallback callback )
{
  ActiveContext->getInputController()->setMouseScrollCallback( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::AddKeyListener( const KeyListenerPtr callback )
{
  ActiveContext->getInputController()->addKeyListener( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::AddCharListener( const CharListenerPtr callback )
{
  ActiveContext->getInputController()->addCharListener( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::AddMouseListener( const MouseListenerPtr callback )
{
  ActiveContext->getInputController()->addMouseListener( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::RemoveKeyListener( const KeyListenerPtr callback )
{
  ActiveContext->getInputController()->removeKeyListener( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::RemoveCharListener( const CharListenerPtr callback )
{
  ActiveContext->getInputController()->removeCharListener( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::RemoveMouseListener( const MouseListenerPtr callback )
{
  ActiveContext->getInputController()->removeMouseListener( callback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Input::GetKeyState( const Keycode key )
{
  return ActiveContext->getInputController()->getKeyState( ActiveContext->getActiveWindow(), key );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Input::GetKeymodState( const Keymod mod )
{
  return ActiveContext->getInputController()->getKeymodState( mod );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::GetCursorPos( int32_t& x, int32_t& y )
{
  ActiveContext->getInputController()->getCursorPos( ActiveContext->getActiveWindow(), x, y );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool Input::GetMouseButtonState( const MouseButton button )
{
  return ActiveContext->getInputController()->getMouseButtonState( ActiveContext->getActiveWindow(), button );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::SetCursorEnabled( const bool enabled )
{
  ActiveContext->getInputController()->setCursorEnabled( ActiveContext->getActiveWindow(), enabled );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Input::AssignContext( ContextPtr context )
{
  ActiveContext = context;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
