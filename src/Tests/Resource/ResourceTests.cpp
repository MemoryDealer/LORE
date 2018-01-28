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
#include "TestUtils.h"

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Load and unload resources in code", "[resource]" )
{
  TEST_CREATE_CONTEXT();

  // Create a window which is required for resources.
  auto window = context->createWindow( "UnitTest", 50, 50 );

  constexpr const size_t size = 4;
  const std::vector<Lore::string> names = { "1", "2", "3", "4" };

  for ( size_t i = 0; i < size; ++i ) {
    REQUIRE( Lore::Resource::CreateBox( names[i] ) );
    REQUIRE( Lore::Resource::CreateCamera( names[i] ) );
    REQUIRE( Lore::Resource::CreateEntity( names[i], Lore::MeshType::TexturedQuad ) );
    REQUIRE( Lore::Resource::CreateFragmentShader( names[i] ) );
    REQUIRE( Lore::Resource::CreateGPUProgram( names[i] ) );
    REQUIRE( Lore::Resource::CreateMaterial( names[i] ) );
    REQUIRE( Lore::Resource::CreateMesh( names[i], Lore::MeshType::Quad ) );
    REQUIRE( Lore::Resource::CreateRenderTarget( names[i], 640, 480 ) );
    REQUIRE( Lore::Resource::CreateSprite( names[i] ) );
    REQUIRE( Lore::Resource::CreateTextbox( names[i] ) );
    REQUIRE( Lore::Resource::CreateTexture( names[i], 32, 32 ) );
    REQUIRE( Lore::Resource::CreateUI( names[i] ) );
    REQUIRE( Lore::Resource::CreateVertexBuffer( names[i], Lore::MeshType::Quad ) );
    REQUIRE( Lore::Resource::CreateVertexShader( names[i] ) );
  }

  for ( size_t i = 0; i < size; ++i ) {
    REQUIRE( Lore::Resource::GetMaterial( names[i] ) );
  }

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
