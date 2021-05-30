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

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Memory Pool", "[memory]" )
{
  constexpr const size_t size = 128;
  Lore::MemoryPool<Lore::Node> pool( "test", size );

  SECTION( "Creation/destruction" )
  {
    // Create some objects.
    std::vector<Lore::Node*> nodes;
    for ( int i = 0; i < size; ++i ) {
      nodes.push_back( pool.create() );
      REQUIRE( true == nodes[i]->isInUse() );
      nodes.back()->setPosition( 1.f, 1.f );
      nodes.back()->createSpriteController();
    }

    auto nodeToDestroy = nodes[50];
    pool.destroy( nodeToDestroy );
    REQUIRE( false == nodeToDestroy->isInUse() );
    REQUIRE( 0.f == nodeToDestroy->getPosition().x );
    REQUIRE( 0.f == nodeToDestroy->getPosition().y );
    REQUIRE_FALSE( nodeToDestroy->getSpriteController() );

    pool.destroyAll();
    for ( int i = 0; i < size; ++i ) {
      auto node = pool.getObjectAt( i );
      REQUIRE( false == node->isInUse() );
      REQUIRE( 0.f == node->getPosition().x );
      REQUIRE( 0.f == node->getPosition().y );
      REQUIRE_FALSE( node->getSpriteController() );
    }
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Multiple create/destroy", "[memory]" )
{
  constexpr const size_t size = 2;
  Lore::MemoryPool<Lore::Prefab> pool( "test", size );
  REQUIRE( pool.getSizeInBytes() == ( sizeof( Lore::Prefab ) * size ) );

  SECTION( "Creation/destruction" )
  {
    REQUIRE( pool.getActiveObjectCount() == 0 );
    auto e1 = pool.create();
    auto e2 = pool.create();
    REQUIRE( pool.getActiveObjectCount() == 2 );

    pool.destroy( e1 );
    REQUIRE( pool.getActiveObjectCount() == 1 );

    e1 = pool.create();
    REQUIRE( e1 );
    REQUIRE( pool.getActiveObjectCount() == 2 );

    pool.destroy( e2 );
    pool.destroy( e1 );
    REQUIRE( pool.getActiveObjectCount() == 0 );
  }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Pool Cluster", "[memory]" )
{
  constexpr const size_t size = 128;
  Lore::PoolCluster cluster( "test" );
  cluster.registerPool<Lore::Node>( size );
  cluster.registerPool<Lore::Material>( size );

  SECTION( "Creation/destruction" )
  {
    std::vector<Lore::NodePtr> nodes;
    std::vector<Lore::MaterialPtr> materials;
    for ( int i = 0; i < size; ++i ) {
      nodes.push_back( cluster.create<Lore::Node>() );
      materials.push_back( cluster.create<Lore::Material>() );

      REQUIRE( true == nodes[i]->isInUse() );
      REQUIRE( true == materials[i]->isInUse() );
    }

    cluster.destroy<Lore::Node>( nodes[100] );
    cluster.destroy<Lore::Material>( materials[100] );
    REQUIRE( false == nodes[100]->isInUse() );
    REQUIRE( false == materials[100]->isInUse() );
  }

  cluster.unregisterPool<Lore::Node>();
  cluster.unregisterPool<Lore::Material>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
