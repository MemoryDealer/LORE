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

#include "catch.hpp"
#include <LORE2D/Lore.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Memory Pool", "[memory]" )
{
    const size_t size = 128;
    Lore::MemoryPool<Lore::Node> pool( "test", size );

    SECTION( "Creation/destruction" )
    {
        // Create some objects.
        std::vector<Lore::Node*> nodes;
        for ( int i = 0; i < size; ++i ) {
            nodes.push_back( pool.create() );
            REQUIRE( true == nodes[i]->getInUse() );
        }

        pool.destroy( nodes[50] );
        REQUIRE( false == nodes[50]->getInUse() );

        pool.destroyAll();
        for ( int i = 0; i < size; ++i ) {
            REQUIRE( false == pool.getObjectAt( i )->getInUse() );
        }
    }
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Pool Cluster", "[memory]" )
{
    const size_t size = 128;
    Lore::PoolCluster cluster( "test" );
    cluster.registerPool<Lore::Node>( size );
    cluster.registerPool<Lore::Camera>( size );

    SECTION( "Creation/destruction" )
    {
        std::vector<Lore::Node*> nodes;
        std::vector<Lore::Camera*> shaders;
        for ( int i = 0; i < size; ++i ) {
            nodes.push_back( cluster.create<Lore::Node>() );
            shaders.push_back( cluster.create<Lore::Camera>() );

            REQUIRE( true == nodes[i]->getInUse() );
            REQUIRE( true == shaders[i]->getInUse() );
        }

        cluster.destroy<Lore::Node>( nodes[100] );
        cluster.destroy<Lore::Camera>( shaders[100] );
        REQUIRE( false == nodes[100]->getInUse() );
        REQUIRE( false == shaders[100]->getInUse() );
    }

    cluster.unregisterPool<Lore::Node>();
    cluster.unregisterPool<Lore::Camera>();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
