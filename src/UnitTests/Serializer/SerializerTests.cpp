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
#include "UnitTests.h"
#include <LORE2D/Lore.h>
#include <LORE2D/Serializer/Serializer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Basic JSON values correctly deserialized", "[serializer]" )
{
  TEST_CREATE_CONTEXT();

  Lore::Serializer serializer;

  serializer.deserialize( "TestData/Serializer/Simple.json" );
  auto value1 = serializer.getValue( "A" );
  REQUIRE( Lore::SerializerValue::Type::Int == value1.getType() );
  REQUIRE( 234 == value1.getInt() );

  auto value2 = serializer.getValue( "B" );
  REQUIRE( Lore::SerializerValue::Type::String == value2.getType() );
  REQUIRE( "Hello!" == value2.getString() );

  auto value3 = serializer.getValue( "C" );
  REQUIRE( Lore::SerializerValue::Type::Real == value3.getType() );
  REQUIRE( 66.6f == value3.getReal() );

  auto value4 = serializer.getValue( "D" );
  REQUIRE( Lore::SerializerValue::Type::Bool == value4.getType() );
  REQUIRE( true == value4.getBool() );

  auto value5 = serializer.getValue( "E" );
  REQUIRE( Lore::SerializerValue::Type::Bool == value5.getType() );
  REQUIRE( false == value5.getBool() );

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Complex JSON values correctly deserialized", "[serializer]" )
{
  TEST_CREATE_CONTEXT();

  Lore::Serializer serializer;
  serializer.deserialize( "TestData/Serializer/Complex.json" );

  auto object1 = serializer.getValue( "Object1" );

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
