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

namespace LocalNS {

  static Game* GameInstance = nullptr;

}
using namespace LocalNS;

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Game::Game()
{
  // Create a Lore context which is required for all Lore functionality.
  // We will explicitly use the OpenGL plugin for now.
  _context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

  // Create a window and set it to the active window.
  _window = _context->createWindow( "Demo 3D", 1280, 720, Lore::RendererType::Forward3D );
  _window->setActive();

  // Allow the DebugUI.
  Lore::DebugUI::Enable();

  Lore::Input::SetMouseMovedCallback( Game::onMouseMove );
  Lore::Input::SetCursorEnabled( false );

  GameInstance = this;
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
  Lore::Resource::LoadResourceConfiguration( "res/demo3d/resources.json" );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadScene()
{
  // Create a scene with default skybox color.
  _scene = _context->createScene( "core", Lore::RendererType::Forward3D );

  // Create a camera to view the scene.
  _camera = _context->createCamera( "core", Lore::Camera::Type::Type3D );
  _camera->lookAt( glm::vec3( 0.f, 1.5f, 0.f ), glm::vec3( 0.f, 0.f, -1.f ), glm::vec3( 0.f, 1.f, 0.f ) );

  // TODO: This is a hack that should be taken care of internally.
  Lore::CLI::SetActiveScene( _scene );

  // We must setup a RenderView to inform Lore how to render this scene in relation
  // to the window. We provide a viewport of [0, 0, 1, 1] to render the scene to the
  // entire window.
  Lore::RenderView rv( "core", _scene, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
  rv.camera = _camera;

  // Add the RenderView to the window so it will render our scene.
  _window->addRenderView( rv );

  // Load the scene from disk.
  Lore::SceneLoader loader;
  loader.process( "res/demo3d/demo3d.scene", _scene );

  // Add some cubes to test instanced rendering.
  auto material = Lore::Resource::GetEntity( "CheckeredCube", "Demo3D" )->getMaterial();
  material->sprite->addTexture( Lore::Texture::Type::Specular, Lore::Resource::GetTexture( "criminal-impact_dn" ) );
  material->sprite->addTexture( Lore::Texture::Type::Diffuse, Lore::Resource::GetTexture( "criminal-impact_dn" ) );
  for ( int i = 1; i < 450; ++i ) {
    std::string cubeName = "cube" + std::to_string(i);
    Lore::NodePtr node = _scene->createNode( cubeName );
    node->attachObject( Lore::Resource::GetEntity( "CheckeredCube", "Demo3D" ) );
    node->setPosition( i * 5.f, 0.f, 0.f );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::processInput()
{
  // Player movement.
  constexpr const Lore::real PlayerSpeed = 0.05f;
  glm::vec3 playerOffset( 0.f );
  if ( Lore::Input::GetKeyState( Lore::Keycode::W ) ) {
    playerOffset += PlayerSpeed * _camera->getTarget();
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::A ) ) {
    playerOffset -= glm::normalize( glm::cross( _camera->getTarget(), glm::vec3( 0.f, 1.f, 0.f ) ) ) * PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::S ) ) {
    playerOffset -= PlayerSpeed * _camera->getTarget();
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::D ) ) {
    playerOffset += glm::normalize( glm::cross( _camera->getTarget(), glm::vec3( 0.f, 1.f, 0.f ) ) ) * PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::E ) ) {
    playerOffset.y += PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::Q ) ) {
    playerOffset.y -= PlayerSpeed;
  }
  if ( playerOffset.x != 0.f || playerOffset.y != 0.f || playerOffset.z != 0.f ) {
    _camera->translate( playerOffset );
  }

  if ( Lore::Input::GetKeyState( Lore::Keycode::Left ) ) {
    _camera->yaw( -1.f );
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::Right ) ) {
    _camera->yaw( 1.f );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::update()
{
  auto node = _scene->getNode( "Cube0" );
  node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.001f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::render()
{
  _context->renderFrame();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::onMouseMove( const int32_t x, const int32_t y )
{
  static int32_t lastX = 400;
  static int32_t lastY = 300;

  const auto xOffset = x - lastX;
  const auto yOffset = y - lastY;
  lastX = x;
  lastY = y;

  const Lore::real sensitivity = 0.1f;
  auto camera = GameInstance->getCamera();
  camera->yaw( xOffset * sensitivity );
  camera->pitch( yOffset * sensitivity );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::CameraPtr Game::getCamera() const
{
  return _camera;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
