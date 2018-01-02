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

#include "Game.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

Game::Game()
{
  // Create a Lore context which is required for all Lore functionality.
  // We will explicitly use the OpenGL plugin for now.
  _context = Lore::CreateContext( Lore::RenderPlugin::OpenGL );

  // Create a window and set it to the active window.
  _window = _context->createWindow( "Complex Scene", 640, 480 );
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
  // Index all resource locations for complex scene as specified in the configuration file.
  Lore::Resource::LoadResourceConfiguration( "res/complexscene/resources.json" );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::loadScene()
{
  // Create a scene with default background color.
  _scene = _context->createScene( "core" );
  _scene->setBackgroundColor( Lore::StockColor::White );
  _scene->setAmbientLightColor( Lore::StockColor::White );

  // Create a camera to view the scene.
  _camera = Lore::Resource::CreateCamera( "core" ); // TODO: Should this come from context?

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

  // In order to render our player sprite, we must create an entity, which describes
  // how to render a sprite (most of the time this is a TexturedQuad).
  Lore::EntityPtr playerEntity = Lore::Resource::CreateEntity( "player", Lore::MeshType::TexturedQuad );
  // Assign our player sprite to the entity so it can be rendered with it.
  // (The "player" sprite was loaded into the Core resource group in loadResources()).
  playerEntity->setSprite( Lore::Resource::GetSprite( "player" ) );

  // Now attach the player entity to the player node, so wherever the node goes,
  // the player sprite will be rendered.
  _playerNode->attachObject( playerEntity );

  // Setup animations for player sprite (this animation set was loaded from main.animation).
  auto spc = _playerNode->createSpriteController();
  spc->useAnimationSet( Lore::Resource::GetAnimationSet( "player" ) );
  spc->startAnimation( "idle" );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::processInput()
{

  // Camera zooming.
  if ( Lore::Input::GetKeyState( Lore::Keycode::Z ) ) {
    _camera->zoom( 0.01f );
  }
  else if ( Lore::Input::GetKeyState( Lore::Keycode::X ) ) {
    _camera->zoom( -0.01f );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

void Game::render()
{
  _context->renderFrame();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
