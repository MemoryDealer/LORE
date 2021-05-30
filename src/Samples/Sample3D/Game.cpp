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
  _window = _context->createWindow( "Sample 3D", 1280, 720, 8, Lore::RendererType::Forward3D );
  _window->setActive();

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
  Lore::Resource::SetWorkingDirectory( "../../../" );

  // Index all resource locations for scene as specified in the configuration file.
  Lore::Resource::LoadResourceConfiguration( Lore::FileUtil::ApplyWorkingDirectory( "res/sample3d/resources.json" ) );
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

  _camera->initPostProcessing( _window->getWidth(), _window->getHeight(), 8 );

  // TODO: This is a hack that should be taken care of internally.
  Lore::CLI::SetActiveScene( _scene );

  // We must setup a RenderView to inform Lore how to render this scene in relation
  // to the window. We provide a viewport of [0, 0, 1, 1] to render the scene to the
  // entire window.
  Lore::RenderView rv( "core", _scene, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
  rv.camera = _camera;

  //rv.renderTarget = Lore::Resource::CreateRenderTarget( "rt1", 1920, 1080, 8 );
  rv.gamma = 1.f; // Post-processing will apply gamma.

  // Add the RenderView to the window so it will render our scene.
  _window->addRenderView( rv );


// 
//   Lore::ScenePtr postScene = _context->createScene( "pp", Lore::RendererType::Forward2D );
//   Lore::RenderView postProcessor( "pp", postScene, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
//   postProcessor.camera = _context->createCamera( "postCam", Lore::Camera::Type::Type2D );
//   _window->addRenderView( postProcessor );
// 
//   auto ppe = Lore::Resource::CreateEntity( "ppe", Lore::Mesh::Type::TexturedQuad );
//   ppe->getMaterial()->program = Lore::StockResource::GetGPUProgram( "UnlitTexturedRTT" );
// 
//   auto rttSprite = Lore::Resource::CreateSprite( "rtt" );
//   rttSprite->addTexture( Lore::Texture::Type::Diffuse, rv.renderTarget->getTexture() );
//   ppe->getMaterial()->lighting = false;
//   ppe->getMaterial()->sprite = rttSprite;
//   auto ppeNode = postScene->createNode( "ppe" );
//   ppeNode->attachObject( ppe );
//   ppeNode->scale( glm::vec2( 10.6664f, 8.f ) );

//   Lore::SkyboxPtr skybox = postScene->getSkybox();
// 
//   // The first layer will be some scrolling clouds.
//   auto& layer0 = skybox->addLayer( "rtt" );
//   layer0.setSprite( ppe->getMaterial()->sprite );



  // Load the scene from disk.
  Lore::SceneLoader loader;
  loader.process( Lore::FileUtil::ApplyWorkingDirectory( "res/sample3d/sample3d.scene" ), _scene );

  // Create stone flooring.
  auto stoneEntity = Lore::Resource::GetEntity( "StoneQuad", "Sample3D" );
  constexpr Lore::real StoneFloorScale = 2.f;
  constexpr int stoneFloorGridSize = 8;
  for ( int i = -( stoneFloorGridSize / 2 ); i < ( stoneFloorGridSize / 2 ); ++i ) {
    for ( int j = -( stoneFloorGridSize / 2 ); j < ( stoneFloorGridSize / 2 ); ++j ) {
      auto node = _scene->createNode( "stone-floor" + std::to_string( i ) + std::to_string( j ) );
      node->attachObject( stoneEntity );
      node->setPosition( static_cast<Lore::real>( i ) * StoneFloorScale * 2.f, 0.f, static_cast<Lore::real>( j ) * StoneFloorScale * 2.f);
      node->rotate( Lore::Vec3PosX, glm::radians( 90.f ) );
      node->scale( StoneFloorScale );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::processInput()
{
  // Player movement.
  const Lore::real PlayerSpeed = ( Lore::Input::GetKeyState( Lore::Keycode::LeftShift ) ) ? 0.5f : 0.05f;
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
  auto node = _scene->getNode( "Cube2" );
  node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0001f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::render()
{
  _context->renderFrame();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::onMouseMove( const int32_t x, const int32_t y )
{
  const Lore::real sensitivity = 0.1f;
  auto camera = GameInstance->getCamera();
  camera->yaw( x * sensitivity );
  camera->pitch( y * sensitivity );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Lore::CameraPtr Game::getCamera() const
{
  return _camera;
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
