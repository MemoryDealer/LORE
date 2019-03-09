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

#ifdef LORE_DEBUG_UI

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#include "DebugUI.h"

#include <External/imgui/imgui.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static DebugUI* DebugUIInstance { nullptr };

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void OnKeyChanged( const Keycode key, const bool pressed )
  {
    ImGuiIO& io = ImGui::GetIO();
    if ( Keycode::GraveAccent != key ) {
      io.KeysDown[static_cast< int >( key )] = pressed;
    }

    static bool initialized { false };
    if ( pressed ) {
      switch ( key ) {
      default:
        break;

      case Keycode::GraveAccent:
        if ( initialized ) {
          DebugUIInstance->setEnabled( false );
          Input::SetCursorEnabled( false );
        }
        break;
      }
    }
    else {
      switch ( key ) {
      default:
        break;

      case Keycode::GraveAccent:
        initialized = !initialized;
        if ( !initialized ) {
          Input::ResetHooks();
        }
        break;
      }
    }
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void OnChar( const char c )
  {
    ImGuiIO& io = ImGui::GetIO();
    if ( c > 0 && c < 0x10000 && '`' != c && '~' != c )
      io.AddInputCharacter( static_cast< unsigned short >( c ) );
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void OnMouseMoved( const int32_t x, const int32_t y )
  {
    // Nothing to do here yet.
  }

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

DebugUI::DebugUI()
{
  DebugUIInstance = this;

  _inputHooks.keyCallback = OnKeyChanged;
  _inputHooks.charCallback = OnChar;
  _inputHooks.mousePosCallback = OnMouseMoved;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUI::render( ImGuiContext* context )
{
  ImGui::SetCurrentContext( context );

  switch ( _panel ) {
  default:
  case Panel::PerformanceStats:
    _perfStats.render();
    break;

  case Panel::Console:
    _perfStats.render();
    _console.render();
    break;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUI::setEnabled( const bool enabled )
{
  UI::setEnabled( enabled );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void DebugUI::setWindowDimensions( const Dimensions& dimensions )
{
  UI::setWindowDimensions( dimensions );
  _perfStats.setWindowDimensions( dimensions );
  _console.setWindowDimensions( dimensions );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
