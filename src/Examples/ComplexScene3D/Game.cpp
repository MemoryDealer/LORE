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

Game::Game()
{
  // Create a Lore context which is required for all Lore functionality.
  // We will explicitly use the OpenGL plugin for now.
  _context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

  // Create a window and set it to the active window.
  _window = _context->createWindow( "Complex Scene 3D", 640, 480, Lore::RendererType::Forward3D );
  _window->setActive();

  // Allow the DebugUI.
  Lore::DebugUI::Enable();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Game::~Game()
{
  // Free the Lore context with a helper macro (avoids extra typing).
  DestroyLoreContext( _context );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadResources()
{
  // Index all resource locations for complex scene as specified in the configuration file.
  Lore::Resource::LoadResourceConfiguration( "res/complexscene/resources.json" );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadScene()
{
  // Create a scene with default skybox color.
  _scene = _context->createScene( "core", Lore::RendererType::Forward3D );
  _scene->setSkyboxColor( Lore::StockColor::Green );
  _scene->setAmbientLightColor( Lore::Color( 0.75f, 0.75f, 0.75f, 1.f ) );

  // Create a camera to view the scene.
  _camera = _context->createCamera( "core", Lore::Camera::Type::Type3D );
  _camera->lookAt( Lore::Vec3(), Lore::Vec3( 0.f, 0.f, 1.f ), Lore::Vec3( 0.f, 1.f, 0.f ) );

  // TODO: This is a hack that should be taken care of internally.
  Lore::CLI::SetActiveScene( _scene );

  // We must setup a RenderView to inform Lore how to render this scene in relation
  // to the window. We provide a viewport of [0, 0, 1, 1] to render the scene to the
  // entire window.
  Lore::RenderView rv( "core", _scene, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
  rv.camera = _camera;

  // Add the RenderView to the window so it will render our scene.
  _window->addRenderView( rv );

  //
  // Setup is done, now we can add some contents to the scene.

  // Add a cube.

  Lore::EntityPtr cubeEntity = Lore::Resource::CreateEntity( "cube", Lore::VertexBuffer::Type::Quad );
  cubeEntity->getMaterial()->ambient = Lore::StockColor::Blue;
  auto node = _scene->createNode( "cube" );
  node->attachObject( cubeEntity );
  node->setPosition( 0.f, 0.f, 1.f );
  //node->rotate( Lore::Vec3( 0.f, 1.f, 0.f ), Lore::Degree( 180.f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::processInput()
{
  // Player movement.
  constexpr const Lore::real PlayerSpeed = 0.01f;
  Lore::Vec3 playerOffset;
  if ( Lore::Input::GetKeyState( Lore::Keycode::W ) ) {
    playerOffset.z -= PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::A ) ) {
    playerOffset.x -= PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::S ) ) {
    playerOffset.z += PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::D ) ) {
    playerOffset.x += PlayerSpeed;
  }
  if ( playerOffset.x != 0.f || playerOffset.y != 0.f ) {
    _camera->translate( playerOffset );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::update()
{
  static float blockOffset = 0.f;
  float blockExtraOffset = 0.f;
  int i = 0;
  for ( auto& block : _floatingBlocks ) {
    block->setPosition( block->getPosition().x, std::sinf( blockOffset ) * 0.5f + blockExtraOffset );
    block->rotate( ( i++ % 2 == 0 ) ? Lore::Math::POSITIVE_Z_AXIS : Lore::Math::POSITIVE_Y_AXIS, Lore::Degree( 0.31f ) );
  }
  blockOffset += 0.01f;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::render()
{
  _context->renderFrame();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
