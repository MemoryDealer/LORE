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

TEST_CASE( "Load and unload resources in code", "[resource]" )
{
  TEST_CREATE_CONTEXT();

  // Create a window which is required for resources.
  auto window = context->createWindow( "Test", 50, 50 );

  constexpr const size_t size = 4;
  const std::vector<Lore::string> names = { "1", "2", "3", "4" };

  for ( size_t i = 0; i < size; ++i ) {
    REQUIRE( Lore::Resource::CreateBox( names[i] ) );
    REQUIRE( Lore::Resource::CreateEntity( names[i], Lore::VertexBuffer::Type::TexturedQuad ) );
    REQUIRE( Lore::Resource::CreateGPUProgram( names[i] ) );
    REQUIRE( Lore::Resource::CreateMaterial( names[i] ) );
    REQUIRE( Lore::Resource::CreateMesh( names[i], Lore::VertexBuffer::Type::Quad ) );
    REQUIRE( Lore::Resource::CreateRenderTarget( names[i], 640, 480 ) );
    REQUIRE( Lore::Resource::CreateShader( names[i] + "_FS", Lore::Shader::Type::Fragment ) );
    REQUIRE( Lore::Resource::CreateShader( names[i] + "_VS", Lore::Shader::Type::Vertex ) );
    REQUIRE( Lore::Resource::CreateSprite( names[i] ) );
    REQUIRE( Lore::Resource::CreateSpriteAnimationSet( names[i] ) );
    REQUIRE( Lore::Resource::CreateTextbox( names[i] ) );
    REQUIRE( Lore::Resource::CreateTexture( names[i], 32, 32, Lore::StockColor::White ) );
    REQUIRE( Lore::Resource::CreateUI( names[i] ) );
    REQUIRE( Lore::Resource::CreateVertexBuffer( names[i], Lore::VertexBuffer::Type::Quad ) );
  }

  for ( size_t i = 0; i < size; ++i ) {
    REQUIRE( Lore::Resource::GetBox( names[i] ) );
    REQUIRE( Lore::Resource::GetEntity( names[i] ) );
    REQUIRE( Lore::Resource::GetGPUProgram( names[i] ) );
    REQUIRE( Lore::Resource::GetMaterial( names[i] ) );
    REQUIRE( Lore::Resource::GetMesh( names[i] ) );
    REQUIRE( Lore::Resource::GetRenderTarget( names[i] ) );
    REQUIRE( Lore::Resource::GetShader( names[i] + "_FS" ) );
    REQUIRE( Lore::Resource::GetShader( names[i] + "_VS" ) );
    REQUIRE( Lore::Resource::GetSprite( names[i] ) );
    REQUIRE( Lore::Resource::GetSpriteAnimationSet( names[i] ) );
    REQUIRE( Lore::Resource::GetTexture( names[i] ) );
    REQUIRE( Lore::Resource::GetTextbox( names[i] ) );
    REQUIRE( Lore::Resource::GetUI( names[i] ) );
    REQUIRE( Lore::Resource::GetVertexBuffer( names[i] ) );
  }

  Lore::Resource::UnloadGroup( "Core" );

  for ( size_t i = 0; i < size; ++i ) {
    CHECK_THROWS( Lore::Resource::GetBox( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetEntity( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetFont( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetGPUProgram( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetMaterial( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetMesh( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetRenderTarget( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetShader( names[i] + "_FS" ) );
    CHECK_THROWS( Lore::Resource::GetShader( names[i] + "_VS" ) );
    CHECK_THROWS( Lore::Resource::GetSprite( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetSpriteAnimationSet( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetTexture( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetTextbox( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetUI( names[i] ) );
    CHECK_THROWS( Lore::Resource::GetVertexBuffer( names[i] ) );
  }

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Load and unload resources in code, different group", "[resource]" )
{
  TEST_CREATE_CONTEXT();

  // Create a window which is required for resources.
  auto window = context->createWindow( "Test", 50, 50 );

  constexpr const size_t size = 4;
  const std::vector<Lore::string> names = { "1", "2", "3", "4" };
  const Lore::string groupName = "LevelOne";

  for ( size_t i = 0; i < size; ++i ) {
    REQUIRE( Lore::Resource::CreateBox( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateEntity( names[i], Lore::VertexBuffer::Type::TexturedQuad, groupName ) );
    REQUIRE( Lore::Resource::CreateGPUProgram( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateMaterial( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateMesh( names[i], Lore::VertexBuffer::Type::Quad, groupName ) );
    REQUIRE( Lore::Resource::CreateRenderTarget( names[i], 640, 480, groupName ) );
    REQUIRE( Lore::Resource::CreateShader( names[i] + "_FS", Lore::Shader::Type::Fragment, groupName ) );
    REQUIRE( Lore::Resource::CreateShader( names[i] + "_VS", Lore::Shader::Type::Vertex, groupName ) );
    REQUIRE( Lore::Resource::CreateSprite( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateSpriteAnimationSet( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateTextbox( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateTexture( names[i], 32, 32, Lore::StockColor::White, groupName ) );
    REQUIRE( Lore::Resource::CreateUI( names[i], groupName ) );
    REQUIRE( Lore::Resource::CreateVertexBuffer( names[i], Lore::VertexBuffer::Type::Quad, groupName ) );
  }

  for ( size_t i = 0; i < size; ++i ) {
    REQUIRE( Lore::Resource::GetBox( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetEntity( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetGPUProgram( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetMaterial( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetMesh( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetRenderTarget( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetShader( names[i] + "_FS", groupName ) );
    REQUIRE( Lore::Resource::GetShader( names[i] + "_VS", groupName ) );
    REQUIRE( Lore::Resource::GetSprite( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetSpriteAnimationSet( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetTexture( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetTextbox( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetUI( names[i], groupName ) );
    REQUIRE( Lore::Resource::GetVertexBuffer( names[i], groupName ) );
  }

  Lore::Resource::UnloadGroup( groupName );

  for ( size_t i = 0; i < size; ++i ) {
    CHECK_THROWS( Lore::Resource::GetBox( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetEntity( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetFont( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetGPUProgram( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetMaterial( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetMesh( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetRenderTarget( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetShader( names[i] + "_FS", groupName ) );
    CHECK_THROWS( Lore::Resource::GetShader( names[i] + "_VS", groupName ) );
    CHECK_THROWS( Lore::Resource::GetSprite( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetSpriteAnimationSet( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetTexture( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetTextbox( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetUI( names[i], groupName ) );
    CHECK_THROWS( Lore::Resource::GetVertexBuffer( names[i], groupName ) );
  }

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
