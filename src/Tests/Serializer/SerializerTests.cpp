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
#include <LORE/Serializer/Serializer.h>

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Basic JSON values correctly deserialized", "[serializer]" )
{
  TEST_CREATE_CONTEXT();

  Lore::Serializer serializer;

  serializer.deserialize( "TestData/Serializer/Simple.json" );
  auto value1 = serializer.getValue( "A" );
  REQUIRE( Lore::SerializerValue::Type::Int == value1.getType() );
  REQUIRE( 234 == value1.toInt() );

  auto value2 = serializer.getValue( "B" );
  REQUIRE( Lore::SerializerValue::Type::String == value2.getType() );
  REQUIRE( "Hello!" == value2.toString() );

  auto value3 = serializer.getValue( "C" );
  REQUIRE( Lore::SerializerValue::Type::Real == value3.getType() );
  REQUIRE( 66.6f == value3.toReal() );

  auto value4 = serializer.getValue( "D" );
  REQUIRE( Lore::SerializerValue::Type::Bool == value4.getType() );
  REQUIRE( true == value4.toBool() );

  auto value5 = serializer.getValue( "E" );
  REQUIRE( Lore::SerializerValue::Type::Bool == value5.getType() );
  REQUIRE( false == value5.toBool() );

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Complex JSON values correctly deserialized", "[serializer]" )
{
  TEST_CREATE_CONTEXT();

  Lore::Serializer serializer;
  serializer.deserialize( "TestData/Serializer/Complex.json" );

  auto object1 = serializer.getValue( "Object1" );
  REQUIRE( Lore::SerializerValue::Type::Container == object1.getType() );
  {
    auto value1 = object1.getValue( "A" );
    REQUIRE( 1 == value1.toInt() );

    auto value2 = object1.getValue( "B" );
    REQUIRE( 3.14f == value2.toReal() );

    auto value3 = object1.getValue( "C" );
    REQUIRE( "Hello!" == value3.toString() );
  }

  auto object2 = serializer.getValue( "Object2" );
  REQUIRE( Lore::SerializerValue::Type::Container == object2.getType() );
  {
    auto value1 = object2.getValue( "A" );
    REQUIRE( Lore::SerializerValue::Type::Array == value1.getType() );
    {
      auto& valueArray = value1.toArray();
      for ( int i = 0; i < 5; ++i ) {
        REQUIRE( i == valueArray[i].toInt() );
      }
    }

    auto value2 = object2.getValue( "B" );
    REQUIRE( "Hello 2!" == value2.toString() );
  }

  auto object3 = object2.getValue( "Object3" );
  REQUIRE( Lore::SerializerValue::Type::Container == object3.getType() );
  {
    auto& value1 = object3.getValue( "A" );
    REQUIRE( Lore::SerializerValue::Type::Array == value1.getType() );
    auto& valueArray = value1.toArray();
    REQUIRE( 3 == valueArray.size() );

    auto a1 = valueArray[0];
    REQUIRE( Lore::SerializerValue::Type::String == a1.getType() );
    REQUIRE( "A" == a1.toString() );
    auto a2 = valueArray[1];
    REQUIRE( Lore::SerializerValue::Type::Real == a2.getType() );
    REQUIRE( 3.14f == a2.toReal() );
    auto a3 = valueArray[2];
    REQUIRE( Lore::SerializerValue::Type::Int == a3.getType() );
    REQUIRE( 0 == a3.toInt() );
  }

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //

TEST_CASE( "Values correctly written in JSON format", "[serializer]" )
{
  TEST_CREATE_CONTEXT();

  const Lore::string path = "TestData/Serializer/Out.json";

  SECTION( "Write Data" )
  {
    Lore::Serializer serializer;

    auto& v1 = serializer.addValue( "A" );
    v1 = true;

    auto& v2 = serializer.addValue( "B" );
    v2 = 42;

    auto& v3 = serializer.addValue( "C" );
    v3 = "Hello!";

    // Add child object.
    auto& object1 = serializer.addValue( "Object1" );
    object1["A"] = "Hello object1!";
    object1["12345"] = 12345;

    // Add array.
    auto& array1 = object1["array1"];
    Lore::SerializerValue::Array arrayValues;
    for ( int i = 0; i < 5; ++i ) {
      Lore::SerializerValue v;
      v = i;
      arrayValues.push_back( v );
    }
    array1 = arrayValues;

    // Add array with objects inside.
    auto& array2 = object1["array2"];
    arrayValues.clear();
    for ( int i = 0; i < 5; ++i ) {
      Lore::SerializerValue arrayObject1;
      arrayObject1[std::to_string(i)] = i;

      if ( 4 == i ) {
        // Add array within object.
        Lore::SerializerValue::Array arrayValues2;
        arrayValues2.resize( 3 );
        for ( int j = 0; j < 3; ++j ) {
          arrayValues2[j] = j;
        }
        arrayObject1["array"] = arrayValues2;
      }

      arrayValues.push_back( arrayObject1 );
    }
    array2 = arrayValues;

    serializer.serialize( path );
  }

  SECTION( "Read Data" )
  {
    Lore::Serializer serializer;
    serializer.deserialize( path );

    auto value1 = serializer.getValue( "A" );
    REQUIRE( Lore::SerializerValue::Type::Bool == value1.getType() );
    REQUIRE( true == value1.toBool() );

    auto value2 = serializer.getValue( "B" );
    REQUIRE( Lore::SerializerValue::Type::Int == value2.getType() );
    REQUIRE( 42 == value2.toInt() );

    auto value3 = serializer.getValue( "C" );
    REQUIRE( Lore::SerializerValue::Type::String == value3.getType() );
    REQUIRE( "Hello!" == value3.toString() );

    auto object1 = serializer.getValue( "Object1" );
    REQUIRE( Lore::SerializerValue::Type::Container == object1.getType() );
    {
      auto v1 = object1.getValue( "A" );
      REQUIRE( Lore::SerializerValue::Type::String == v1.getType() );
      REQUIRE( "Hello object1!" == v1.toString() );

      auto v2 = object1.getValue( "12345" );
      REQUIRE( Lore::SerializerValue::Type::Int == v2.getType() );
      REQUIRE( 12345 == v2.toInt() );

      auto array1 = object1.getValue( "array1" );
      REQUIRE( Lore::SerializerValue::Type::Array == array1.getType() );
      SECTION( "Simple Array" )
      {
        auto values = array1.toArray();
        REQUIRE( 5 == values.size() );
        for ( int i = 0; i < 5; ++i ) {
          REQUIRE( Lore::SerializerValue::Type::Int == values[i].getType() );
          REQUIRE( i == values[i].toInt() );
        }
      }

      auto array2 = object1.getValue( "array2" );
      REQUIRE( Lore::SerializerValue::Type::Array == array2.getType() );
      SECTION( "Array With Objects" )
      {
        auto values = array2.toArray();
        REQUIRE( 5 == values.size() );
        for ( int i = 0; i < 5; ++i ) {
          REQUIRE( Lore::SerializerValue::Type::Container == values[i].getType() );
          auto v = values[i].getValue( std::to_string( i ) );
          REQUIRE( Lore::SerializerValue::Type::Int == v.getType() );
          REQUIRE( i == v.toInt() );
        }

        // Check array within the object within the array within the object within the root object.
        auto obj = values[4];
        auto v = obj.getValue( "array" );
        REQUIRE( Lore::SerializerValue::Type::Array == v.getType() );
        auto arrayValues = v.toArray();
        for ( int i = 0; i < 3; ++i ) {
          auto arrayValue = arrayValues[i];
          REQUIRE( Lore::SerializerValue::Type::Int == arrayValue.getType() );
          REQUIRE( i == arrayValue.toInt() );
        }
      }
    }
  }

  TEST_DESTROY_CONTEXT();
}

// ::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::: //
