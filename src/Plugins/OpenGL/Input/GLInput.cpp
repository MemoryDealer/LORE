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

#include "GLInput.h"

#include <Plugins/OpenGL/Window/GLWindow.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static Lore::OpenGL::GLInputController* InputControllerInstance { nullptr };
  static std::unordered_map<Lore::Keymod, bool> KeymodStates {};

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void GLFWKeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
  {
    Lore::KeyCallback callback = InputControllerInstance->getKeyCallback();
    const auto loreKey = static_cast< Lore::Keycode >( key );
    const bool pressed = ( action > 0 ) ? true : false;

    if ( callback ) {
      // Trigger developer-provided callback.
      callback( loreKey, pressed );
    }

    const auto& listeners = InputControllerInstance->getKeyListeners();
    for ( auto listener : listeners ) {
      if ( pressed ) {
        listener->onKeyDown( loreKey );
      }
      else {
        listener->onKeyUp( loreKey );
      }
    }

    // Set modifier bits.
    KeymodStates[Lore::Keymod::Shift] = ( mods & GLFW_MOD_SHIFT );
    KeymodStates[Lore::Keymod::Control] = ( mods & GLFW_MOD_CONTROL );
    KeymodStates[Lore::Keymod::Alt] = ( mods & GLFW_MOD_ALT );
    KeymodStates[Lore::Keymod::Super] = ( mods & GLFW_MOD_SUPER );
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void GLFWCharCallback( GLFWwindow* window, unsigned int codepoint )
  {
    Lore::CharCallback callback = InputControllerInstance->getCharCallback();
    if ( callback ) {
      callback( static_cast<char>( codepoint ) );
    }

    const auto& listeners = InputControllerInstance->getCharListeners();
    for ( auto listener : listeners ) {
      listener->onChar( static_cast< char >( codepoint ) );
    }
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void GLFWMousePosCallback( GLFWwindow* window, double x, double y )
  {
    Lore::MousePosCallback callback = InputControllerInstance->getMousePosCallback();
    if ( callback ) {
      callback( static_cast<const int32_t>( x ), static_cast<const int32_t>( y ) );
    }

    const auto& listeners = InputControllerInstance->getMouseListeners();
    for ( auto listener : listeners ) {
      listener->onMouseMoved( static_cast< const int32_t >( x ), static_cast< const int32_t >( y ) );
    }
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void GLFWMouseButtonCallback( GLFWwindow* window, int button, int action, int mods )
  {
    Lore::MouseButtonCallback callback = InputControllerInstance->getMouseButtonCallback();
    const bool pressed = ( action > 0 ) ? true : false;
    static const std::map<int, Lore::MouseButton> MouseButtonMap = {
      { GLFW_MOUSE_BUTTON_LEFT, Lore::MouseButton::Left },
      { GLFW_MOUSE_BUTTON_MIDDLE, Lore::MouseButton::Middle },
      { GLFW_MOUSE_BUTTON_RIGHT, Lore::MouseButton::Right }
    };

    Lore::MouseButton loreButton;
    auto it = MouseButtonMap.find( button );
    if ( MouseButtonMap.end() != it ) {
      loreButton = it->second;
    }
    else {
      loreButton = static_cast< Lore::MouseButton >( button );
    }

    if ( callback ) {
      callback( loreButton, pressed );
    }

    const auto& listeners = InputControllerInstance->getMouseListeners();
    for ( auto listener : listeners ) {
      if ( pressed ) {
        listener->onMouseButtonDown( loreButton );
      }
      else {
        listener->onMouseButtonUp( loreButton );
      }
    }
  }

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void GLFWMouseScrollCallback( GLFWwindow* window, double xOffset, double yOffset )
  {
    Lore::MouseScrollCallback callback = InputControllerInstance->getMouseScrollCallback();
    if ( callback ) {
      callback( xOffset, yOffset );
    }

    const auto& listeners = InputControllerInstance->getMouseListeners();
    for ( auto listener : listeners ) {
      listener->onMouseScroll( xOffset, yOffset );
    }
  }

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace Lore::OpenGL;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLInputController::GLInputController()
{
  InputControllerInstance = this;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

GLInputController::~GLInputController()
{
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLInputController::createCallbacks( Lore::WindowPtr window )
{
  GLFWwindow* glfwWindow = static_cast<GLWindow*>( window )->getInternalWindow();
  glfwSetKeyCallback( glfwWindow, GLFWKeyCallback );
  glfwSetCharCallback( glfwWindow, GLFWCharCallback );
  glfwSetCursorPosCallback( glfwWindow, GLFWMousePosCallback );
  glfwSetMouseButtonCallback( glfwWindow, GLFWMouseButtonCallback );
  glfwSetScrollCallback( glfwWindow, GLFWMouseScrollCallback );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool GLInputController::getKeyState( Lore::WindowPtr window, const Lore::Keycode key )
{
  GLFWwindow* glfwWindow = static_cast<GLWindow*>( window )->getInternalWindow();
  return glfwGetKey( glfwWindow, static_cast< int >( key ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool GLInputController::getKeymodState( const Lore::Keymod key )
{
  return KeymodStates[key];
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLInputController::getCursorPos( Lore::WindowPtr window, int32_t& x, int32_t& y )
{
  GLFWwindow* glfwWindow = static_cast<GLWindow*>( window )->getInternalWindow();
  double _x, _y;
  glfwGetCursorPos( glfwWindow, &_x, &_y );
  x = static_cast<int32_t>( _x );
  y = static_cast<int32_t>( _y );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool GLInputController::getMouseButtonState( Lore::WindowPtr window, const Lore::MouseButton button )
{
  GLFWwindow* glfwWindow = static_cast<GLWindow*>( window )->getInternalWindow();
  int glfwBtn = -1;
  switch ( button ) {
  case Lore::MouseButton::Left:
    glfwBtn = GLFW_MOUSE_BUTTON_LEFT;
    break;

  case Lore::MouseButton::Right:
    glfwBtn = GLFW_MOUSE_BUTTON_RIGHT;
    break;

  case Lore::MouseButton::Middle:
    glfwBtn = GLFW_MOUSE_BUTTON_MIDDLE;
    break;

  default:
    glfwBtn = static_cast< int >( button );
  }

  return ( GLFW_PRESS == glfwGetMouseButton( glfwWindow, glfwBtn ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void GLInputController::setCursorEnabled( Lore::WindowPtr window, const bool enabled )
{
  GLFWwindow* glfwWindow = static_cast<GLWindow*>( window )->getInternalWindow();
  glfwSetInputMode( glfwWindow, GLFW_CURSOR,
    ( enabled ) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
