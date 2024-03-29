// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
// The MIT License (MIT)
// This source file is part of LORE
// ( Lightweight Object-oriented Rendering Engine )
//
// Copyright (c) 2017-2021 Jordan Sparks
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

#include "Shaders.h"

#include <LORE/Core/APIVersion.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

#ifndef _DEBUG
#define HUGE_SCENE true
#endif

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

namespace LocalNS {

  static Game* GameInstance = nullptr;

  static const std::string SampleResourceGroupName = "Sample3D";

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
  loadScene2D();

  // Create a scene with default skybox color.
  _scene = _context->createScene( "core", Lore::RendererType::Forward3D );

  // Create a camera to view the scene.
  _camera = _context->createCamera( "core", Lore::Camera::Type::Type3D );
  _camera->lookAt( glm::vec3( 0.f, 1.5f, 0.f ), glm::vec3( 0.f, 0.f, -1.f ), glm::vec3( 0.f, 1.f, 0.f ) );

  _camera->initPostProcessing( _window->getWidth(), _window->getHeight(), 8 );

  // TODO: This is a hack that should be taken care of internally.
  Lore::CLI::ActiveScene = _scene;

  // We must setup a RenderView to inform Lore how to render this scene in relation
  // to the window. We provide a viewport of [0, 0, 1, 1] to render the scene to the
  // entire window.
  Lore::RenderView rv( "core", _scene, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
  rv.camera = _camera;
  rv.gamma = 1.f; // Post-processing will apply gamma.

  // Add the RenderView to the window so it will render our scene.
  _window->addRenderView( rv );

  // Load the scene from disk.
  Lore::SceneLoader loader;
  loader.process( Lore::FileUtil::ApplyWorkingDirectory( "res/sample3d/sample3d.scene" ), _scene );

  // Create stone flooring.
  auto stonePrefab = Lore::Resource::GetPrefab( "StoneQuad", SampleResourceGroupName );
  constexpr Lore::real StoneFloorScale = 2.f;
  constexpr int stoneFloorGridSize = 16;
  for ( int i = -( stoneFloorGridSize / 2 ); i < ( stoneFloorGridSize / 2 ); ++i ) {
    for ( int j = -( stoneFloorGridSize / 2 ); j < ( stoneFloorGridSize / 2 ); ++j ) {
      auto node = _scene->createNode( "stone-floor" + std::to_string( i ) + std::to_string( j ) );
      node->attachObject( stonePrefab );
      node->setPosition( static_cast<Lore::real>( i ) * StoneFloorScale * 2.f, 0.f, static_cast<Lore::real>( j ) * StoneFloorScale * 2.f);
      node->rotate( Lore::Vec3PosX, glm::radians( 90.f ) );
      node->scale( StoneFloorScale );
    }
  }

  // Setup three cube lights at equidistant points around magic sphere.
  for ( int i = 0; i < 3; ++i ) {
    Lore::NodePtr cube {};
    switch ( i ) {
    case 0: default: cube = _scene->getNode( "RedCube" ); break;
    case 1: cube = _scene->getNode( "GreenCube" ); break;
    case 2: cube = _scene->getNode( "BlueCube" ); break;
    }

    constexpr auto r = 3.0f;
    float angle = glm::radians( 120.0f * i );
    glm::vec2 p;
    p.x = r * glm::cos( angle );
    p.y = r * glm::sin( angle );
    cube->setPosition( p.x, 0.0f, p.y );
  }

  auto rttPrefab = Lore::Resource::GetPrefab( "RTTCube", SampleResourceGroupName );
  rttPrefab->_material->sprite = _rttSprite;
  //rttPrefab->_material->program = Lore::StockResource::GetGPUProgram( "UnlitTextured3D" );

#ifdef _DEBUG
  constexpr auto RTTQuadCount = 10;
#elif HUGE_SCENE
  constexpr auto RTTQuadCount = 50;
#else
  constexpr auto RTTQuadCount = 15;
#endif
  Lore::u32 ctr = 0;
  for ( int i = - ( RTTQuadCount / 2 ); i < ( RTTQuadCount / 2 ); ++i ) {
    for ( int j = -( RTTQuadCount / 2 ); j < ( RTTQuadCount / 2 ); ++j ) {
      auto node = _scene->createNode( "RTTQuad" + std::to_string( ctr++ ) );
      node->attachObject( rttPrefab );
      Lore::real x = 4.f * i;
#ifdef HUGE_SCENE
      Lore::real y = 3.f * j + 50.0f;
      node->setPosition( -80.f, y, x );
#else
      Lore::real y = 3.f * j + 10.0f;
      node->setPosition( -40.f, y, x );
#endif

      node->rotate( Lore::Vec3PosY, glm::radians( -90.0f ) );
      node->rotate( Lore::Vec3PosX, glm::radians( 180.0f ) );
      node->scale( glm::vec3( 1.6f, 1.f, 1.f ) );
    }
  }

  auto metalStarPrefab = Lore::Resource::GetPrefab( "MetalStar", SampleResourceGroupName );
#ifdef _DEBUG
  constexpr auto MetalStarCount = 10;
#elif HUGE_SCENE
  constexpr auto MetalStarCount = 50;
#else
  constexpr auto MetalStarCount = 15;
#endif
  ctr = 0;
  float rot = 0.f;
  for ( int i = -( MetalStarCount / 2 ); i < ( MetalStarCount / 2 ); ++i ) {
    for ( int j = -( MetalStarCount / 2 ); j < ( MetalStarCount / 2 ); ++j ) {
      auto node = _scene->createNode( "MetalStar" + std::to_string( ctr++ ) );
      node->attachObject( metalStarPrefab );
      
#ifdef HUGE_SCENE
      Lore::real x = 4.f * i + 25.0f;
      Lore::real y = 4.f * j + 90.0f;
      node->setPosition( x, y, -80.f );
#else
      Lore::real x = 4.f * i;
      Lore::real y = 4.f * j + 10.0f;
      node->setPosition( x, y, -40.f );
#endif

      node->rotate( Lore::Vec3PosY, glm::radians( rot ) );

      _metalStars.push_back( node );
    }

    rot += 20.0f;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadScene2D()
{
  //
  // Setup secondary 2D scene for render to texture.

  Lore::Resource::LoadResourceConfiguration( Lore::FileUtil::ApplyWorkingDirectory( "res/sample2d/resources.json" ) );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );

  _scene2D = _context->createScene( "2D", Lore::RendererType::Forward2D );
  _scene2D->setSkyboxColor( Lore::StockColor::White );
  _scene2D->setAmbientLightColor( Lore::Color( 0.75f, 0.75f, 0.75f, 1.f ) );

  _camera2D = _context->createCamera( "2D", Lore::Camera::Type::Type2D );
  _camera2D->initPostProcessing( 1920, 1080, 8 );

  // TODO: This is a hack that should be taken care of internally.
  Lore::CLI::ActiveScene = _scene2D;

  Lore::RenderView rv( "2D", _scene2D, Lore::Rect( 0.f, 0.f, 1.f, 1.f ) );
  rv.camera = _camera2D;
  rv.renderTarget = Lore::Resource::CreateRenderTarget( "rt1", 1920, 1080, 0 );
  rv.gamma = 1.0f; // Don't need gamma correction since this is sampled in the 3D scene.
  _window->addRenderView( rv );

  // Create a sprite to sample the 2D render target texture.
  _rttSprite = Lore::Resource::CreateSprite( "rtt" );
  _rttSprite->addTexture( Lore::Texture::Type::Diffuse, rv.renderTarget->getTexture() );

  // Create a node for our player sprite.
  _playerNode2D = _scene2D->createNode( "player" );
  // Decrease its depth so it is rendered above other nodes.
  _playerNode2D->setDepth( -50.f );
  _playerNode2D->setPosition( 0.f, 0.25f );

  Lore::PrefabPtr playerPrefab = Lore::Resource::CreatePrefab( "player", Lore::Mesh::Type::TexturedQuad );
  playerPrefab->setSprite( Lore::Resource::GetSprite( "player" ) );
  playerPrefab->cullingMode = Lore::IRenderAPI::CullingMode::None;
  _playerNode2D->attachObject( playerPrefab );

  auto spc = _playerNode2D->createSpriteController();
  spc->useAnimationSet( Lore::Resource::GetSpriteAnimationSet( "player" ) );
  spc->startAnimation( "idle" );

  _camera2D->trackNode( _playerNode2D );
  _camera2D->setZoom( 2.4f );

  //
  // Add some blocks to the scene.

  Lore::PrefabPtr blockPrefab = Lore::Resource::CreatePrefab( "block", Lore::Mesh::Type::TexturedQuad );
  blockPrefab->enableInstancing( 1000 );
  blockPrefab->setSprite( Lore::Resource::GetSprite( "block" ) );
  blockPrefab->cullingMode = Lore::IRenderAPI::CullingMode::None; // Some blocks we rotate to the backface.
  for ( int i = 0; i < 10; ++i ) {
    auto blockNode = _scene2D->createNode( "block" + std::to_string( i ) );
    blockNode->attachObject( blockPrefab );

    blockNode->scale( 0.4f );
    blockNode->setPosition( -1.f + static_cast<Lore::real>( i * 0.2f ), 0.f );
    _floatingBlocks.push_back( blockNode );
  }

  //
  // Add some stone walls behind the blocks.

  Lore::PrefabPtr stonePrefab = Lore::Resource::CreatePrefab( "stone2d", Lore::Mesh::Type::TexturedQuad );

  const size_t count = 40;
  stonePrefab->enableInstancing( count );
  stonePrefab->setSprite( Lore::Resource::GetSprite( "stone2d" ) );
  for ( int i = 0; i < count / 2; ++i ) {
    auto stoneNode = _scene2D->createNode( "stone" + std::to_string( i ) );
    stoneNode->attachObject( stonePrefab );

    stoneNode->scale( 2.f );
    stoneNode->setDepth( 10.f );
    stoneNode->setPosition( -4.f + static_cast<Lore::real>( i * 0.4f ), 0.f );

    // Add a 2nd row that will go above the stained glass.
    auto stoneNode2 = _scene2D->createNode( "2stone" + std::to_string( i ) );
    stoneNode2->attachObject( stonePrefab );
    stoneNode2->scale( 2.f );
    stoneNode2->setDepth( 10.f );
    stoneNode2->setPosition( -4.f + static_cast<Lore::real>( i * 0.4f ), .8f );
  }

  //
  // Create some torches.

  Lore::PrefabPtr torchPrefab = Lore::Resource::CreatePrefab( "torch", Lore::Mesh::Type::TexturedQuad );
  torchPrefab->setSprite( Lore::Resource::GetSprite( "torch" ) );
  for ( int i = 0; i < 3; ++i ) {
    auto torchNode = _scene2D->createNode( "torch" + std::to_string( i ) );
    torchNode->attachObject( torchPrefab );
    torchNode->setPosition( 0.5f - static_cast<float>( i * 2 ), 0.24f );
    torchNode->scale( 0.5f );
    auto torchSPC = torchNode->createSpriteController();
    torchSPC->useAnimationSet( Lore::Resource::GetSpriteAnimationSet( "torch" ) );
    torchSPC->startAnimation( "flame" );

    //
    // Add some lights to the torches.

    auto torchLight = _scene2D->createPointLight( "torch" + std::to_string( i ) );
    //torchLight->setDiffuse( Lore::Color( 1.f, .69f, .4f, 1.f ) );
    torchLight->setAttenuation( 2.0f, 1.0f, 0.4f, 0.8f );
    torchNode->attachObject( torchLight );
  }

  //
  // Add blended stained glass.

  Lore::PrefabPtr glassPrefab = Lore::Resource::CreatePrefab( "glass1", Lore::Mesh::Type::TexturedQuad );
  glassPrefab->enableInstancing( 10 );
  glassPrefab->setMaterial( Lore::Resource::GetMaterial( "glass1" ) );
  for ( int i = 0; i < 5; ++i ) {
    auto glassNode = _scene2D->createNode( "glass1" + std::to_string( i ) );
    glassNode->attachObject( glassPrefab );
    glassNode->scale( 2.f );
    glassNode->setPosition( -1.f + static_cast<Lore::real>( i * 0.4f ), .4f );
  }

  //
  // Add a skybox to the scene.

  Lore::SkyboxPtr skybox = _scene2D->getSkybox();

  // The first layer will be some scrolling clouds.
  auto& layerClouds = skybox->addLayer( "clouds" );
  layerClouds.setSprite( Lore::Resource::GetSprite( "clouds" ) );
  layerClouds.setScrollSpeed( glm::vec2( 0.001f, 0.00005f ) );
  layerClouds.setDepth( 1000.f ); // This is the default but we are setting it here for reference.

  // Add far layer with heavy parallax.
  auto& layerFar = skybox->addLayer( "far" );
  layerFar.setSprite( Lore::Resource::GetSprite( "bg-far" ) );
  layerFar.setDepth( 990.f );
  layerFar.setParallax( glm::vec2( 0.1f, 0.08f ) );

  // Add middle layer with lighter parallax.
  auto& layerMiddle = skybox->addLayer( "middle" );
  layerMiddle.setSprite( Lore::Resource::GetSprite( "bg-middle" ) );
  layerMiddle.setDepth( 980.f );
  layerMiddle.setParallax( glm::vec2( .3f, .10f ) );

  // Add closest layer with the least amount of parallax.
  auto& layerForeground = skybox->addLayer( "foreground" );
  layerForeground.setSprite( Lore::Resource::GetSprite( "bg-foreground" ) );
  layerForeground.setDepth( 970.f );
  layerForeground.setParallax( glm::vec2( .4f, 0.12f ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadCustomShaders()
{
  LoadCustomShaders();
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

  // 2D scene.
  // Player movement.
  constexpr const Lore::real PlayerSpeed2D = 0.005f;
  auto spc = _playerNode2D->getSpriteController();
  glm::vec2 playerOffset2D {};
  if ( Lore::Input::GetKeyState( Lore::Keycode::Up ) ) {
    playerOffset2D.y += PlayerSpeed2D;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::Left ) ) {
    playerOffset2D.x -= PlayerSpeed2D;
    spc->setXFlipped( false );
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::Down ) ) {
    playerOffset2D.y -= PlayerSpeed2D;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::Right ) ) {
    playerOffset2D.x += PlayerSpeed2D;
    spc->setXFlipped( true );
  }
  _playerNode2D->translate( playerOffset2D );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::update()
{
  static auto magicSphere = _scene->getNode( "MagicSphere" );
  magicSphere->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0001f ) );

  static auto multicube1 = _scene->getNode( "Multicube1" );
  static auto multicube2 = _scene->getNode( "Multicube2" );
  static auto mixSprite = Lore::Resource::GetPrefab( "Multicube", SampleResourceGroupName )->_material->sprite;
  static auto mixTime = 0.0f;
  mixSprite->setMixValue( 0, Lore::Texture::Type::Diffuse, 1, Lore::Util::Pulse( mixTime, 0.5f ) );
  mixTime += 0.01f;
  multicube1->rotate( glm::vec3( 1.f, 1.f, 0.f ), glm::degrees( 0.0001f ) );
  multicube2->rotate( glm::vec3( 1.f, 1.f, 0.f ), glm::degrees( -0.0001f ) );

  static auto checkeredCube1 = _scene->getNode( "checkeredcube1" );
  static auto checkeredCube2 = _scene->getNode( "checkeredcube2" );
  checkeredCube1->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0001f ) );
  checkeredCube2->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( -0.0001f ) );

  static auto dorrieParent = _scene->getNode( "dorrieParent" );
  dorrieParent->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees(-0.0004f ) );
  static auto dorrieStar = _scene->getNode( "dorrieStar" );
  dorrieStar->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0012f ) );

  for ( auto& metalStar : _metalStars ) {
    metalStar->rotate( Lore::Vec3PosY, glm::radians( 1.f ) );
  }

  static auto discoBall = _scene->getNode( "DiscoBall" );
  discoBall->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.0001f ) );

  static auto magicSphere3 = _scene->getNode( "MagicSphere3" );
  magicSphere3->rotate( glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( -0.0001f ) );

  // 2D scene updates.

  static float blockOffset = 0.f;
  float blockExtraOffset = 0.f;
  int i = 0;
  for ( auto& block : _floatingBlocks ) {
    block->rotate( ( i++ % 2 == 0 ) ? glm::vec3( 0.f, 0.f, 1.f ) : glm::vec3( 0.f, 1.f, 0.f ), glm::degrees( 0.00031f ) );
  }
  blockOffset += 0.01f;
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
