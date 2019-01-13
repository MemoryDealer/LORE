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
  _window = _context->createWindow( "Complex Scene 3D", 1280, 720, 4, Lore::RendererType::Forward3D );
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
  Lore::Resource::LoadResourceConfiguration( "res/complexscene3d/resources.json" );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadScene()
{
  // Create a scene with default skybox color.
  _scene = _context->createScene( "core", Lore::RendererType::Forward3D );
  _scene->setSkyboxColor( Lore::Color( 0.3f, 0.8f, 0.75f, 1.f ) );
  _scene->setAmbientLightColor( Lore::Color( 0.15f, 0.15f, 0.15f, 1.f ) );

  // Create a camera to view the scene.
  _camera = _context->createCamera( "core", Lore::Camera::Type::Type3D );
  _camera->lookAt( glm::vec3( 0.f, 2.f, -15.f ), glm::vec3( 0.f, 0.f, -1.f ), glm::vec3( 0.f, 1.f, 0.f ) );

  // TODO: This is a hack that should be taken care of internally.
  Lore::CLI::SetActiveScene( _scene );

  // We must setup a RenderView to inform Lore how to render this scene in relation
  // to the window. We provide a viewport of [0, 0, 1, 1] to render the scene to the
  // entire window.
  Lore::RenderView rv( "core", _scene, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
  rv.camera = _camera;

  // Add the RenderView to the window so it will render our scene.
  _window->addRenderView( rv );

  // Setup skybox.
  auto& skyboxLayer = _scene->getSkybox()->addLayer( "skyboxLayer1" );
  skyboxLayer.setSprite( Lore::Resource::GetSprite( "skybox" ) );

  //
  // Setup is done, now we can add some contents to the scene.

  // Add a cube.

  Lore::EntityPtr cubeEntity = Lore::Resource::CreateEntity( "TexturedCube", Lore::Mesh::Type::TexturedCube );

  cubeEntity->setSprite( Lore::Resource::GetSprite( "block" ) );
  auto node = _scene->createNode( "cube" );
  node->attachObject( cubeEntity );
  node->setPosition( 0.f, 0.f, -10.f );
  //node->scale( 0.10f );
  
  for ( int i = 1; i < 20; ++i ) {
    auto node = _scene->createNode( "cubeCopy" + std::to_string( i ) );
    node->attachObject( cubeEntity );
    node->setPosition( ( Lore::real )i * 2.f, 0.f, -10.f );
    //node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 180.f ) );
  }

  Lore::EntityPtr transparentCubeEntity = Lore::Resource::CreateEntity( "TransparentCube", Lore::Mesh::Type::Cube );
  transparentCubeEntity->getMaterial()->diffuse = Lore::StockColor::Blue;
  transparentCubeEntity->getMaterial()->blendingMode.enabled = true;
  transparentCubeEntity->getMaterial()->diffuse.a = 0.5f;
  for ( int i = 0; i < 20; ++i ) {
    auto node = _scene->createNode( "transparentCube" + std::to_string( i ) );
    node->attachObject( transparentCubeEntity );
    node->setPosition( ( Lore::real )i * -2.f, 0.f, -10.f );
  }

  // Add a quad.

  Lore::EntityPtr quad = Lore::Resource::CreateEntity( "quad", Lore::Mesh::Type::Quad3D );
  quad->getMaterial()->diffuse = Lore::StockColor::White;
  auto quadNode = _scene->createNode( "quad0" );
  quadNode->setPosition( -3.f, 0.f, -2.f );
  quadNode->attachObject( quad );
  quadNode->scale( 20.f );

  // Add a textured quad.

  Lore::EntityPtr texturedQuad = Lore::Resource::CreateEntity( "texturedQuad", Lore::Mesh::Type::TexturedQuad3D );
  texturedQuad->setSprite( Lore::Resource::GetSprite( "block" ) );
  texturedQuad->getMaterial()->setTextureScrollSpeed( glm::vec2( 0.01f, 0.f ) );
  auto textureQuadNode = _scene->createNode( "texturedQuad0" );
  textureQuadNode->attachObject( texturedQuad );
  textureQuadNode->setPosition( 5.f, 0.f, 10.f );
  textureQuadNode->scale( 20.f );

  //
  // Lighting.

  // Directional light.
  auto dirLight = _scene->createDirectionalLight( "dir1" );
  dirLight->setDirection( -.5f, -.2f, -.5f );

  auto dirLight2 = _scene->createDirectionalLight( "dir2" );
  dirLight2->setDirection( 5.f, -.3f, -5.f );
  dirLight2->setDiffuse( Lore::StockColor::Red );
  dirLight2->setSpecular( Lore::StockColor::Red );

  auto light = _scene->createPointLight( "core" );
  light->setAmbient( Lore::Color( 0.25f, 0.25f, 0.25f, 1.f ) );
  //light->setIntensity( 5.f );
  light->setDiffuse( Lore::StockColor::Blue );
  light->setSpecular( Lore::StockColor::Green );
  light->setAttenuation( 3.5f, 0.5f, 0.25f, 0.01f );
  auto lightEntity = Lore::Resource::CreateEntity( "light", Lore::Mesh::Type::Cube );
  lightEntity->getMaterial()->ambient = Lore::StockColor::White;
  auto light0 = _scene->createNode( "light0" );
  light0->attachObject( light );
  light0->attachObject( lightEntity );
  light0->setPosition( 1.f, 4.f, -5.f );
  light0->scale( 0.25f );

  auto light2 = _scene->createPointLight( "core2" );
  light2->setAttenuation( 3.5f, 0.5f, 0.25f, 0.01f );
  light2->setAmbient( Lore::Color( 0.f, 0.2f, 0.f, 1.f ) );
  //light2->setDiffuse( Lore::StockColor::Green );
  //light2->setSpecular( Lore::StockColor::Green );
  light2->setIntensity( 4.f );
  auto light2Node = _scene->createNode( "light2" );
  //light2Node->attachObject( light2 );
  light2Node->attachObject( lightEntity );
  light2Node->scale( 0.25f );
  light2Node->setPosition( 4.f, 5.f, -6.f );

  //
  // Environment mapping objects.

  Lore::EntityPtr solidReflectCube = Lore::Resource::CreateEntity( "SolidCube", Lore::Mesh::Type::Cube );
  solidReflectCube->setMaterial( Lore::StockResource::GetMaterial( "Reflect3D" ) );
  solidReflectCube->setSprite( Lore::Resource::GetSprite( "skybox" ) );

  Lore::EntityPtr solidRefractCube = Lore::Resource::CreateEntity( "SolidRefractCube", Lore::Mesh::Type::Cube );
  solidRefractCube->setMaterial( Lore::StockResource::GetMaterial( "Refract3D" ) );
  solidRefractCube->setSprite( Lore::Resource::GetSprite( "skybox" ) );

  // Cubes.
  auto reflectCube = _scene->createNode( "ReflectCube" );
  reflectCube->attachObject( solidReflectCube );
  reflectCube->setPosition( 0.f, 3.f, -10.f );

  auto refractCube = _scene->createNode( "RefractCube" );
  refractCube->attachObject( solidRefractCube );
  refractCube->setPosition( 0.f, 6.f, -10.f );
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
   static float blockOffset = 0.f;
//   float blockExtraOffset = 0.f;
//   int i = 0;
//   for ( auto& block : _floatingBlocks ) {
//     block->setPosition( block->getPosition().x, std::sinf( blockOffset ) * 0.5f + blockExtraOffset );
//     block->rotate( ( i++ % 2 == 0 ) ? glm::vec3( 0.f, 0.f, 1.f ) : glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.31f ) );
//   }
   blockOffset += 0.01f;
  auto node = _scene->getNode( "cube" );
  node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0001f ) );
  node->setPosition( 0.f, 0.f, -10.f + 3.f * std::sinf( blockOffset ) );
  node->setScale( std::sinf( blockOffset ) );

  node = _scene->getNode( "light2" );
  node->setPosition( 6.f + 5.f * std::sinf( blockOffset ), node->getPosition().y, node->getPosition().z );

  node = _scene->getNode( "texturedQuad0" );
  node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0002f ) );

  node = _scene->getNode( "ReflectCube" );
  node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( -0.00025f ) );

  node = _scene->getNode( "RefractCube" );
  node->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees(0.00025f ) );
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
