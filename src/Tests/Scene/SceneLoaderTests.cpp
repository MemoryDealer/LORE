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

#include "catch.hpp"
#include "TestUtils.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Load scene from file", "[scene]" )
{
  LoreTestHelper helper;

  auto scene = helper.getContext()->createScene( "test", Lore::RendererType::Forward3D );

  // Index all resource locations for complex scene as specified in the configuration file.
  Lore::Resource::LoadResourceConfiguration( "TestData/Scene/resources.json" );
  // Now load the Core resource group, which contains the resource locations we just indexed.
  Lore::Resource::LoadGroup( Lore::ResourceController::DefaultGroupName );

  // Load the scene contents.
  Lore::SceneLoader loader;
  loader.process( "TestData/Scene/test.scene", scene );

  const Lore::string resourceGroup = "test";

  // Check scene properties.
  auto skybox = scene->getSkybox();
  REQUIRE( skybox->getLayer( "layer0" ).getMaterial()->sprite->getName() == "alien-sky" );
  const Lore::Color skyboxColor( .3f, .8f, .75f, 1.f );
  REQUIRE( scene->getSkyboxColor() == skyboxColor );
  const Lore::Color ambientLightColor( .05f, .05f, .05f, 1.f );
  REQUIRE( scene->getAmbientLightColor() == ambientLightColor );

  // Check loaded prefabs.
  REQUIRE( Lore::Resource::GetPrefab( "CheckeredFloor", resourceGroup ) );
  REQUIRE( Lore::Resource::GetPrefab( "CheckeredCube", resourceGroup ) );

  // Check lights.
  REQUIRE( 1 == scene->getDirectionalLightCount() );
  REQUIRE( scene->getLight( Lore::Light::Type::Point, "PointLight0" ) );

  // Check nodes.
  REQUIRE( scene->getNode( "MainFloor0" ) );
  REQUIRE( scene->getNode( "MainFloor1" ) );
  REQUIRE( scene->getNode( "Cube0" ) );
  REQUIRE( scene->getNode( "FloatingLight" ) );
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
