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

  static Lore::OpenGL::GLInputController* InputControllerInstance = nullptr;

  // ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

  static void GLFWKeyCallback( GLFWwindow* window, int key, int scancode, int action, int mods )
  {
    Lore::KeyCallback callback = InputControllerInstance->getKeyCallback();
    if ( callback ) {
      callback( static_cast<Lore::Keycode>( key ), ( action > 0 ) ? true : false );
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
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

bool GLInputController::getKeyState( Lore::WindowPtr window, const Lore::Keycode key )
{
  GLFWwindow* glfwWindow = static_cast<GLWindow*>( window )->getInternalWindow();
  return glfwGetKey( glfwWindow, static_cast< int >( key ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
