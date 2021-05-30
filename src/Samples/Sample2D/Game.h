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

#include <LORE/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// A simple class to handle setup and processing for the complex scene.
class Game final
{

public:

  Game();
  ~Game();

  void loadResources();

  void loadScene();

  void processInput();

  void update();

  void render();

private:

  // This class is the owner of the Lore context, so it must use a unique_ptr.
  // Most other Lore objects are accessed via raw pointers, since they are owned
  // inside the Lore library.
  std::unique_ptr<Lore::Context> _context { nullptr };
  Lore::WindowPtr _window { nullptr };

  Lore::ScenePtr _scene { nullptr };
  Lore::CameraPtr _camera { nullptr };

  Lore::NodePtr _playerNode { nullptr };

  //
  // Game objects.

  std::vector<Lore::NodePtr> _floatingBlocks {};

};

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
