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

#include "Game.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

using namespace std::chrono_literals;

using Clock = std::chrono::high_resolution_clock;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

int main( int argc, char** argv )
{
  // Use our simple game class.
  // The constructor will create a Lore context and a small window.
  Game game;

  // Index and load resources specified in res/complexscene/resources.json.
  game.loadResources();

  // Add some contents to the scene.
  game.loadScene();

  //
  // Setup is complete, begin processing the scene.

  // Start a frame rate independent game loop with a fixed timestep.
  constexpr std::chrono::nanoseconds timestep( 16ms );
  bool running = true;
  std::chrono::nanoseconds lag( 0ns );
  auto lastTime = Clock::now();

  while ( running ) {
    // Calculate delta time since last update.
    const auto now = Clock::now();
    const auto dt = now - lastTime;
    lastTime = now;

    // Increment our lag counter to track how much "catching up" we need to do.
    lag += std::chrono::duration_cast< std::chrono::nanoseconds >( dt );

    // TODO: Fix timestep which was causing frame stuttering.
    game.processInput();
    while ( lag > timestep ) {
      lag -= timestep;

      //
      // Update game/scene.

      // Process input.
      if ( Lore::Input::GetKeyState( Lore::Keycode::Escape ) ||
           Lore::Input::GetKeyState( Lore::Keycode::End ) ) {
        running = false;
      }

      game.update();
    }

    // We are done updating, render a frame.
    game.render();
  }

  return 0;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
