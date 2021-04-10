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
  _window = _context->createWindow( "Complex Scene 2D", 640, 480, 4, Lore::RendererType::Forward2D );
  _window->setActive();
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

  // Index all resource locations for complex scene as specified in the configuration file.
  Lore::Resource::LoadResourceConfiguration( Lore::FileUtil::ApplyWorkingDirectory( "res/complexscene/resources.json" ) );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadScene()
{
  // Create a scene with default skybox color.
  _scene = _context->createScene( "core", Lore::RendererType::Forward2D );
  _scene->setSkyboxColor( Lore::StockColor::White );
  _scene->setAmbientLightColor( Lore::Color( 0.75f, 0.75f, 0.75f, 1.f ) );

  // Create a camera to view the scene.
  _camera = _context->createCamera( "core", Lore::Camera::Type::Type2D );

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

  // Create a node for our player sprite.
  _playerNode = _scene->createNode( "player" );
  // Decrease its depth so it is rendered above other nodes.
  _playerNode->setDepth( -50.f );
  _playerNode->setPosition( 0.f, 0.25f );

  // In order to render our player sprite, we must create an entity, which describes
  // how to render a sprite (most of the time this is a TexturedQuad).
  Lore::EntityPtr playerEntity = Lore::Resource::CreateEntity( "player", Lore::Mesh::Type::TexturedQuad );

  // Assign our player sprite to the entity so it can be rendered with it.
  // (The "player" sprite was loaded into the Core resource group in loadResources()).
  playerEntity->setSprite( Lore::Resource::GetSprite( "player" ) );

  // Now attach the player entity to the player node, so wherever the node goes,
  // the player sprite will be rendered.
  _playerNode->attachObject( playerEntity );

  // Setup animations for player sprite (this animation set was loaded from main.animation).
  auto spc = _playerNode->createSpriteController();
  spc->useAnimationSet( Lore::Resource::GetSpriteAnimationSet( "player" ) );
  spc->startAnimation( "idle" );

  // We can make the camera automatically track the player.
  _camera->trackNode( _playerNode );
  // Set to a reasonable zoom so the player sprite fits well in the viewport.
  _camera->setZoom( 2.4f );

  //
  // Add some blocks to the scene.

  Lore::EntityPtr blockEntity = Lore::Resource::CreateEntity( "block", Lore::Mesh::Type::TexturedQuad );
  blockEntity->enableInstancing( 1000 );
  blockEntity->setSprite( Lore::Resource::GetSprite( "block" ) );
  for ( int i = 0; i < 10; ++i ) {
    auto blockNode = _scene->createNode( "block" + std::to_string( i ) );
    blockNode->attachObject( blockEntity );

    blockNode->scale( 0.4f );
    blockNode->setPosition( -1.f + static_cast< Lore::real >( i * 0.2f ), 0.f );
    _floatingBlocks.push_back( blockNode );
  }

  //
  // Add some stone walls behind the blocks.

  Lore::EntityPtr stoneEntity = Lore::Resource::CreateEntity( "stone", Lore::Mesh::Type::TexturedQuad );
#ifdef _DEBUG
  const size_t count = 100;
#else
  const size_t count = 10000;
#endif
  stoneEntity->enableInstancing( count );
  stoneEntity->setSprite( Lore::Resource::GetSprite( "stone" ) );
  for ( int i = 0; i < count / 2; ++i ) {
    auto stoneNode = _scene->createNode( "stone" + std::to_string( i ) );
    stoneNode->attachObject( stoneEntity );

    stoneNode->scale( 2.f );
    stoneNode->setDepth( 10.f );
    stoneNode->setPosition( -4.f + static_cast< Lore::real >( i * 0.4f ), 0.f );

    // Add a 2nd row that will go above the stained glass.
    auto stoneNode2 = _scene->createNode( "2stone" + std::to_string( i ) );
    stoneNode2->attachObject( stoneEntity );
    stoneNode2->scale( 2.f );
    stoneNode2->setDepth( 10.f );
    stoneNode2->setPosition( -4.f + static_cast< Lore::real >( i * 0.4f ), .8f );
  }

  //
  // Create some torches.

  Lore::EntityPtr torchEntity = Lore::Resource::CreateEntity( "torch", Lore::Mesh::Type::TexturedQuad );
  torchEntity->setSprite( Lore::Resource::GetSprite( "torch" ) );
  for ( int i = 0; i < 3; ++i ) {
    auto torchNode = _scene->createNode( "torch" + std::to_string( i ) );
    torchNode->attachObject( torchEntity );
    torchNode->setPosition( 0.5f - static_cast<float>(i * 2), 0.24f );
    torchNode->scale( 0.5f );
    auto torchSPC = torchNode->createSpriteController();
    torchSPC->useAnimationSet( Lore::Resource::GetSpriteAnimationSet( "torch" ) );
    torchSPC->startAnimation( "flame" );

    //
    // Add some lights to the torches.

    auto torchLight = _scene->createPointLight( "torch" + std::to_string( i ) );
    //torchLight->setDiffuse( Lore::Color( 1.f, .69f, .4f, 1.f ) );
    torchLight->setAttenuation( 2.0f, 1.0f, 0.4f, 0.8f );
    torchNode->attachObject( torchLight );
  }

  //
  // Add blended stained glass.

  Lore::EntityPtr glassEntity = Lore::Resource::CreateEntity( "glass1", Lore::Mesh::Type::TexturedQuad );
  glassEntity->enableInstancing( 10 );
  glassEntity->setMaterial( Lore::Resource::GetMaterial( "glass1" ) );
  for ( int i = 0; i < 5; ++i ) {
    auto glassNode = _scene->createNode( "glass1" + std::to_string( i ) );
    glassNode->attachObject( glassEntity );
    glassNode->scale( 2.f );
    glassNode->setPosition( -1.f + static_cast< Lore::real >( i * 0.4f ), .4f );
  }

  //
  // Add a skybox to the scene.

  Lore::SkyboxPtr skybox = _scene->getSkybox();

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

void Game::processInput()
{
  // Player movement.
  constexpr const Lore::real PlayerSpeed = 0.01f;
  auto spc = _playerNode->getSpriteController();
  glm::vec2 playerOffset {};
  if ( Lore::Input::GetKeyState( Lore::Keycode::W ) ) {
    playerOffset.y += PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::A ) ) {
    playerOffset.x -= PlayerSpeed;
    spc->setXFlipped( false );
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::S ) ) {
    playerOffset.y -= PlayerSpeed;
  }
  if ( Lore::Input::GetKeyState( Lore::Keycode::D ) ) {
    playerOffset.x += PlayerSpeed;
    spc->setXFlipped( true );
  }
  _playerNode->translate( playerOffset );

  // Camera zooming.
  if ( Lore::Input::GetKeyState( Lore::Keycode::Z ) ) {
    _camera->zoom( 0.03f );
  }
  else if ( Lore::Input::GetKeyState( Lore::Keycode::X ) ) {
    _camera->zoom( -0.03f );
  }

  if ( Lore::Input::GetKeyState( Lore::Keycode::R ) ) {
    Lore::Resource::UnloadGroup( "Core" );
  }

  // Temporary cloning test.
  if ( Lore::Input::GetKeyState( Lore::Keycode::C ) ) {
    static glm::vec2 offset { 0.f, 0.2f };
    static int ctr = 0;
    auto node = _scene->getNode( "block0" );
    auto clone = node->clone( node->getName() + std::to_string( ctr++ ) );
    clone->setPosition( offset );
    offset.y += 0.2f;
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::update()
{
  static float blockOffset = 0.f;
  float blockExtraOffset = 0.f;
  int i = 0;
  for ( auto& block : _floatingBlocks ) {
    //block->setPosition( block->getPosition().x, std::sinf( blockOffset ) * 0.5f + blockExtraOffset );
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
